/*
 * 게임 내 모든 오브젝트와 컴포넌트, 물리/렌더링 시스템을 관리하는 핵심 씬(Scene) 클래스입니다.
 * 런타임 및 에디터 모드에서의 업데이트 흐름과 렌더링, 충돌 처리를 담당합니다.
 */

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>

#include "Scene.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/Rigidbody.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/NativeScriptComponent.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"
#include "../Asset/AssetManager.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/EditorCamera.h"
#include "../Core/Log.h"

namespace Proto
{
	namespace
	{
		static constexpr float GRAVITY = -9.81f;
		static constexpr float GRID_SIZE = 100.0f;
		static constexpr float COLLISION_EPSILON = 0.0001f;
	}

	Scene::Scene()
	{}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(name);
		gameObject->AddComponent<Transform>();

		GameObject* ptr = gameObject.get();
		m_GameObjects.push_back(std::move(gameObject));
		SetDirty(true);
		return ptr;
	}

	GameObject* Scene::CreateMeshGameObject(const std::string& name, UUID meshUUID)
	{
		GameObject* go = CreateGameObject(name);
		auto* mr = go->AddComponent<MeshRenderer>();

		mr->SetMesh(AssetManager::GetAssetAs<VertexArray>(meshUUID));
		mr->SetShader(AssetManager::GetAssetAs<Shader>(UUID(DefaultAsset::SHADER)));
		mr->SetMeshTypeName(name);

		return go;
	}

	void Scene::RemoveGameObject(GameObject* gameObject)
	{
		if (!gameObject)
		{
			return;
		}

		for (const auto& comp : gameObject->GetComponents())
		{
			comp->OnDestroy();
		}

		auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
			[gameObject](const std::unique_ptr<GameObject>& obj)
			{
				return obj.get() == gameObject;
			});

		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it);
			SetDirty(true);
		}
	}

	void Scene::CreateDefault()
	{
		auto* camera = CreateGameObject("Main Camera");
		camera->AddComponent<CameraComponent>();
		camera->GetComponent<Transform>()->Translation = { 0.0f, 0.0f, 5.0f };

		auto* light = CreateGameObject("Directional Light");
		light->AddComponent<LightComponent>();
		light->GetComponent<Transform>()->Rotation = { 2.35619f, 0.0f, 0.785398f };
	}

	void Scene::OnRuntimeStart()
	{
		for (const auto& go : m_GameObjects)
		{
			for (const auto& comp : go->GetComponents())
			{
				comp->OnStart();
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		for (const auto& go : m_GameObjects)
		{
			for (const auto& comp : go->GetComponents())
			{
				comp->OnDestroy();
			}
		}
	}

	void Scene::OnUpdateRuntime(float deltaTime, bool isFocused)
	{
		if (deltaTime > 0.0f)
		{
			UpdatePhysics(deltaTime);
			ResolveCollisions();

			for (const auto& go : m_GameObjects)
			{
				go->Update(deltaTime);
			}
		}

		GameObject* mainCameraGO = nullptr;
		GameObject* fallbackCameraGO = nullptr;
		for (const auto& go : m_GameObjects)
		{
			auto cameraComponent = go->GetComponent<CameraComponent>();
			if (!cameraComponent)
			{
				continue;
			}

			if (!fallbackCameraGO)
			{
				fallbackCameraGO = go.get();
			}

			if (cameraComponent->Primary)
			{
				mainCameraGO = go.get();
				break;
			}
		}

		if (!mainCameraGO)
		{
			mainCameraGO = fallbackCameraGO;
		}

		if (mainCameraGO)
		{
			auto cameraComponent = mainCameraGO->GetComponent<CameraComponent>();
			auto transform = mainCameraGO->GetComponent<Transform>();

			if (cameraComponent && transform)
			{
				const glm::mat4 cameraTransform = transform->GetTransform();
				const glm::mat4 viewProjection = cameraComponent->Camera.GetProjection() * glm::affineInverse(cameraTransform);
				const glm::vec3 viewPos = glm::vec3(cameraTransform[3]);

				RenderObjects(viewProjection, viewPos);
			}
		}
	}

	void Scene::OnUpdateEditor(float deltaTime, EditorCamera& camera)
	{
		const glm::mat4 viewProjection = camera.GetViewProjection();
		const glm::vec3 viewPos = camera.GetPosition();

		RenderObjects(viewProjection, viewPos, true);
		RenderGrid(viewProjection, viewPos);
	}

	void Scene::UpdatePhysics(float deltaTime)
	{
		for (const auto& go : m_GameObjects)
		{
			auto rb = go->GetComponent<Rigidbody>();
			auto transform = go->GetComponent<Transform>();

			if (!rb || !transform)
			{
				continue;
			}

			if (rb->UseGravity)
			{
				rb->Acceleration += glm::vec3(0.0f, GRAVITY, 0.0f);
			}

			rb->Velocity += rb->Acceleration * deltaTime;
			rb->Velocity *= glm::max(0.0f, 1.0f - rb->Drag * deltaTime);
			transform->Translation += rb->Velocity * deltaTime;

			rb->Acceleration = glm::vec3(0.0f);
		}
	}

	void Scene::ResolveCollisions()
	{
		for (size_t i = 0; i < m_GameObjects.size(); i++)
		{
			for (size_t j = i + 1; j < m_GameObjects.size(); j++)
			{
				ResolveCollision(m_GameObjects[i].get(), m_GameObjects[j].get());
			}
		}
	}

	void Scene::ResolveCollision(GameObject* goA, GameObject* goB)
	{
		auto transformA = goA->GetComponent<Transform>();
		auto transformB = goB->GetComponent<Transform>();
		auto colliderA = goA->GetComponent<Collider>();
		auto colliderB = goB->GetComponent<Collider>();

		if (!transformA || !transformB || !colliderA || !colliderB)
		{
			return;
		}

		auto* boxA = dynamic_cast<BoxCollider*>(colliderA);
		auto* sphereA = dynamic_cast<SphereCollider*>(colliderA);
		auto* boxB = dynamic_cast<BoxCollider*>(colliderB);
		auto* sphereB = dynamic_cast<SphereCollider*>(colliderB);

		bool isColliding = false;
		glm::vec3 normal(0.0f);
		float penetration = 0.0f;

		if (boxA && boxB)
		{
			const glm::vec3 posA = transformA->Translation + boxA->Offset;
			const glm::vec3 minA = posA - boxA->Size * 0.5f;
			const glm::vec3 maxA = posA + boxA->Size * 0.5f;

			const glm::vec3 posB = transformB->Translation + boxB->Offset;
			const glm::vec3 minB = posB - boxB->Size * 0.5f;
			const glm::vec3 maxB = posB + boxB->Size * 0.5f;

			isColliding = (minA.x < maxB.x && maxA.x > minB.x) &&
				(minA.y < maxB.y && maxA.y > minB.y) &&
				(minA.z < maxB.z && maxA.z > minB.z);

			if (isColliding)
			{
				const float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
				const float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
				const float overlapZ = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

				penetration = std::min({ overlapX, overlapY, overlapZ });
				if (penetration == overlapX)
				{
					normal = glm::vec3(posA.x < posB.x ? -1.0f : 1.0f, 0.0f, 0.0f);
				}
				else if (penetration == overlapY)
				{
					normal = glm::vec3(0.0f, posA.y < posB.y ? -1.0f : 1.0f, 0.0f);
				}
				else
				{
					normal = glm::vec3(0.0f, 0.0f, posA.z < posB.z ? -1.0f : 1.0f);
				}
			}
		}
		else if (sphereA && sphereB)
		{
			const glm::vec3 posA = transformA->Translation + sphereA->Offset;
			const glm::vec3 posB = transformB->Translation + sphereB->Offset;
			const float distSq = glm::dot(posA - posB, posA - posB);
			const float radSum = sphereA->Radius + sphereB->Radius;

			if (distSq < (radSum * radSum) && distSq > COLLISION_EPSILON)
			{
				isColliding = true;
				const float dist = std::sqrt(distSq);
				penetration = radSum - dist;
				normal = (posA - posB) / dist;
			}
		}

		if (isColliding)
		{
			auto rbA = goA->GetComponent<Rigidbody>();
			auto rbB = goB->GetComponent<Rigidbody>();
			const glm::vec3 separation = normal * penetration;

			if (rbA && !rbB)
			{
				transformA->Translation += separation;
				if (glm::dot(rbA->Velocity, normal) < 0.0f)
				{
					rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
				}
			}
			else if (!rbA && rbB)
			{
				transformB->Translation -= separation;
				if (glm::dot(rbB->Velocity, -normal) < 0.0f)
				{
					rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
				}
			}
			else if (rbA && rbB)
			{
				transformA->Translation += separation * 0.5f;
				transformB->Translation -= separation * 0.5f;
				if (glm::dot(rbA->Velocity, normal) < 0.0f)
				{
					rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
				}

				if (glm::dot(rbB->Velocity, -normal) < 0.0f)
				{
					rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
				}
			}

			if (auto nscA = goA->GetComponent<NativeScriptComponent>())
			{
				nscA->DispatchCollisionEnter(goB);
			}

			if (auto nscB = goB->GetComponent<NativeScriptComponent>())
			{
				nscB->DispatchCollisionEnter(goA);
			}
		}
	}

	Scene::LightInfo Scene::GetMainLightInfo()
	{
		LightInfo info;
		for (const auto& go : m_GameObjects)
		{
			auto light = go->GetComponent<LightComponent>();
			if (light)
			{
				const glm::mat4 lightTrans = go->GetComponent<Transform>()->GetTransform();
				info.Direction = glm::normalize(glm::vec3(lightTrans[2]));
				info.Color = light->Color;
				info.Intensity = light->Intensity;
				break;
			}
		}

		return info;
	}

	void Scene::RenderObjects(const glm::mat4& viewProjection, const glm::vec3& viewPos, bool isEditor)
	{
		const LightInfo light = GetMainLightInfo();

		for (const auto& go : m_GameObjects)
		{
			auto transform = go->GetComponent<Transform>();
			auto mr = go->GetComponent<MeshRenderer>();

			if (!mr || !transform)
			{
				continue;
			}

			auto shader = mr->GetShader();
			auto mesh = mr->GetMesh();

			if (!shader || !mesh)
			{
				continue;
			}

			shader->Bind();
			shader->UploadUniformMat4("u_ViewProjection", viewProjection);
			shader->UploadUniformMat4("u_Transform", transform->GetTransform());
			shader->UploadUniformFloat3("u_ViewPos", viewPos);
			shader->UploadUniformFloat3("u_LightDir", light.Direction);
			shader->UploadUniformFloat3("u_LightColor", light.Color * light.Intensity);

			if (isEditor)
			{
				shader->UploadUniformInt("u_EntityID", static_cast<int>(go->GetRuntimeID()));
			}

			Renderer::Submit(mesh, shader);
		}
	}

	void Scene::RenderGrid(const glm::mat4& viewProjection, const glm::vec3& viewPos)
	{
		if (!m_GridVAO)
		{
			m_GridVAO = std::make_shared<VertexArray>();

			float gridVertices[] = {
				-GRID_SIZE, 0.0f, -GRID_SIZE, 0.0f, 1.0f, 0.0f,
				 GRID_SIZE, 0.0f, -GRID_SIZE, 0.0f, 1.0f, 0.0f,
				 GRID_SIZE, 0.0f,  GRID_SIZE, 0.0f, 1.0f, 0.0f,
				-GRID_SIZE, 0.0f,  GRID_SIZE, 0.0f, 1.0f, 0.0f
			};

			uint32_t gridIndices[] = { 0, 1, 2, 2, 3, 0 };

			m_GridVAO->Bind();
			auto vbo = std::make_shared<VertexBuffer>(gridVertices, sizeof(gridVertices));
			m_GridVAO->AddVertexBuffer(vbo);

			auto ibo = std::make_shared<IndexBuffer>(gridIndices, 6);
			m_GridVAO->SetIndexBuffer(ibo);

			const std::string gridVertexSrc = R"(
				#version 330 core
				layout(location = 0) in vec3 a_Position;
				out vec3 v_FragPos;
				uniform mat4 u_ViewProjection;
				void main() {
					v_FragPos = a_Position;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				}
			)";

			const std::string gridFragmentSrc = R"(
				#version 330 core
				in vec3 v_FragPos;
				out vec4 o_Color;
				uniform vec3 u_CameraPos;
				void main() {
					float gridSize = 1.0;
					float thickness = 0.03;
					vec2 coord = v_FragPos.xz / gridSize;
					vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
					float line = min(grid.x, grid.y);
					float alpha = 1.0 - min(line, 1.0);
					vec4 gridColor = vec4(0.1, 0.1, 0.1, alpha * 0.6);
					if (abs(v_FragPos.z) < thickness) gridColor = vec4(1.0, 0.3, 0.3, 0.8);
					if (abs(v_FragPos.x) < thickness) gridColor = vec4(0.3, 0.3, 1.0, 0.8);
					float dist = distance(v_FragPos, vec3(u_CameraPos.x, 0.0, u_CameraPos.z));
					float fading = max(0.0, 1.0 - dist / 50.0);
					gridColor.a *= fading;
					if(gridColor.a < 0.01) discard;
					o_Color = gridColor;
				}
			)";

			m_GridShader = std::make_shared<Shader>(gridVertexSrc, gridFragmentSrc);
		}

		if (m_GridShader && m_GridVAO)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			m_GridShader->Bind();
			m_GridShader->UploadUniformMat4("u_ViewProjection", viewProjection);
			m_GridShader->UploadUniformFloat3("u_CameraPos", viewPos);
			Renderer::Submit(m_GridVAO, m_GridShader);

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		for (const auto& go : m_GameObjects)
		{
			auto cameraComponent = go->GetComponent<CameraComponent>();
			if (cameraComponent && !cameraComponent->FixedAspectRatio)
			{
				cameraComponent->Camera.SetViewportSize(width, height);
			}
		}
	}

	GameObject* Scene::GetGameObjectByRuntimeID(uint32_t id)
	{
		for (const auto& go : m_GameObjects)
		{
			if (go->GetRuntimeID() == id)
			{
				return go.get();
			}
		}

		return nullptr;
	}

	GameObject* Scene::GetGameObjectByUUID(UUID uuid)
	{
		for (const auto& go : m_GameObjects)
		{
			if (go->GetUUID() == uuid)
			{
				return go.get();
			}
		}

		return nullptr;
	}
}

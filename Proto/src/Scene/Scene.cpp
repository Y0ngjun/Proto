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
#include "../Core/Application.h"
#include "CollisionEvent.h"

namespace Proto
{
	namespace
	{
		static constexpr float GRAVITY = -9.81f;
		static constexpr float COLLISION_EPSILON = 0.0001f;

		// --- 에디터 하늘 그라디언트 색상 ---
		static constexpr glm::vec3 SKY_COLOR     = { 0.29f, 0.38f, 0.49f }; // 하늘 (파란색)
		static constexpr glm::vec3 HORIZON_COLOR = { 0.77f, 0.82f, 0.82f }; // 지평선 (청백색)
		static constexpr glm::vec3 GROUND_COLOR  = { 0.41f, 0.39f, 0.37f }; // 대지 (회갈색)

		// 지평선 범위 (단위: dir.y, 0~1). 값을 키울수록 그라디언트가 넓고 부드러워짐
		static constexpr float SKY_HORIZON_RANGE    = 0.45f; // 하늘↔지평선 전환 폭 (y=0 지평선부터 y=값까지 선형 블렌드)
		static constexpr float GROUND_HORIZON_RANGE = 0.05f; // 대지↔지평선 전환 폭

		// --- 에디터 그리드 ---
		static constexpr glm::vec3 GRID_COLOR_FINE   = { 0.42f, 0.42f, 0.42f }; // 세밀 그리드 색 (1유닛)
		static constexpr glm::vec3 GRID_COLOR_COARSE = { 0.47f, 0.47f, 0.47f }; // 굵은 그리드 색 (10유닛)
		static constexpr float     GRID_FINE_ALPHA   = 0.15f; // 세밀 선 최대 불투명도
		static constexpr float     GRID_COARSE_ALPHA = 0.35f; // 굵은 선 최대 불투명도
		static constexpr float     GRID_FADE_MULTIPLIER = 20.0f; // 페이드 거리 = 카메라 높이 × 이 값
		static constexpr float     GRID_FADE_MIN        = 80.0f; // 페이드 거리 최솟값 (유닛)
	}

	Scene::Scene()
	{}

	std::string Scene::MakeUniqueName(const std::string& name, const GameObject* exclude) const
	{
		std::string uniqueName = name;
		int suffix = 1;
		while (true)
		{
			bool conflict = false;
			for (const auto& go : m_GameObjects)
			{
				if (go.get() == exclude) continue;
				if (go->GetName() == uniqueName)
				{
					conflict = true;
					break;
				}
			}
			if (!conflict) break;
			uniqueName = name + " (" + std::to_string(suffix++) + ")";
		}
		return uniqueName;
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(MakeUniqueName(name));
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

		DrawSkyGradient(camera);
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

			Application::Get().GetEventBus().Publish(CollisionEvent{ goA, goB });
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

	void Scene::DrawSkyGradient(const EditorCamera& camera)
	{
		if (!m_SkyVAO)
		{
			// 전체화면 쿼드 (NDC 공간). VertexArray의 24-byte stride에 맞게 더미 법선 포함
			const float v[] = {
				-1.f, -1.f, 0.f,  0.f, 0.f, 1.f,
				 1.f, -1.f, 0.f,  0.f, 0.f, 1.f,
				 1.f,  1.f, 0.f,  0.f, 0.f, 1.f,
				-1.f,  1.f, 0.f,  0.f, 0.f, 1.f,
			};
			const uint32_t idx[] = { 0, 1, 2, 2, 3, 0 };

			m_SkyVAO = std::make_shared<VertexArray>();
			auto vb = std::make_shared<VertexBuffer>(v, sizeof(v));
			m_SkyVAO->AddVertexBuffer(vb);
			m_SkyVAO->SetIndexBuffer(std::make_shared<IndexBuffer>(idx, 6));

			const std::string vertSrc = R"(
				#version 330 core
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec3 a_Normal;
				out vec2 v_NDC;
				void main()
				{
					v_NDC = a_Position.xy;
					gl_Position = vec4(a_Position.xy, 1.0, 1.0);
				}
			)";

			const std::string fragSrc = R"(
				#version 330 core
				in vec2 v_NDC;
				uniform mat4 u_InvViewProj;
				uniform vec3 u_CameraPos;
				uniform vec3 u_SkyColor;
				uniform vec3 u_HorizonColor;
				uniform vec3 u_GroundColor;
				uniform float u_SkyHorizonRange;
				uniform float u_GroundHorizonRange;
				layout(location = 0) out vec4 color;
				layout(location = 1) out int color2;
				void main()
				{
					vec4 worldPos = u_InvViewProj * vec4(v_NDC, 1.0, 1.0);
					worldPos /= worldPos.w;
					vec3 dir = normalize(worldPos.xyz - u_CameraPos);

					// 대지→지평선: -GROUND_RANGE ~ 0 (선형)
					float groundBlend = clamp(dir.y / u_GroundHorizonRange + 1.0, 0.0, 1.0);
					// 지평선→하늘: 0 ~ SKY_RANGE (선형)
					float skyBlend = clamp(dir.y / u_SkyHorizonRange, 0.0, 1.0);
					color = vec4(mix(mix(u_GroundColor, u_HorizonColor, groundBlend), u_SkyColor, skyBlend), 1.0);
					color2 = -1;
				}
			)";

			m_SkyShader = std::make_shared<Shader>(vertSrc, fragSrc);
		}

		glDisable(GL_DEPTH_TEST);

		m_SkyShader->Bind();
		m_SkyShader->UploadUniformMat4("u_InvViewProj", glm::inverse(camera.GetViewProjection()));
		m_SkyShader->UploadUniformFloat3("u_CameraPos",    camera.GetPosition());
		m_SkyShader->UploadUniformFloat3("u_SkyColor",     SKY_COLOR);
		m_SkyShader->UploadUniformFloat3("u_HorizonColor", HORIZON_COLOR);
		m_SkyShader->UploadUniformFloat3("u_GroundColor",  GROUND_COLOR);
		m_SkyShader->UploadUniformFloat("u_SkyHorizonRange",    SKY_HORIZON_RANGE);
		m_SkyShader->UploadUniformFloat("u_GroundHorizonRange", GROUND_HORIZON_RANGE);
		Renderer::Submit(m_SkyVAO, m_SkyShader);

		glEnable(GL_DEPTH_TEST);
	}

	void Scene::RenderGrid(const glm::mat4& viewProjection, const glm::vec3& viewPos)
	{
		if (!m_GridVAO)
		{
			// 무한 그리드: 전체화면 쿼드 + 셰이더에서 y=0 평면 레이캐스팅
			const float v[] = {
				-1.f, -1.f, 0.f,  0.f, 0.f, 1.f,
				 1.f, -1.f, 0.f,  0.f, 0.f, 1.f,
				 1.f,  1.f, 0.f,  0.f, 0.f, 1.f,
				-1.f,  1.f, 0.f,  0.f, 0.f, 1.f,
			};
			const uint32_t idx[] = { 0, 1, 2, 2, 3, 0 };

			m_GridVAO = std::make_shared<VertexArray>();
			m_GridVAO->AddVertexBuffer(std::make_shared<VertexBuffer>(v, sizeof(v)));
			m_GridVAO->SetIndexBuffer(std::make_shared<IndexBuffer>(idx, 6));

			const std::string vertSrc = R"(
				#version 330 core
				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec3 a_Normal;
				out vec2 v_NDC;
				void main()
				{
					v_NDC = a_Position.xy;
					gl_Position = vec4(a_Position.xy, 1.0, 1.0);
				}
			)";

			const std::string fragSrc = R"(
				#version 330 core
				in vec2 v_NDC;
				uniform mat4 u_InvViewProj;
				uniform vec3 u_CameraPos;
				uniform vec3 u_GridColorFine;
				uniform vec3 u_GridColorCoarse;
				uniform float u_GridFineAlpha;
				uniform float u_GridCoarseAlpha;
				uniform float u_FadeMultiplier;
				uniform float u_FadeMin;
				layout(location = 0) out vec4 color;
				layout(location = 1) out int color2;

				// 주어진 셀 크기의 그리드 선 강도 반환 (fwidth 기반 AA)
				float gridLine(vec2 worldXZ, float cellSize)
				{
					vec2 coord = worldXZ / cellSize;
					vec2 g = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
					return 1.0 - min(min(g.x, g.y), 1.0);
				}

				void main()
				{
					// 픽셀 → 월드 레이 계산
					vec4 nearH = u_InvViewProj * vec4(v_NDC, -1.0, 1.0);
					vec4 farH  = u_InvViewProj * vec4(v_NDC,  1.0, 1.0);
					vec3 rayDir = normalize(farH.xyz / farH.w - nearH.xyz / nearH.w);

					// y=0 평면 교점 (수평선에 가까우면 또는 역방향이면 기각)
					if (abs(rayDir.y) < 0.0001) discard;
					float t = -u_CameraPos.y / rayDir.y;
					if (t < 0.0) discard;

					vec3 hit = u_CameraPos + t * rayDir;

					// 카메라 높이 기반 동적 페이드
					float camH     = abs(u_CameraPos.y);
					float fadeDist = max(camH * u_FadeMultiplier, u_FadeMin);
					float fade     = 1.0 - clamp(length(hit.xz - u_CameraPos.xz) / fadeDist, 0.0, 1.0);
					if (fade < 0.001) discard;

					// 멀티스케일: 1유닛(세밀) + 10유닛(굵은)
					float fine   = gridLine(hit.xz, 1.0);
					float coarse = gridLine(hit.xz, 10.0);

					// LOD: 카메라 높이 5~25 구간에서 세밀 그리드 선형 페이드 아웃
					float fineFade = 1.0 - clamp((camH - 5.0) / 20.0, 0.0, 1.0);
					float alpha = coarse * u_GridCoarseAlpha + fine * u_GridFineAlpha * fineFade;

					vec4 col = vec4(mix(u_GridColorFine, u_GridColorCoarse, coarse), alpha * fade);

					if (col.a < 0.005) discard;
					color  = col;
					color2 = -1;
				}
			)";

			m_GridShader = std::make_shared<Shader>(vertSrc, fragSrc);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL); // 전체화면 쿼드 depth=1.0 이 빈 픽셀(깊이 1.0)과 같으므로 LEQUAL 필요

		m_GridShader->Bind();
		m_GridShader->UploadUniformMat4("u_InvViewProj", glm::inverse(viewProjection));
		m_GridShader->UploadUniformFloat3("u_CameraPos",       viewPos);
		m_GridShader->UploadUniformFloat3("u_GridColorFine",   GRID_COLOR_FINE);
		m_GridShader->UploadUniformFloat3("u_GridColorCoarse", GRID_COLOR_COARSE);
		m_GridShader->UploadUniformFloat("u_GridFineAlpha",    GRID_FINE_ALPHA);
		m_GridShader->UploadUniformFloat("u_GridCoarseAlpha",  GRID_COARSE_ALPHA);
		m_GridShader->UploadUniformFloat("u_FadeMultiplier",   GRID_FADE_MULTIPLIER);
		m_GridShader->UploadUniformFloat("u_FadeMin",          GRID_FADE_MIN);
		Renderer::Submit(m_GridVAO, m_GridShader);

		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
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

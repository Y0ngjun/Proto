#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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

#include "../Renderer/Renderer.h"
#include "../Renderer/EditorCamera.h"

namespace Proto
{
	Scene::Scene()
	{
		// 1. Grid Vertex Array & Shader Initialization
		m_GridVAO = std::make_shared<VertexArray>();

		float gridVertices[] = {
			// Position            // Normal
			-100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 0.0f,
			 100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 0.0f,
			 100.0f, 0.0f,  100.0f, 0.0f, 1.0f, 0.0f,
			-100.0f, 0.0f,  100.0f, 0.0f, 1.0f, 0.0f
		};

		std::shared_ptr<VertexBuffer> gridVB = std::make_shared<VertexBuffer>(gridVertices, sizeof(gridVertices));
		m_GridVAO->AddVertexBuffer(gridVB);

		uint32_t gridIndices[] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> gridIB = std::make_shared<IndexBuffer>(gridIndices, 6);
		m_GridVAO->SetIndexBuffer(gridIB);

		std::string gridVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			
			out vec3 v_FragPos;

			uniform mat4 u_ViewProjection;

			void main()
			{
				v_FragPos = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string gridFragmentSrc = R"(
			#version 330 core
			in vec3 v_FragPos;
			out vec4 o_Color;

			uniform vec3 u_CameraPos;

			void main()
			{
				float gridSize = 1.0;
				float thickness = 0.03; // Line thickness

				vec2 coord = v_FragPos.xz / gridSize;
				vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
				float line = min(grid.x, grid.y);

				float alpha = 1.0 - min(line, 1.0);
				
				// Base grid color
				vec4 gridColor = vec4(0.1, 0.1, 0.1, alpha * 0.6);

				// Highlight Axis
				if (abs(v_FragPos.z) < thickness) gridColor = vec4(1.0, 0.3, 0.3, 0.8); // X Axis (Red)
				if (abs(v_FragPos.x) < thickness) gridColor = vec4(0.3, 0.3, 1.0, 0.8); // Z Axis (Blue)

				// Fade out with distance
				float dist = distance(v_FragPos, vec3(u_CameraPos.x, 0.0, u_CameraPos.z));
				float fading = max(0.0, 1.0 - dist / 50.0);
				gridColor.a *= fading;

				if(gridColor.a < 0.01) discard;

				o_Color = gridColor;
			}
		)";

		m_GridShader = std::make_shared<Shader>(gridVertexSrc, gridFragmentSrc);
	}

	GameObject* Scene::CreateGameObject(const std::string& name)
	{
		auto gameObject = std::make_unique<GameObject>(name);

		gameObject->AddComponent<Transform>();

		GameObject* ptr = gameObject.get();
		m_GameObjects.push_back(std::move(gameObject));
		return ptr;
	}

	void Scene::OnRuntimeStart()
	{
		for (auto& go : m_GameObjects)
		{
			for (auto& comp : go->GetComponents())
			{
				comp->OnStart();
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		for (auto& go : m_GameObjects)
		{
			for (auto& comp : go->GetComponents())
			{
				comp->OnDestroy();
			}
		}
	}

	void Scene::OnUpdateRuntime(float deltaTime, bool isFocused)
	{
		// 로직 업데이트는 플레이 상태(deltaTime > 0)일 때만 실행
		if (deltaTime > 0.0f)
		{
			// 1. Physics (Rigidbody) 적분
			for (auto& go : m_GameObjects)
			{
				auto rb = go->GetComponent<Rigidbody>();
				auto transform = go->GetComponent<Transform>();
				if (rb && transform)
				{
					if (rb->UseGravity)
					{
						rb->Acceleration += glm::vec3(0.0f, -9.81f, 0.0f); // 중력 가속도
					}

					rb->Velocity += rb->Acceleration * deltaTime;
					rb->Velocity *= glm::max(0.0f, 1.0f - rb->Drag * deltaTime); // 공기 저항
					transform->Translation += rb->Velocity * deltaTime;

					// 다음 프레임을 위해 가속도 초기화
					rb->Acceleration = glm::vec3(0.0f);
				}
			}

			// 2. Collision Detection (O(N^2) 단순 비교 - MVP 용)
			for (size_t i = 0; i < m_GameObjects.size(); i++)
			{
				auto& goA = m_GameObjects[i];
				auto transformA = goA->GetComponent<Transform>();
				if (!transformA) continue;

				auto colliderA = goA->GetComponent<Collider>();
				if (!colliderA) continue;

				auto* boxA = dynamic_cast<BoxCollider*>(colliderA);
				auto* sphereA = dynamic_cast<SphereCollider*>(colliderA);

				for (size_t j = i + 1; j < m_GameObjects.size(); j++)
				{
					auto& goB = m_GameObjects[j];
					auto transformB = goB->GetComponent<Transform>();
					if (!transformB) continue;

					auto colliderB = goB->GetComponent<Collider>();
					if (!colliderB) continue;

					auto* boxB = dynamic_cast<BoxCollider*>(colliderB);
					auto* sphereB = dynamic_cast<SphereCollider*>(colliderB);

					bool isColliding = false;

					// Box vs Box
					if (boxA && boxB)
					{
						glm::vec3 posA = transformA->Translation + boxA->Offset;
						glm::vec3 minA = posA - boxA->Size * 0.5f;
						glm::vec3 maxA = posA + boxA->Size * 0.5f;

						glm::vec3 posB = transformB->Translation + boxB->Offset;
						glm::vec3 minB = posB - boxB->Size * 0.5f;
						glm::vec3 maxB = posB + boxB->Size * 0.5f;

						isColliding = (minA.x < maxB.x && maxA.x > minB.x) &&
									  (minA.y < maxB.y && maxA.y > minB.y) &&
									  (minA.z < maxB.z && maxA.z > minB.z);

						if (isColliding)
						{
							float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
							float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
							float overlapZ = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

							float minOverlap = std::min({overlapX, overlapY, overlapZ});
							glm::vec3 normal(0.0f);

							if (minOverlap == overlapX)
								normal = glm::vec3(posA.x < posB.x ? -1.0f : 1.0f, 0.0f, 0.0f);
							else if (minOverlap == overlapY)
								normal = glm::vec3(0.0f, posA.y < posB.y ? -1.0f : 1.0f, 0.0f);
							else
								normal = glm::vec3(0.0f, 0.0f, posA.z < posB.z ? -1.0f : 1.0f);

							glm::vec3 separation = normal * minOverlap;

							auto rbA = goA->GetComponent<Rigidbody>();
							auto rbB = goB->GetComponent<Rigidbody>();

							if (rbA && !rbB)
							{
								transformA->Translation += separation;
								if (glm::dot(rbA->Velocity, normal) < 0.0f)
									rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
							}
							else if (!rbA && rbB)
							{
								transformB->Translation -= separation;
								if (glm::dot(rbB->Velocity, -normal) < 0.0f)
									rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
							}
							else if (rbA && rbB)
							{
								transformA->Translation += separation * 0.5f;
								transformB->Translation -= separation * 0.5f;
								if (glm::dot(rbA->Velocity, normal) < 0.0f)
									rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
								if (glm::dot(rbB->Velocity, -normal) < 0.0f)
									rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
							}
						}
					}
					// Sphere vs Sphere
					else if (sphereA && sphereB)
					{
						glm::vec3 posA = transformA->Translation + sphereA->Offset;
						glm::vec3 posB = transformB->Translation + sphereB->Offset;
						float distSq = glm::dot(posA - posB, posA - posB);
						float radSum = sphereA->Radius + sphereB->Radius;
						
						isColliding = distSq < (radSum * radSum) && distSq > 0.0001f;

						if (isColliding)
						{
							float dist = std::sqrt(distSq);
							float overlap = radSum - dist;
							glm::vec3 normal = (posA - posB) / dist;
							glm::vec3 separation = normal * overlap;

							auto rbA = goA->GetComponent<Rigidbody>();
							auto rbB = goB->GetComponent<Rigidbody>();

							if (rbA && !rbB)
							{
								transformA->Translation += separation;
								if (glm::dot(rbA->Velocity, normal) < 0.0f)
									rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
							}
							else if (!rbA && rbB)
							{
								transformB->Translation -= separation;
								if (glm::dot(rbB->Velocity, -normal) < 0.0f)
									rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
							}
							else if (rbA && rbB)
							{
								transformA->Translation += separation * 0.5f;
								transformB->Translation -= separation * 0.5f;
								if (glm::dot(rbA->Velocity, normal) < 0.0f)
									rbA->Velocity -= normal * glm::dot(rbA->Velocity, normal);
								if (glm::dot(rbB->Velocity, -normal) < 0.0f)
									rbB->Velocity -= -normal * glm::dot(rbB->Velocity, -normal);
							}
						}
					}
					// Box vs Sphere
					else
					{
						auto box = boxA ? boxA : boxB;
						auto sphere = sphereA ? sphereA : sphereB;
						auto transBox = boxA ? transformA : transformB;
						auto transSphere = sphereA ? transformA : transformB;

						glm::vec3 boxPos = transBox->Translation + box->Offset;
						glm::vec3 minBox = boxPos - box->Size * 0.5f;
						glm::vec3 maxBox = boxPos + box->Size * 0.5f;

						glm::vec3 spherePos = transSphere->Translation + sphere->Offset;
						glm::vec3 closest = glm::clamp(spherePos, minBox, maxBox);
						float distSq = glm::dot(closest - spherePos, closest - spherePos);
						isColliding = distSq < (sphere->Radius * sphere->Radius);

						// [임시] Box vs Sphere 충돌 해결은 계산이 복잡하여 MVP에서는 감지만 수행합니다.
					}

					// 충돌 시 스크립트에만 콜백 전달
					if (isColliding)
					{
						if (auto nscA = goA->GetComponent<NativeScriptComponent>())
							nscA->DispatchCollisionEnter(goB.get());

						if (auto nscB = goB->GetComponent<NativeScriptComponent>())
							nscB->DispatchCollisionEnter(goA.get());
					}
				}
			}

			// 3. GameObject 로직 업데이트
			for (auto& go : m_GameObjects)
			{
				go->Update(deltaTime);
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		for (auto& go : m_GameObjects)
		{
			auto cameraComponent = go->GetComponent<CameraComponent>();
			if (cameraComponent && cameraComponent->Primary)
			{
				auto transform = go->GetComponent<Transform>();
				if (transform)
				{
					mainCamera = &cameraComponent->Camera;
					cameraTransform = transform->GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			// 카메라의 뷰포트가 설정되지 않았으면 기본값 설정
			glm::mat4 projMat = mainCamera->GetProjection();
			if (projMat == glm::mat4(1.0f))
			{
				// 기본 화면 크기로 설정 (이것은 임시 설정이며, OnViewportResize에서 올바른 크기로 업데이트됨)
				static_cast<SceneCamera*>(mainCamera)->SetViewportSize(1280, 720);
			}

			glm::mat4 viewProjection = mainCamera->GetProjection() * glm::affineInverse(cameraTransform);
			glm::vec3 viewPos = glm::vec3(cameraTransform[3]); // 카메라는 Transform의 4번째 열(마지막 열) 벡터의 x,y,z 위치를 가짐

			// 1. 빛 정보 추출 (Directional Light 하나만 있다고 가정)
			// 조명이 없을 때 물체 정면이 잘 보이도록 뒤에서 앞으로 쏘는 대각선 방향으로 변경
			glm::vec3 lightDir = { -0.5f, -0.5f, -1.0f }; 
			glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
			float lightIntensity = 1.0f;

			for (auto& go : m_GameObjects)
			{
				auto light = go->GetComponent<LightComponent>();
				if (light)
				{
					// Directional Light의 빙향은 객체가 바라보는(Forward) 방향
					// 여기서는 Transform의 x, y, z 회전을 방향 벡터로 간단히 변환하거나 강제로 세팅
					// 임시로 객체의 z축(Forward) 역방향을 반환하도록 설정
					glm::mat4 lightTrans = go->GetComponent<Transform>()->GetTransform();
					lightDir = glm::normalize(glm::vec3(lightTrans[2])); 
					lightColor = light->Color;
					lightIntensity = light->Intensity;
					break;
				}
			}

			for (auto& go : m_GameObjects)
			{
				auto transform = go->GetComponent<Transform>();
				auto meshRenderer = go->GetComponent<MeshRenderer>();

				if (meshRenderer && transform)
				{
					auto shader = meshRenderer->GetShader();
					auto mesh = meshRenderer->GetMesh();

					if (shader && mesh)
					{
						shader->Bind();

						shader->UploadUniformMat4("u_ViewProjection", viewProjection);
						shader->UploadUniformMat4("u_Transform", transform->GetTransform());

						// 빛 및 카메라 위치 전송
						shader->UploadUniformFloat3("u_ViewPos", viewPos);
						shader->UploadUniformFloat3("u_LightDir", lightDir);
						shader->UploadUniformFloat3("u_LightColor", lightColor * lightIntensity);

						Renderer::Submit(mesh, shader);
					}
				}
			}
		}
	}

	void Scene::OnUpdateEditor(float deltaTime, EditorCamera& camera)
	{
		glm::mat4 viewProjection = camera.GetViewProjection();
		glm::vec3 viewPos = camera.GetPosition();

		// 에디터에서도 정면이 잘 보이도록 대각선 조명 적용
		glm::vec3 lightDir = { -0.5f, -0.5f, -1.0f };
		glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
		float lightIntensity = 1.0f;

		for (auto& go : m_GameObjects)
		{
			auto light = go->GetComponent<LightComponent>();
			if (light)
			{
				glm::mat4 lightTrans = go->GetComponent<Transform>()->GetTransform();
				lightDir = glm::normalize(glm::vec3(lightTrans[2]));
				lightColor = light->Color;
				lightIntensity = light->Intensity;
				break;
			}
		}

		for (auto& go : m_GameObjects)
		{
			auto transform = go->GetComponent<Transform>();
			auto meshRenderer = go->GetComponent<MeshRenderer>();

			if (meshRenderer && transform)
			{
				auto shader = meshRenderer->GetShader();
				auto mesh = meshRenderer->GetMesh();

				if (shader && mesh)
				{
					shader->Bind();
					shader->UploadUniformMat4("u_ViewProjection", viewProjection);
					shader->UploadUniformMat4("u_Transform", transform->GetTransform());
					shader->UploadUniformInt("u_EntityID", (int)go->GetRuntimeID());

					shader->UploadUniformFloat3("u_ViewPos", viewPos);
					shader->UploadUniformFloat3("u_LightDir", lightDir);
					shader->UploadUniformFloat3("u_LightColor", lightColor * lightIntensity);

					Renderer::Submit(mesh, shader);
				}
			}
		}

		// Draw Grid (에디터 뷰에서만 렌더링)
		// [TODO] 현재 격자는 원점 기준(-100 ~ 100)으로 고정되어 있어 카메라가 범위를 벗어나면 보이지 않습니다.
		// 나중에 완벽한 무한 격자를 위해 1) 도화지가 카메라 X,Z 위치를 따라다니게 하거나(Treadmill) 
		// 2) NDC 역투영(Full-screen Quad) 방식으로 개선할 수 있습니다.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE); // 격자를 그릴 때 깊이 버퍼에 쓰지 않도록 하여 뒤에 그려지더라도 잘리지 않게 함 (혹은 순서 제어용)

		if (m_GridShader && m_GridVAO)
		{
			m_GridShader->Bind();
			m_GridShader->UploadUniformMat4("u_ViewProjection", viewProjection);
			m_GridShader->UploadUniformFloat3("u_CameraPos", viewPos);
			Renderer::Submit(m_GridVAO, m_GridShader);
		}

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		for (auto& go : m_GameObjects)
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
		for (auto& go : m_GameObjects)
		{
			if (go->GetRuntimeID() == id)
				return go.get();
		}
		return nullptr;
	}

	GameObject* Scene::GetGameObjectByUUID(UUID uuid)
	{
		for (auto& go : m_GameObjects)
		{
			if (go->GetUUID() == uuid)
				return go.get();
		}
		return nullptr;
	}

}

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Scene.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/EditorCamera.h"

namespace Proto
{
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
	}

	void Scene::OnRuntimeStop()
	{
	}

	void Scene::OnUpdateRuntime(float deltaTime)
	{
		for (auto& go : m_GameObjects)
		{
			go->Update(deltaTime);
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		for (auto& go : m_GameObjects)
		{
			auto cameraComponent = go->GetComponent<CameraComponent>();
			if (cameraComponent && cameraComponent->Primary)
			{
				mainCamera = &cameraComponent->Camera;
				cameraTransform = go->GetComponent<Transform>()->GetTransform();
				break;
			}
		}

		if (mainCamera)
		{
			glm::mat4 viewProjection = mainCamera->GetProjection() * glm::affineInverse(cameraTransform);
			glm::vec3 viewPos = glm::vec3(cameraTransform[3]); // 카메라는 Transform의 4번째 열(마지막 열) 벡터의 x,y,z 위치를 가짐

			// 1. 빛 정보 추출 (Directional Light 하나만 있다고 가정)
			glm::vec3 lightDir = { -0.2f, -1.0f, -0.3f };
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

		glm::vec3 lightDir = { -0.2f, -1.0f, -0.3f };
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
					shader->UploadUniformInt("u_EntityID", (int)go->GetID());

					shader->UploadUniformFloat3("u_ViewPos", viewPos);
					shader->UploadUniformFloat3("u_LightDir", lightDir);
					shader->UploadUniformFloat3("u_LightColor", lightColor * lightIntensity);

					Renderer::Submit(mesh, shader);
				}
			}
		}
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

	GameObject* Scene::GetGameObjectByID(uint32_t id)
	{
		for (auto& go : m_GameObjects)
		{
			if (go->GetID() == id)
				return go.get();
		}
		return nullptr;
	}

}
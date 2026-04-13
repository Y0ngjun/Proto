#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Scene.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/CameraComponent.h"

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

						Renderer::Submit(mesh, shader);
					}
				}
			}
		}
	}

	void Scene::OnUpdateEditor(float deltaTime, EditorCamera& camera)
	{
		glm::mat4 viewProjection = camera.GetViewProjection();

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

}
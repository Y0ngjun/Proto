#pragma once

#include <memory>
#include <string>
#include <cmath>

#include "../Core/Application.h"
#include "../Scene/Scene.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/Shader.h"
#include "../Scene/Components/LightComponent.h"
#include "../Asset/AssetManager.h"
#include "../Scene/Components/NativeScriptComponent.h"
#include "../Core/Input.h"
#include <GLFW/glfw3.h>
namespace Proto
{
	namespace Test
	{
		class CameraController : public ScriptableEntity
		{
		public:
			void OnUpdate(float ts) override
			{
				auto transform = GetComponent<Transform>();
				if (!transform) return;

				float speed = 5.0f;
				if (Input::GetKey(GLFW_KEY_A))
					transform->Translation.x -= speed * ts;
				if (Input::GetKey(GLFW_KEY_D))
					transform->Translation.x += speed * ts;
				if (Input::GetKey(GLFW_KEY_W))
					transform->Translation.y += speed * ts;
				if (Input::GetKey(GLFW_KEY_S))
					transform->Translation.y -= speed * ts;
				if (Input::GetKey(GLFW_KEY_E))
					transform->Translation.z -= speed * ts;
				if (Input::GetKey(GLFW_KEY_Q))
					transform->Translation.z += speed * ts;
			}
		};

		inline void SetupCubeTest(Application& app)
		{
			Scene* scene = new Scene();
			app.SetScene(scene);

			// Camera Setup
			auto cameraGo = scene->CreateGameObject("MainCamera");
			auto& transform = *cameraGo->GetComponent<Transform>();
			transform.Translation = { 0.0f, 3.0f, 5.0f }; // Move camera back
			transform.Rotation = { glm::radians(-30.0f), 0.0f, 0.0f};
			auto& cameraComponent = *cameraGo->AddComponent<CameraComponent>();
			cameraComponent.Camera.SetViewportSize(1920, 1080);
			cameraGo->AddComponent<NativeScriptComponent>()->Bind<CameraController>();

			// 메시 로드
			auto cubeMesh = AssetManager::GetAssetAs<VertexArray>(UUID(1));
			auto planeMesh = AssetManager::GetAssetAs<VertexArray>(UUID(2));

			// 셰이더 로드
			auto standardShader = AssetManager::GetAssetAs<Shader>(UUID(100));
			auto planeShader = AssetManager::GetAssetAs<Shader>(UUID(101));

			// 빛 객체 생성
			auto lightGo = scene->CreateGameObject("DirectionalLight");
			auto& lightTrans = *lightGo->GetComponent<Transform>();
			// 태양 빛처럼 아래와 약간 대각선으로 비추도록 회전값을 설정
			lightTrans.Rotation = glm::vec3(glm::radians(45.0f), glm::radians(30.0f), 0.0f);
			lightGo->AddComponent<LightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

			// Floor Object
			auto planeGo = scene->CreateGameObject("FloorPlane");
			auto* planeMeshRenderer = planeGo->AddComponent<MeshRenderer>();
			planeMeshRenderer->SetMesh(planeMesh);
			planeMeshRenderer->SetShader(planeShader);
			planeGo->GetComponent<Transform>()->Translation.y = 0.1f;

			auto cubeGo = scene->CreateGameObject("Cube");
			auto* cubeMeshRenderer = cubeGo->AddComponent<MeshRenderer>();
			cubeMeshRenderer->SetMesh(cubeMesh);
			cubeMeshRenderer->SetShader(standardShader);
			cubeGo->GetComponent<Transform>()->Translation.y += 1;

			// 큐브 테스트
			app.SetUpdateCallback([cubeGo, time = 0.0f](float deltaTime) mutable
				{
					auto transform = cubeGo->GetComponent<Transform>();
					if (transform)
					{
						transform->Rotation.x += deltaTime * 0.5f;
						transform->Rotation.y += deltaTime * 1.0f;

						time += deltaTime;
						float scale = 1.0f + 0.5f * std::sin(time * 2.0f);
						transform->Scale = { scale, scale, scale };
					}
				});
		}

	}
}

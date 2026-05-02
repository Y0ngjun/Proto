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
#include "../Scene/Components/Rigidbody.h"
#include "../Scene/Components/BoxCollider.h"
#include "../Scene/Components/SphereCollider.h"
#include "../Core/Input.h"
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
				if (Input::GetKey(Key::A))
					transform->Translation.x -= speed * ts;
				if (Input::GetKey(Key::D))
					transform->Translation.x += speed * ts;
				if (Input::GetKey(Key::E))
					transform->Translation.y += speed * ts;
				if (Input::GetKey(Key::Q))
					transform->Translation.y -= speed * ts;
				if (Input::GetKey(Key::W))
					transform->Translation.z -= speed * ts;
				if (Input::GetKey(Key::S))
					transform->Translation.z += speed * ts;
			}
		};

		class CubeScript : public ScriptableEntity
		{
		public:
			void OnCollisionEnter(GameObject* other) override
			{
				if (other->GetName() == "FloorPlane")
				{
					auto rb = GetComponent<Rigidbody>();
					if (rb)
					{
						// 바닥과 부딪히면 위로 튕기기 (Bounce)
						// rb->Velocity.y = 5.0f;
					}
				}
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
			planeMeshRenderer->SetShader(standardShader);
			planeGo->GetComponent<Transform>()->Translation.y = 0.1f;
			auto planeCollider = planeGo->AddComponent<BoxCollider>();
			planeCollider->Size = { 1.0f, 0.1f, 1.0f };

			auto cubeGo = scene->CreateGameObject("Cube");
			auto* cubeMeshRenderer = cubeGo->AddComponent<MeshRenderer>();
			cubeMeshRenderer->SetMesh(cubeMesh);
			cubeMeshRenderer->SetShader(standardShader);
			cubeGo->GetComponent<Transform>()->Translation.y = 3.0f;

			auto cubeRb = cubeGo->AddComponent<Rigidbody>();
			cubeRb->UseGravity = true;

			auto cubeCollider = cubeGo->AddComponent<BoxCollider>();
			cubeCollider->Size = { 1.0f, 1.0f, 1.0f };

			cubeGo->AddComponent<NativeScriptComponent>()->Bind<CubeScript>();
		}

	}
}

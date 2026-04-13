#pragma once

#include <memory>
#include <string>
#include <cmath>

#include "../Core/Application.h"
#include "../Scene/Scene.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/MeshRenderer.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"

namespace Proto
{
	namespace Test
	{
		inline void SetupCubeTest(Application& app)
		{
			Scene* scene = new Scene();
			app.SetScene(scene);

			// Camera Setup
			auto cameraGo = scene->CreateGameObject("MainCamera");
			auto& transform = *cameraGo->GetComponent<Transform>();
			transform.Translation = { 0.0f, 0.0f, 5.0f }; // Move camera back
			auto& cameraComponent = *cameraGo->AddComponent<CameraComponent>();
			cameraComponent.Camera.SetViewportSize(1920, 1080);

			// Cube Setup
			float vertices[] =
			{
				-0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f
			};

			uint32_t indices[] =
			{
				0, 1, 2, 2, 3, 0, // Front
				1, 5, 6, 6, 2, 1, // Right
				5, 4, 7, 7, 6, 5, // Back
				4, 0, 3, 3, 7, 4, // Left
				3, 2, 6, 6, 7, 3, // Top
				4, 5, 1, 1, 0, 4  // Bottom
			};

			auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
			auto ebo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
			auto vao = std::make_shared<VertexArray>();
			vao->AddVertexBuffer(vbo);
			vao->SetIndexBuffer(ebo);

			const std::string vertexSrc = R"(
				#version 330 core
				layout(location=0) in vec3 a_Position;
				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;
				out vec3 v_Position;
				void main() {
					v_Position = a_Position;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

			const std::string fragmentSrc = R"(
				#version 330 core
				in vec3 v_Position;
				out vec4 color;
				void main() {
					color = vec4(v_Position * 0.5 + 0.5, 1.0);
				}
			)";

			auto shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);

			auto cubeGo = scene->CreateGameObject("Cube");
			cubeGo->AddComponent<MeshRenderer>(vao, shader);

			// 람다 함수를 통해 Application 루프 안에서 큐브가 회전 및 스케일링 될 수 있도록 Update 콜백 등록
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
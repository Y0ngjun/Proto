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
#include "../Scene/Components/LightComponent.h"

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
			float vertices[] = {
				// Front face (Z = 0.5)
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 0
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 1
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 2
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // 3
				// Back face (Z = -0.5)
				 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 4
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 5
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 6
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // 7
				// Left face (X = -0.5)
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 8
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 9
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // 10
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // 11
				// Right face (X = 0.5)
				 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 12
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 13
				 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // 14
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // 15
				// Top face (Y = 0.5)
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 16
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // 17
				 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 18
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // 19
				// Bottom face (Y = -0.5)
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 20
				 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // 21
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // 22
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f  // 23
			};

			uint32_t indices[] = {
				 0,  1,  2,  2,  3,  0, // Front
				 4,  5,  6,  6,  7,  4, // Back
				 8,  9, 10, 10, 11,  8, // Left
				12, 13, 14, 14, 15, 12, // Right
				16, 17, 18, 18, 19, 16, // Top
				20, 21, 22, 22, 23, 20  // Bottom
			};

			auto vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
			auto ebo = std::make_shared<IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
			auto vao = std::make_shared<VertexArray>();
			vao->AddVertexBuffer(vbo);
			vao->SetIndexBuffer(ebo);

			const std::string vertexSrc = R"(
				#version 330 core
				layout(location=0) in vec3 a_Position;
				layout(location=1) in vec3 a_Normal;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;

				out vec3 v_FragPos;
				out vec3 v_Normal;

				void main() {
					v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
					// 법선 행렬: 비율(Scale) 변형 시 법선이 왜곡되는 것을 방지
					v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
					gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
				}
			)";

			const std::string fragmentSrc = R"(
				#version 330 core
				layout(location = 0) out vec4 color;
				layout(location = 1) out int color2;

				in vec3 v_FragPos;
				in vec3 v_Normal;

				uniform int u_EntityID;
				uniform vec3 u_ViewPos;
				uniform vec3 u_LightDir;
				uniform vec3 u_LightColor;

				void main() {
					// 기본 큐브 색상 지정
					vec3 objectColor = vec3(1.0, 0.5, 0.31); 

					// 1. Ambient (주변광)
					float ambientStrength = 0.2;
					vec3 ambient = ambientStrength * u_LightColor;

					// 2. Diffuse (난반사광)
					vec3 norm = normalize(v_Normal);
					vec3 lightDir = normalize(-u_LightDir); // 빛이 향하는 방향의 반대 방향 계산
					float diff = max(dot(norm, lightDir), 0.0);
					vec3 diffuse = diff * u_LightColor;

					// 3. Specular (정반사광)
					float specularStrength = 0.5;
					vec3 viewDir = normalize(u_ViewPos - v_FragPos);
					vec3 reflectDir = reflect(-lightDir, norm);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
					vec3 specular = specularStrength * spec * u_LightColor;

					vec3 result = (ambient + diffuse + specular) * objectColor;
					color = vec4(result, 1.0);
					color2 = u_EntityID;
				}
			)";

			auto shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);

			// 빛 객체 생성
			auto lightGo = scene->CreateGameObject("DirectionalLight");
			auto& lightTrans = *lightGo->GetComponent<Transform>();
			// 태양 빛처럼 아래와 약간 대각선으로 비추도록 회전값을 설정
			lightTrans.Rotation = glm::vec3(glm::radians(45.0f), glm::radians(30.0f), 0.0f);
			lightGo->AddComponent<LightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

			auto cubeGo = scene->CreateGameObject("Cube");
			cubeGo->AddComponent<MeshRenderer>(vao, shader);

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

/*
 * 게임 내 모든 오브젝트와 컴포넌트, 물리/렌더링 시스템을 관리하는 핵심 씬(Scene) 클래스입니다.
 * 런타임 및 에디터 모드에서의 업데이트 흐름과 렌더링, 충돌 처리를 담당합니다.
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "GameObject.h"

namespace Proto
{
	class EditorCamera;
	class VertexArray;
	class Shader;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		GameObject* CreateMeshGameObject(const std::string& name, class UUID meshUUID);
		void RemoveGameObject(GameObject* gameObject);
		void CreateDefault();

		void OnUpdateRuntime(float deltaTime, bool isFocused = true);
		void OnUpdateEditor(float deltaTime, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		void OnRuntimeStart();
		void OnRuntimeStop();

		std::vector<std::unique_ptr<GameObject>>& GetGameObjects()
		{
			return m_GameObjects;
		}

		GameObject* GetGameObjectByRuntimeID(uint32_t id);
		GameObject* GetGameObjectByUUID(UUID uuid);

		// 상태 변경(Dirty Flag) 관리
		void SetDirty(bool dirty)
		{
			m_IsDirty = dirty;
		}

		bool IsDirty() const
		{
			return m_IsDirty;
		}

	private:
		struct LightInfo
		{
			glm::vec3 Direction = { -0.5f, -0.5f, -1.0f };
			glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
			float Intensity = 1.0f;
		};

		void UpdatePhysics(float deltaTime);
		void ResolveCollisions();
		void ResolveCollision(GameObject* goA, GameObject* goB);

		void RenderObjects(const glm::mat4& viewProjection, const glm::vec3& viewPos, bool isEditor = false);
		void RenderGrid(const glm::mat4& viewProjection, const glm::vec3& viewPos);

		LightInfo GetMainLightInfo();

		std::vector<std::unique_ptr<GameObject>> m_GameObjects;

		std::shared_ptr<VertexArray> m_GridVAO;
		std::shared_ptr<Shader> m_GridShader;

		bool m_IsDirty = false;
	};
}

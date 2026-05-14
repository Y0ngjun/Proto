#pragma once

#include <vector>
#include <memory>
#include <string>

#include "GameObject.h"

namespace Proto
{
	class EditorCamera;

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

		std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }
		GameObject* GetGameObjectByRuntimeID(uint32_t id);
		GameObject* GetGameObjectByUUID(UUID uuid);

		// Dirty Flag 관리
		void SetDirty(bool dirty) { m_IsDirty = dirty; }
		bool IsDirty() const { return m_IsDirty; }

	private:

		std::vector<std::unique_ptr<GameObject>> m_GameObjects;

		std::shared_ptr<class VertexArray> m_GridVAO;
		std::shared_ptr<class Shader> m_GridShader;

		bool m_IsDirty = false;
	};

}

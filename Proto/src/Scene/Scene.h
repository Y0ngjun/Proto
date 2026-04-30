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

		void OnUpdateRuntime(float deltaTime);
		void OnUpdateEditor(float deltaTime, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		void OnRuntimeStart();
		void OnRuntimeStop();

		std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }
		GameObject* GetGameObjectByID(uint32_t id);

		// 직렬화
		void SaveScene(const std::string& filepath);
		void LoadScene(const std::string& filepath);

	private:

		std::vector<std::unique_ptr<GameObject>> m_GameObjects;

		std::shared_ptr<class VertexArray> m_GridVAO;
		std::shared_ptr<class Shader> m_GridShader;
	};

}

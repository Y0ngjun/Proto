#pragma once

#include "GameObject.h"
#include <vector>
#include <memory>
#include <string>

namespace Proto {

	class Scene {
	public:
		Scene() = default;
		~Scene() = default;

		GameObject* CreateGameObject(const std::string& name = "GameObject");

		void OnUpdate(float deltaTime);

		std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }

	private:

		std::vector<std::unique_ptr<GameObject>> m_GameObjects;
	};

}
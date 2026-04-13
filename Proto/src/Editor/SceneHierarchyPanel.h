#pragma once

#include <memory>

#include "../Scene/Scene.h"

namespace Proto
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Scene* context);

		void SetContext(Scene* context);

		void OnImGuiRender();

		GameObject* GetSelectedGameObject() const { return m_SelectionContext; }
		void SetSelectedGameObject(GameObject* gameObject) { m_SelectionContext = gameObject; }

	private:
		void DrawEntityNode(GameObject* gameObject);

		Scene* m_Context = nullptr;
		GameObject* m_SelectionContext = nullptr;
	};
}
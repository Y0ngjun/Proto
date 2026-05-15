/*
 * 씬 내의 게임 오브젝트 계층 구조를 보여주는 에디터 패널 클래스입니다.
 * 오브젝트 선택, 생성, 삭제 및 이름 변경 기능을 제공합니다.
 */

#pragma once

#include <memory>
#include <string>

namespace Proto
{
	class Scene;
	class GameObject;

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Scene* context);

		void SetContext(Scene* context);
		void OnImGuiRender();

		GameObject* GetSelectedGameObject() const
		{
			return m_SelectionContext;
		}

		void SetSelectedGameObject(GameObject* gameObject)
		{
			m_SelectionContext = gameObject;
		}

	private:
		void DrawEntityNode(GameObject* gameObject);

		Scene* m_Context = nullptr;
		GameObject* m_SelectionContext = nullptr;
		GameObject* m_RenamingGameObject = nullptr;

		static constexpr size_t RENAME_BUFFER_SIZE = 256;
		char m_RenamingBuffer[RENAME_BUFFER_SIZE] = {};
	};
}

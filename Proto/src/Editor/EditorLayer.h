/*
 * 에디터의 메인 레이아웃 및 핵심 로직을 관리하는 클래스입니다.
 * 툴바, 뷰포트, 패널 배치(Docking) 및 기즈모 제어 등을 담당합니다.
 */

#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>

#include "../Renderer/Framebuffer.h"
#include "../Renderer/EditorCamera.h"

namespace Proto
{
	class Scene;
	class GameObject;
	class SceneHierarchyPanel;
	class InspectorPanel;
	class ConsolePanel;
	class ContentBrowserPanel;

	class EditorLayer
	{
	public:
		EditorLayer();
		~EditorLayer();

		void Init(void* nativeWindow);
		void Shutdown();

		void OnUpdate(float deltaTime);
		void OnImGuiRender(Scene*& activeScene, float deltaTime);

		void OnScenePlay();
		void OnSceneStop();

		int GetGizmoType() const
		{
			return m_GizmoType;
		}

		void SetGizmoType(int type)
		{
			m_GizmoType = type;
		}

		bool IsGameViewFocused() const
		{
			return m_IsGameViewFocused;
		}

		bool IsGameViewHovered() const
		{
			return m_IsGameViewHovered;
		}

		Framebuffer* GetEditorFramebuffer()
		{
			return m_EditorFramebuffer.get();
		}

		Framebuffer* GetGameFramebuffer()
		{
			return m_GameFramebuffer.get();
		}

		EditorCamera& GetEditorCamera()
		{
			return m_EditorCamera;
		}

	private:
		void SetDarkThemeColors();
		void RenderMenuBar();
		void RenderSceneViewportHeader();
		void HandleGizmos(GameObject* selectedEntity, const glm::vec2& viewportMin, const glm::vec2& viewportSize);

		void BeginFrame() const;
		void EndFrame() const;

		void RenderToolbar(Scene*& activeScene);
		void RenderDockSpace();
		void RenderSceneViewport(Scene* activeScene);
		void RenderGameViewport(Scene* activeScene);
		void HandleObjectPicking(Scene* activeScene, const glm::vec2* viewportBounds, const glm::vec2& viewportSize);

		std::unique_ptr<Framebuffer> m_EditorFramebuffer;
		std::unique_ptr<Framebuffer> m_GameFramebuffer;
		EditorCamera m_EditorCamera;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;
		bool m_IsGameViewFocused = false;
		bool m_IsGameViewHovered = false;

		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;
		std::unique_ptr<InspectorPanel> m_InspectorPanel;
		std::unique_ptr<ConsolePanel> m_ConsolePanel;
		std::unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;

		int m_GizmoType = 7; // ImGuizmo::OPERATION::TRANSLATE
	};
}

#pragma once

#include <functional>
#include <memory>
#include <string>
#include "Window.h"
#include "../Renderer/Framebuffer.h"
#include "../Renderer/EditorCamera.h"

namespace Proto
{
	class SceneHierarchyPanel;
	class InspectorPanel;

	class Application
	{
	public:
		static Application& Get();

		void Run();
		float GetDeltaTime() const;

		void SetScene(class Scene* scene);
		void SetUpdateCallback(const std::function<void(float)>& callback) { m_UpdateCallback = callback; }

		void SaveScene(const std::string& filepath);
		void LoadScene(const std::string& filepath);

		int GetGizmoType() const { return m_GizmoType; }
		void SetGizmoType(int type) { m_GizmoType = type; }

	private:
		Application();
		~Application();

		void Init();
		void Shutdown();
		void Update();
		void Render();
		void UpdateFrameTiming();
		void ProcessInput();
		void BeginImGuiFrame() const;
		void EndImGuiFrame() const;
		void HandleObjectPicking(const glm::vec2* viewportBounds, const glm::vec2& viewportSize);

		void OnScenePlay();
		void OnSceneStop();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Window m_Window;
		float m_DeltaTime;
		float m_LastFrameTime;
		bool m_IsInitialized;

		std::unique_ptr<Framebuffer> m_EditorFramebuffer;
		std::unique_ptr<Framebuffer> m_GameFramebuffer;
		EditorCamera m_EditorCamera;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;

		class Scene* m_Scene = nullptr;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;
		std::unique_ptr<InspectorPanel> m_InspectorPanel;

		int m_GizmoType = 7; // ImGuizmo::OPERATION::TRANSLATE (기본값 이동)

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;
		std::string m_SceneBackupString;

		std::function<void(float)> m_UpdateCallback;
	};
}

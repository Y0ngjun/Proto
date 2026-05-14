#pragma once

#include <functional>
#include <memory>
#include <string>
#include <filesystem>
#include "Window.h"
#include "../Renderer/Framebuffer.h"
#include "../Renderer/EditorCamera.h"
#include "Project.h"

namespace Proto
{
	class SceneHierarchyPanel;
	class InspectorPanel;
	class ConsolePanel;
	class ContentBrowserPanel;

	class Application
	{
	public:
		static Application& Get();

		void Run();
		float GetDeltaTime() const;

		void SetScene(class Scene* scene);
		void SetUpdateCallback(const std::function<void(float)>& callback) { m_UpdateCallback = callback; }

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void NewProject();
		void OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		int GetGizmoType() const { return m_GizmoType; }
		void SetGizmoType(int type) { m_GizmoType = type; }

		bool IsGameViewFocused() const { return m_IsGameViewFocused; }
		bool IsGameViewHovered() const { return m_IsGameViewHovered; }

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

		// 저장 경고 및 유효성 검사
		bool SaveModifiedPrompt(const std::string& itemName);

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Window m_Window;
		float m_DeltaTime;
		float m_LastFrameTime;
		bool m_IsInitialized;
		std::filesystem::path m_ActiveProjectPath;  // 현재 열린 프로젝트 경로
		std::filesystem::path m_ActiveScenePath;     // 현재 열린 씬 경로

		std::unique_ptr<Framebuffer> m_EditorFramebuffer;
		std::unique_ptr<Framebuffer> m_GameFramebuffer;
		EditorCamera m_EditorCamera;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;
		bool m_IsGameViewFocused = false;
		bool m_IsGameViewHovered = false;

		class Scene* m_Scene = nullptr;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;
		std::unique_ptr<InspectorPanel> m_InspectorPanel;
		std::unique_ptr<ConsolePanel> m_ConsolePanel;
		std::unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;

		int m_GizmoType = 7; // ImGuizmo::OPERATION::TRANSLATE (기본값 이동)

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;
		std::string m_SceneBackupString;

		// 저장 경고 상태 관리
		enum class SavePromptState
		{
			None = 0, Pending = 1, Confirmed = 2, Cancelled = 3, DontSave = 4
		};
		SavePromptState m_SavePromptState = SavePromptState::None;

		bool m_ShouldClose = false;
		std::function<void(float)> m_UpdateCallback;
	};
}

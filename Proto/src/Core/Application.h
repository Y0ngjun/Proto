#pragma once

#include <functional>
#include <memory>
#include <string>
#include <filesystem>
#include "Window.h"
#include "Project.h"

namespace Proto
{
	class EditorLayer;
	class Scene;

	class Application
	{
	public:
		static Application& Get();

		void Run();
		float GetDeltaTime() const;

		void SetScene(Scene* scene);
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

		bool IsPlaying() const { return m_SceneState == SceneState::Play; }
		bool IsGameViewFocused() const;
		void OnScenePlay();
		void OnSceneStop();

	private:
		Application();
		~Application();

		void Init();
		void Shutdown();
		void Update();
		void Render();
		void UpdateFrameTiming();
		void ProcessInput();

		bool SaveModifiedPrompt(const std::string& itemName);

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Window m_Window;
		float m_DeltaTime;
		float m_LastFrameTime;
		bool m_IsInitialized;
		std::filesystem::path m_ActiveProjectPath;
		std::filesystem::path m_ActiveScenePath;

		std::unique_ptr<EditorLayer> m_EditorLayer;
		Scene* m_Scene = nullptr;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;
		std::string m_SceneBackupString;

		bool m_ShouldClose = false;
		std::function<void(float)> m_UpdateCallback;
	};
}

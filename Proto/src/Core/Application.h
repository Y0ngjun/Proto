#pragma once

#include "Window.h"
#include <functional>
#include <memory>
#include "../Renderer/Framebuffer.h"

#include "../Renderer/EditorCamera.h"

namespace Proto
{
	class SceneHierarchyPanel;

	class Application
	{
	public:
		static Application& Get();

		void Run();
		float GetDeltaTime() const;

		void SetScene(class Scene* scene);
		void SetUpdateCallback(const std::function<void(float)>& callback) { m_UpdateCallback = callback; }

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

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		Window m_Window;
		float m_DeltaTime;
		float m_LastFrameTime;
		bool m_IsInitialized;

		std::unique_ptr<Framebuffer> m_Framebuffer;
		std::unique_ptr<Framebuffer> m_GameFramebuffer;
		EditorCamera m_EditorCamera;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;

		class Scene* m_Scene = nullptr;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;

		std::function<void(float)> m_UpdateCallback;
	};
}

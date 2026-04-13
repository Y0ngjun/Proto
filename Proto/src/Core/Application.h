#pragma once

#include <functional>
#include <memory>

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

		std::unique_ptr<Framebuffer> m_EditorFramebuffer;
		std::unique_ptr<Framebuffer> m_GameFramebuffer;
		EditorCamera m_EditorCamera;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;

		class Scene* m_Scene = nullptr;
		std::unique_ptr<SceneHierarchyPanel> m_SceneHierarchyPanel;
		std::unique_ptr<InspectorPanel> m_InspectorPanel;

		std::function<void(float)> m_UpdateCallback;
	};
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.h"
#include "Input.h"

#include "../Scene/Scene.h"
#include "../Editor/SceneHierarchyPanel.h"
#include "../Editor/InspectorPanel.h"

namespace Proto
{
	Application& Application::Get()
	{
		static Application instance;
		return instance;
	}

	void Application::Run()
	{
		while (!m_Window.ShouldClose())
		{
			m_Window.PollEvents();

			Update();
			Render();

			m_Window.SwapBuffers();
		}
	}

	float Application::GetDeltaTime() const
	{
		return m_DeltaTime;
	}

	void Application::SetScene(Scene* scene)
	{
		m_Scene = scene;

		if (m_SceneHierarchyPanel)
		{
			m_SceneHierarchyPanel->SetContext(m_Scene);
		}

		if (m_Scene && m_EditorFramebuffer)
		{
			m_Scene->OnViewportResize(m_EditorFramebuffer->GetSpecification().Width, m_EditorFramebuffer->GetSpecification().Height);
		}
	}

	Application::Application()
		: m_Window(1920, 1080, "Proto Engine Test"),
		m_DeltaTime(0.0f),
		m_LastFrameTime(0.0f),
		m_IsInitialized(false)
	{
		Init();
	}

	Application::~Application()
	{
		Shutdown();
	}

	void Application::Init()
	{
		if (m_IsInitialized)
		{
			return;
		}

		Input::Initialize(m_Window.GetNativeWindow());

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// 폰트 스케일
		io.FontGlobalScale = 1.1f;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_Window.GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();
		m_InspectorPanel = std::make_unique<InspectorPanel>();

		FramebufferSpecification fbSpec;
		fbSpec.Width = m_Window.GetWidth();
		fbSpec.Height = m_Window.GetHeight();
		m_EditorFramebuffer = std::make_unique<Framebuffer>(fbSpec);
		m_GameFramebuffer = std::make_unique<Framebuffer>(fbSpec);

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);

		m_LastFrameTime = static_cast<float>(glfwGetTime());
		m_IsInitialized = true;
	}

	void Application::Shutdown()
	{
		if (!m_IsInitialized)
		{
			return;
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		if (m_Scene)
		{
			delete m_Scene;
			m_Scene = nullptr;
		}

		m_IsInitialized = false;
	}

	void Application::Update()
	{
		UpdateFrameTiming();
		ProcessInput();

		if (m_UpdateCallback)
		{
			m_UpdateCallback(m_DeltaTime);
		}
	}

	void Application::Render()
	{
		if (m_IsViewportFocused)
		{
			m_EditorCamera.OnUpdate(m_DeltaTime);
		}

		// 1. Render Editor Scene
		m_EditorFramebuffer->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (m_Scene)
		{
			m_Scene->OnUpdateEditor(m_DeltaTime, m_EditorCamera);
		}
		m_EditorFramebuffer->Unbind();

		// 2. Render Game Scene
		m_GameFramebuffer->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (m_Scene)
		{
			m_Scene->OnUpdateRuntime(m_DeltaTime);
		}
		m_GameFramebuffer->Unbind();

		// 3. Render UI
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		BeginImGuiFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		ImGui::End();

		// --- Scene View Panel ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene View");
		m_IsViewportFocused = ImGui::IsWindowFocused();
		m_IsViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_EditorFramebuffer->GetSpecification().Width != viewportPanelSize.x || m_EditorFramebuffer->GetSpecification().Height != viewportPanelSize.y)
		{
			m_EditorFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
		}

		uint32_t textureID = m_EditorFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		// --- Game View Panel ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Game View");
		ImVec2 gameViewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_GameFramebuffer->GetSpecification().Width != gameViewportPanelSize.x || m_GameFramebuffer->GetSpecification().Height != gameViewportPanelSize.y)
		{
			m_GameFramebuffer->Resize((uint32_t)gameViewportPanelSize.x, (uint32_t)gameViewportPanelSize.y);

			if (m_Scene)
			{
				m_Scene->OnViewportResize((uint32_t)gameViewportPanelSize.x, (uint32_t)gameViewportPanelSize.y);
			}
		}

		uint32_t gameTextureID = m_GameFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)gameTextureID), ImVec2{ gameViewportPanelSize.x, gameViewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		if (m_SceneHierarchyPanel)
		{
			m_SceneHierarchyPanel->OnImGuiRender();
		}

		if (m_InspectorPanel && m_SceneHierarchyPanel)
		{
			m_InspectorPanel->OnImGuiRender(m_SceneHierarchyPanel->GetSelectedGameObject());
		}

		EndImGuiFrame();
	}

	void Application::UpdateFrameTiming()
	{
		const float currentTime = static_cast<float>(glfwGetTime());
		m_DeltaTime = currentTime - m_LastFrameTime;
		m_LastFrameTime = currentTime;
	}

	void Application::ProcessInput()
	{
		if (Input::GetKey(GLFW_KEY_ESCAPE))
		{
			m_Window.SetShouldClose(true);
		}
	}

	void Application::BeginImGuiFrame() const
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Application::EndImGuiFrame() const
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

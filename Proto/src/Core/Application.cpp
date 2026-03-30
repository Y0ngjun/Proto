#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.h"
#include "Input.h"

#include "../Scene/Scene.h"
#include "../Editor/SceneHierarchyPanel.h"

namespace Proto

{
	Application& Application::Get()

	{
		static Application instance;
		return instance;
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

	void Application::SetScene(Scene* scene)
	{
		m_Scene = scene;
		if (m_SceneHierarchyPanel)
			m_SceneHierarchyPanel->SetContext(m_Scene);
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

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_Window.GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();

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
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Added Depth buffer clean
		glEnable(GL_DEPTH_TEST);

		if (m_Scene)
		{
			m_Scene->OnUpdate(m_DeltaTime);
		}

		BeginImGuiFrame();

		// Set docking workspace to a transparent state to fix obscured 3D view
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | 
										ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
										ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
										ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		// 1 << 0 is commonly ImGuiDockNodeFlags_PassthroughCentralNode in docking branch
		ImGuiDockNodeFlags dockspace_flags = 1 << 0; 
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		ImGui::End();
		ImGui::PopStyleColor();

		if (m_SceneHierarchyPanel)
			m_SceneHierarchyPanel->OnImGuiRender();

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

	float Application::GetDeltaTime() const
	{
		return m_DeltaTime;
	}
}

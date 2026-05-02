#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include "../Asset/AssetManager.h"
#include <glm/gtc/type_ptr.hpp>
#include "Application.h"
#include "RawInput.h"
#include "../Scene/Scene.h"
#include "../Editor/SceneHierarchyPanel.h"
#include "../Editor/InspectorPanel.h"
#include "../Scene/Components/Transform.h"
#include "FileDialog.h"
#include "../Scene/SceneSerializer.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Scene/Components/LightComponent.h"
#include "../Scene/GameObject.h"
#include "Log.h"
#include "Project.h"
#include "../Editor/ConsolePanel.h"
#include "../Editor/ContentBrowserPanel.h"

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
		m_UpdateCallback = nullptr;

		if (m_SceneHierarchyPanel)
		{
			m_SceneHierarchyPanel->SetContext(m_Scene);
		}

		if (m_Scene && m_EditorFramebuffer)
		{
			m_Scene->OnViewportResize(m_EditorFramebuffer->GetSpecification().Width, m_EditorFramebuffer->GetSpecification().Height);
		}
	}

	void Application::NewScene()
	{
		if (m_Scene) delete m_Scene;

		Scene* newScene = new Scene();
		newScene->CreateDefault(); // 공용 메서드 호출
		
		SetScene(newScene);

		m_ActiveScenePath = "";
		PROTO_LOG_INFO("Created new scene.");
	}

	void Application::OpenScene(const std::filesystem::path& path)
	{
		if (path.extension() != ".scene")
		{
			PROTO_LOG_ERROR("Failed to load scene: Invalid extension (expected .scene)");
			return;
		}

		Scene* newScene = new Scene();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			if (m_Scene) delete m_Scene;
			
			SetScene(newScene);
			m_ActiveScenePath = path;
			PROTO_LOG_INFO("Loaded scene: " + path.string());
		}
		else
		{
			delete newScene;
			PROTO_LOG_ERROR("Failed to load scene: " + path.string());
		}
	}

	void Application::SaveScene()
	{
		if (m_ActiveScenePath.empty())
		{
			auto path = FileDialog::SaveFile("Scene Files (*.scene)\0*.scene\0");
			if (path)
				SaveScene(*path);
		}
		else
		{
			SaveScene(m_ActiveScenePath);
		}
	}

	void Application::SaveScene(const std::filesystem::path& path)
	{
		if (m_Scene)
		{
			std::filesystem::path finalPath = path;
			if (finalPath.extension() != ".scene")
			{
				finalPath += ".scene";
			}

			SceneSerializer serializer(m_Scene);
			serializer.Serialize(finalPath.string());
			m_ActiveScenePath = finalPath;
			PROTO_LOG_INFO("Scene saved: " + finalPath.string());
		}
	}

	void Application::OpenProject(const std::filesystem::path& path)
	{
		if (path.extension() != ".proto")
		{
			PROTO_LOG_ERROR("Failed to load project: Invalid extension (expected .proto)");
			return;
		}

		if (Project::Load(path))
		{
			auto& config = Project::GetActive()->GetConfig();
			std::filesystem::path scenePath = config.ProjectDirectory / config.StartScene;
			OpenScene(scenePath);
		}
	}

	void Application::SaveProject()
	{
		// 프로젝트 저장 시 현재 씬도 자동으로 함께 저장
		SaveScene();

		auto activeProject = Project::GetActive();
		if (activeProject)
		{
			if (!m_ActiveScenePath.empty())
			{
				// 현재 씬이 프로젝트 폴더 내부에 있는지 확인
				auto& projectDir = activeProject->GetConfig().ProjectDirectory;
				std::string activePathStr = m_ActiveScenePath.string();
				std::string projectDirStr = projectDir.string();

				if (activePathStr.find(projectDirStr) != std::string::npos)
				{
					activeProject->GetConfig().StartScene = std::filesystem::relative(m_ActiveScenePath, projectDir);
				}
			}

			std::filesystem::path projectFilePath = activeProject->GetConfig().ProjectDirectory / activeProject->GetConfig().ProjectFileName;
			Project::SaveActive(projectFilePath);
			PROTO_LOG_INFO("Project saved successfully.");
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

		// 입력 시스템 초기화 (RawInput)
		RawInput::Initialize(m_Window.GetNativeWindow());

		AssetManager::Init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// 주 모니터의 DPI 배율 가져오기
		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);

		// 폰트 스케일에 배율 적용
		io.FontGlobalScale = 1.1f * xscale;

		ImGui::StyleColorsDark();

		// ImGui 스타일 전체 요소(패딩, 창 크기 등)에 배율 곱하기
		ImGui::GetStyle().ScaleAllSizes(xscale);

		ImGui_ImplGlfw_InitForOpenGL(m_Window.GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		m_SceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();
		m_InspectorPanel = std::make_unique<InspectorPanel>();
		m_ConsolePanel = std::make_unique<ConsolePanel>();
		m_ContentBrowserPanel = std::make_unique<ContentBrowserPanel>();

		PROTO_LOG_INFO("Engine Initialized Successfully.");

		FramebufferSpecification fbSpec;
		fbSpec.Width = m_Window.GetWidth();
		fbSpec.Height = m_Window.GetHeight();
		m_EditorFramebuffer = std::make_unique<Framebuffer>(fbSpec);
		m_GameFramebuffer = std::make_unique<Framebuffer>(fbSpec);

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
		m_EditorCamera.SetDistance(10.0f);
		m_EditorCamera.SetPitch(0.6f);

		m_LastFrameTime = static_cast<float>(glfwGetTime());
		m_IsInitialized = true;

		// 프로젝트의 시작 씬 로드
		auto activeProject = Project::GetActive();
		if (activeProject)
		{
			std::filesystem::path startScenePath = activeProject->GetConfig().ProjectDirectory / activeProject->GetConfig().StartScene;
			if (std::filesystem::exists(startScenePath))
			{
				OpenScene(startScenePath);
			}
		}
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
		RawInput::Update();
		UpdateFrameTiming();
		ProcessInput();

		if (m_SceneState == SceneState::Play)
		{
			if (m_UpdateCallback)
			{
				m_UpdateCallback(m_DeltaTime);
			}
		}
	}

	void Application::Render()
	{
		if (m_IsViewportFocused)
		{
			m_EditorCamera.OnUpdate(m_DeltaTime);
		}

		// Render Editor Scene
		m_EditorFramebuffer->Bind();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Unity 스타일 에디터 배경색
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_EditorFramebuffer->ClearAttachment(1, -1);
		glEnable(GL_DEPTH_TEST);

		if (m_Scene)
		{
			m_Scene->OnUpdateEditor(m_DeltaTime, m_EditorCamera);
		}
		m_EditorFramebuffer->Unbind();

		// Render Game Scene
		m_GameFramebuffer->Bind();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (m_Scene)
		{
			if (m_SceneState == SceneState::Play)
				m_Scene->OnUpdateRuntime(m_DeltaTime, m_IsGameViewFocused);
			else
				m_Scene->OnUpdateRuntime(0.0f, m_IsGameViewFocused); // 렌더링만 수행
		}
		m_GameFramebuffer->Unbind();

		// Render UI
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // UI 배경은 기존 유지 (다크 모드)
		glClear(GL_COLOR_BUFFER_BIT);

		BeginImGuiFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();

		// 모니터 스케일에 맞춘 Toolbar 높이 계산
		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
		float toolbarHeight = 36.0f * yscale;

		// --- Toolbar Panel (Fixed, Non-dockable) ---
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;

		bool isPlaying = m_SceneState == SceneState::Play;

		// 바(Toolbar)의 배경색도 상태에 따라 변경
		if (isPlaying)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.15f, 0.15f, 1.0f)); // 어두운 빨간색 배경
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.25f, 0.15f, 1.0f)); // 어두운 녹색 배경
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Toolbar", nullptr, toolbar_flags);
		ImGui::PopStyleColor(); // WindowBg 팝

		const char* buttonLabel = isPlaying ? "Stop" : "Play";

		float buttonWidth = 55.0f * xscale;
		float buttonHeight = 25.0f * yscale;

		float windowWidth = ImGui::GetWindowWidth();
		float windowHeight = ImGui::GetWindowHeight();
		float offsetX = (windowWidth - buttonWidth) * 0.5f;
		float offsetY = (windowHeight - buttonHeight) * 0.5f;
		if (offsetX < 0.0f) offsetX = 0.0f;
		if (offsetY < 0.0f) offsetY = 0.0f;

		ImGui::SetCursorPosX(offsetX);
		ImGui::SetCursorPosY(offsetY);

		if (isPlaying)
		{
			// Stop 상태 (빨간색)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
		}
		else
		{
			// Play 상태 (녹색)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		}

		if (ImGui::Button(buttonLabel, ImVec2(buttonWidth, buttonHeight)))
		{
			if (m_SceneState == SceneState::Edit) OnScenePlay();
			else if (m_SceneState == SceneState::Play) OnSceneStop();
		}

		ImGui::PopStyleColor(3);
		ImGui::End();
		ImGui::PopStyleVar(2);

		// --- DockSpace ---
		ImVec2 dockspacePos = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + toolbarHeight);
		ImVec2 dockspaceSize = ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - toolbarHeight);

		ImGui::SetNextWindowPos(dockspacePos);
		ImGui::SetNextWindowSize(dockspaceSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		static bool first_time = true;
		if (first_time)
		{
			first_time = false;
			
			// ini 파일에서 불러온 레이아웃이 없을 때만 기본 레이아웃 구성
			if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
			{
				ImGui::DockBuilderRemoveNode(dockspace_id);
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspace_id, dockspaceSize);

				auto dock_id_main = dockspace_id;

				auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.3f, nullptr, &dock_id_main);
				auto dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.3f, nullptr, &dock_id_main);
				auto dock_id_bottom_left = ImGui::DockBuilderSplitNode(dock_id_bottom, ImGuiDir_Left, 0.5f, nullptr, &dock_id_bottom);

				ImGui::DockBuilderDockWindow("Hierarchy", dock_id_right);
				ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
				ImGui::DockBuilderDockWindow("Scene", dock_id_main);
				ImGui::DockBuilderDockWindow("Game", dock_id_main);
				ImGui::DockBuilderDockWindow("Content Browser", dock_id_bottom_left);
				ImGui::DockBuilderDockWindow("Console", dock_id_bottom);

				ImGui::DockBuilderFinish(dockspace_id);
			}
		}

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		ImGui::End();

		// --- Main Menu Bar ---
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				{
					auto path = FileDialog::OpenFile("Proto Project (*.proto)\0*.proto\0");
					if (path) OpenProject(*path);
				}
				
				ImGui::Separator();
				
				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) SaveScene();
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{
					auto path = FileDialog::SaveFile("Scene Files (*.scene)\0*.scene\0");
					if (path) SaveScene(*path);
				}
				
				ImGui::Separator();
				
				if (ImGui::MenuItem("Save Project")) SaveProject();
				
				ImGui::Separator();
				
				if (ImGui::MenuItem("Exit", "Alt+F4")) m_Window.SetShouldClose(true);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// --- Scene Panel ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");
		m_IsViewportFocused = ImGui::IsWindowFocused();

		// --- Gizmo Bar (Inside Scene View) ---
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
		float barHeight = ImGui::GetFrameHeight() + 8.0f;
		ImGui::BeginChild("GizmoBar", ImVec2(0, barHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetCursorPos(ImVec2(8.0f, 4.0f));

		float gizmoButtonWidth = 120.0f;
		float gizmoSpacing = 4.0f;

		ImVec4 buttonColor(0.2f, 0.2f, 0.2f, 0.8f);
		ImVec4 buttonHoveredColor(0.3f, 0.3f, 0.3f, 0.9f);
		ImVec4 buttonActiveColor(0.15f, 0.15f, 0.15f, 0.9f);

		ImVec4 selectedButtonColor(0.4f, 0.4f, 0.4f, 0.9f);
		ImVec4 selectedButtonHoveredColor(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 selectedButtonActiveColor(0.35f, 0.35f, 0.35f, 1.0f);

		auto pushButtonColors = [&](bool isSelected) {
			if (isSelected) {
				ImGui::PushStyleColor(ImGuiCol_Button, selectedButtonColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selectedButtonHoveredColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, selectedButtonActiveColor);
			} else {
				ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
			}
		};

		pushButtonColors(m_GizmoType == -1);
		if (ImGui::Button("None(Q)", ImVec2(gizmoButtonWidth, 0))) m_GizmoType = -1;
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, gizmoSpacing);
		pushButtonColors(m_GizmoType == ImGuizmo::OPERATION::TRANSLATE);
		if (ImGui::Button("Move(W)", ImVec2(gizmoButtonWidth, 0))) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, gizmoSpacing);
		pushButtonColors(m_GizmoType == ImGuizmo::OPERATION::ROTATE);
		if (ImGui::Button("Rot(E)", ImVec2(gizmoButtonWidth, 0))) m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0, gizmoSpacing);
		pushButtonColors(m_GizmoType == ImGuizmo::OPERATION::SCALE);
		if (ImGui::Button("Scale(R)", ImVec2(gizmoButtonWidth, 0))) m_GizmoType = ImGuizmo::OPERATION::SCALE;
		ImGui::PopStyleColor(3);

		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_EditorFramebuffer->GetSpecification().Width != viewportPanelSize.x || m_EditorFramebuffer->GetSpecification().Height != viewportPanelSize.y)
		{
			m_EditorFramebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
		}

		uint32_t textureID = m_EditorFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		m_IsViewportHovered = ImGui::IsItemHovered();

		ImVec2 imageMin = ImGui::GetItemRectMin();
		ImVec2 imageMax = ImGui::GetItemRectMax();

		glm::vec2 viewportBoundsArray[2] = {
			{ imageMin.x, imageMin.y },
			{ imageMax.x, imageMax.y }
		};
		glm::vec2 viewportSize = { imageMax.x - imageMin.x, imageMax.y - imageMin.y };
		HandleObjectPicking(viewportBoundsArray, viewportSize);

		// ImGuizmo
		GameObject* selectedEntity = m_SceneHierarchyPanel ? m_SceneHierarchyPanel->GetSelectedGameObject() : nullptr;
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto transformComponent = selectedEntity->GetComponent<Transform>();
			if (transformComponent)
			{
				glm::mat4 transform = transformComponent->GetTransform();

				bool snap = RawInput::GetKey(GLFW_KEY_LEFT_CONTROL);
				float snapValue = 0.5f;
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					float translation[3], rotation[3], scale[3];
					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), translation, rotation, scale);

					transformComponent->Translation = glm::vec3(translation[0], translation[1], translation[2]);
					transformComponent->Rotation = glm::vec3(glm::radians(rotation[0]), glm::radians(rotation[1]), glm::radians(rotation[2]));
					transformComponent->Scale = glm::vec3(scale[0], scale[1], scale[2]);
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(2);

		// --- Game Panel ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Game");
		m_IsGameViewFocused = ImGui::IsWindowFocused();
		m_IsGameViewHovered = ImGui::IsWindowHovered();

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

		if (m_ConsolePanel)
		{
			m_ConsolePanel->OnImGuiRender();
		}

		if (m_ContentBrowserPanel)
		{
			m_ContentBrowserPanel->OnImGuiRender();
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
		bool control = RawInput::GetKey(GLFW_KEY_LEFT_CONTROL) || RawInput::GetKey(GLFW_KEY_RIGHT_CONTROL);
		bool shift = RawInput::GetKey(GLFW_KEY_LEFT_SHIFT) || RawInput::GetKey(GLFW_KEY_RIGHT_SHIFT);

		if (control && RawInput::GetKeyDown(GLFW_KEY_S))
		{
			if (shift)
			{
				auto path = FileDialog::SaveFile("Scene Files (*.scene)\0*.scene\0");
				if (path) SaveScene(*path);
			}
			else
			{
				SaveScene();
			}
		}

		if (control && RawInput::GetKeyDown(GLFW_KEY_N)) NewScene();
		if (control && RawInput::GetKeyDown(GLFW_KEY_O))
		{
			auto path = FileDialog::OpenFile("Proto Project (*.proto)\0*.proto\0");
			if (path) OpenProject(*path);
		}

		if (RawInput::GetKey(GLFW_KEY_ESCAPE))
		{
			m_Window.SetShouldClose(true);
		}

		if (!RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (RawInput::GetKey(GLFW_KEY_Q))
				m_GizmoType = -1;
			if (RawInput::GetKey(GLFW_KEY_W))
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			if (RawInput::GetKey(GLFW_KEY_E))
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			if (RawInput::GetKey(GLFW_KEY_R))
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}
	}

	void Application::BeginImGuiFrame() const
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void Application::EndImGuiFrame() const
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Application::HandleObjectPicking(const glm::vec2* viewportBounds, const glm::vec2& viewportSize)
	{
		if (!m_IsViewportHovered || !ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGuizmo::IsOver())
		{
			return;
		}

		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos.x -= viewportBounds[0].x;
		mousePos.y -= viewportBounds[0].y;

		mousePos.y = viewportSize.y - mousePos.y;

		int mouseX = (int)mousePos.x;
		int mouseY = (int)mousePos.y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			m_EditorFramebuffer->Bind();
			int pixelData = m_EditorFramebuffer->ReadPixel(1, mouseX, mouseY);
			m_EditorFramebuffer->Unbind();

			if (pixelData != -1 && m_Scene)
			{
				GameObject* pickedEntity = m_Scene->GetGameObjectByRuntimeID((uint32_t)pixelData);
				if (m_SceneHierarchyPanel) m_SceneHierarchyPanel->SetSelectedGameObject(pickedEntity);
			}
			else
			{
				if (m_SceneHierarchyPanel) m_SceneHierarchyPanel->SetSelectedGameObject(nullptr);
			}
		}
	}

	void Application::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		if (m_Scene)
		{
			SceneSerializer serializer(m_Scene);
			m_SceneBackupString = serializer.SerializeToString();
			m_Scene->OnRuntimeStart();
		}
	}

	void Application::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		if (m_Scene)
		{
			m_Scene->OnRuntimeStop();
		}

		if (!m_SceneBackupString.empty())
		{
			Scene* newScene = new Scene();
			SceneSerializer serializer(newScene);
			if (serializer.DeserializeFromString(m_SceneBackupString))
			{
				if (m_Scene) delete m_Scene;
				SetScene(newScene);
			}
			else
			{
				delete newScene;
			}
		}
	}
}

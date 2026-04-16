#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../../Dependencies/ImGuizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include "Application.h"
#include "Input.h"
#include "../Scene/Scene.h"
#include "../Editor/SceneHierarchyPanel.h"
#include "../Editor/InspectorPanel.h"
#include "../Scene/Components/Transform.h"

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

		// 1. 주 모니터의 DPI 배율(Scale) 가져오기
		float xscale, yscale;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);

		// 2. 폰트 스케일에 배율 적용
		io.FontGlobalScale = 1.1f * xscale;

		ImGui::StyleColorsDark();

		// 3. ImGui 스타일 전체 요소(패딩, 창 크기 등)에 배율 곱하기
		ImGui::GetStyle().ScaleAllSizes(xscale);

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

		// 1. Render Editor Scene
		m_EditorFramebuffer->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_EditorFramebuffer->ClearAttachment(1, -1);
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

		// --- Toolbar Panel ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool isPlaying = m_SceneState == SceneState::Play;
		const char* buttonLabel = isPlaying ? "Stop" : "Play";

		float buttonWidth = 80.0f;
		float buttonHeight = 32.0f;

		// 버튼을 중앙 정렬
		float availX = ImGui::GetContentRegionAvail().x;
		float availY = ImGui::GetContentRegionAvail().y;

		float offsetX = (availX - buttonWidth) * 0.5f;
		float offsetY = (availY - buttonHeight) * 0.5f;

		// 가용 공간이 버튼보다 작을 때 커서가 화면 밖(음수)으로 밀려 영구적으로 잘리는 현상 방지
		if (offsetX < 0.0f) offsetX = 0.0f;
		if (offsetY < 0.0f) offsetY = 0.0f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

		if (isPlaying)
		{
			// 어두운 회색
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
		}
		else
		{
			// 밝은 회색
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		}

		if (ImGui::Button(buttonLabel, ImVec2(buttonWidth, buttonHeight)))
		{
			if (m_SceneState == SceneState::Edit)
			{
				m_SceneState = SceneState::Play;
				if (m_Scene) m_Scene->OnRuntimeStart();
			}
			else if (m_SceneState == SceneState::Play)
			{
				m_SceneState = SceneState::Edit;
				if (m_Scene) m_Scene->OnRuntimeStop();
			}
		}

		ImGui::PopStyleColor(3);
		ImGui::End();
		ImGui::PopStyleVar(2);

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

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		ImVec2 viewportBounds[2] = {
			{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y },
			{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y }
		};

		uint32_t textureID = m_EditorFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>((uintptr_t)textureID), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (m_IsViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver())
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= viewportBounds[0].x;
			mousePos.y -= viewportBounds[0].y;
			ImVec2 viewportSize = { viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y };

			// Y축이 뒤집혀 있으므로 보정 (OpenGL 좌표계)
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
					GameObject* pickedEntity = m_Scene->GetGameObjectByID((uint32_t)pixelData);
					if (m_SceneHierarchyPanel) m_SceneHierarchyPanel->SetSelectedGameObject(pickedEntity);
				}
				else
				{
					if (m_SceneHierarchyPanel) m_SceneHierarchyPanel->SetSelectedGameObject(nullptr);
				}
			}
		}

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

				bool snap = Input::GetKey(GLFW_KEY_LEFT_CONTROL);
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

		if (!Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (Input::GetKey(GLFW_KEY_Q))
				m_GizmoType = -1;
			if (Input::GetKey(GLFW_KEY_W))
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			if (Input::GetKey(GLFW_KEY_E))
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			if (Input::GetKey(GLFW_KEY_R))
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
}

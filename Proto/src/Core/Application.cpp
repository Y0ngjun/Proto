#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Application.h"
#include "Log.h"
#include "Project.h"
#include "Input.h"
#include "RawInput.h"
#include "FileDialog.h"
#include "../Asset/BuiltinAssetLoader.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneSerializer.h"
#include "../Editor/EditorLayer.h"

namespace Proto
{
	Application& Application::Get()
	{
		static Application instance;
		return instance;
	}

	Application::Application()
		: m_Window(1920, 1080, "Proto Engine"),
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
		if (m_IsInitialized) return;

		RawInput::Initialize(m_Window.GetNativeWindow());
		BuiltinAssetLoader::Load();

		m_EditorLayer = std::make_unique<EditorLayer>();
		m_EditorLayer->Init(m_Window.GetNativeWindow());

		m_LastFrameTime = static_cast<float>(glfwGetTime());
		m_IsInitialized = true;

		auto activeProject = Project::GetActive();
		if (activeProject)
		{
			std::filesystem::path startScenePath = activeProject->GetConfig().ProjectDirectory / activeProject->GetConfig().StartScene;
			if (std::filesystem::exists(startScenePath))
				OpenScene(startScenePath);
		}

		PROTO_LOG_INFO("Engine Initialized Successfully.");
	}

	void Application::Shutdown()
	{
		if (!m_IsInitialized) return;

		m_EditorLayer->Shutdown();
		if (m_Scene) delete m_Scene;
		
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
		RawInput::Update();
		UpdateFrameTiming();
		ProcessInput();

		// Window Title
		std::string title = "Proto Engine - ";
		if (Project::GetActive())
		{
			title += Project::GetActive()->GetConfig().Name;
			if (Project::GetActive()->IsDirty()) title += " *";
		}
		else title += "No Project";

		if (!m_ActiveScenePath.empty())
		{
			title += " - " + m_ActiveScenePath.filename().string();
			if (m_Scene && m_Scene->IsDirty()) title += " *";
		}
		m_Window.SetTitle(title.c_str());

		m_EditorLayer->OnUpdate(m_DeltaTime);

		if (m_SceneState == SceneState::Play && m_UpdateCallback)
			m_UpdateCallback(m_DeltaTime);
	}

	void Application::Render()
	{
		// 1. Editor Viewport Rendering
		auto editorFB = m_EditorLayer->GetEditorFramebuffer();
		editorFB->Bind();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		editorFB->ClearAttachment(1, -1);
		glEnable(GL_DEPTH_TEST);
		if (m_Scene) m_Scene->OnUpdateEditor(m_DeltaTime, m_EditorLayer->GetEditorCamera());
		editorFB->Unbind();

		// 2. Game Viewport Rendering
		auto gameFB = m_EditorLayer->GetGameFramebuffer();
		gameFB->Bind();
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		if (m_Scene)
		{
			float dt = (m_SceneState == SceneState::Play) ? m_DeltaTime : 0.0f;
			Input::SetEnabled(m_EditorLayer->IsGameViewFocused());
			m_Scene->OnUpdateRuntime(dt, m_EditorLayer->IsGameViewFocused());
		}
		gameFB->Unbind();

		// 3. UI Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_EditorLayer->OnImGuiRender(m_Scene, m_DeltaTime);
	}

	void Application::UpdateFrameTiming()
	{
		float time = static_cast<float>(glfwGetTime());
		m_DeltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;
	}

	void Application::ProcessInput()
	{
		if (RawInput::GetKeyDown(GLFW_KEY_N) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL)) NewScene();
		if (RawInput::GetKeyDown(GLFW_KEY_O) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL)) OpenScene();
		if (RawInput::GetKeyDown(GLFW_KEY_S) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL)) SaveScene();
	}

	void Application::SetScene(Scene* scene)
	{
		m_Scene = scene;
		m_UpdateCallback = nullptr;
		// EditorLayer will update its context in OnImGuiRender
	}

	void Application::NewScene()
	{
		if (m_Scene && m_Scene->IsDirty() && !SaveModifiedPrompt("Current scene")) return;
		if (m_Scene) delete m_Scene;
		m_Scene = new Scene();
		m_Scene->CreateDefault();
		m_ActiveScenePath = "";
	}

	void Application::OpenScene()
	{
		auto path = FileDialog::OpenFile("Proto Scene (*.proto)\0*.proto\0");
		if (path) OpenScene(*path);
	}

	void Application::OpenScene(const std::filesystem::path& path)
	{
		if (m_Scene && m_Scene->IsDirty() && !SaveModifiedPrompt("Current scene")) return;
		Scene* newScene = new Scene();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			if (m_Scene) delete m_Scene;
			m_Scene = newScene;
			m_ActiveScenePath = path;
		}
		else delete newScene;
	}

	void Application::SaveScene()
	{
		if (m_ActiveScenePath.empty()) SaveSceneAs();
		else SaveScene(m_ActiveScenePath);
	}

	void Application::SaveScene(const std::filesystem::path& path)
	{
		SceneSerializer serializer(m_Scene);
		serializer.Serialize(path.string());
		m_ActiveScenePath = path;
		m_Scene->SetDirty(false);
	}

	void Application::SaveSceneAs()
	{
		auto path = FileDialog::SaveFile("Proto Scene (*.proto)\0*.proto\0");
		if (path) SaveScene(*path);
	}

	void Application::NewProject() { Project::New(); }
	void Application::OpenProject()
	{
		auto path = FileDialog::OpenFile("Proto Project (*.proto)\0*.proto\0");
		if (path) Project::Load(*path);
	}
	void Application::OpenProject(const std::filesystem::path& path) { Project::Load(path); }
	void Application::SaveProject() 
	{ 
		if (auto project = Project::GetActive())
			Project::SaveActive(project->GetConfig().ProjectDirectory / project->GetConfig().ProjectFileName);
	}

	bool Application::IsGameViewFocused() const
	{
		return m_EditorLayer->IsGameViewFocused();
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
		if (m_Scene) m_Scene->OnRuntimeStop();
		if (!m_SceneBackupString.empty())
		{
			Scene* newScene = new Scene();
			SceneSerializer serializer(newScene);
			if (serializer.DeserializeFromString(m_SceneBackupString))
			{
				if (m_Scene) delete m_Scene;
				m_Scene = newScene;
			}
			else delete newScene;
		}
	}

	bool Application::SaveModifiedPrompt(const std::string& itemName)
	{
		// For now, return true (auto-save or skip) to keep it simple, 
		// but ideally this would call back into EditorLayer for a modal.
		return true; 
	}

	float Application::GetDeltaTime() const { return m_DeltaTime; }
}

/*
 * 엔진의 메인 애플리케이션 클래스입니다.
 * 루프 제어, 윈도우 관리, 씬 로딩 및 에디터 레이어와의 연동을 담당합니다.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#undef DELETE
#undef ERROR
#undef far
#undef near
#endif

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
	namespace
	{
		static constexpr int DEFAULT_WINDOW_WIDTH = 1920;
		static constexpr int DEFAULT_WINDOW_HEIGHT = 1080;
		static constexpr const char* DEFAULT_WINDOW_TITLE = "Proto Engine";

		static constexpr float EDITOR_CLEAR_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static constexpr float UI_CLEAR_COLOR[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	}

	Application& Application::Get()
	{
		static Application instance;
		return instance;
	}

	Application::Application()
		: m_Window(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_TITLE),
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

		RawInput::Initialize(m_Window.GetNativeWindow());
		BuiltinAssetLoader::Load();

		m_EditorLayer = std::make_unique<EditorLayer>();
		m_EditorLayer->Init(m_Window.GetNativeWindow());

		m_LastFrameTime = static_cast<float>(glfwGetTime());
		m_IsInitialized = true;

		const auto activeProject = Project::GetActive();
		if (activeProject)
		{
			const std::filesystem::path startScenePath = activeProject->GetConfig().ProjectDirectory / activeProject->GetConfig().StartScene;
			if (std::filesystem::exists(startScenePath))
			{
				OpenScene(startScenePath);
			}
		}

		PROTO_LOG_INFO("엔진 초기화 성공.");
	}

	void Application::Shutdown()
	{
		if (!m_IsInitialized)
		{
			return;
		}

		m_EditorLayer->Shutdown();
		if (m_Scene)
		{
			delete m_Scene;
		}

		m_IsInitialized = false;
	}

	void Application::Run()
	{
		while (true)
		{
			m_Window.PollEvents();

			if (m_Window.ShouldClose())
			{
				if (SaveModifiedPrompt("프로그램"))
				{
					break;
				}
				else
				{
					m_Window.SetShouldClose(false);
				}
			}

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

		std::string title = std::string(DEFAULT_WINDOW_TITLE) + " - ";
		if (Project::GetActive())
		{
			title += Project::GetActive()->GetConfig().Name;
			if (Project::GetActive()->IsDirty())
			{
				title += " *";
			}
		}
		else
		{
			title += "No Project";
		}

		if (!m_ActiveScenePath.empty())
		{
			title += " - " + m_ActiveScenePath.filename().string();
			if (m_Scene && m_Scene->IsDirty())
			{
				title += " *";
			}
		}
		m_Window.SetTitle(title.c_str());

		m_EditorLayer->OnUpdate(m_DeltaTime);

		if (m_SceneState == SceneState::Play && m_UpdateCallback)
		{
			m_UpdateCallback(m_DeltaTime);
		}
	}

	void Application::Render()
	{
		// 1. 에디터 뷰포트 렌더링
		const auto editorFB = m_EditorLayer->GetEditorFramebuffer();
		editorFB->Bind();
		glClearColor(EDITOR_CLEAR_COLOR[0], EDITOR_CLEAR_COLOR[1], EDITOR_CLEAR_COLOR[2], EDITOR_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		editorFB->ClearAttachment(1, -1);
		glEnable(GL_DEPTH_TEST);
		if (m_Scene)
		{
			m_Scene->OnUpdateEditor(m_DeltaTime, m_EditorLayer->GetEditorCamera());
		}
		editorFB->Unbind();

		// 2. 게임 뷰포트 렌더링
		const auto gameFB = m_EditorLayer->GetGameFramebuffer();
		gameFB->Bind();
		glClearColor(EDITOR_CLEAR_COLOR[0], EDITOR_CLEAR_COLOR[1], EDITOR_CLEAR_COLOR[2], EDITOR_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		if (m_Scene)
		{
			const float dt = (m_SceneState == SceneState::Play) ? m_DeltaTime : 0.0f;
			Input::SetEnabled(m_EditorLayer->IsGameViewFocused());
			m_Scene->OnUpdateRuntime(dt, m_EditorLayer->IsGameViewFocused());
		}
		gameFB->Unbind();

		// 3. UI 렌더링
		glClearColor(UI_CLEAR_COLOR[0], UI_CLEAR_COLOR[1], UI_CLEAR_COLOR[2], UI_CLEAR_COLOR[3]);
		glClear(GL_COLOR_BUFFER_BIT);
		m_EditorLayer->OnImGuiRender(m_Scene, m_DeltaTime);
	}

	void Application::UpdateFrameTiming()
	{
		const float time = static_cast<float>(glfwGetTime());
		m_DeltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;
	}

	void Application::ProcessInput()
	{
		if (RawInput::GetKeyDown(GLFW_KEY_N) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL))
		{
			NewScene();
		}

		if (RawInput::GetKeyDown(GLFW_KEY_O) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL))
		{
			OpenScene();
		}

		if (RawInput::GetKeyDown(GLFW_KEY_S) && RawInput::GetKey(GLFW_KEY_LEFT_CONTROL))
		{
			SaveScene();
		}
	}

	void Application::SetScene(Scene* scene)
	{
		m_Scene = scene;
		m_UpdateCallback = nullptr;

		if (m_EditorLayer && m_Scene)
		{
			const auto gameSpec = m_EditorLayer->GetGameFramebuffer()->GetSpecification();
			m_Scene->OnViewportResize(gameSpec.Width, gameSpec.Height);
		}
	}

	void Application::NewScene()
	{
		if (m_Scene && m_Scene->IsDirty() && !SaveModifiedPrompt("현재 씬"))
		{
			return;
		}

		const auto path = FileDialog::SaveFile("Proto Scene (*.scene)\0*.scene\0");
		if (!path)
		{
			PROTO_LOG_INFO("새 씬 생성이 취소되었습니다.");
			return;
		}

		if (m_Scene)
		{
			delete m_Scene;
		}

		m_Scene = new Scene();
		m_Scene->CreateDefault();

		if (m_EditorLayer)
		{
			const auto gameSpec = m_EditorLayer->GetGameFramebuffer()->GetSpecification();
			m_Scene->OnViewportResize(gameSpec.Width, gameSpec.Height);
		}

		SaveScene(*path);
		PROTO_LOG_INFO("새 씬 생성 및 저장 완료: " + m_ActiveScenePath.string());
	}

	void Application::OpenScene()
	{
		const auto path = FileDialog::OpenFile("Proto Scene (*.scene)\0*.scene\0");
		if (path)
		{
			OpenScene(*path);
		}
		else
		{
			PROTO_LOG_INFO("씬 열기가 취소되었습니다.");
		}
	}

	void Application::OpenScene(const std::filesystem::path& path)
	{
		if (m_Scene && m_Scene->IsDirty() && !SaveModifiedPrompt("현재 씬"))
		{
			return;
		}

		Scene* newScene = new Scene();
		SceneSerializer serializer(newScene);
		if (!serializer.Deserialize(path.string()))
		{
			delete newScene;
			return;
		}

		if (m_Scene)
		{
			delete m_Scene;
		}

		m_Scene = newScene;
		m_ActiveScenePath = path;
		m_Scene->SetDirty(false); // 로드 직후 플래그 초기화

		if (m_EditorLayer)
		{
			const auto gameSpec = m_EditorLayer->GetGameFramebuffer()->GetSpecification();
			m_Scene->OnViewportResize(gameSpec.Width, gameSpec.Height);
		}

		PROTO_LOG_INFO("씬 로드 완료: " + path.string());
	}

	void Application::SaveScene()
	{
		if (m_ActiveScenePath.empty())
		{
			SaveSceneAs();
			return;
		}

		SaveScene(m_ActiveScenePath);
	}

	void Application::SaveScene(const std::filesystem::path& path)
	{
		std::filesystem::path scenePath = path;
		if (scenePath.extension() != ".scene")
		{
			scenePath += ".scene";
		}

		SceneSerializer serializer(m_Scene);
		serializer.Serialize(scenePath.string());
		m_ActiveScenePath = scenePath;
		m_Scene->SetDirty(false);

		PROTO_LOG_INFO("씬 저장 완료: " + scenePath.string());
	}

	void Application::SaveSceneAs()
	{
		const auto path = FileDialog::SaveFile("Proto Scene (*.scene)\0*.scene\0");
		if (path)
		{
			SaveScene(*path);
		}
	}

	void Application::NewProject()
	{
		if (!SaveModifiedPrompt("현재 프로젝트"))
		{
			return;
		}

		const auto path = FileDialog::SaveFile("Proto Project (*.proto)\0*.proto\0");
		if (path)
		{
			const auto project = Project::New(*path);
			if (project)
			{
				const std::filesystem::path startScenePath = project->GetConfig().ProjectDirectory / project->GetConfig().StartScene;
				OpenScene(startScenePath);
			}
		}
	}

	void Application::OpenProject()
	{
		if (!SaveModifiedPrompt("현재 프로젝트"))
		{
			return;
		}

		const auto path = FileDialog::OpenFile("Proto Project (*.proto)\0*.proto\0");
		if (path)
		{
			const auto project = Project::Load(*path);
			if (project)
			{
				const std::filesystem::path startScenePath = project->GetConfig().ProjectDirectory / project->GetConfig().StartScene;
				OpenScene(startScenePath);
			}
		}
		else
		{
			PROTO_LOG_INFO("프로젝트 열기가 취소되었습니다.");
		}
	}

	void Application::OpenProject(const std::filesystem::path& path)
	{
		Project::Load(path);
	}

	void Application::SaveProject()
	{
		const auto project = Project::GetActive();
		if (project)
		{
			// 현재 열려 있는 씬도 함께 저장
			SaveScene();

			Project::SaveActive(project->GetConfig().ProjectDirectory / project->GetConfig().ProjectFileName);
		}
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

		if (m_Scene)
		{
			m_Scene->OnRuntimeStop();
		}

		if (m_SceneBackupString.empty())
		{
			return;
		}

		Scene* newScene = new Scene();
		SceneSerializer serializer(newScene);
		if (serializer.DeserializeFromString(m_SceneBackupString))
		{
			if (m_Scene)
			{
				delete m_Scene;
			}
			m_Scene = newScene;
			PROTO_LOG_INFO("씬 복원 완료");
		}
		else
		{
			delete newScene;
		}
	}

	bool Application::SaveModifiedPrompt(const std::string& itemName)
	{
		bool isDirty = false;
		if (m_Scene && m_Scene->IsDirty()) isDirty = true;
		if (Project::GetActive() && Project::GetActive()->IsDirty()) isDirty = true;

		if (!isDirty)
		{
			return true;
		}

		std::string message = itemName + "의 변경 사항을 저장하시겠습니까?";
		std::string title = "저장 확인";

		// UTF-8 -> WideChar 변환
		int wMsgLen = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, NULL, 0);
		std::wstring wMessage(wMsgLen, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, &wMessage[0], wMsgLen);

		int wTitleLen = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, NULL, 0);
		std::wstring wTitle(wTitleLen, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, &wTitle[0], wTitleLen);

		int result = MessageBoxW(glfwGetWin32Window(m_Window.GetNativeWindow()), wMessage.c_str(), wTitle.c_str(), MB_YESNOCANCEL | MB_ICONQUESTION);

		if (result == IDYES)
		{
			SaveProject();
			return true;
		}
		else if (result == IDNO)
		{
			return true;
		}
		else // IDCANCEL
		{
			return false;
		}
	}

	float Application::GetDeltaTime() const
	{
		return m_DeltaTime;
	}
}

/*
 * 엔진 프로젝트 설정을 관리하는 클래스입니다.
 * 프로젝트 파일(.proto)의 로드, 저장 및 디렉토리 관리 기능을 담당합니다.
 */

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

#include "Project.h"
#include "Log.h"
#include "../Scene/Scene.h"
#include "../Scene/GameObject.h"
#include "../Scene/Components/Transform.h"
#include "../Scene/Components/CameraComponent.h"
#include "../Scene/Components/LightComponent.h"
#include "../Scene/SceneSerializer.h"

namespace Proto
{
	namespace
	{
		static constexpr const char* DEFAULT_PROJECT_NAME = "DefaultProject";
		static constexpr const char* DEFAULT_ASSET_DIR = "assets";
		static constexpr const char* DEFAULT_SCENE_DIR = "scenes";
		static constexpr const char* DEFAULT_START_SCENE = "scenes/DefaultScene.scene";
		static constexpr const char* PROJECT_EXTENSION = ".proto";
		static constexpr const char* SCENE_EXTENSION = ".scene";
	}

	std::shared_ptr<Project> Project::m_ActiveProject = nullptr;

	std::shared_ptr<Project> Project::New(const std::filesystem::path& path)
	{
		m_ActiveProject = std::make_shared<Project>();

		std::filesystem::path projectPath = path;
		if (projectPath.empty())
		{
			projectPath = std::filesystem::current_path() / (std::string(DEFAULT_PROJECT_NAME) + PROJECT_EXTENSION);
		}
		else if (projectPath.extension() != PROJECT_EXTENSION)
		{
			projectPath += PROJECT_EXTENSION;
		}

		const std::string projectName = projectPath.stem().string();
		const std::filesystem::path baseDir = projectPath.parent_path();
		const std::filesystem::path projectDir = baseDir / projectName;
		const std::filesystem::path finalProjectPath = projectDir / (projectName + PROJECT_EXTENSION);

		auto& config = m_ActiveProject->m_Config;
		config.Name = projectName;
		config.AssetDirectory = DEFAULT_ASSET_DIR;
		config.ProjectDirectory = projectDir;
		config.StartScene = DEFAULT_START_SCENE;

		if (!std::filesystem::exists(projectDir))
		{
			std::filesystem::create_directories(projectDir);
		}

		std::filesystem::create_directories(projectDir / DEFAULT_ASSET_DIR);
		std::filesystem::create_directories(projectDir / DEFAULT_SCENE_DIR);

		// 기본 씬 생성
		const std::filesystem::path fullScenePath = projectDir / config.StartScene;
		{
			Scene defaultScene;
			defaultScene.CreateDefault();
			SceneSerializer serializer(&defaultScene);
			serializer.Serialize(fullScenePath.string());
		}

		SaveActive(finalProjectPath);
		m_ActiveProject->SetDirty(false);

		PROTO_LOG_INFO("새 프로젝트 생성 완료: " + projectDir.string());
		return m_ActiveProject;
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			return nullptr;
		}

		try
		{
			const YAML::Node data = YAML::LoadFile(path.string());
			const YAML::Node configNode = data["Project"];
			if (!configNode)
			{
				return nullptr;
			}

			auto project = std::make_shared<Project>();
			auto& config = project->m_Config;

			config.Name = configNode["Name"].as<std::string>();
			config.AssetDirectory = configNode["AssetDirectory"].as<std::string>();

			if (configNode["StartScene"])
			{
				config.StartScene = configNode["StartScene"].as<std::string>();
			}

			config.ProjectDirectory = path.parent_path();
			config.ProjectFileName = path.filename();

			// 시작 씬 유효성 검사 및 폴백
			const bool isSceneValid = !config.StartScene.empty() &&
				std::filesystem::exists(config.ProjectDirectory / config.StartScene);

			if (!isSceneValid)
			{
				const std::filesystem::path scenesDir = config.ProjectDirectory / DEFAULT_SCENE_DIR;
				if (std::filesystem::exists(scenesDir))
				{
					for (const auto& entry : std::filesystem::directory_iterator(scenesDir))
					{
						if (entry.path().extension() == SCENE_EXTENSION)
						{
							config.StartScene = std::filesystem::relative(entry.path(), config.ProjectDirectory);
							PROTO_LOG_INFO("유효하지 않은 시작 씬을 대체했습니다: " + config.StartScene.string());
							break;
						}
					}
				}
			}

			m_ActiveProject = project;
			PROTO_LOG_INFO("프로젝트 로드 완료: " + config.Name);
			return m_ActiveProject;
		}
		catch (const std::exception& e)
		{
			PROTO_LOG_ERROR("프로젝트 로드 실패: " + path.string() + " 오류: " + std::string(e.what()));
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		if (!m_ActiveProject)
		{
			return false;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << m_ActiveProject->m_Config.Name;
		out << YAML::Key << "AssetDirectory" << YAML::Value << m_ActiveProject->m_Config.AssetDirectory.string();
		out << YAML::Key << "StartScene" << YAML::Value << m_ActiveProject->m_Config.StartScene.string();
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path.string());
		if (!fout.is_open())
		{
			return false;
		}

		fout << out.c_str();
		fout.close();

		m_ActiveProject->m_Config.ProjectFileName = path.filename();
		m_ActiveProject->m_Config.ProjectDirectory = path.parent_path();

		PROTO_LOG_INFO("프로젝트 저장 완료: " + path.string());
		m_ActiveProject->SetDirty(false); // 저장 완료 후 플래그 초기화
		return true;
	}
}

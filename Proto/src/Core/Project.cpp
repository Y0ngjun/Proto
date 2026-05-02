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
	std::shared_ptr<Project> Project::s_ActiveProject = nullptr;

	std::shared_ptr<Project> Project::New()
	{
		s_ActiveProject = std::make_shared<Project>();
		
		std::filesystem::path projectDir = std::filesystem::current_path() / "DefaultProject";
		std::string projectName = "Default Project";

		s_ActiveProject->m_Config.Name = projectName;
		s_ActiveProject->m_Config.AssetDirectory = "assets";
		s_ActiveProject->m_Config.ProjectDirectory = projectDir;
		s_ActiveProject->m_Config.StartScene = "scenes/DefaultScene.scene";
		
		if (!std::filesystem::exists(projectDir))
			std::filesystem::create_directories(projectDir);

		std::filesystem::create_directories(projectDir / "assets");
		std::filesystem::create_directories(projectDir / "scenes");

		// --- 기본 씬 파일 생성 (Scene 객체 및 직렬화 사용) ---
		std::string sceneFileName = s_ActiveProject->m_Config.StartScene.string();
		std::filesystem::path fullScenePath = projectDir / sceneFileName;

		{
			Scene defaultScene;
			defaultScene.CreateDefault();
			SceneSerializer serializer(&defaultScene);
			serializer.Serialize(fullScenePath.string());
		}

		std::string projectFileName = projectName + ".proto";
		std::filesystem::path fullProjectPath = projectDir / projectFileName;

		SaveActive(fullProjectPath);

		PROTO_LOG_INFO("Created new default project at: " + projectDir.string());
		return s_ActiveProject;
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		if (std::filesystem::exists(path))
		{
			try {
				YAML::Node data = YAML::LoadFile(path.string());
				YAML::Node config = data["Project"];
				if (config)
				{
					std::shared_ptr<Project> project = std::make_shared<Project>();
					project->m_Config.Name = config["Name"].as<std::string>();
					project->m_Config.AssetDirectory = config["AssetDirectory"].as<std::string>();
					
					if (config["StartScene"])
						project->m_Config.StartScene = config["StartScene"].as<std::string>();

					project->m_Config.ProjectDirectory = path.parent_path();
					project->m_Config.ProjectFileName = path.filename();

					bool isSceneValid = !project->m_Config.StartScene.empty() && 
										std::filesystem::exists(project->m_Config.ProjectDirectory / project->m_Config.StartScene);

					if (!isSceneValid)
					{
						std::filesystem::path scenesDir = project->m_Config.ProjectDirectory / "scenes";
						if (std::filesystem::exists(scenesDir) && std::filesystem::is_directory(scenesDir))
						{
							for (auto& entry : std::filesystem::directory_iterator(scenesDir))
							{
								if (entry.path().extension() == ".scene")
								{
									project->m_Config.StartScene = std::filesystem::relative(entry.path(), project->m_Config.ProjectDirectory);
									PROTO_LOG_INFO("Fallback scene assigned: " + project->m_Config.StartScene.string());
									break;
								}
							}
						}
					}

					s_ActiveProject = project;
					PROTO_LOG_INFO("Loaded project: " + project->m_Config.Name);
					return s_ActiveProject;
				}
			}
			catch (const std::exception& e) {
				PROTO_LOG_ERROR("Failed to load project: " + path.string() + " Error: " + std::string(e.what()));
			}
		}
		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		if (!s_ActiveProject) return false;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << s_ActiveProject->m_Config.Name;
		out << YAML::Key << "AssetDirectory" << YAML::Value << s_ActiveProject->m_Config.AssetDirectory.string();
		out << YAML::Key << "StartScene" << YAML::Value << s_ActiveProject->m_Config.StartScene.string();
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path.string());
		if (fout.is_open())
		{
			fout << out.c_str();
			fout.close();

			s_ActiveProject->m_Config.ProjectFileName = path.filename();
			s_ActiveProject->m_Config.ProjectDirectory = path.parent_path();

			PROTO_LOG_INFO("Project saved to: " + path.string());
			return true;
		}
		return false;
	}
}

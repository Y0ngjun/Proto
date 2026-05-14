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

		const std::string projectName = "DefaultProject";
		const std::filesystem::path projectDir = std::filesystem::current_path() / projectName;

		auto& config = s_ActiveProject->m_Config;
		config.Name = projectName;
		config.AssetDirectory = "assets";
		config.ProjectDirectory = projectDir;
		config.StartScene = "scenes/DefaultScene.scene";
		
		if (!std::filesystem::exists(projectDir))
			std::filesystem::create_directories(projectDir);

		std::filesystem::create_directories(projectDir / "assets");
		std::filesystem::create_directories(projectDir / "scenes");

		// Create default scene
		const std::filesystem::path fullScenePath = projectDir / config.StartScene;
		{
			Scene defaultScene;
			defaultScene.CreateDefault();
			SceneSerializer serializer(&defaultScene);
			serializer.Serialize(fullScenePath.string());
		}

		const std::filesystem::path fullProjectPath = projectDir / (projectName + ".proto");
		SaveActive(fullProjectPath);

		PROTO_LOG_INFO("Created new default project at: " + projectDir.string());
		return s_ActiveProject;
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
			return nullptr;

		try 
		{
			YAML::Node data = YAML::LoadFile(path.string());
			YAML::Node configNode = data["Project"];
			if (!configNode) return nullptr;

			auto project = std::make_shared<Project>();
			auto& config = project->m_Config;
			
			config.Name = configNode["Name"].as<std::string>();
			config.AssetDirectory = configNode["AssetDirectory"].as<std::string>();
			
			if (configNode["StartScene"])
				config.StartScene = configNode["StartScene"].as<std::string>();

			config.ProjectDirectory = path.parent_path();
			config.ProjectFileName = path.filename();

			// Fallback for missing or invalid start scene
			bool isSceneValid = !config.StartScene.empty() && 
								std::filesystem::exists(config.ProjectDirectory / config.StartScene);

			if (!isSceneValid)
			{
				std::filesystem::path scenesDir = config.ProjectDirectory / "scenes";
				if (std::filesystem::exists(scenesDir))
				{
					for (auto& entry : std::filesystem::directory_iterator(scenesDir))
					{
						if (entry.path().extension() == ".scene")
						{
							config.StartScene = std::filesystem::relative(entry.path(), config.ProjectDirectory);
							PROTO_LOG_INFO("Fallback scene assigned: " + config.StartScene.string());
							break;
						}
					}
				}
			}

			s_ActiveProject = project;
			PROTO_LOG_INFO("Loaded project: " + config.Name);
			return s_ActiveProject;
		}
		catch (const std::exception& e) 
		{
			PROTO_LOG_ERROR("Failed to load project: " + path.string() + " Error: " + std::string(e.what()));
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
		if (!fout.is_open()) return false;

		fout << out.c_str();
		fout.close();

		s_ActiveProject->m_Config.ProjectFileName = path.filename();
		s_ActiveProject->m_Config.ProjectDirectory = path.parent_path();

		PROTO_LOG_INFO("Project saved to: " + path.string());
		return true;
	}
}

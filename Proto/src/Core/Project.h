#pragma once

#include <string>
#include <filesystem>
#include <memory>

namespace Proto {

	struct ProjectConfig
	{
		std::string Name = "Untitled";
		std::filesystem::path StartScene;
		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;

		std::filesystem::path ProjectFileName;
		std::filesystem::path ProjectDirectory;
	};

	class Project
	{
	public:
		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

		static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }
		
		ProjectConfig& GetConfig() { return m_Config; }

		static std::filesystem::path GetProjectDirectory()
		{
			if (!s_ActiveProject) return "";
			return s_ActiveProject->m_Config.ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			if (!s_ActiveProject) return "";
			return s_ActiveProject->m_Config.ProjectDirectory / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetEngineResourceDirectory()
		{
			std::filesystem::path resourcePath = std::filesystem::current_path() / "Resources";
			return resourcePath;
		}

	private:
		ProjectConfig m_Config;
		static std::shared_ptr<Project> s_ActiveProject;
	};

}

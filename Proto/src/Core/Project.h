/*
 * 엔진 프로젝트 설정을 관리하는 클래스입니다.
 * 프로젝트 파일(.proto)의 로드, 저장 및 디렉토리 관리 기능을 담당합니다.
 */

#pragma once

#include <string>
#include <filesystem>
#include <memory>

namespace Proto
{
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
		static std::shared_ptr<Project> New(const std::filesystem::path& path = "");
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

		static std::shared_ptr<Project> GetActive()
		{
			return m_ActiveProject;
		}

		ProjectConfig& GetConfig()
		{
			return m_Config;
		}

		static std::filesystem::path GetProjectDirectory()
		{
			if (!m_ActiveProject)
			{
				return "";
			}

			return m_ActiveProject->m_Config.ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			if (!m_ActiveProject)
			{
				return "";
			}

			return m_ActiveProject->m_Config.ProjectDirectory / m_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetEngineResourceDirectory()
		{
			static const std::filesystem::path RESOURCE_PATH = std::filesystem::current_path() / "Resources";
			return RESOURCE_PATH;
		}

		void SetDirty(bool dirty)
		{
			m_IsDirty = dirty;
		}

		bool IsDirty() const
		{
			return m_IsDirty;
		}

	private:
		ProjectConfig m_Config;
		bool m_IsDirty = false;

		static std::shared_ptr<Project> m_ActiveProject;
	};
}

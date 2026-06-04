/*
 * 프로젝트 자산을 탐색하고 관리하는 에디터 콘텐츠 브라우저 패널 클래스입니다.
 * 디렉토리 탐색, 파일 실행(프로젝트/씬 로드) 기능을 제공합니다.
 */

#pragma once

#include <filesystem>

namespace Proto
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		~ContentBrowserPanel() = default;

		void OnImGuiRender();

	private:
		void DrawFolderTree(const std::filesystem::path& dir, const std::filesystem::path& base);
		void CreateAsset(const char* type);

		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_RenamingPath;

		static constexpr int RENAME_BUFFER_SIZE = 256;
		char m_RenameBuffer[RENAME_BUFFER_SIZE] = {};
	};
}

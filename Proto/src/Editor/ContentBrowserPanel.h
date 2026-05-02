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
		std::filesystem::path m_CurrentDirectory;
	};
}

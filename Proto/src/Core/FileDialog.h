/*
 * 운영체제별 파일 탐색기 다이얼로그(열기, 저장) 기능을 제공하는 클래스입니다.
 * 현재 Windows 플랫폼의 네이티브 다이얼로그를 지원합니다.
 */

#pragma once

#include <string>
#include <optional>

namespace Proto
{
	class FileDialog
	{
	public:
		static std::optional<std::string> OpenFile(const char* filter = "YAML Files (*.yaml)\0*.yaml\0All Files (*.*)\0*.*\0");
		static std::optional<std::string> SaveFile(const char* filter = "YAML Files (*.yaml)\0*.yaml\0All Files (*.*)\0*.*\0");
	};
}

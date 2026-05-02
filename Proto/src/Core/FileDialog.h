#pragma once

#include <string>
#include <optional>

namespace Proto
{
    class FileDialog
    {
    public:
        // 파일 열기 다이얼로그
        static std::optional<std::string> OpenFile(const char* filter = "YAML Files (*.yaml)\0*.yaml\0All Files (*.*)\0*.*\0");

        // 파일 저장 다이얼로그
        static std::optional<std::string> SaveFile(const char* filter = "YAML Files (*.yaml)\0*.yaml\0All Files (*.*)\0*.*\0");
    };
}

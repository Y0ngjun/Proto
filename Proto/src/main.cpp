#include <exception>
#include <iostream>

#include "Core/Application.h"
#include "Core/Project.h"
#include "Core/Log.h"

int main(int argc, char** argv)
{
	// 1. 로그 시스템 우선 초기화
	Proto::Log::Init();

	try
	{
		if (argc > 1)
		{
			Proto::Project::Load(argv[1]);
		}
		else
		{
			// 기본 프로젝트 경로 확인
			std::filesystem::path defaultProjectPath = std::filesystem::current_path() / "DefaultProject/DefaultProject.proto";
			PROTO_LOG_INFO("Checking for default project at: " + defaultProjectPath.string());

			if (std::filesystem::exists(defaultProjectPath))
			{
				PROTO_LOG_INFO("Default project found. Loading...");
				Proto::Project::Load(defaultProjectPath);
			}
			else
			{
				PROTO_LOG_INFO("Default project not found. Creating new one...");
				Proto::Project::New();
			}
		}

		auto& app = Proto::Application::Get();

		// Proto::Test::SetupCubeTest(app); // 큐브 테스트 제거

		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
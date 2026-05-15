/*
 * Proto 엔진 진입점
 * 프로젝트 초기화 및 애플리케이션 실행을 담당합니다.
 */

#include <exception>
#include <filesystem>
#include <cstdlib>

#include "Core/Application.h"
#include "Core/Project.h"
#include "Core/Log.h"

namespace
{
	static constexpr const char* DEFAULT_PROJECT_PATH = "DefaultProject/DefaultProject.proto";

	void InitializeProject(int argc, char** argv)
	{
		if (argc > 1)
		{
			Proto::Project::Load(argv[1]);
			return;
		}

		const std::filesystem::path defaultProjectPath = std::filesystem::current_path() / DEFAULT_PROJECT_PATH;
		PROTO_LOG_INFO("기본 프로젝트 확인 경로: " + defaultProjectPath.string());

		if (std::filesystem::exists(defaultProjectPath))
		{
			PROTO_LOG_INFO("기본 프로젝트를 발견했습니다. 로드 중...");
			Proto::Project::Load(defaultProjectPath);
			return;
		}

		PROTO_LOG_INFO("프로젝트를 찾을 수 없습니다. 새 프로젝트를 생성합니다...");
		Proto::Project::New();
	}
}

int main(int argc, char** argv)
{
	Proto::Log::Init();

	try
	{
		PROTO_LOG_INFO("Proto 엔진을 시작합니다...");

		InitializeProject(argc, argv);

		auto& app = Proto::Application::Get();
		app.Run();
	}
	catch (const std::exception& e)
	{
		PROTO_LOG_ERROR("치명적 오류 발생: " + std::string(e.what()));
		return EXIT_FAILURE;
	}

	PROTO_LOG_INFO("엔진이 성공적으로 종료되었습니다.");
	return EXIT_SUCCESS;
}
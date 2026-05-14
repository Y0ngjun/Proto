#include <exception>
#include <iostream>

#include "Core/Application.h"
#include "Core/Project.h"
#include "Core/Log.h"

namespace
{
	constexpr const char* DEFAULT_PROJECT_PATH = "DefaultProject/DefaultProject.proto";

	void InitializeProject(int argc, char** argv)
	{
		if (argc > 1)
		{
			Proto::Project::Load(argv[1]);
			return;
		}

		std::filesystem::path defaultProjectPath = std::filesystem::current_path() / DEFAULT_PROJECT_PATH;
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
}

int main(int argc, char** argv)
{
	Proto::Log::Init();

	try
	{
		InitializeProject(argc, argv);

		auto& app = Proto::Application::Get();
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal Error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
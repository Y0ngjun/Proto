#include <exception>
#include <iostream>

#include "Core/Application.h"
#include "Test/CubeTest.h"

int main()
{
	try
	{
		auto& app = Proto::Application::Get();

		Proto::Test::SetupCubeTest(app);

		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
#include "Log.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace Proto
{
	std::vector<LogMessage> Log::s_Messages;
	std::mutex Log::s_LogMutex;

	void Log::Init()
	{
		s_Messages.reserve(1000);
	}

	void Log::Info(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.push_back({ LogLevel::Info, message, GetCurrentTimestamp() });
		std::cout << "[" << GetCurrentTimestamp() << "] [INFO] " << message << std::endl;
	}

	void Log::Warn(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.push_back({ LogLevel::Warn, message, GetCurrentTimestamp() });
		std::cout << "\033[33m[" << GetCurrentTimestamp() << "] [WARN] " << message << "\033[0m" << std::endl;
	}

	void Log::Error(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.push_back({ LogLevel::Error, message, GetCurrentTimestamp() });
		std::cerr << "\033[31m[" << GetCurrentTimestamp() << "] [ERROR] " << message << "\033[0m" << std::endl;
	}

	void Log::Clear()
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.clear();
	}

	std::string Log::GetCurrentTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		struct tm time_info;
		localtime_s(&time_info, &in_time_t);

		std::stringstream ss;
		ss << std::put_time(&time_info, "%H:%M:%S");
		return ss.str();
	}
}

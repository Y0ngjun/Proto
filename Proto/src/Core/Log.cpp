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
		Submit(LogLevel::Info, message);
	}

	void Log::Warn(const std::string& message)
	{
		Submit(LogLevel::Warn, message);
	}

	void Log::Error(const std::string& message)
	{
		Submit(LogLevel::Error, message);
	}

	void Log::Clear()
	{
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.clear();
	}

	void Log::Submit(LogLevel level, const std::string& message)
	{
		std::string timestamp = GetCurrentTimestamp();
		
		std::lock_guard<std::mutex> lock(s_LogMutex);
		s_Messages.push_back({ level, message, timestamp });

		switch (level)
		{
		case LogLevel::Info:
			std::cout << "[" << timestamp << "] [INFO] " << message << std::endl;
			break;
		case LogLevel::Warn:
			std::cout << "\033[33m[" << timestamp << "] [WARN] " << message << "\033[0m" << std::endl;
			break;
		case LogLevel::Error:
			std::cerr << "\033[31m[" << timestamp << "] [ERROR] " << message << "\033[0m" << std::endl;
			break;
		}
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

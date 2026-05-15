/*
 * 엔진의 로깅 시스템을 담당하는 클래스입니다.
 * 로그 메시지의 저장, 콘솔 출력 및 시간 정보 기록을 관리합니다.
 */

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "Log.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Proto
{
	namespace
	{
		static constexpr size_t INITIAL_MESSAGE_RESERVE = 1000;

		// 콘솔 색상 코드 상수를 정의합니다.
		static constexpr const char* COLOR_YELLOW = "\033[33m";
		static constexpr const char* COLOR_RED = "\033[31m";
		static constexpr const char* COLOR_RESET = "\033[0m";
	}

	std::vector<LogMessage> Log::m_Messages;
	std::mutex Log::m_LogMutex;

	void Log::Init()
	{
#ifdef _WIN32
		// Windows 콘솔에서 UTF-8 한글 출력을 위해 코드 페이지를 65001로 설정합니다.
		SetConsoleOutputCP(65001);
#endif
		m_Messages.reserve(INITIAL_MESSAGE_RESERVE);
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
		const std::lock_guard<std::mutex> lock(m_LogMutex);
		m_Messages.clear();
	}

	void Log::Submit(LogLevel level, const std::string& message)
	{
		const std::string timestamp = GetCurrentTimestamp();

		{
			const std::lock_guard<std::mutex> lock(m_LogMutex);
			m_Messages.push_back({ level, message, timestamp });
		}

		switch (level)
		{
		case LogLevel::Info:
		{
			std::cout << "[" << timestamp << "] [INFO] " << message << std::endl;
			break;
		}

		case LogLevel::Warn:
		{
			std::cout << COLOR_YELLOW << "[" << timestamp << "] [WARN] " << message << COLOR_RESET << std::endl;
			break;
		}

		case LogLevel::Error:
		{
			std::cerr << COLOR_RED << "[" << timestamp << "] [ERROR] " << message << COLOR_RESET << std::endl;
			break;
		}
		}
	}

	std::string Log::GetCurrentTimestamp()
	{
		const auto now = std::chrono::system_clock::now();
		const auto inTimeT = std::chrono::system_clock::to_time_t(now);

		struct tm timeInfo;
		localtime_s(&timeInfo, &inTimeT);

		std::stringstream ss;
		ss << std::put_time(&timeInfo, "%H:%M:%S");
		return ss.str();
	}
}

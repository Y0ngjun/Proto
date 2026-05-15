/*
 * 엔진의 로깅 시스템을 담당하는 클래스입니다.
 * 로그 메시지의 저장, 콘솔 출력 및 시간 정보 기록을 관리합니다.
 */

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>

namespace Proto
{
	enum class LogLevel
	{
		Info = 0,
		Warn,
		Error
	};

	struct LogMessage
	{
		LogLevel Level;
		std::string Message;
		std::string Timestamp;
	};

	class Log
	{
	public:
		static void Init();

		static void Info(const std::string& message);
		static void Warn(const std::string& message);
		static void Error(const std::string& message);

		static void Clear();

		static const std::vector<LogMessage>& GetMessages()
		{
			return m_Messages;
		}

	private:
		static void Submit(LogLevel level, const std::string& message);
		static std::string GetCurrentTimestamp();

	private:
		static std::vector<LogMessage> m_Messages;
		static std::mutex m_LogMutex;
	};
}

#define PROTO_LOG_INFO(...)  ::Proto::Log::Info(__VA_ARGS__)
#define PROTO_LOG_WARN(...)  ::Proto::Log::Warn(__VA_ARGS__)
#define PROTO_LOG_ERROR(...) ::Proto::Log::Error(__VA_ARGS__)

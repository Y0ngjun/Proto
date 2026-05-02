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
		static const std::vector<LogMessage>& GetMessages() { return s_Messages; }

	private:
		static std::string GetCurrentTimestamp();

		static std::vector<LogMessage> s_Messages;
		static std::mutex s_LogMutex;
	};
}

#define PROTO_LOG_INFO(...)  ::Proto::Log::Info(__VA_ARGS__)
#define PROTO_LOG_WARN(...)  ::Proto::Log::Warn(__VA_ARGS__)
#define PROTO_LOG_ERROR(...) ::Proto::Log::Error(__VA_ARGS__)

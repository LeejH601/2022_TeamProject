#include "Logger.h"
#include "..\Object\Object.h"

CLogger::CLogger()
{
	m_sDirectoryPath = "Log/";
	m_sFilePath = "Log/logs.txt";
	out.open(m_sFilePath);

	std::string testText = "test";
	out.write(testText.c_str(), testText.length());

	ClearLogs();
}

CLogger::~CLogger()
{
}

bool CLogger::ClearLogs()
{
	out.close();

	for (const std::filesystem::directory_entry path : std::filesystem::directory_iterator(m_sDirectoryPath)) {
		out.open(path.path().filename().string().c_str(), 'w');
		out.close();
	}

	out.open(m_sFilePath);

	return true;
}

void CLogger::Log(std::string& message, LOG_LEVEL)
{
	out.write(message.c_str(), message.length());
}

void CLogger::LogCollision(CGameObject* Actor, CGameObject* Target, std::string& message, LOG_LEVEL)
{
	std::string LogMessage;
	const std::time_t t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock().now());

	std::string timestr; timestr.resize(50);
	std::tm tm_buf; localtime_s(&tm_buf, &t_c);
	std::strftime(timestr.data(), timestr.length(), "%A %c", &tm_buf);

	LogMessage = "Collision - Actor { " + std::string(Actor->m_pstrFrameName) + " } : Target { " +
		std::string(Target->m_pstrFrameName) + " } : Message { " + message + " } : Time { " + timestr + " }\n";
	out.write(LogMessage.c_str(), LogMessage.length());
}

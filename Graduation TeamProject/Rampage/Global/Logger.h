#pragma once
#include "stdafx.h"

enum class LOG_LEVEL {
	LOG_ERROR,
	LOG_DEBUG,
	COUNT
};

#define LOGGING

class CGameObject;
class CLogger
{
	std::string m_sDirectoryPath;
	std::string m_sFilePath;
	std::ofstream out;

public:
	DECLARE_SINGLE(CLogger);
	CLogger();
	~CLogger();

	bool ClearLogs();
	void Log(std::string& message, LOG_LEVEL = LOG_LEVEL::LOG_DEBUG);
	void LogCollision(CGameObject* Actor, CGameObject* Target, std::string& message, LOG_LEVEL = LOG_LEVEL::LOG_DEBUG);

	std::string GetNowTimeToStr();
};


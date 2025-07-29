#pragma once
#include <iostream>
#include <vector>

//enum  -- enumeration
enum LogType
{
	GAME_LOG_DEFAULT,
	GAME_LOG_INFO,
	GAME_LOG_WARNING,
	GAME_LOG_ERROR

};

struct LogEntry
{
	LogType Type;
	std::string Message;
	std::time_t TimeStamp;
};

class Log
{

public:

	static std::vector<LogEntry> LogEntries;
	static void LogMessage(LogType Type, const std::string& Message);
	static void PrintLog(LogEntry &Entry);
	static void Seperator();
	static void NewLine();
	static void Message(const std::string& message);

private:

};

//Log:: LogMessage(LOG_INFO "Character has Perished!")
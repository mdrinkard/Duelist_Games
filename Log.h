#pragma once
#include <iostream>
#include <vector>

enum LogType
{
	LOG_DEFAULT,
	LOG_INFO, //0
	LOG_WARNING, //1
	LOG_ERROR //2
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
#ifndef __PLUTO_LOGGER_H__
#define __PLUTO_LOGGER_H__


#include <string>
#include <stdarg.h>
#include <map>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sys/time.h>
#include "util.h"

namespace pluto
{

class Logger : public ofstream
{
public:
	Logger();
	~Logger();

public:
	void SetLogPath(const string& path);
	void SetLogLevel(uint16_t log_level);
	void SetServerName(const string& name);
	uint16_t GetLogLevel();

	Logger& NewLine(int16_t log_type);

public:
	inline bool IsSetPath() const
	{
		return !path_.empty();
	}

	friend ostream& EndLine(ostream& Logger);

private:
	string path_;
	string file_;
	string server_name_;
	uint16_t log_level_;
};

extern ostream& EndLine(ostream& logger);
extern ostream& EndFile(ostream& logger);
//暂时只用这4种
extern void LogDebug(const char* msg, ...);
extern void LogInfo(const char* msg, ...);
extern void LogWarning(const char* msg, ...);
extern void LogError(const char* msg, ...);
/*
extern void LogCritical(const char* key, const char* msg, ...);
extern void LogConsole(const char* key, const char* msg, ...);
extern void LogScript(const char* level, const char* msg, ...);

extern void Error(const char* level, const char* msg, ...);
*/
extern Logger g_logger;

};

#endif

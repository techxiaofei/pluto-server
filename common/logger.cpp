#include "logger.h"

namespace pluto
{

Logger g_logger;

Logger::Logger():path_(),file_(),server_name_(),log_level_(LOG_ALL)
{

}

Logger::~Logger()
{

}

void Logger::SetLogPath(const string& path)
{
	path_.assign(path);
}

void Logger::SetLogLevel(uint16_t log_level)
{
	log_level_ = log_level;
}

void Logger::SetServerName(const string& name)
{
	server_name_.assign(name);
}

uint16_t Logger::GetLogLevel()
{
	return log_level_;
}


Logger& Logger::NewLine(int16_t log_type)
{
//using std::ios;

	time_t tNow = time(NULL);
	tm * pTm = localtime(&tNow);

	char szTemp[16];
	memset(szTemp, 0, sizeof(szTemp));

	string pszFormat;
	if(log_type==LOG_ERROR)
	{
		pszFormat = "%Y%m%d.err";           //错误日志
	}
	else
	{
		pszFormat = "%Y%m%d.log";           //一般日志
	}
	strftime(szTemp, sizeof(szTemp), pszFormat.c_str(), pTm);

	bool bOpen = false;
	if(!this->is_open())
	{
		bOpen = true;
	}
	else if(file_ != szTemp)
	{
		this->close();
		bOpen = true;
	}
	else
	{
		//bOpen = false;
	}

	if(bOpen)
	{
		file_.assign(szTemp);

		if(path_.empty())
		{
			this->open(szTemp, ios::out|ios::app);
		}
		else
		{
			char szLogFile[256];
			memset(szLogFile, 0, sizeof(szLogFile));
			snprintf(szLogFile, sizeof(szLogFile), "%s%s_%s", path_.c_str(), server_name_.c_str(), szTemp);
			this->open(szLogFile, ios::out|ios::app);
		}
	}

	return *this;
}

ostream& EndLine(ostream& logger)
{
	logger << endl;
	logger.flush();

	return logger;
}


ostream& EndFile(ostream& logger)
{
	logger << endl;
	((ofstream&)logger).close();

	return logger;
}


//获得当前时间的格式化的时分秒毫秒 HH:MM:SS
void _get_time_hmsu_head(char* s, size_t n)
{

	struct timeval tv;
	if(gettimeofday(&tv, NULL)==0)
	{
		time_t& t = tv.tv_sec;
		struct tm* tm2 = localtime(&t);

		snprintf(s, n, "%02d:%02d:%02d", tm2->tm_hour, tm2->tm_min, tm2->tm_sec);
	}
	else
	{
		snprintf(s, n, "??:??:??");
	}
}

/*
void Log(const char* section, const char* key, const char* msg, va_list& ap)
{
    static const char _hmsu_head[] = "17:04:10.762177";
    enum {_hmsu_head_size = sizeof(_hmsu_head)+1,};

    char szHmsu[32];
    _get_time_hmsu_head(szHmsu, sizeof(szHmsu));

    char szTmp[MAX_LOG_BUFF_SIZE];
    int n1 = snprintf(szTmp, sizeof(szTmp), "%s  [%s][%s]", szHmsu, section, key);
    if(n1 > 0 && n1 < (int)sizeof(szTmp))
    {
        int n2 = vsnprintf(szTmp+n1, sizeof(szTmp)-n1, msg, ap);
        if(n2 > 0 && (n1+n2)<(int)sizeof(szTmp))
        {
            szTmp[n1+n2] = '\0';
        }

    }

    //设置了文件路径才打印到日志文件,否则只打印到stdout/stderr
    if(g_logger.IsSetPath())
    {
        g_logger.NewLine() << szTmp << EndLine;
        //printf("%s\n", szTmp);
    }
    else
    {
        printf("%s\n", szTmp);
    }
}
*/

template <size_t size>
void _Log(char (&strDest)[size], const char* section, const char* key, const char* msg, va_list& ap)
{

	char szHmsu[9];
	memset(szHmsu, 0, 9);

	_get_time_hmsu_head(szHmsu, sizeof(szHmsu));

	int n1 = snprintf(strDest, (int)size, "%s[%s]", szHmsu, section);
	if(n1 > 0 && n1 < (int)size)
	{
		int n2 = vsnprintf(strDest+n1, (int)size-n1, msg, ap);
		if(n2 > 0 && (n1+n2)<(int)size)
		{
			strDest[n1+n2] = '\0';
		}

	}
}

void Log2File(int16_t log_type, const char* key, const char* msg, va_list& ap)
{
	uint16_t log_level = g_logger.GetLogLevel();
	if (!(log_level & log_type))
	{
		return;
	}

	string section;
	switch (log_type)
	{
		case LOG_DEBUG:
			section = "DEBUG  ";
			break;
		case LOG_INFO:
			section = "INFO   ";
			break;
		case LOG_WARNING:
			section = "WARNING";
			break;
		case LOG_ERROR:
			section = "ERROR  ";
			break;
		default:
			section = "UNDEFINE";
			break;

	}

    char szTmp[MAX_LOG_BUFF_SIZE];
    memset(szTmp, 0, MAX_LOG_BUFF_SIZE * sizeof(char));

    _Log(szTmp, section.c_str(), key, msg, ap);

    if(g_logger.IsSetPath())
    {
		g_logger.NewLine(log_type) << szTmp << EndLine;
    }
    else
    {
        printf("%s\n", szTmp);
    }
}
/*
void Log2Console(const char* section, const char* key, const char* msg, va_list& ap, int nFileType = 0)
{
    char szTmp[MAX_LOG_BUFF_SIZE];
    _Log(szTmp, section, key, msg, ap);

#ifdef _DEBUG_VERSION_
    printf("%s\n", szTmp);
#else
    if(g_logger.IsSetPath())
    {
        g_logger.NewLine(nFileType) << szTmp << EndLine;
        printf("%s\n", szTmp);
    }
    else
    {
        printf("%s\n", szTmp);
    }
#endif
}
*/

void LogDebug(const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File(LOG_DEBUG, "", msg, ap);
	va_end(ap);
}

void LogInfo(const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File(LOG_INFO, "", msg, ap);
	va_end(ap);
}

void LogWarning(const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File(LOG_WARNING, "", msg, ap);
	va_end(ap);
}

void LogError(const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File(LOG_ERROR, "", msg, ap);
	va_end(ap);
}
/*
void LogCritical(const char* key, const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File("CRITICAL", key, msg, ap);
	va_end(ap);
}

void LogScript(const char* level, const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File("SCRIPT  ", level, msg, ap);
	va_end(ap);
}

void LogConsole(const char* key, const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2Console("CONSOLE ", key, msg, ap);
	va_end(ap);
}

void Error(const char* level, const char* msg, ...)
{
	va_list ap;
	memset(&ap, 0, sizeof ap);

	va_start(ap, msg);
	Log2File("ERROR  ", level, msg, ap, 1);
	va_end(ap);
}
*/

}
#ifndef __PLUTO_INIT_SERVER_H__
#define __PLUTO_INIT_SERVER_H__

#include <pthread.h>
#include <string>
#include <list>
#include "util.h"
#include "message_queue.h"
#include "cjson_helper.h"

namespace pluto
{

//创建worker线程时要传入的参数
struct WorkerParam
{
	int worker_id;
	JsonHelper* json_helper;

};

class BaseServer
{
public:
	BaseServer();

	virtual ~BaseServer();

	virtual void Start(const char* server_name, const char* config_file) = 0;  //各子类实现

	virtual bool SetLogConfig(JsonHelper& json_hp);

	virtual bool SetWorkNum(JsonHelper& json_hp);

public:  //static
	static bool GetServerPort(JsonHelper& json_hp, uint16_t& port);

	static bool GetServerIpPort(JsonHelper& json_hp, string server_name, string& ip, uint16_t& port);

	static bool GetDBlist(JsonHelper& json_hp, list<string>& db_list);

	static bool GetSqlParam(JsonHelper& json_hp, string& host, string& user, string& passwd, string& db, int& port);

	static string server_name_;  //此服务器名字

protected:
	void CreateThread(pthread_t* thread, void*(cb)(void*), void* arg);

protected:
	int worker_num_;  //worker_thread num
private:
	MessageQueue mq_;  //单例


};

}

#endif
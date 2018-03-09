#include "base_server.h"
#include "logger.h"

namespace pluto
{

string BaseServer::server_name_ = "base_server";

BaseServer::BaseServer():worker_num_(1),mq_()
{

}

BaseServer::~BaseServer()
{

}

bool BaseServer::SetLogConfig(JsonHelper& json_hp)
{	
	char* log_level;
	string log_path;

	if(!json_hp.GetJsonItem("log_level", &log_level))
	{
		printf("get_log_level fail\n");
		return false;
	}
	if(!json_hp.GetJsonItem("log_path", log_path))
	{
		printf("get_log_path fail\n");
		return false;
	}

	g_logger.SetLogLevel(atoi(log_level));
	g_logger.SetLogPath(log_path);

	return true;
}

bool BaseServer::SetWorkNum(JsonHelper& json_hp)
{
	int worker_num;
	if(!json_hp.GetJsonItem2(BaseServer::server_name_.c_str(), "worker_num", worker_num))
	{
		printf("SetWorkNum fail\n");
		return false;
	}
	worker_num_ = worker_num;
	return true;
	
}

bool BaseServer::GetServerPort(JsonHelper& json_hp, uint16_t& port)
{
	int _port;
	if(!json_hp.GetJsonItem2(BaseServer::server_name_.c_str(), "port", _port))
	{
		printf("GetServerPort fail\n");
		return false;
	}
	port = (uint16_t)_port;
	return true;
}

bool BaseServer::GetServerIpPort(JsonHelper& json_hp, string name, string& ip, uint16_t& port)
{	

	if(!json_hp.GetJsonItem2(name.c_str(), "ip", ip))
	{
		printf("GetServerIpPort ip fail\n");
		return false;
	}

	int _port;
	if(!json_hp.GetJsonItem2(name.c_str(), "port", _port))
	{
		printf("GetServerIpPort port fail\n");
		return false;
	}
	port = (uint16_t)_port;
	return true;
}

bool BaseServer::GetDBlist(JsonHelper& json_hp, list<string>& db_list)
{
	string str;
	if(!json_hp.GetJsonItem("db_list", str))
	{
		printf("GetDBlist fail\n");
		return false;
	}

	split(str,",", db_list);

	return true;
}

bool BaseServer::GetSqlParam(JsonHelper& json_hp, string& host, string& user, string& passwd, string& db, int& port)
{
	if(!json_hp.GetJsonItem2("mysql", "host", host))
	{
		printf("GetServerIpPort host fail\n");
		return false;
	}
	if(!json_hp.GetJsonItem2("mysql", "user", user))
	{
		printf("GetServerIpPort user fail\n");
		return false;
	}
	if(!json_hp.GetJsonItem2("mysql", "passwd", passwd))
	{
		printf("GetServerIpPort passwd fail\n");
		return false;
	}
	if(!json_hp.GetJsonItem2("mysql", "db", db))
	{
		printf("GetServerIpPort db fail\n");
		return false;
	}
	if(!json_hp.GetJsonItem2("mysql", "port", port))
	{
		printf("GetServerIpPort port fail\n");
		return false;
	}
	return true;
}

void BaseServer::CreateThread(pthread_t* thread, void*(cb)(void*), void* arg)
{
	if (pthread_create(thread,NULL, cb, arg)) {
		printf("Create thread failed.\n");
		exit(1);
	}
}

}
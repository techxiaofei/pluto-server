#include "game_server.h"
#include "game_py2cpp.h"
#include "game_cpp2py.h"
#include "epoll_server.h"
#include "thread_socket.h"
#include "game_worker.h"
#include "logger.h"
#include "cjson_helper.h"
#include "config_reader.h"

GameServer::GameServer():BaseServer()
{

}

GameServer::~GameServer()
{

}

void GameServer::Start(const char* name, const char* config_file)
{
	GameServer::server_name_ = name;
	g_logger.SetServerName(GameServer::server_name_);

	ConfigReader reader(config_file);
	char* config = reader.GetAllConfig();
	JsonHelper json_hp(config);

	if (!SetLogConfig(json_hp))
	{
		return;
	}
	if (!SetWorkNum(json_hp))
	{
		return;
	}

	pthread_t p_socket;

	CreateThread(&p_socket, Thread_Socket_CB, (void*)&json_hp);

	pthread_t p_worker[worker_num_];

	WorkerParam worker_param[worker_num_];

	for (int i = 0; i < worker_num_; ++i)
	{
		worker_param[i].worker_id = i;
		worker_param[i].json_helper = NULL;  //暂时不需要这个参数
		CreateThread(&p_worker[i], Thread_Worker_CB, (void*)&worker_param[i]);
	}

	for (int i = 0; i < worker_num_; ++i)
	{
		pthread_join(p_worker[i], NULL);
	}

}

void* GameServer::Thread_Socket_CB(void *p)
{
	JsonHelper json_hp = *(JsonHelper*)p;

	LogDebug("Thread_Socket_CB%s",BaseServer::server_name_.c_str());

	uint16_t port;
	if (!GetServerPort(json_hp, port))
	{
		LogError("Thread_Socket_CB: GetServerPort error");
		return NULL;
	}

	//服务器的连接，这里是gameserver，所以需要连接dbserver
	list<string> db_list;
	if (!GetDBlist(json_hp, db_list))
	{
		LogError("Thread_Socket_CB: GetDBlist error");
		return NULL;
	}

	ThreadSocket socket;

	list<string>::iterator iter = db_list.begin();
	for (; iter != db_list.end(); ++iter)
	{
		string ip;
		uint16_t port;
		if (!GetServerIpPort(json_hp, *iter, ip, port))
		{
			LogError("Thread_Socket_CB: GetServerIpPort error");
		}
		LogInfo("Thread_Socket_CB connect_ip_port: %s,%d", ip.c_str(), port);
		EpollServer::getSingleton().ConnectIpPort(ip, port);
	}

	socket.Start(port);

	return NULL;
}

void* GameServer::Thread_Worker_CB(void *p)
{
	WorkerParam worker_param = *(WorkerParam*)p;
	//JsonHelper json_hp = *worker_param.json_helper;
	int worker_id = worker_param.worker_id;

	Game_Cpp2Py* py = new Game_Cpp2Py();

	Game_Py2Cpp* cpp = new Game_Py2Cpp();

	GameWorker worker(worker_id, py);
	worker.Start();
	return NULL;
}

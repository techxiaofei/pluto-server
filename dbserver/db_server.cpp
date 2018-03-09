#include "db_server.h"
#include "thread_socket.h"
#include "db_worker.h"
#include "logger.h"
#include "cjson_helper.h"
#include "config_reader.h"

DBServer::DBServer():BaseServer()
{

}

DBServer::~DBServer()
{

}

void DBServer::Start(const char* name, const char* config_file)
{
	DBServer::server_name_ = name;
	g_logger.SetServerName(DBServer::server_name_);

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
		worker_param[i].json_helper = &json_hp;
		CreateThread(&p_worker[i], Thread_Worker_CB, (void*)&worker_param[i]);
	}

	for (int i = 0; i < worker_num_; ++i)
	{
		pthread_join(p_worker[i], NULL);
	}
}

void* DBServer::Thread_Socket_CB(void *p)
{
	JsonHelper json_hp = *(JsonHelper*)p;

	LogDebug("Thread_Socket_CB%s",BaseServer::server_name_.c_str());

	uint16_t port;
	if (!GetServerPort(json_hp, port))
	{
		LogError("Thread_Socket_CB: GetServerPort error");
		return NULL;
	}

	ThreadSocket socket;
	socket.Start(port);
	return NULL;
}

void* DBServer::Thread_Worker_CB(void *p)
{
	WorkerParam worker_param = *(WorkerParam*)p;
	JsonHelper json_hp = *worker_param.json_helper;
	int worker_id = worker_param.worker_id;

	string host;
	string user;
	string passwd;
	string db;
	int port;

	if (!GetSqlParam(json_hp, host, user, passwd, db, port))
	{
		/* code */
	}

	LogDebug("Worker_id is %d", worker_id);

	DBWorker db_worker(worker_id);
	db_worker.ConnectDB(host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), (unsigned int)port);
	db_worker.Start();
	LogDebug("Thread_Worker Noting\n");
	return NULL;
}

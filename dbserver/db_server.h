#ifndef __DB_SERVER_H__
#define __DB_SERVER_H__

#include "base_server.h"

using namespace pluto;

class DBServer : public BaseServer
{
public:
	DBServer();
	~DBServer();

	void Start(const char* server_name, const char* config_file);

	static void* Thread_Socket_CB(void *p);
	static void* Thread_Worker_CB(void *p);

//private:
	//MessageQueue mq_;

};

#endif
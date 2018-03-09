#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include <Python.h>
#include "base_server.h"

using namespace pluto;

class GameServer : public BaseServer
{
public:
	GameServer();
	~GameServer();

	void Start(const char* server_name, const char* config_file);
	//TODO: move to common
	static void* Thread_Socket_CB(void *p);
	static void* Thread_Worker_CB(void *p);

//private:
	//MessageQueue mq_;

};

#endif
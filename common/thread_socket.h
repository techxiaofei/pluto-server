#ifndef __PLUTO_THREAD_SOCKET_H__
#define __PLUTO_THREAD_SOCKET_H__

#include "epoll_server.h"

namespace pluto
{

class ThreadSocket
{
public:
	ThreadSocket();

	~ThreadSocket();

	void Start(int port);

private:
	EpollServer server_;
};

}

#endif
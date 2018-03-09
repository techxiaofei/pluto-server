#include "thread_socket.h"

namespace pluto
{

ThreadSocket::ThreadSocket()
{

}

ThreadSocket::~ThreadSocket()
{

}

void ThreadSocket::Start(int port)
{
	server_.StartServer(port);
}

}
#include "connector.h"

namespace pluto
{

Connector::Connector(int fd, string server_ip, uint16_t port):Channel(fd, NULL, FD_TYPE_CONNECT)
{
	int len = server_ip.length();
	server_ip_ = new char[len + 1];
	memset(server_ip_, 0, len+1);
	memcpy(server_ip_,server_ip.c_str(), len);
	port_ = port;
	success_ = false;
}

Connector::~Connector()
{
	delete [] server_ip_;

}

}


#include <sys/eventfd.h>
#include <string.h>
#include "net_util.h"

namespace pluto
{
int netlib_setnonblocking(int sockfd)
{
	return fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFD, 0 )|O_NONBLOCK );
}

int netlib_socket()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		return -1;
	}

	if (netlib_setnonblocking(sockfd) < 0)
	{
		return -1;
	}
	return sockfd;
}

int netlib_bind(int sockfd, uint16_t port)
{
	struct sockaddr_in addr;
	//memset(&addr, 0, sizeof(addr));
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl( INADDR_ANY ); 

	int r = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (r < 0)
		return -1;
	return 0;
}

int netlib_accept(int sockfd)
{
	struct sockaddr_in client_addr;
	socklen_t len = sizeof( struct sockaddr_in );
	int conn_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len);
	return conn_fd;
}

int netlib_listen(int sockfd, int backlog)
{
	return listen(sockfd, backlog);
}


int netlib_connect(int sockfd, const char* server_ip, uint16_t port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(server_ip);

	int r = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (r < 0)
	{
		return r;
	}
	return 0;
}

int netlib_send(int sockfd, void* buf, int len)
{
	int ret = send(sockfd, (char*)buf, len, 0);
	if (ret < 0)
	{

	}
	return ret;
}

int netlib_recv(int sockfd, void* buf, int len)
{
	return recv(sockfd, (char*)buf, len, 0);
}

int netlib_eventfd()
{
	int evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evfd < 0)
	{
		return -1;
	}
	return evfd;
}

int netlib_eventfd_read(int eventfd)
{
	uint64_t one = 1;
	int n = read(eventfd, &one, sizeof one);
	if (n != sizeof one)
	{
		return -1;
	}
	return 0;
}

int netlib_eventfd_write(int eventfd)
{
	uint64_t one = 1;
	int n = write(eventfd, &one, sizeof one);
	if (n != sizeof one)
	{
		return -1;
	}
	return 0;
}

}

#ifndef __PLUTO_NET_UTIL_H__
#define __PLUTO_NET_UTIL_H__

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util.h"

namespace pluto
{

extern int netlib_setnonblocking(int sockfd);
extern int netlib_socket();
extern int netlib_bind(int sockfd, uint16_t port);
extern int netlib_accept(int sockfd);
extern int netlib_listen(int sockfd, int backlog);
extern int netlib_connect(int sockfd, const char* server_ip, uint16_t port);
extern int netlib_send(int sockfd, void* buf, int len);
extern int netlib_recv(int sockfd, void* buf, int len);

extern int netlib_eventfd();
extern int netlib_eventfd_write(int eventfd);
extern int netlib_eventfd_read(int eventfd);
	
};


#endif  //  __PLUTO_NET_UTIL_H__

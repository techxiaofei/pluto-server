#ifndef __PLUTO_UTIL_H__
#define __PLUTO_UTIL_H__

#include <stdint.h>		// define int8_t ...
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <list>
//#include <sys/types.h>

using namespace std;

#define READ_BUF_SIZE	1024
#define WRITE_BUF_SIZE	1024

#define MAX_BUF_SIZE (WRITE_BUF_SIZE * 4)

#define MAX_LOG_BUFF_SIZE 1024
//typedef int SOCKET  //socket

//typedef unsigned char	uchar_t;
typedef char			uchar_t;
typedef int				net_handle_t;
typedef int				conn_handle_t;

#define MQ_TYPE_PACK		1  //package
#define MQ_TYPE_TIMER		2  //timer
#define MQ_TYPE_CLOSE		3  //close socket fd
#define MQ_TYPE_CONNECT 	4  //connect to other server
#define MQ_TYPE_TICK		5  //tick
#define MQ_TYPE_BROADCAST	6  //broad_cast
//#define MQ_TYPE_SQL		4  //sql use package

#define SQL_QUERY		1  //查询
#define SQL_UPDATE		2  //更新

#define LOG_NONE		0x00  //LOG_NONE
#define LOG_DEBUG		0x01  //LOG_DEBUG
#define LOG_INFO		0x02  //LOG_INFO
#define LOG_WARNING		0x04  //LOG_WARNING
#define LOG_ERROR		0x08  //LOG_ERROR
#define LOG_ALL			0xff  //LOG_ALL

enum EFDTYPE
{
    //FD_TYPE_ERROR = 0,
    //FD_TYPE_ACCEPT = 1,
    FD_TYPE_SERVER = 2,
    //FD_TYPE_MAILBOX = 3,
    FD_TYPE_CONNECT = 4,
    
};

unsigned int hash_string(const char *str);

void split(string& s, string separate, list<string>& ret);

int min_pow(int num);  //大于num的最小的2的幂

#endif  //  __PLUTO_UTIL_H__
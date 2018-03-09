#ifndef __PLUTO_CONNECTOR_H__
#define __PLUTO_CONNECTOR_H__

#include <string>
#include "util.h"
#include "channel.h"

namespace pluto
{

/*
用于连接其他Server
 */

class Connector : public Channel
{
public:
	Connector(int fd, string server_ip, uint16_t port);
	~Connector();


	bool IsSuccess(){return success_;}

	void SetSuccess(bool success){success_ = success;}
	//void SetIpPort(const char* server_ip, uint16_t port);
	void SetFd(int fd){fd_ = fd;}

	char* GetIp(){return server_ip_;}
	uint16_t GetPort(){return port_;}

private:
	char* server_ip_;
	uint16_t port_;

	bool success_;

};

}

#endif
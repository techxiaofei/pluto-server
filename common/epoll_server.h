#ifndef __PLUTO_EPOLL_SERVER_H__
#define __PLUTO_EPOLL_SERVER_H__

#include <ev.h>  // for libev
#include <iostream>
#include <string>
#include <map>
#include <list>
#include "util.h"
#include "singleton.h"


namespace pluto
{

class Channel;
class Package;
class Timer;
class Connector;


class EpollServer : public Singleton<EpollServer>
{
public:
	EpollServer();
	
	virtual ~EpollServer();

	void StartServer(uint16_t port);

	//void OnWrite();

	int Eventfd(){return eventfd_;}

protected:
	static void accept_cb(struct ev_loop* loop, struct ev_io* watcher, int revents);
	static void r_w_cb(struct ev_loop* loop, struct ev_io* watcher, int revents);
	static void eventfd_cb(struct ev_loop* loop, struct ev_io* watcher, int revents);

	static void timeout_cb(struct ev_loop* loop, struct ev_timer* watcher, int revents);

	static void connector_cb(struct ev_loop* loop, struct ev_io* watcher, int revents);
	static void init_timeout_cb(struct ev_loop* loop, struct ev_timer* watcher, int revents);  //初始化一个1秒定时器用来connect其他server


	void OnAccept(int fd);  //监听socket有新连接

	void OnRead(int fd, EFDTYPE fdtype);

	void OnWrite(int fd, EFDTYPE fdtype);

	void OnEventfd(int fd);  //eventfd可读

	void OnFdClosed(int fd, EFDTYPE fdtype);

	void OnTimer(ev_timer* timer);

	//void OnConnect(int fd, int revents);  //通过EFDTYPE区分 

	void OnTryConnect(ev_timer* timer);

public:
	void ConnectIpPort(string server_ip, uint16_t port);  //外部调用用来connect其他server

	//void HandlePackage(Channel* channel,Package* package);  //

public:
	//void HandleSendPackage(uint32_t uniqid, Package* package);  //把要send的packet放到channel的out buffer里面	

private:
	int AddEvent(struct ev_io* io_watcher, int fd, short events);

	int UpdateEvent(int fd, short events, Channel* channel);

	void RemoveEvent(int fd, Channel* channel);

	Channel* GetChannel(int fd);
	void AddChannel(ev_io* io_watcher, int fd, EFDTYPE fdtype);

	Connector* GetUserfulConnector();

	Connector* GetConnector(int fd);

	void InitEventfd();

	void InitTimer();

private:
	int GetFdFromUniqid(uint32_t uniqid);

	uint32_t GetUniqidFromFd(int fd);

	//uint32_t InsertFd2Uniqid(int fd);

	//void RemoveFd2Uniqid(int fd);

private:
	struct ev_loop* loop_;

	bool running_;

	int eventfd_;

	typedef map<int, Channel*> ChannelMap;  //正常的socket
	ChannelMap channel_map_;

	typedef map<uint32_t, Timer*> TimerMap;  //定时器
	TimerMap timer_map_;

	typedef list<Connector*> ConnectorList;  //连接其他服务器
	ConnectorList connector_list_;

	//fd->uniqid | uniqid->fd
	//map<int, uint32_t> fd_2_uniqid_;
	//map<uint32_t, int> uniqid_2_fd_;
};

}

#endif  //  __PLUTO_EPOLL_SERVER_H__
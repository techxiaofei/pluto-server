#include <errno.h>
#include <time.h>
#include "net_util.h"
#include "epoll_server.h"
#include "package.h"
#include "channel.h"
#include "timer.h"
#include "connector.h"
#include "message_queue.h"
#include "logger.h"

namespace pluto
{

PLUTO_SINGLETON_INIT(EpollServer);

EpollServer::EpollServer():loop_(NULL),running_(false),eventfd_(0)
{

}

EpollServer::~EpollServer()
{
	if (loop_ != NULL)
	{
		ev_loop_destroy(loop_);
	}

	close(eventfd_);
}

void EpollServer::StartServer(uint16_t port)
{
	int fd = netlib_socket();
	if (fd < 0)
	{
		LogError("netlib_socket error:%d",errno);
		exit(EXIT_FAILURE);
	}

	netlib_setnonblocking(fd);

	//TODO,
	struct linger linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *) &linger, sizeof(linger));

	if(netlib_bind(fd, port) < 0)
	{
		LogError("netlib_bind error:%d\n",errno);
		exit(EXIT_FAILURE);
	}
	if (netlib_listen(fd,20))
	{
		LogError("netlib_listen error:%d\n",errno);
		exit(EXIT_FAILURE);
	}
	
	loop_ = ev_default_loop(EVBACKEND_EPOLL | EVFLAG_NOENV);

	struct ev_io io_watcher;
	ev_init(&io_watcher, accept_cb);
	AddEvent(&io_watcher, fd, EV_READ);

	LogInfo("EpollServer::StartServer:listen fd:%d\n",fd);
	
	InitEventfd();
	InitTimer();

	ev_run(loop_, 0);
}

void EpollServer::InitEventfd()
{
	//eventfd wakeup socket thread
	eventfd_ = netlib_eventfd();
	if (eventfd_ < 0)
	{
		LogError("netlib_socket error:%d\n",errno);
		exit(EXIT_FAILURE);
	}

	struct ev_io* efd_watcher = (struct ev_io*) malloc(sizeof(struct ev_io));
	ev_init(efd_watcher, eventfd_cb);
	AddEvent(efd_watcher, eventfd_, EV_READ);
}

void EpollServer::InitTimer()
{
	//
	//timer
	//struct ev_timer* timeout_watcher = (struct ev_timer*) malloc(sizeof(struct ev_timer));
	/*
	timer_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	int remain = 3600 - (p->tm_min * 60 + p->tm_sec);

	Timer* timer = Timer(1, timeout_watcher);
	timer_map_.insert(pair<uint32_t, Timer*>(1, timer));
	*/
	//ev_timer_init(timeout_watcher, timeout_cb, remain, 3600);
	//ev_timer_start(loop_, timeout_watcher);
	
	struct ev_timer* timeout_watcher = (struct ev_timer*) malloc(sizeof(struct ev_timer));
	ev_timer_init(timeout_watcher, init_timeout_cb, 1, 1);
	ev_timer_start(loop_, timeout_watcher);
}

void EpollServer::OnAccept(int fd)
{
	LogDebug("EpollServer::OnAccept %d\n", fd);

	int conn_fd = netlib_accept(fd);
	if (conn_fd < 0)
	{
		LogError("accept error\n");
	}

	netlib_setnonblocking(conn_fd);

	struct ev_io* conn_ev = (struct ev_io*) malloc(sizeof(struct ev_io));  //TODO, delete ptr
	if (conn_ev == NULL)
	{
		LogError("malloc error in accept_cb\n");
		return;
	}

	ev_init(conn_ev, r_w_cb);
	//ev_io_set(conn_ev, fd, EV_READ);
	//ev_io_start(loop_, conn_ev);
	AddEvent(conn_ev, conn_fd, EV_READ);
	AddChannel(conn_ev, conn_fd, FD_TYPE_SERVER);

	//InsertFd2Uniqid(conn_fd);
}


void EpollServer::OnRead(int fd, EFDTYPE fdtype)
{
	LogDebug("EpollServer::OnRead:%d\n", fd);

	Channel* channel;
	if (fdtype == FD_TYPE_SERVER)
	{
		channel = GetChannel(fd);
		if (channel == NULL)
		{
			LogError("EpollServer::OnRead channel null\n");
			return;
		}
	}
	else if (fdtype == FD_TYPE_CONNECT)
	{
		channel = GetConnector(fd);
		if (channel == NULL)
		{
			LogError("EpollServer::OnRead connector null\n");
			return;
		}
	}
	else
	{
		return;
	}
	

	Buffer& in_buf = channel->InBuffer();

	uint32_t free_buf_len = in_buf.GetAllocSize() - in_buf.GetWriteOffset();
	if (free_buf_len < READ_BUF_SIZE)
	{
		in_buf.Extend(READ_BUF_SIZE);  //TODO,上限判定
	}

	//LogDebug("EpollServer::OnRead: offset %d\n", in_buf.GetWriteOffset());

	//LogDebug("netlib_recv before%d\n", (int)strlen(in_buf.GetBuffer()));
	int bytes = netlib_recv(fd, in_buf.GetBuffer() + in_buf.GetWriteOffset(), READ_BUF_SIZE);
	if (bytes <= 0)
	{
		//remote close fd actively
		RemoveEvent(fd, channel);
		OnFdClosed(fd, fdtype);
		LogInfo("remote close fd actively: %d\n", fd);
		return;
	}
	//LogDebug("netlib_recv after%d\n", (int)strlen(in_buf.GetBuffer()));
	in_buf.IncWriteOffset(bytes);
	//LogDebug("EpollServer::OnRead: offset %d, %d\n", in_buf.GetWriteOffset(), bytes);

	Package* pack = NULL;
	while((pack = channel->ReadPackage()))
	{
		//LogDebug("EpollServer::OnReadPackage: %d\n", pack->GetTotalLength());
		
		if (fdtype == FD_TYPE_SERVER)
		{
			uint32_t uniqid = GetUniqidFromFd(fd);
			/*
			if (uniqid == 0)
			{
				LogError("EpollServer::OnRead uniqid is 0 %d\n", fd);
				break;
			}
			*/
			
			//LogDebug("EpollServer::OnRead pack:%s\n", pack->GetBodyData());
			MessageQueue::getSingleton().MQ2S_Push(uniqid, MQ_TYPE_PACK, pack);
		}
		else if (fdtype == FD_TYPE_CONNECT)
		{
			//LogDebug("EpollServer::OnRead pack:%s\n", pack->GetBodyData());
			MessageQueue::getSingleton().MQ2S_Push(0, MQ_TYPE_CONNECT, pack);
		}

		//delete pack;
		//pack = NULL;
	}

	//LogDebug("recv packs end\n");
	//TODO,
	int events = EV_READ;
	if (channel->OutBuffer().GetWriteOffset() > 0)
	{
		
		events |= EV_WRITE;
	}
	UpdateEvent(fd, events, channel);
	
}

void EpollServer::OnWrite(int fd, EFDTYPE fdtype)
{	
	LogDebug("EpollServer::OnWrite:%d\n",fd);

	Channel* channel;
	if (fdtype == FD_TYPE_SERVER)
	{
		channel = GetChannel(fd);
		if (channel == NULL)
		{
			LogError("EpollServer::OnWrite channel null\n");
			return;
		}
	}
	else if (fdtype == FD_TYPE_CONNECT)
	{
		channel = GetConnector(fd);
		if (channel == NULL)
		{
			LogError("EpollServer::OnWrite connector null\n");
			return;
		}
	}
	else
	{
		return;
	}

	Buffer& out_buf = channel->OutBuffer();

	int events = EV_READ;

	while(out_buf.GetWriteOffset() > 0)
	{
		uint32_t send_len = (int)out_buf.GetWriteOffset();
		if (send_len > WRITE_BUF_SIZE)
		{
			send_len = WRITE_BUF_SIZE;
		}

		LogDebug("EpollServer::OnWrite send_len %d\n", send_len);
		int ret = netlib_send(fd, out_buf.GetBuffer(), send_len);
		if (ret == -1)
		{
			LogWarning("netlib_send -1\n");
			//
			//UpdateEvent(fd, EPOLLIN | EPOLLOUT);
			events |= EV_WRITE;
			break;
		}
		else if (ret == 0)
		{	
			LogDebug("netlib_send 0\n");
			break;
		}
		else if (ret > 0)
		{
			LogDebug("netlib_send %d\n", ret);
			out_buf.Read(NULL, ret);
		}
	}

	UpdateEvent(fd, events, channel);
}

void EpollServer::OnEventfd(int efd)
{
	LogDebug("EpollServer::OnEventfd %d\n",efd);
	if (efd != eventfd_)
	{
		LogError("EpollServer::OnEventfd error %d != %d\n",efd, eventfd_);
	}

	int ret = netlib_eventfd_read(eventfd_);
	if (ret != 0)
	{
		LogError("EpollServer::eventfd read error\n");
		return;
	}

	while(1)
	{
		message_queue* mq = MessageQueue::getSingleton().MQ2C_Pop();
		if (mq == NULL)
			break;
		if (mq->type == MQ_TYPE_PACK)
		{
			int fd = GetFdFromUniqid(mq->uniqid);
			Channel* channel = GetChannel(fd);

			if (channel == NULL)
			{
				delete mq->pack;
				delete mq;
				LogWarning("EpollServer::OnEventfd channel null\n");
				continue;
			}

			channel->OutBuffer().Write(mq->pack->GetBuffer(), mq->pack->GetTotalLength());

			delete mq->pack;
			mq->pack = NULL;
			delete mq;
			mq = NULL;

			//update channel
			int events = EV_READ;
			if (channel->OutBuffer().GetWriteOffset() > 0)
			{
				events |= EV_WRITE;
			}
			UpdateEvent(fd, events, channel);
		}
		else if (mq->type == MQ_TYPE_BROADCAST)
		{
			Broadcast* broadcast = mq->broad;
			list<uint32_t>& uniqids = broadcast->uniqids;
			while(!uniqids.empty())
			{
				uint32_t uniqid = uniqids.front();
				uniqids.pop_front();
				//same with MQ_TYPE_PACK
				int fd = GetFdFromUniqid(uniqid);
				Channel* channel = GetChannel(fd);

				if (channel == NULL)
				{
					LogWarning("EpollServer::OnEventfd channel null\n");
					continue;
				}

				channel->OutBuffer().Write(broadcast->pack->GetBuffer(), broadcast->pack->GetTotalLength());

				//update channel
				int events = EV_READ;
				if (channel->OutBuffer().GetWriteOffset() > 0)
				{
					events |= EV_WRITE;
				}
				UpdateEvent(fd, events, channel);
			}

			delete broadcast->pack;
			broadcast->pack = NULL;
			delete broadcast;
			broadcast = NULL;
			delete mq;
			mq = NULL;

		}
		else if (mq->type == MQ_TYPE_TIMER)
		{
			struct ev_timer* timeout_watcher = (struct ev_timer*) malloc(sizeof(struct ev_timer));
			
			TimerStruct* timer_struct = mq->timer;

			Timer* timer = new Timer(timer_struct->timerid, timer_struct->repeat_sec, timeout_watcher);
			timer_map_.insert(pair<uint32_t, Timer*>(1, timer));

			ev_timer_init(timeout_watcher, timeout_cb, timer_struct->delay, timer_struct->repeat_sec);
			ev_timer_start(loop_, timeout_watcher);
			//LogWarning("EpollServer::OnEventfd timer: %d,%d,%d\n", timer_struct->timerid, timer_struct->delay, timer_struct->repeat_sec);
		}
		else if (mq->type == MQ_TYPE_CONNECT)
		{
			Connector* connector = GetUserfulConnector();
			if (connector == NULL)
			{
				//LOG_ERROR("EpollServer::OnEventfd connector null");
				delete mq->pack;
				delete mq;
				continue;
			}

			int fd = connector->Fd();

			connector->OutBuffer().Write(mq->pack->GetBuffer(), mq->pack->GetTotalLength());

			delete mq->pack;
			mq->pack = NULL;
			delete mq;
			mq = NULL;

			//update channel
			int events = EV_READ;
			if (connector->OutBuffer().GetWriteOffset() > 0)
			{
				events |= EV_WRITE;  //then, 
			}
			
			UpdateEvent(fd, events, connector);
		}

		else
		{
			LogError("EpollServer::OnEventfd type error:%d\n",mq->type);
			delete mq;
			continue;
		}
	}
}

void EpollServer::OnFdClosed(int fd, EFDTYPE fdtype)
{
	LogDebug("OnFdClosed %d\n", fd);
	if (fdtype == FD_TYPE_SERVER)
	{
		ChannelMap::iterator iter = channel_map_.find(fd);
		if (iter != channel_map_.end())
		{
			Channel* channel = iter->second;
			channel_map_.erase(fd);

			delete channel;
			channel = NULL;
		}

		//let py to remove this socket
		uint32_t uniqid = GetUniqidFromFd(fd);
		MessageQueue::getSingleton().MQ2S_Push_Close(uniqid);

		//RemoveFd2Uniqid(fd);
	}
	else if (fdtype == FD_TYPE_CONNECT)
	{
		Connector* connector = GetConnector(fd);

		if (connector != NULL)
		{
			close(fd);
			connector->SetSuccess(false);
			connector->SetFd(0);
		}
	}
	
}

void EpollServer::OnTimer(ev_timer* watcher)
{
	LogDebug("EpollServer::OnTimer\n");
	Timer* timer = NULL;
	TimerMap::const_iterator iter = timer_map_.begin();
	for ( ; iter != timer_map_.end(); ++iter)
	{
		if (iter->second->EV_Timer() == watcher)
		{
			timer = iter->second;
			break;
		}
	}
	if (timer != NULL)
	{
		MessageQueue::getSingleton().MQ2S_Push_Timer(timer->TimerId());

		if (!timer->Repeat())
		{
			timer_map_.erase(timer->TimerId());
		}
	}
	else
	{
		//destroy timer
		LogError("OnTimerError: timer is null\n");
	}
}

void EpollServer::OnTryConnect(ev_timer* timer)
{
	ConnectorList::iterator iter = connector_list_.begin();
	for ( ; iter != connector_list_.end(); ++iter)
	{
		Connector* connector = *iter;
		if ( !connector->IsSuccess() )
		{
			//connect
			int conn_fd = connector->Fd();
			if (conn_fd <= 0)
			{
				conn_fd = netlib_socket();
				if (conn_fd > 0)
				{
					connector->SetFd(conn_fd);
				}
				else
				{
					continue;
				}
			}

			int ret = netlib_connect(conn_fd, connector->GetIp(), connector->GetPort());
			if (ret == 0)
			{
				struct ev_io* conn_ev = (struct ev_io*) malloc(sizeof(struct ev_io));
				if (conn_ev == NULL)
				{
					LogError("malloc error in OnTryConnect\n");
					return;
				}

				ev_init(conn_ev, connector_cb);
				AddEvent(conn_ev, conn_fd, EV_READ);

				connector->SetIoWatcher(conn_ev);  //加入ev_io
				connector->SetSuccess(true);
				
				LogDebug("connect success:%s,%d\n", connector->GetIp(),connector->GetPort());
			}
			else
			{
				//LogDebug("connect fail:%d,%s,%d\n",conn_fd, connector->GetIp(), connector->GetPort());
				//close(conn_fd);
			}
		}
		else
		{
			//LogDebug("connect always success:");
		}
	}
}

void EpollServer::ConnectIpPort(string server_ip, uint16_t port)
{
	LogDebug("EpollServer::ConnectFd %d\n", port);

	int conn_fd = netlib_socket();
	if (conn_fd < 0)
	{
		conn_fd = 0;
	}
	Connector* connector = new Connector(conn_fd, server_ip, port);
	connector_list_.push_back(connector);
	//LogDebug("EpollServer::ConnectFd %d\n", port);
}

/*
void EpollServer::HandlePackage(Channel* channel, Package* package)
{
	LogDebug("EpollServer::HandlePackage\n");

	this->GetWorld()->FromRpcCall(channel, *package);
}
*/
/*
void EpollServer::HandleSendPackage(uint32_t uniqid, Package* package)
{
	//workerÃÃŸÂ³ÃŒÂ²Ã™Ã—Ã·
	LogDebug("EpollServer::HandleSendPackage\n");
	
	int fd = GetFdFromUniqid(uniqid);
	if (fd == 0)
	{
		LogDebug("EpollServer::HandleSendPackage fd 0\n");
		return;
	}

	Channel* channel = GetChannel(fd);
	if (channel == NULL)
	{
		LogDebug("EpollServer::HandleSendPackage channel null:%d \n", fd);
		return;
	}
	channel->OutBuffer().Write(package->GetBuffer(), package->GetLength());

	int ret = netlib_eventfd_write(eventfd_);
	if (ret != 0)
	{
		LogDebug("EpollServer::eventfd write error\n");
	}
}
*/

int EpollServer::AddEvent(struct ev_io* ev, int fd, short events)
{
	ev_io_set(ev, fd, events);
	ev_io_start(loop_, ev);  //

	return 0;
}

int EpollServer::UpdateEvent(int fd, short events, Channel* channel)
{	
	//LogDebug("UpdateEvent:%d,%d\n", fd, events);

	struct ev_io* io_watcher = channel->GetIoWatcher();

	if (io_watcher == NULL)
	{
		LogError("UpdateEvent error: io_watcher null\n");
		return -1;
	}
	//TODO, how to update fd event efficiently
	ev_io_stop(loop_, io_watcher);
	ev_io_set(io_watcher, fd, events);
	ev_io_start(loop_, io_watcher);

	return 0;
}

void EpollServer::RemoveEvent(int fd, Channel* channel)
{
	LogDebug("RemoveEvent: %d\n",fd);
	struct ev_io* io_watcher = channel->GetIoWatcher();
	if (io_watcher == NULL)
	{
		LogError("RemoveEvent error:io_watcher null\n");
		return;
	}
	ev_io_stop(loop_, io_watcher);
}

Channel* EpollServer::GetChannel(int fd)
{
	ChannelMap::const_iterator iter = channel_map_.find(fd);
	if (iter == channel_map_.end())
	{
		return NULL;
	}
	else
	{
		return iter->second;
	}
}

/*
 */
void EpollServer::AddChannel(struct ev_io* io_watcher, int fd, EFDTYPE fdtype)
{
	ChannelMap::iterator iter = channel_map_.lower_bound(fd);
	if (iter != channel_map_.end() && iter->first == fd)
	{
		//Channel* p = iter->second;
		LogWarning("AddChannel error, channel exists\n");

		//Channel* channel = iter->second; //TODO, modify or other
	}
	else
	{
		Channel* channel = new Channel(fd, io_watcher, fdtype);
		channel_map_.insert(iter, make_pair(fd, channel));
	}
}

Connector* EpollServer::GetUserfulConnector()
{
	//TODO,选第一个可用的Connector
	ConnectorList::iterator iter = connector_list_.begin();
	for ( ; iter != connector_list_.end(); ++iter)
	{
		Connector* connector = *iter;
		if (connector->IsSuccess())
		{
			return connector;
		}
	}
	return NULL;
}

Connector* EpollServer::GetConnector(int fd)
{
	ConnectorList::iterator iter = connector_list_.begin();
	for ( ; iter != connector_list_.end(); ++iter)
	{
		Connector* connector = *iter;
		if (connector->Fd() == fd)
		{
			return connector;
		}
		
	}
	return NULL;
}

int EpollServer::GetFdFromUniqid(uint32_t uniqid)
{
	return (int)uniqid;
	/*
	map<uint32_t, int>::iterator iter = uniqid_2_fd_.find(uniqid);
	if (iter == uniqid_2_fd_.end())
	{
		return 0;
	}
	return iter->second;
	*/
}

uint32_t EpollServer::GetUniqidFromFd(int fd)
{
	return (uint32_t)fd;
	/*
	//LogDebug("GetUniqidFromFd:%d\n",fd);
	map<int, uint32_t>::iterator iter = fd_2_uniqid_.find(fd);
	if (iter == fd_2_uniqid_.end())
	{
		return 0;
	}
	return iter->second;
	*/
}

/*
uint32_t EpollServer::InsertFd2Uniqid(int fd)
{
	uint32_t unique_id = fd + 1000000;

	map<int, uint32_t>::iterator iter = fd_2_uniqid_.find(fd);
	if (iter == fd_2_uniqid_.end())
	{
		LogDebug("InsertFd2Uniqid: %d,%d\n",fd,unique_id);
		fd_2_uniqid_.insert(pair<int,uint32_t>(fd, unique_id));
		uniqid_2_fd_.insert(pair<uint32_t,int>(unique_id, fd));
		return unique_id;
	}
	else
	{
		LogDebug("insert fd 2 uniqid\n");
		return iter->second;
	}
}
*/
/*
void EpollServer::RemoveFd2Uniqid(int fd)
{
	//fd->uniqid
	map<int, uint32_t>::iterator iter = fd_2_uniqid_.find(fd);
	if (iter != fd_2_uniqid_.end())
	{
		uint32_t unique_id = iter->second;

		uniqid_2_fd_.erase(unique_id);
		fd_2_uniqid_.erase(fd);
	}
}
*/

void EpollServer::accept_cb(struct ev_loop* loop, struct ev_io* watcher, int revents)
{
	int fd = watcher->fd;

	if (EV_ERROR & revents)
	{
		LogDebug("error event in accept\n");
		return;
	}

	EpollServer::getSingleton().OnAccept(fd);
}

void EpollServer::r_w_cb(struct ev_loop* loop, struct ev_io* watcher, int revents)
{
	int fd = watcher->fd;
	LogDebug("r_w_cb %d,%d\n", fd, revents);
	if (EV_ERROR & revents)
	{
		LogError("error event in read or write\n");
		return;
	}

	if (EV_READ & revents)
	{
		EpollServer::getSingleton().OnRead(fd, FD_TYPE_SERVER);
	}
	if (EV_WRITE & revents)
	{
		EpollServer::getSingleton().OnWrite(fd, FD_TYPE_SERVER);
	}
}

void EpollServer::eventfd_cb(struct ev_loop* loop, struct ev_io* watcher, int revents)
{
	int fd = watcher->fd;
	LogDebug("eventfd_cb %d,%d\n", fd, revents);
	if (EV_ERROR & revents)
	{
		LogError("error event in eventfd\n");
		return;
	}

	EpollServer::getSingleton().OnEventfd(fd);
	
}

void EpollServer::timeout_cb(struct ev_loop* loop, struct ev_timer* watcher, int revents)
{
	LogDebug("timeout_cb \n");

	EpollServer::getSingleton().OnTimer(watcher);
}

void EpollServer::connector_cb(struct ev_loop* loop, struct ev_io* watcher, int revents)
{
	int fd = watcher->fd;
	LogDebug("connector_cb %d,%d\n", fd, revents);
	if (EV_ERROR & revents)
	{
		LogError("error event in connector_cb\n");
		return;
	}
	if (EV_READ & revents)
	{
		EpollServer::getSingleton().OnRead(fd, FD_TYPE_CONNECT);
	}
	if (EV_WRITE & revents)
	{
		EpollServer::getSingleton().OnWrite(fd, FD_TYPE_CONNECT);
	}
}

void EpollServer::init_timeout_cb(struct ev_loop* loop, struct ev_timer* watcher, int revents)
{
	//LogDebug("init_timeout_cb \n");
	EpollServer::getSingleton().OnTryConnect(watcher);
	MessageQueue::getSingleton().MQ2S_Push_Tick();
}

}

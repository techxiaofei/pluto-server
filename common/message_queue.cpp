#include "message_queue.h"
#include "package.h"
#include "epoll_server.h"
#include "net_util.h"
#include "logger.h"

namespace pluto
{

PLUTO_SINGLETON_INIT(MessageQueue);

MessageQueue::MessageQueue():timerid_max_(0)
{
	pthread_mutex_init(&mq2s_lock_, NULL);
	pthread_cond_init(&mq2s_cond_, NULL);

	pthread_mutex_init(&mq2c_lock_, NULL);
}

MessageQueue::~MessageQueue()
{
	pthread_cond_destroy(&mq2s_cond_);

	if (!mq2s_.empty())
	{
		list<message_queue*>::iterator iter;
		for(iter = mq2s_.begin(); iter != mq2s_.end(); ++iter)
		{
			message_queue* mq = *iter;
			if (mq->type == MQ_TYPE_PACK || mq->type == MQ_TYPE_CONNECT)
			{
				delete mq->pack;
			}

			delete mq;
		}
	}

	if (!mq2c_.empty())
	{
		list<message_queue*>::iterator iter2;
		for(iter2 = mq2c_.begin(); iter2 != mq2c_.end(); ++iter2)
		{
			message_queue* mq2 = *iter2;
			if (mq2->type == MQ_TYPE_PACK || mq2->type == MQ_TYPE_CONNECT)
			{
				delete mq2->pack;
			}

			delete mq2;
		}
	}
}

void MessageQueue::MQ2S_Push(uint32_t uniqid, uint8_t type, Package* pack)
{
	LogDebug("MessageQueue::MQ2S_Push\n");

	if (type != MQ_TYPE_PACK && type != MQ_TYPE_CONNECT)
	{
		LogError("MessageQueue::MQ2S_Push type error %d\n", type);
		return;
	}

	MQ2S_Lock();

	message_queue* queue = new message_queue;
	queue->type = type;
	queue->uniqid = uniqid;
	queue->pack = pack;
	
	mq2s_.push_back(queue);

	MQ2S_Broadcast();
	/*
	bool notify = _Push_With_Notify(queue);
	if (notify)
	{
		MQ2S_Signal();
	}*/
	
	MQ2S_Unlock();
}

void MessageQueue::MQ2S_Push_Close(uint32_t uniqid)
{
	LogDebug("MessageQueue::MQ2S_Push_Close\n");
	MQ2S_Lock();

	message_queue* queue = new message_queue;
	queue->type = MQ_TYPE_CLOSE;
	queue->uniqid = uniqid;
	//queue->pack = NULL;

	mq2s_.push_back(queue);

	MQ2S_Broadcast();
	/*
	bool notify = _Push_With_Notify(queue);
	if (notify)
	{
		MQ2S_Signal();
	}
	*/

	MQ2S_Unlock();
}

void MessageQueue::MQ2S_Push_Timer(uint32_t timerid)
{
	LogDebug("MessageQueue::MQ2S_Push_Timer\n");
	MQ2S_Lock();

	message_queue* queue = new message_queue;
	//queue->uniqid = uniqid;
	queue->type = MQ_TYPE_TIMER;

	TimerStruct* timer = new TimerStruct;
	timer->timerid = timerid;
	queue->timer = timer;

	mq2s_.push_back(queue);

	MQ2S_Broadcast();
	/*
	bool notify = _Push_With_Notify(queue);
	if (notify)
	{
		//LogDebug("MessageQueue::MQ2S_Push_Timer signal\n");
		MQ2S_Signal();
	}
	*/

	MQ2S_Unlock();
}

void MessageQueue::MQ2S_Push_Tick()
{
	//LogDebug("MessageQueue::MQ2S_Push_Tick\n");
	MQ2S_Lock();

	message_queue* queue = new message_queue;
	//queue->uniqid = uniqid;
	queue->type = MQ_TYPE_TICK;

	mq2s_.push_back(queue);

	MQ2S_Broadcast();
	/*
	bool notify = _Push_With_Notify(queue);
	if (notify)
	{
		//LogDebug("MessageQueue::MQ2S_Push_Timer signal\n");
		MQ2S_Signal();
	}*/

	MQ2S_Unlock();
}

struct message_queue* MessageQueue::MQ2S_Pop()
{
	//LogDebug("MessageQueue::mq2s_Pop\n");
	MQ2S_Lock();

	while (mq2s_.empty())  //用while，不能用if
	{
		//LogDebug("MessageQueue::mq2s_Pop wait\n");
		MQ2S_Wait();
	}

	message_queue* queue = mq2s_.front();
	mq2s_.pop_front();

	MQ2S_Unlock();
	return queue;
}

void MessageQueue::MQ2C_Push(uint32_t uniqid, uint16_t command_id, uint16_t sub_id, char* data)
{
	LogDebug("MessageQueue::mq2c_Push:%d,%d,%d,%s\n",uniqid,command_id,sub_id,data);
	MQ2C_Lock();
	Package* send_pack = new Package();
	//uint32_t len = strlen(data);
	//LogDebug("MQ2C_Push:call_client:%d\n", len);
	//string str = data;  //TODO,后续要尽量减少字符串的copy
	send_pack->SetMsg(command_id, sub_id, data);

	//直接插入channel里面
	//EpollServer::getSingleton().HandleSendPackage(uniqid, send_pack);

	
	message_queue* queue = new message_queue;
	queue->type = MQ_TYPE_PACK;
	queue->uniqid = uniqid;
	queue->pack = send_pack;
	mq2c_.push_back(queue);

	int ret = netlib_eventfd_write(EpollServer::getSingleton().Eventfd());
	if (ret != 0)
	{
		LogError("EpollServer::eventfd write error\n");
	}

	MQ2C_Unlock();
}

void MessageQueue::MQ2C_Push_Broadcast(uint16_t command_id, uint16_t sub_id, bool all, char* data, char* uniqids)
{
	LogDebug("MessageQueue::MQ2C_Push_Broadcast:%d,%d,%s,%s\n",command_id,sub_id,data, uniqids);
	MQ2C_Lock();
	Broadcast* broadcast = new Broadcast();

	Package* send_pack = new Package();

	send_pack->SetMsg(command_id, sub_id, data);
	broadcast->pack = send_pack;
	broadcast->all = all;

	if (!all)
	{
		string uniqids_str(uniqids);
		list<string> _list;
		split(uniqids_str,",",_list);

		list<string>::iterator iter = _list.begin();
		for (; iter != _list.end(); ++iter)
		{
			(broadcast->uniqids).push_back(atoi((*iter).c_str()));
		}
	}

	message_queue* queue = new message_queue;
	queue->type = MQ_TYPE_BROADCAST;
	queue->uniqid = 0;
	queue->broad = broadcast;
	mq2c_.push_back(queue);

	int ret = netlib_eventfd_write(EpollServer::getSingleton().Eventfd());
	if (ret != 0)
	{
		LogError("EpollServer::eventfd write error\n");
	}

	MQ2C_Unlock();
}

void MessageQueue::MQ2C_Push_Connector(uint16_t conn_type, uint16_t return_id, char* data)
{
	LogDebug("MessageQueue::MQ2C_Push_Connector:%d,%d,%s\n",conn_type,return_id,data);
	MQ2C_Lock();
	Package* send_pack = new Package();
	//uint32_t len = strlen(data);
	//LogDebug("MQ2C_Push:call_client:%d\n", len);
	//string str = data;  //TODO,后续要尽量减少字符串的copy
	send_pack->SetMsg(conn_type, return_id, data);

	//直接插入channel里面
	//EpollServer::getSingleton().HandleSendPackage(uniqid, send_pack);

	
	message_queue* queue = new message_queue;
	queue->type = MQ_TYPE_CONNECT;
	queue->uniqid = 0;
	queue->pack = send_pack;
	mq2c_.push_back(queue);

	int ret = netlib_eventfd_write(EpollServer::getSingleton().Eventfd());
	if (ret != 0)
	{
		LogError("EpollServer::eventfd write error\n");
	}

	MQ2C_Unlock();
}

uint32_t MessageQueue::MQ2C_Push_Timer(uint32_t delay, uint32_t repeat_sec)
{
	LogDebug("MessageQueue::MQ2S_Push_Timer:%d,%d\n", delay, repeat_sec);
	MQ2C_Lock();

	TimerStruct* timer = new TimerStruct;
	timerid_max_ += 1;
	timer->timerid = timerid_max_;
	timer->delay = delay;
	timer->repeat_sec = repeat_sec;

	message_queue* queue = new message_queue;
	queue->type = MQ_TYPE_TIMER;
	queue->timer = timer;
	mq2c_.push_back(queue);

	int ret = netlib_eventfd_write(EpollServer::getSingleton().Eventfd());
	if (ret != 0)
	{
		LogError("EpollServer::eventfd write error\n");
	}

	MQ2C_Unlock();
	return timerid_max_;
}

struct message_queue* MessageQueue::MQ2C_Pop()
{
	LogDebug("MessageQueue::mq2c_Pop\n");
	MQ2C_Lock();

	if (mq2c_.empty())
	{
		MQ2C_Unlock();  //forgot it
		return NULL;
	}
	message_queue* queue = mq2c_.front();
	mq2c_.pop_front();

	MQ2C_Unlock();
	return queue;
}

bool MessageQueue::_Push_With_Notify(message_queue* queue)
{
	bool notify = false;
	if (mq2s_.empty())
	{
		notify = true;
	}

	mq2s_.push_back(queue);
	return notify;

}

void MessageQueue::MQ2S_Lock()
{
	pthread_mutex_lock(&mq2s_lock_);
}

void MessageQueue::MQ2S_Unlock()
{
	pthread_mutex_unlock(&mq2s_lock_);
}

void MessageQueue::MQ2S_Wait()
{
	pthread_cond_wait(&mq2s_cond_, &mq2s_lock_);
}

void MessageQueue::MQ2S_Signal()
{
	pthread_cond_signal(&mq2s_cond_);
}

void MessageQueue::MQ2S_Broadcast()
{
	pthread_cond_broadcast(&mq2s_cond_);
}

void MessageQueue::MQ2C_Lock()
{
	pthread_mutex_lock(&mq2c_lock_);
}

void MessageQueue::MQ2C_Unlock()
{
	pthread_mutex_unlock(&mq2c_lock_);
}

}


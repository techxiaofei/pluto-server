#ifndef __PLUTO_MQ_H__
#define __PLUTO_MQ_H__

#include <pthread.h>
#include <list>
#include "util.h"
#include "singleton.h"

namespace pluto
{

class Package;

struct TimerStruct
{
	uint32_t timerid;
	uint32_t delay;
	uint32_t repeat_sec;
};

struct Broadcast
{
	Package* pack;
	bool all;
	list<uint32_t> uniqids;
};

struct message_queue
{
	uint32_t uniqid;
	union {
		Package* pack;  //package/connector
		TimerStruct* timer;  //timer
		Broadcast* broad;  //broadcast
	};
	uint8_t type;  //类型
};

/*
MQ2S: socket线程read的数据，给worker线程处理
MQ2C: worker线程发送给客户端的数据，经socket线程处理
 */
class MessageQueue : public Singleton<MessageQueue>
{
public:
	MessageQueue();
	~MessageQueue();

public:
	void MQ2S_Push(uint32_t uniqid, uint8_t type, Package* pack);  //pack or connect

	void MQ2S_Push_Close(uint32_t uniqid);  //close socket fd

	void MQ2S_Push_Timer(uint32_t timerid);  //timer

	void MQ2S_Push_Tick();  //tick

	struct message_queue* MQ2S_Pop();

public:
	void MQ2C_Push(uint32_t uniqid, uint16_t command_id, uint16_t sub_id, char* data);

	void MQ2C_Push_Broadcast(uint16_t command_id, uint16_t sub_id, bool all, char* data, char* bc_list);

	void MQ2C_Push_Connector(uint16_t conn_type, uint16_t return_id, char* data);

	uint32_t MQ2C_Push_Timer(uint32_t delay, uint32_t repeat_sec);  //repeat_sec: 0 means no repeat, else repeat

	struct message_queue* MQ2C_Pop();

private:
	bool _Push_With_Notify(message_queue* queue);

	void MQ2S_Lock();

	void MQ2S_Unlock();

	void MQ2S_Wait();

	void MQ2S_Signal();

	void MQ2S_Broadcast();  //暂时使用broadcast替换signal

	void MQ2C_Lock();

	void MQ2C_Unlock();


private:
	uint32_t timerid_max_;  //定时器分配的id的最大值

	list<message_queue*> mq2s_;  //list方便插入和删除
	list<message_queue*> mq2c_;  

	pthread_mutex_t mq2s_lock_;
	pthread_cond_t mq2s_cond_;

	pthread_mutex_t mq2c_lock_;
};

}

#endif

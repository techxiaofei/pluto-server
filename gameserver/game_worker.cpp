#include "game_worker.h"
#include "game_cpp2py.h"
#include "message_queue.h"
#include "logger.h"
#include "util.h"

GameWorker::GameWorker(int worker_id, Game_Cpp2Py* py):ThreadWorker(worker_id)
{
	cpp2py_ = py;
	cpp2py_->Init();
}

GameWorker::~GameWorker()
{
	if (cpp2py_ != NULL)
	{
		delete cpp2py_;
	}
}

void GameWorker::Start()
{	
	struct message_queue* q = NULL;
	while(1)
	{
		//LogDebug("GameWorker:Start before\n");
		q = MessageDispatch();
		//call python to deal data
		
		switch(q->type)
		{
			case MQ_TYPE_PACK:
				cpp2py_->OnServer(q->uniqid, q->pack);
				//处理完delete掉
				delete q->pack;
				q->pack = NULL;
				break;
			case MQ_TYPE_CLOSE:
				cpp2py_->OnFdClosed(q->uniqid);
				break;
			case MQ_TYPE_TIMER:
				cpp2py_->OnTimer(q->timer->timerid);
				delete q->timer;
				q->timer = NULL;
				break;
			case MQ_TYPE_CONNECT:
				cpp2py_->OnConnector(q->pack);
				delete q->pack;
				q->pack = NULL;
				break;
			case MQ_TYPE_TICK:
				cpp2py_->OnTick();
				break;
			default:
				LogDebug("GameWorker:MQ_TYPE_ERROR\n");	
				break;
		}
		

		
		delete q;
		q = NULL;

		//LogDebug("GameWorker:Start after\n");
	}
}

// message_queue* GameWorker::MessageDispatch()
// {
// 	return MessageQueue::getSingleton().MQ2S_Pop();
// }

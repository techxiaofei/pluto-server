#include "util.h"
#include "thread_worker.h"
#include "message_queue.h"

namespace pluto
{

ThreadWorker::ThreadWorker(int worker_id):worker_id_(worker_id)
{

}

ThreadWorker::~ThreadWorker()
{
}

void ThreadWorker::Start()
{	

}

message_queue* ThreadWorker::MessageDispatch()
{
	return MessageQueue::getSingleton().MQ2S_Pop();
}

}
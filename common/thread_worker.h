#ifndef __PLUTO_THREAD_WORKER_H__
#define __PLUTO_THREAD_WORKER_H__

namespace pluto
{

struct message_queue;

class ThreadWorker
{
public:
	ThreadWorker(int worker_id);
	virtual ~ThreadWorker();

	virtual void Start() = 0;

	message_queue* MessageDispatch();

private:
	int worker_id_;  //worker线程的idx,从0开始
	
};

}

#endif
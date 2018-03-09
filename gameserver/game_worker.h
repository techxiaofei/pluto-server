#ifndef __GAME_WORKER_H__
#define __GAME_WORKER_H__

#include <Python.h>
#include "thread_worker.h"

using namespace pluto;

class Game_Cpp2Py;

class GameWorker : ThreadWorker
{
public:
	GameWorker(int worker_id, Game_Cpp2Py* py);
	~GameWorker();

	void Start();

	//message_queue* MessageDispatch();

private:
	Game_Cpp2Py* cpp2py_;
	
};

#endif
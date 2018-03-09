#ifndef __GAME_CPP2PY_H__
#define __GAME_CPP2PY_H__

#include <Python.h>
#include "package.h"

using namespace pluto;

class Game_Cpp2Py
{
public:
	Game_Cpp2Py();
	~Game_Cpp2Py();

	void Init();
	
	int OnServer(uint32_t uniqid, Package* pack);
	int OnFdClosed(uint32_t uniqid);
	int OnTimer(uint32_t timerid);
	int OnConnector(Package* package);  //db过来的
	int OnTick();

private:
	PyObject* pModule_;
	PyObject* pFunc_;
	PyObject* pResult_;

};

#endif
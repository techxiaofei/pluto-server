#include "game_cpp2py.h"
#include "logger.h"


Game_Cpp2Py::Game_Cpp2Py():pModule_(NULL),pFunc_(NULL),pResult_(NULL)
{
	Py_Initialize();

}

Game_Cpp2Py::~Game_Cpp2Py()
{
	Py_Finalize();
}

void Game_Cpp2Py::Init()
{
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('../pyscript/')"); 

	pModule_ = PyImport_ImportModule("pymain");

	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("init"), "");

}

int Game_Cpp2Py::OnServer(uint32_t uniqid, Package* pack)
{
	string body_data = (char*)pack->GetBodyData();

	uint16_t command_id = pack->GetCommandId();

	uint16_t sub_id = pack->GetSubId();

	//LogDebug("CallMethod:%s\n", body_data.c_str());

	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("OnServer"), const_cast<char*>("iiis"), uniqid, command_id, sub_id, body_data.c_str());
	if (pResult_ != NULL)
	{
		char* ret;
		int result = 0;

		if (PyArg_ParseTuple(pResult_, "i|s",  &result, &ret) == -1)
		{
			LogDebug("error return\n");
		}
		else
		{
			//LogDebug("return from py: %d, %s\n", result, ret);
		}
	}

	return 0;
}

int Game_Cpp2Py::OnFdClosed(uint32_t uniqid)
{
	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("OnFdClosed"), const_cast<char*>("i"), uniqid);
	if (pResult_ != NULL)
	{
		char* ret;
		int result = 0;

		if (PyArg_ParseTuple(pResult_, "i|s",  &result, &ret) == -1)
		{
			LogDebug("error return\n");
		}
		else
		{
			//LogDebug("return from py: %d, %s\n", result, ret);
		}
	}
	return 0;
}

int Game_Cpp2Py::OnTimer(uint32_t timerid)
{
	
	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("OnTimer"), const_cast<char*>("i"), timerid);
	if (pResult_ != NULL)
	{
		char* ret;
		int result = 0;

		if (PyArg_ParseTuple(pResult_, "i|s",  &result, &ret) == -1)
		{
			LogDebug("error return\n");
		}
		else
		{
			//LogDebug("return from py: %d, %s\n", result, ret);
		}
	}
	return 0;
}

int Game_Cpp2Py::OnConnector(Package* pack)
{
	LogDebug("Game_Cpp2Py::OnConnector\n");

	string body_data = (char*)pack->GetBodyData();

	uint16_t command_id = pack->GetCommandId();

	uint16_t sub_id = pack->GetSubId();

	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("OnConnector"), const_cast<char*>("iis"), command_id, sub_id, body_data.c_str());
	if (pResult_ != NULL)
	{
		char* ret;
		int result = 0;

		if (PyArg_ParseTuple(pResult_, "i|s",  &result, &ret) == -1)
		{
			LogDebug("error return\n");
		}
		else
		{
			//LogDebug("return from py: %d, %s\n", result, ret);
		}
	}
	return 0;
}

int Game_Cpp2Py::OnTick()
{
	pResult_ = PyObject_CallMethod(pModule_, const_cast<char*>("OnTick"), const_cast<char*>("i"), 0);
	if (pResult_ != NULL)
	{
		char* ret;
		int result = 0;

		if (PyArg_ParseTuple(pResult_, "i|s",  &result, &ret) == -1)
		{
			LogDebug("error return\n");
		}
		else
		{
			//LogDebug("return from py: %d, %s\n", result, ret);
		}
	}
	return 0;
}
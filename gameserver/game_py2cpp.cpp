#include <Python.h>
#include "util.h"
#include "message_queue.h"
#include "game_py2cpp.h"
#include "logger.h"

static PyObject*
call_client(PyObject* self, PyObject* args)
{
	//LogDebug("py->cpp call_client\n");

	int uniqid;
	int command_id;  //用uint16_t会读不到数据
	int sub_id;
	char* str;
	if (!PyArg_ParseTuple(args, "iiis", &uniqid, &command_id, &sub_id, &str))
		Py_RETURN_NONE;
	MessageQueue::getSingleton().MQ2C_Push(uniqid, command_id, sub_id, str);
	
	Py_RETURN_NONE;
}

static PyObject*
broad_cast(PyObject* self, PyObject* args)
{
	int command_id;
	int sub_id;
	int all;
	char* str;
	char* bc_list;
	if (!PyArg_ParseTuple(args, "iiiss", &command_id, &sub_id, &all, &str, &bc_list))
		Py_RETURN_NONE;

	bool flag = all > 0 ? true : false;
	MessageQueue::getSingleton().MQ2C_Push_Broadcast(command_id, sub_id, flag, str, bc_list);
	
	Py_RETURN_NONE;
}

static PyObject*
add_timer(PyObject* self, PyObject* args)
{
	//LogDebug("py->cpp add_timer\n");

	int delay;
	int repeat_sec;
	if (!PyArg_ParseTuple(args, "ii", &delay, &repeat_sec))
		Py_RETURN_NONE;

	uint32_t timerid = MessageQueue::getSingleton().MQ2C_Push_Timer(delay, repeat_sec);

	Py_BuildValue("i", timerid);
}

static PyObject*
on_load(PyObject* self, PyObject* args)
{
	//LogDebug("py->cpp on_load\n");

	int return_id;
	char* str;
	if (!PyArg_ParseTuple(args, "is", &return_id, &str))
		Py_RETURN_NONE;
	MessageQueue::getSingleton().MQ2C_Push_Connector(SQL_QUERY, return_id, str);
	
	Py_RETURN_NONE;
}

static PyObject*
on_save(PyObject* self, PyObject* args)
{
	//LogDebug("py->cpp on_save\n");

	char* str;
	if (!PyArg_ParseTuple(args, "s", &str))
		Py_RETURN_NONE;
	MessageQueue::getSingleton().MQ2C_Push_Connector(SQL_UPDATE, 0, str);
	
	Py_RETURN_NONE;
}

static PyObject*
log_info(PyObject* self, PyObject* args)
{
	//LogDebug("py->cpp log_info\n");

	int type;
	char* str;
	if (!PyArg_ParseTuple(args, "is", &type, &str))
		Py_RETURN_NONE;

	//LogDebug("py->cpp log_info,%d,%s\n", type, str);

	if (type & LOG_DEBUG)
	{
		LogDebug("%s",str);
	}
	else if (type & LOG_INFO)
	{
		LogInfo("%s",str);
	}
	else if (type & LOG_WARNING)
	{
		LogWarning("%s",str);
	}
	else if (type & LOG_ERROR)
	{
		LogError("%s",str);
	}
	
	Py_RETURN_NONE;
}

/*
static PyObject*
add_repeat_timer(PyObject* self, PyObject* args)
{

	Py_RETURN_NONE;
}
*/

static PyMethodDef PlutoMethods[] = {
    {"call_client", call_client, METH_VARARGS, "Call client method"},
    {"broad_cast", broad_cast, METH_VARARGS, "broadcast method"},
    {"add_timer", add_timer, METH_VARARGS, "add_timer method"},
    {"on_load", on_load, METH_VARARGS, "on_load method"},
    {"on_save", on_save, METH_VARARGS, "on_save method"},
    {"log_info", log_info, METH_VARARGS, "log info"},
    //{"add_repeat_timer", add_repeat_timer, METH_VARARGS, "add_repeat_timer method"},
    {NULL, NULL, 0, NULL}
};

Game_Py2Cpp::Game_Py2Cpp()
{
	Py_InitModule("plutoEngine", PlutoMethods);
}

Game_Py2Cpp::~Game_Py2Cpp()
{

}

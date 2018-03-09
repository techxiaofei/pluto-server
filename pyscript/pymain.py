# -*- coding: utf-8 -*-


try:
	from server import GameServerRepo,GameServer
	from server.ProtocolDispatcher import *
	import json
	from common import Logger
	from logic.login.LoginMgr import LoginMgr
except:
	import traceback
	traceback.print_exc()

def init():
	'''初始化python server'''
	Logger.LogInfo("Py Init")
	GameServerRepo.game_server = GameServer.GameServer()

def OnServer(uniqid, command_id, sub_id, data):
	"""接收协议数据"""
	Logger.LogDebug("OnServer:%d,%d,%d,%s"%(uniqid, command_id, sub_id, data))
	
	dict = None
	try:
		dict = json.loads(data)
		print dict.keys()
	except:
		import traceback
		traceback.print_exc()
	
	if dict is None:
		Logger.LogError("OnServer Error: dict is None:%d,%d,%d,%s"%(uniqid, command_id, sub_id, data))
		return
	
	dispatch_protocol(uniqid, command_id, sub_id, dict)
	
	return 0, "success"

def OnFdClosed(sockfd):
	"""关闭socket，删除player"""
	Logger.LogDebug("Py OnFdClosed:%d" % sockfd)
	
	LoginMgr().player_sockfd_close(sockfd)
	
	return 0, "success"

def OnTimer(timerid):
	Logger.LogDebug("Py OnTimer:%d" % timerid)
	
	from common import Timer
	Timer.onTimer(timerid)
	
	return 0, "success"

def OnTick(timerid):
	"""每秒钟执行"""
	GameServerRepo.game_server.OnTick()
	return 0, "success"

def OnConnector(command_id, sub_id, data):
	"""游戏服作为客户端连接的服务器返回的数据"""
	Logger.LogDebug("OnConnector: %d,%d,%s" % (command_id, sub_id, data))
	
	dict = None
	
	try:
		dict = json.loads(data)
		print dict.keys()
	except:
		import traceback
		traceback.print_exc()
	
	if dict is None:
		Logger.LogError("CallMethod Error: dict is None:%d,%d"%(command_id, sub_id, data))
		return
	
	from common import SqlExec
	SqlExec.OnExecCallback(sub_id, dict)
	
	return 0, "success"

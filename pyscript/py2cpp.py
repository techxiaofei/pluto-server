# -*- coding: utf-8 -*-
__author = "majianfei"

import plutoEngine

def call_client(uniqid, command_id, sub_id, data):
	"""发送协议数据"""
	try:
		ret = plutoEngine.call_client(uniqid, command_id, sub_id, data)
	except:
		import traceback
		traceback.print_exc()

def broad_cast(command_id, sub_id, data, sockfds = None):
	"""广播协议数据, sockfds = None代表广播所有"""
	if sockfds is None:
		all = 1
		sockfd_str = ""
	else:
		all = 0
		sockfd_str = ",".join(sockfds)
 	print "broad_cast",command_id,sub_id,all,data,sockfd_str
	try:
		ret = plutoEngine.broad_cast(command_id, sub_id, all, data, sockfd_str)
	except:
		import traceback
		traceback.print_exc()

def add_timer(delay, repeat_sec):
	"""注册定时器"""
	try:
		ret = plutoEngine.add_timer(delay, repeat_sec)
		return ret
	except:
		import traceback
		traceback.print_exc()

def on_load(sql_id, sql_query):
	"""从dbserver取数据"""
	try:
		ret = plutoEngine.on_load(sql_id, sql_query)
		return ret
	except:
		import traceback
		traceback.print_exc()
	
def on_save(sql_query):
	"""向dbserver存数据，不回调"""
	try:
		ret = plutoEngine.on_save(sql_query)
		return ret
	except:
		import traceback
		traceback.print_exc()

def log_info(log_level, str):
	"""记录log信息"""
	try:
		ret = plutoEngine.log_info(log_level, str)
		return ret
	except:
		import traceback
		traceback.print_exc()
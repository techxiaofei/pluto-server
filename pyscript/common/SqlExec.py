# -*- coding: utf-8 -*-
__author = "majianfei"

import json
import py2cpp
import Logger

class SqlManager(object):
	def __init__(self):
		super(SqlManager, self).__init__()
		self.sql_dict = {}
		self._sql_id = 1
	
	def add_sql_id(self):
		self._sql_id += 1
		if self._sql_id >= 32000:  # todo ,uint16_t
			self._sql_id = 1
		return self._sql_id
	
	def OnExec(self, proxy):
		sql_id = self.add_sql_id()
		py2cpp.on_load(sql_id, proxy.sql_query)
		self.sql_dict[sql_id] = proxy
		Logger.LogDebug("OnExec:%d"%sql_id)
	
	def OnCallback(self, sql_id, data):
		Logger.LogDebug("OnCallback:%d" % sql_id)
		proxy = self.sql_dict.get(sql_id)
		if not proxy:
			return
		proxy.callback(data)
		del self.sql_dict[sql_id]
	
	
_sqlmanager = SqlManager()

class SqlProxy(object):
	def __init__(self, func, sql_query, args, kwargs, sqlmanager):
		self.func = func
		self.sql_query = sql_query
		self.args = args
		self.kwargs = kwargs
		self.sqlmanager =sqlmanager
		sqlmanager.OnExec(self)
		
	def callback(self, data):
		Logger.LogDebug("sql_callback")
		self.func(data, *self.args, **self.kwargs)


def SqlExec(sql, func = None, *args, **kwargs):
	sql_query = json.dumps({"sql":sql})
	
	if func is None:
		py2cpp.on_save(sql_query)
	else:
		SqlProxy(func, sql_query, args, kwargs, _sqlmanager)

def OnExecCallback(sql_id, data):
	_sqlmanager.OnCallback(sql_id, data)


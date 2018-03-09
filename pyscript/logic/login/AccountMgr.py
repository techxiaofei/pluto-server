# -*- coding: utf-8 -*-
__author = "majianfei"

from common.Singleton import Singleton
from common import SqlExec
from server.ClientProxy import ClientProxy
from server.ProtocolDefines import *
from common import Interface
import json

@Singleton
class AccountMgr(object):
	'''账号相关，登录前处理'''
	def __init__(self):
		self._table = "account"
		self._primiry_key = "id"  # 主键自增id，作为玩家唯一id
		self._user_id = "uid"  # 用户输入或者sdk返回的用户id
		self._column_list = ("uid", "channel", "os", "admin_lv")
		
		self._account_list = {}  # uid: clientproxy
	
	def __getitem__(self, uid):
		return self._account_list.get(uid)
	
	def __setitem__(self, uid, clientproxy):
		self._account_list[uid] = clientproxy
	
	def __delitem__(self, uid):
		if uid in self._account_list:
			del self._account_list[uid]
	
	@property
	def table(self):
		return self._table
	
	@property
	def uid(self):
		return self._user_id
	
	def login_account(self, sockfd, params):
		"""一般作为客户端第一条协议"""
		# 条件判断
		
		# 判断成功
		if self.uid not in params:
			
			return
		uid = params[self.uid]
		clientproxy = ClientProxy(sockfd)
		self[uid] = clientproxy
		
		sql = Interface.join_get_sql(self.table, self.uid, uid)
		print "sql:",sql
		
		SqlExec.SqlExec(sql, self._get_uniqid_cb, json.dumps(params))
	
	def user_check(self, sockfd, params):
		"""根据发给客户端的uniqid判断有没有player"""
		if self.uid not in params:
			
			return
		uid = params[self.uid]
		uniqid = params['id']
		clientproxy = self._account_list.get(uid)
		if not clientproxy:
			return
		
		sql = Interface.join_get_sql("player", 'id', uniqid)
		
		SqlExec.SqlExec(sql, self._check_user_cb, json.dumps(params))
	
	
	
	
	
	def _get_uniqid_cb(self, data, params):
		params = json.loads(params)
		
		save_data = {key: str(value) for key, value in params.items() if key in self._column_list}
		print "save_data:", save_data
		
		if not data:  # 没有数据，插入
			sql = Interface.join_insert_sql(self.table, save_data)
			print "insert sql:",sql
			SqlExec.SqlExec(sql)
			
			sql = Interface.join_get_sql(self.table, self.uid, params[self.uid])
			SqlExec.SqlExec(sql, self._get_uniqid_cb2, json.dumps(params))
			
		else:  # 有数据，更新
			self._get_uniqid_cb2(data, json.dumps(params))

	def _get_uniqid_cb2(self, data, params):
		"""最终获取好数据的接口"""
		params = json.loads(params)
		uid = params[self.uid]
		if not data:
			clientproxy = self[uid]
			if clientproxy:
				clientproxy.send_data(COMMAND_ID_LOGIN, SUB_LOGIN_ACCOUNT_GET, {'result':0})
				del self[uid]
		else:
			clientproxy = self[uid]
			if clientproxy:
				clientproxy.send_data(COMMAND_ID_LOGIN,SUB_LOGIN_ACCOUNT_GET, {'result':1,'id':data['id']})
	
	def _check_user_cb(self, data, params):
		params = json.loads(params)
		uid = params[self.uid]
		uniqid = params['id']
		if not data:  # 没有玩家，告诉客户端创建
			clientproxy = self[uid]
			if clientproxy:
				clientproxy.send_data(COMMAND_ID_LOGIN,SUB_LOGIN_USER_CHECK, {'result':1,'id':uniqid, 'new':1})
		else:
			clientproxy = self[uid]
			if clientproxy:
				clientproxy.send_data(COMMAND_ID_LOGIN, SUB_LOGIN_USER_CHECK, {'result':1,'id':uniqid, 'new':0})
	
	
		
		
		
	
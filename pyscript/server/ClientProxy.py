# -*- coding: utf-8 -*-
__author = "majianfei"

import json
import py2cpp
from server.ClientProxyManager import ClientProxyManager

class ClientProxy(object):
	def __init__(self, sockfd, uniqid = 0):
		super(ClientProxy, self).__init__()
		
		self._sockfd = sockfd
		self._uniqid = uniqid
	
	@property
	def sockfd(self):
		return self._sockfd
	
	def close(self):
		"""关闭socket"""
		ClientProxyManager().remove_client_proxy(self)
	
	@property
	def uniqid(self):
		return self._uniqid
	
	@uniqid.setter
	def uniqid(self, value):
		self._uniqid = value
	
	def send_data(self, command_id, sub_id, data):
		"""向客户端发送"""
		py2cpp.call_client(self.sockfd, command_id, sub_id, json.dumps(data))
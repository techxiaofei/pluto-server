# -*- coding: utf-8 -*-
__author = "majianfei"

from common.Singleton import Singleton,Singleton2

@Singleton
class ClientProxyManager(object):
	def __init__(self):
		self.client_map = {}
	
	def get_client_proxy(self, sockfd):
		return self.client_map.get(sockfd, None)
	
	def set_client_proxy(self, client_proxy):
		self.client_map[client_proxy.sockfd] = client_proxy
	
	def remove_client_proxy(self, clientproxy):
		if clientproxy.sockfd in self.client_map:
			del self.client_map[clientproxy.sockfd]
	
	def get_client_proxy_by_uniqid(self, uniqid):
		for i, obj in self.client_map.iteritems():
			if obj.uniqid == uniqid:
				return obj
		return None
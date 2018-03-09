# -*- coding: utf-8 -*-
__author = "majianfei"

"""
装饰协议,可选。一般除了login之外的协议都尽量使用装饰器装饰
"""

from logic.player.PlayerMgr import PlayerMgr
from server.ClientProxyManager import ClientProxyManager

def protocol_parse(func):
	def wrapped(sockfd, data):
		clientproxy = ClientProxyManager().get_client_proxy(sockfd)
		uniqid = 0
		player = None
		if clientproxy is not None:
			uniqid = clientproxy.uniqid
			if uniqid > 0:
				player = PlayerMgr().GetObj(uniqid)
		return func(uniqid, player, data)
	return wrapped
# -*- coding: utf-8 -*-
__author = "majianfei"

from common.Singleton import Singleton
from common import Logger
from server.ClientProxy import ClientProxy
from server.ClientProxyManager import ClientProxyManager
from server.ProtocolDefines import *
from logic.player.PlayerMgr import PlayerMgr

@Singleton
class LoginMgr(object):
	'''负责玩家登陆时的处理'''
	def __init__(self):
		self.login_list = {}  # 登陆的列表，不存盘
	
	def create_player(self, sockfd, data):
		Logger.LogDebug("create_player")
		
		uniqid = int(data['id'])
		clientproxy = ClientProxy(sockfd, uniqid)
		ClientProxyManager().set_client_proxy(clientproxy)
		
		player = PlayerMgr().CreateObj(uniqid, clientproxy)
		#player.init_channel(1)
		
		result = {"result":1}
		player.send_data(COMMAND_ID_LOGIN, SUB_LOGIN_CREATE, result)
	
	def login_player(self, sockfd, data):
		Logger.LogDebug("login_player")
		
		uniqid = data['id']
		
		# 判断是不是relogin
		if self._check_relogin(uniqid):
			Logger.LogDebug("relogin error: uniqid,%d"%uniqid)
			self._kick_player(uniqid)
			
		
		clientproxy = ClientProxy(sockfd, uniqid)
		ClientProxyManager().set_client_proxy(clientproxy)
		PlayerMgr().LoadObj(uniqid, self._login_callback, clientproxy)
	
	def login_out_player(self, sockfd, data):
		'''登出'''
		client_proxy = ClientProxyManager().get_client_proxy(sockfd)
		if client_proxy:
			player = PlayerMgr().GetObj(client_proxy.uniqid)
			if player:
				player.destroy()
			
			ClientProxyManager().remove_client_proxy(client_proxy)
			PlayerMgr().DelObj(client_proxy.uniqid)
			
			print "len player:", len(PlayerMgr().SaveObjs())
			print "len proxy :", len(ClientProxyManager().client_map)
	
	def player_sockfd_close(self, sockfd):
		'''对端关闭socket'''
		client_proxy = ClientProxyManager().get_client_proxy(sockfd)
		if client_proxy:
			player = PlayerMgr().GetObj(client_proxy.uniqid)
			if player:
				player.destroy()
			
			ClientProxyManager().remove_client_proxy(client_proxy)
			PlayerMgr().DelObj(client_proxy.uniqid)
			
			print "len player:",len(PlayerMgr().SaveObjs())
			print "len proxy :",len(ClientProxyManager().client_map)
	
	def _login_callback(self, uniqid, player):
		Logger.LogDebug("login_callback")
		if player is None:
			Logger.LogError("login_callback error,not exists")
			#TODO,登陆失败后清理数据
			
			clientproxy = ClientProxyManager().get_client_proxy_by_uniqid(uniqid)
			if clientproxy:
				result = {"result": -1}
				clientproxy.send_data(COMMAND_ID_LOGIN,SUB_LOGIN_LOGIN, result)
				ClientProxyManager().remove_client_proxy(clientproxy)
		else:
			PlayerMgr().AddObj(uniqid, player)
			
			result = {"result": 1, "name": player.name(), "sex": player.sex()}
			player.send_data(COMMAND_ID_LOGIN, SUB_LOGIN_LOGIN, result)
	
	
	def _check_relogin(self, uniqid):
		'''登陆判断已经存在'''
		clientproxy = ClientProxyManager().get_client_proxy_by_uniqid(uniqid)
		return True if clientproxy else False
	
	def _kick_player(self, uniqid):
		player = PlayerMgr().GetObj(uniqid)
		if player:
			player.destroy()
		
		clientproxy = ClientProxyManager().get_client_proxy_by_uniqid(uniqid)
		ClientProxyManager().remove_client_proxy(clientproxy)
		PlayerMgr().DelObj(uniqid)
	
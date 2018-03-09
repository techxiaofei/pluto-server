# -*- coding: utf-8 -*-
__author = "majianfei"

#from common.Singleton import Singleton
from server.ProtocolDefines import *
from logic.player.PlayerMgr import PlayerMgr
from logic.defines import *
import py2cpp
import json

#@Singleton
class ChatRoom(object):
	"""聊天室"""
	def __init__(self):
		self._chat_channel = {}
	
	def __getitem__(self, channel):
		if channel not in self._chat_channel:
			return None
		return self._chat_channel[channel]
	
	def add_player_2_channel(self, channel, uniqid):
		if channel not in self._chat_channel:
			self._chat_channel[channel] = {}
		self._chat_channel[channel][uniqid] = 1
	
	def talk(self, player, data):
		if player is None:
			print "talk player is None"
			return
		
		channel = player.channel()
		if channel not in CHANNEL_LIST:
			return
		
		player_id_list = self[channel]
		if not player_id_list:
			pass
		
		sockfds = []
		for uniqid in player_id_list.keys():
			#if uniqid == player.uniqid:
			#	continue
			obj = PlayerMgr().GetObj(uniqid)
			if not obj:
				continue
			if not obj.clientproxy:
				continue
			sockfd = obj.clientproxy.sockfd
			sockfds.append("%d"%sockfd)
		
		#sockfds.append("111111")
		
		resp = {"msg":data["msg"]}
		py2cpp.broad_cast(COMMAND_ID_PLAYER, SUB_PLAYER_TALK, json.dumps(resp), sockfds)
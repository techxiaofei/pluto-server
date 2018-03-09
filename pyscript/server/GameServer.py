# -*- coding: utf-8 -*-

from common import Logger
from logic.player.PlayerMgr import PlayerMgr
from logic.player.ChatRoom import ChatRoom
from server import GameServerRepo

class GameServer(object):
	'''主Server入口'''
	def __init__(self):
		Logger.LogDebug("GameServer init")
		GameServerRepo.g_chat_room = ChatRoom()
		
	def OnTick(self):
		"""每加一个存盘管理器需要在这里添加OnTick"""
		PlayerMgr().OnTick()
		
		
	
	
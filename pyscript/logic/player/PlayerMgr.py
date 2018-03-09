# -*- coding: utf-8 -*-
__author = "majianfei"

from common.SaveObjMgr import SaveObjMgr
from common.Singleton import Singleton,Singleton2
from server.ClientProxy import ClientProxy
from server.ClientProxyManager import ClientProxyManager
from logic.player.Player import Player
from common import Logger

#@Singleton
class PlayerMgr(SaveObjMgr):
	__metaclass__ = Singleton2
	def __init__(self):
		super(PlayerMgr, self).__init__(Player)
	
	def player_process(self, sockfd, data):
		Logger.LogDebug("player_process")



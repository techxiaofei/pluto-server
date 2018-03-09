# -*- coding: utf-8 -*-

from common import Logger
from common.SaveObj import SaveObj
from logic.database import DATABASE
from server import GameServerRepo
	
class Player(SaveObj):
	def __init__(self, uniqid, clientproxy = None):
		super(Player, self).__init__(DATABASE["player"])
		self._uniqid = uniqid
		self._clientproxy = clientproxy
		if clientproxy is not None:
			self._clientproxy.uniqid = uniqid
		self.setId(uniqid)
		
		self._data = {}
		self._channel = 0
		self._pos = 0
		
		Logger.LogDebug("player init")
	
	def after_create(self):
		"""创建完player之后创建玩家其他相关的存盘对象"""
		self.setName("default")
		self.setSex(1)
		self.setLv(1)
		self.setData({})
		self.createDb()
		# TODO, 创建此玩家其他存盘对象
	
	def after_load(self, success):
		"""Load完player之后Load其他存盘对象"""
		Logger.LogDebug("after_load,%d,%s,%s"%(self.id(), self.name(),str(self.data())))
		
		GameServerRepo.g_chat_room.add_player_2_channel(self._channel, self._uniqid)
		# TODO, Load此玩家其他存盘对象
	
	def loadData(self, data):
		self._channel = data.get("channel", 1)
		self._pos = data.get("pos", 1)
		
		print "======================>loadData",data
	
	def saveData(self):
		data = {}
		data["channel"] = self._channel
		data["pos"] = self._pos
		return data
	
	def set_channel(self):
		self.update()
		self._channel = 1
	
	def channel(self):
		return self._channel
	
	@property
	def uniqid(self):
		return self._uniqid
	
	@property
	def clientproxy(self):
		return self._clientproxy
	
	@clientproxy.setter
	def clientproxy(self, value):
		self._clientproxy = value
	
	def destroy(self):
		'''存盘及删除相关引用'''
		super(Player,self).destroy()
		# 删除此玩家身上的存盘对象
		#ChatMgr().destroy(self.uniqid)
		self.clientproxy = None
		
	def send_data(self, command_id, sub_id, data):
		self.clientproxy.send_data(command_id, sub_id, data)

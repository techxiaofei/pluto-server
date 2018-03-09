# -*- coding: utf-8 -*-
__author = "majianfei"

from common import Logger
from server.ProtocolParse import  protocol_parse
from logic.player.PlayerMgr import PlayerMgr
from logic.player.ChatRoom import ChatRoom
from server.GameServerRepo import g_chat_room

@protocol_parse
def proto_2_1(uniqid, player, data):
	Logger.LogDebug("proto_1_1%d,%s"%(uniqid, str(data)))
	player.set_channel()

@protocol_parse
def proto_2_2(uniqid, player, data):
	player.setLv(data['lv'])

@protocol_parse
def proto_2_3(uniqid, player, data):
	Logger.LogDebug("proto_2_3")
	g_chat_room.talk(player, data)
	
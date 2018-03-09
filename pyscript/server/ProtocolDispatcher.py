# -*- coding: utf-8 -*-
__author = "majianfei"

from server.ProtocolDefines import *
from common import Logger

def dispatch_protocol(uniqid, command_id, sub_id, data):
	path = proto_2_path.get(command_id, None)
	if path is None:
		Logger.LogError("Dispatch_Protocol error:command_id:%d"%command_id)
		return
	
	try:
		module_name = __import__('logic.%s.zprotocol' % path, fromlist=[''])
		attr = getattr(module_name, 'proto_%d_%d' % (command_id, sub_id))
		attr(uniqid, data)
	except:
		import traceback
		traceback.print_exc()
		return
	
	Logger.LogDebug("Dispatch_Protocol finish")
	
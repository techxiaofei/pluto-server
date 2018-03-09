# -*- coding: utf-8 -*-
__author = "majianfei"

class SaveObjMgr(object):
	"""存盘对象管理器，各存盘对象继承"""
	def __init__(self, ClassName):
		super(SaveObjMgr, self).__init__()
		self.ClassName = ClassName
		self.save_objs = {}
	
	def GetObj(self, id):
		return self.save_objs.get(id, None)
	
	def SaveObjs(self):
		return self.save_objs
	
	def AddObj(self, id ,obj):
		if id in self.save_objs:
			pass
		self.save_objs[id] = obj
	
	def DelObj(self, id):
		if id in self.save_objs:
			del self.save_objs[id]
	
	def LoadObj(self, id, func, clientproxy = None):
		"""Load一个存盘对象到内存"""
		obj = self.GetObj(id)
		if obj:
			func(id, obj)
		else:
			if clientproxy is not None:
				obj = self.ClassName(id, clientproxy)
			else:
				obj = self.ClassName(id)
			obj.queryDb(func)
	
	def CreateObj(self, id, clientproxy = None):
		"""创建一个存盘对象"""
		obj = self.GetObj(id)
		if obj:
			return obj
		if clientproxy is not None:
			obj = self.ClassName(id, clientproxy)
		else:
			obj = self.ClassName(id)
		obj.after_create()
		self.AddObj(id, obj)
		return obj
	
	def OnTick(self):
		"""每一秒触发"""
		for id, obj in self.save_objs.iteritems():
			obj.OnTick()
	
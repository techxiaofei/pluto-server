# -*- coding: utf-8 -*-
__author = "majianfei"

def not_persistent(self):
	return False

def is_persistent(_self):
	return True

def Persistent(klass):
	"""
	类的decorator，用来修饰Entity的子类。如：
	@Persistent
	class ServerAvatar(AvatarEntity):
		...
	这样的类才会被序列化到mongodb中
	"""
	klass.is_persistent = is_persistent
	return klass

def PersistentTime(savetime = None):
	"""
	可以指定存盘时间的Persistent、单位为s
	@PersistentTime(15)
	"""
	def _PersistentTime(klass):
		klass.is_persistent = is_persistent
		if savetime is not None:
			klass.get_persistent_time = lambda _self:savetime
		return klass

	return _PersistentTime
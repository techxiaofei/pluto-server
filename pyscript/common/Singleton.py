# -*- coding: utf-8 -*-
__author = "majianfei"

def Singleton(class_):
	"""使用装饰器实现单例,缺点是被装饰的类没办法使用super"""
	instances = {}

	def getinstance(*args, **kwargs):
		if class_ not in instances:
			instances[class_] = class_(*args, **kwargs)
		return instances[class_]
	return getinstance


class Singleton2(type):
	"""使用metaclass实现单例类"""
	def __call__(cls, *args, **kwargs):
		if not hasattr(cls, '_instance'):
			cls._instance = super(Singleton2, cls).__call__(*args, **kwargs)
		return cls._instance


class Singleton3(object):
	"""使用继承实现单例"""
	def __new__(cls, *args, **kwargs):
		if not hasattr(cls, '_instance'):
			orig = super(Singleton3, cls)
			cls._instance = orig.__new__(cls, *args, **kwargs)
		return cls._instance
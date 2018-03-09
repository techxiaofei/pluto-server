# -*- coding: utf-8 -*-
__author = "majianfei"

import types
import inspect
from logic.chat.Chat import Chat

SaveObjMembers ={
	"Chat": Chat,
}

class MetaClass(type):
	def __init__(cls, name, bases, dict):
		_class_filter_list = ('__module__', '__doc__')

		super(MetaClass, cls).__init__(name, bases, dict)
		funset = set()

		#member_path = os.path.split(__file__)[0] + '/../../avatar_member'# 'script/modules/entity/avatar_member'  # 'script/modules/entity/avatar_member'#os.path.split(__file__)[0] + '/avatar_member'
		#AvatarMembers = EntityScanner.scan_entity_classes(member_path, AvatarMember)
		for memClsName, memCls in SaveObjMembers.iteritems():
			#处理函数
			for funcName, func in inspect.getmembers(memCls, inspect.ismethod):
				#print 'funcName:', funcName, func.im_func, memClsName
				#去掉一些特别的函数
				if funcName in ('__init__', '__new__', '__call__', '__setattr__', '__str__'):
					continue
				#判断是否有重复
				if funcName in funset:
					raise Exception('got duplicate method name: %s, %s' % (memCls.__name__, funcName))
				#设置属性
				funset.add(funcName)
				setattr(cls, funcName, func.im_func)
			#处理属性
			for membName, memb in inspect.getmembers(memCls):
				if membName in _class_filter_list:
					continue

				if isinstance(memb, types.IntType) or isinstance(memb, types.LongType) or \
					isinstance(memb, types.StringType) or isinstance(memb, types.ListType) or \
					isinstance(memb, types.DictType) or isinstance(memb, types.TupleType) or \
					isinstance(memb, types.FloatType) or isinstance(memb, set) or \
					isinstance(memb, frozenset):
					#检查属性是否重复
					if hasattr(cls, membName):
						raise Exception('got duplicate attr name: %s, %s' % (memCls.__name__, membName))
					#设置属性内容
					setattr(cls, membName, memb)
# -*- coding: utf-8 -*-
__author = "majianfei"

import py2cpp

class TimerManager(object):
	def __init__(self):
		self.timers = {}
	
	def addTimer(self, proxy):
		timerID = py2cpp.add_timer(proxy.delay, 0)
		self.timers[timerID] = proxy
		#print "addtimer===>",len(self.timers), timerID
		return timerID
	
	def addRepeatTimer(self, proxy, repeat_sec):
		timerID = py2cpp.add_timer(proxy.delay, repeat_sec)
		self.timers[timerID] = proxy
		return timerID
	
	def delTimer(self, timerID):
		self.timers.pop(timerID, None)
		py2cpp.del_timer(timerID)
	
	def onTimer(self, timerID):
		#print "TiemrManager OnTimer",len(self.timers),timerID
		proxy = self.timers.get(timerID, None)
		if not proxy:
			#print "===============null"
			return
		if proxy.repeat_sec == 0:
			del self.timers[timerID]
		proxy.tick()

_timermanager = TimerManager()

class TimerProxy(object):
	def __init__(self, delay, func, args, kwargs, repeat_sec, timermanager):
		self.delay = delay
		self.func = func
		self.args = args
		self.kwargs = kwargs
		self.repeat_sec = repeat_sec
		self.timermanager = timermanager
		if repeat_sec > 0:
			self.timerID = timermanager.addRepeatTimer(self, repeat_sec)
		else:
			self.timerID = timermanager.addTimer(self)
		
	def cancel(self):
		self.timermanager.delTimer(self.timerID)
	
	def tick(self):
		self.func(*self.args, **self.kwargs)

def addTimer(delay, func, *args, **kwargs):
	return TimerProxy(delay, func, args, kwargs, 0, _timermanager)

def addRepeatTimer(delay, repeat_sec, func, *args, **kwargs):
	return TimerProxy(delay, func, args, kwargs, repeat_sec, _timermanager)

def onTimer(timerID):
	_timermanager.onTimer(timerID)

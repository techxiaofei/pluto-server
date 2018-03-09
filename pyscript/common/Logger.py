# -*- coding: utf-8 -*-
__author = "majianfei"

import py2cpp

# LOG LEVEL
TYPE_LOG_DEBUG = 0x01
TYPE_LOG_INFO = 0x02
TYPE_LOG_WARNING = 0x04
TYPE_LOG_ERROR = 0x08

def LogDebug(str):
	py2cpp.log_info(TYPE_LOG_DEBUG, str)

def LogInfo(str):
	py2cpp.log_info(TYPE_LOG_INFO, str)

def LogWarning(str):
	py2cpp.log_info(TYPE_LOG_WARNING, str)

def LogError(str):
	py2cpp.log_info(TYPE_LOG_ERROR, str)
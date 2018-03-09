# -*- coding: utf-8 -*-
__author = "majianfei"

"""
用户协议定义：
command_id:协议号
sub_id:子协议号
用双层协议号来定义某条协议
"""

#command_id，协议ID

COMMAND_ID_LOGIN = 1  # 登陆创角相关
COMMAND_ID_PLAYER = 2  # 玩家相关
COMMAND_ID_CHAT = 3  # 聊天相关

proto_2_path = {
	COMMAND_ID_LOGIN : "login",
	COMMAND_ID_PLAYER : "player",
	COMMAND_ID_CHAT : "chat",  # 暂时没用
}


#sub_id, 子协议ID

# COMMAND_ID_LOGIN = 1==========>
SUB_LOGIN_ACCOUNT_GET = 1  # 账号获取
SUB_LOGIN_USER_CHECK = 2  # 玩家检测
SUB_LOGIN_CREATE = 11  # 创建角色
SUB_LOGIN_LOGIN = 12  # 登陆角色
SUB_LOGIN_LOGIN_OUT = 99  # 登出角色
# COMMAND_ID_LOGIN = 1==========<

# COMMAND_ID_PLAYER = 2==========>
SUB_PLAYER_LV_UP = 1  # 角色升级
SUB_PLAYER_SET_NAME = 2  # 设置名字
SUB_PLAYER_TALK = 3     # 聊天
# COMMAND_ID_PLAYER = 2==========<

# COMMAND_ID_CHAT = 3==========>
SUB_CHAT_TALK = 1  # 聊天

# COMMAND_ID_CHAT = 3==========<

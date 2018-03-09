# -*- coding: utf-8 -*-
__author = "majianfei"


from logic.login.LoginMgr import LoginMgr
from logic.login.AccountMgr import AccountMgr
from logic.defines import *

def proto_1_1(sockfd, data):
	AccountMgr().login_account(sockfd, data)

def proto_1_2(sockfd, data):
	AccountMgr().user_check(sockfd, data)

def proto_1_11(sockfd, data):
	LoginMgr().create_player(sockfd, data)

def proto_1_12(sockfd, data):
	LoginMgr().login_player(sockfd, data)

def proto_1_99(sockfd, data):
	LoginMgr().login_out_player(sockfd, data)
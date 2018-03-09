# coding:utf8

import time

from socket import AF_INET, SOCK_STREAM, socket
from thread import start_new
import struct, json
import sys
import time

HEAD_LEN = 8

global_data = ""

def sendData(command_id, sub_id, data):
	json_data = json.dumps(data)
	length = HEAD_LEN + len(json_data)

	data = struct.pack('!ihh', length, command_id, sub_id)
	senddata = data + json_data
	print "==============>",len(senddata)
	return senddata


def resolveRecvdata():
	global global_data

	#print "data",len(global_data)
	
	if len(global_data) < HEAD_LEN:
		print "error head short than 8:",len(global_data)
		return
		

	length, command, reverse = struct.unpack('!ihh', global_data[0:HEAD_LEN])
	print "receive",length,command,reverse
	if length > 1024 or length < HEAD_LEN:
		print "error receive length:",length
		return
	if len(global_data) < length:
		print "error length is to short:",len(global_data)
		return
	data = global_data[HEAD_LEN:length]
	
	print "data2:",length,"-- ", len(data)

	global_data = global_data[length:]
	
	print "json", data
	#data = data[0:len(data)-1]
	#print "json", data
	#data = '{"result":"yes","lv":1}'
	
	
	
	result = json.loads(data)
	
	print "---",result
	#print "result",result['id']
	#print "lv",result['name']
	
	resolveRecvdata()


def receiveMessage(connection):
	global global_data
	while 1:
		message = connection.recv(1024)
		global_data += message
		resolveRecvdata()
		#print "message",message
		#connection.close()

HOST = '127.0.0.1'
PORT = 5000
ADDR = (HOST, PORT)
client = socket(AF_INET, SOCK_STREAM)
client.connect(ADDR)

if __name__ == "__main__":
	args = sys.argv
	servername = None
	config = None

	start_new(receiveMessage, (client,))
	
	data = {"id":3, "n":"h"}
	
	#json_data = json.dumps(data)
	
	print "before send"
	client.sendall(sendData(1, 3, data))  # 创建角色
	print "111111"
	time.sleep(2)
	print "222222"
	for x in range(10):
		#client.sendall(sendData(3,1,{"msg":"hello%d"%x}))
		time.sleep(2)
	print "333333"
	while(1):
		time.sleep(0.1)
	



# Makefile, 去掉 .so,直接打进,如果想生成 .so,可以使用 common/makefile

CC = g++
CCFLAGS =-g

BIN_HOME = ./bin

LDFLAGS = -L/usr/local/lib

MOBILE_INCL = -I./common -I/usr/include/python2.7 -I/usr/include/mysql
MOBILE_LIB = -lev -lpython2.7 -L/usr/lib64/mysql -lmysqlclient

CCFLAGS += $(MOBILE_INCL)
LDFLAGS += $(MOBILE_LIB)

#-lprotobuf
USER_LDFLAGS = 

COMMON_O = common/buffer.o common/channel.o common/epoll_server.o \
	common/net_util.o common/package.o common/stream.o common/util.o common/cjson.o common/cjson_helper.o \
	common/base_server.o common/message_queue.o common/thread_socket.o common/thread_worker.o common/timer.o \
	common/connector.o common/logger.o common/config_reader.o

GAME_O= gameserver/game_server.o gameserver/game_cpp2py.o gameserver/game_py2cpp.o gameserver/game_worker.o gameserver/main.o

DB_O= dbserver/db_server.o dbserver/db_conn.o dbserver/db_worker.o dbserver/main.o

TARGET = $(BIN_HOME)/gameserver $(BIN_HOME)/dbserver



all:  $(TARGET)

$(BIN_HOME)/gameserver: $(COMMON_O) $(GAME_O)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(USER_LDFLAGS) $^ -o $@

$(BIN_HOME)/dbserver: $(COMMON_O) $(DB_O)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(USER_LDFLAGS) $^ -o $@ 

clean:
	rm -f common/*.o common/*.so common/*.a
	rm -f gameserver/*.o $(BIN_HOME)/gameserver
	rm -f dbserver/*.o $(BIN_HOME)/dbserver
	
# make rule
%.o : %.c
	$(CC) $(CCFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $^ -o $@
#common

CC = g++
AR = ar cru
CCFLAGS = -Wall -g -fPIC
SOFLAGS = -shared

LDFLAGS = -L/usr/local/lib

MOBILE_INCL =
MOBILE_LIB = -lev

CCFLAGS += $(MOBILE_INCL)
LDFLAGS += $(MOBILE_LIB)

#-lprotobuf
USER_LDFLAGS = 

LIBOBJS = buffer.o channel.o epoll_server.o \
	net_util.o package.o stream.o util.o cjson.o cjson_helper.o \
	base_server.o message_queue.o thread_socket.o thread_worker.o timer.o \
	connector.o logger.o config_reader.o

TARGET = libcommon.so libcommon.a

all:  $(TARGET)

libcommon.so: $(LIBOBJS)
	$(CC) $(SOFLAGS) $(LDFLAGS) $(USER_LDFLAGS) $^ -o $@
libcommon.a: $(LIBOBJS)
	$(AR) $@ $^

clean:
	rm -f *.o *.so *.a
	
# make rule
%.o : %.c
	$(CC) $(CCFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $^ -o $@
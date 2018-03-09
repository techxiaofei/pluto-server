#mobilerpc

CC = g++
CCFLAGS = -g
LDFLAGS = -L/usr/local/lib

BIN_HOME = ../bin
#-lprotobuf
USER_LDFLAGS = 

MOBILE_INCL = -I../common -I/usr/include/python2.7
MOBILE_LIB = -L../common -lcommon -lpython2.7

CCFLAGS += $(MOBILE_INCL)
LDFLAGS += $(MOBILE_LIB)

#TMPLIB = .libs

#all: mkdir echo_client echo_server
#mkdir:
#	$(MKDIR) $(TMPLIB)

#LIBOBJS = echo.pb.o
LIBOBJS=game_server.o game_cpp2py.o game_py2cpp.o game_worker.o

TARGET = $(BIN_HOME)/gameserver

all: $(TARGET)

$(BIN_HOME)/gameserver: main.o $(LIBOBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(USER_LDFLAGS) $^ -o $@ 

#echo.pb.cc echo.pb.h: echo.proto
#	$(PROTOC) -I=. --cpp_out=. echo.proto

clean:
	rm -f $(BIN_HOME)/gameserver *.o

# make rule	
%.o : %.c
	$(CC) $(CCFLAGS) -c $^ -o $@

%.o : %.cc
	$(CC) $(CCFLAGS) -c $^ -o $@

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $^ -o $@
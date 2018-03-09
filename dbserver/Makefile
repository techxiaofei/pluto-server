#mobilerpc

CC = g++
CCFLAGS = -g
LDFLAGS = -L/usr/local/lib

BIN_HOME = ../bin

#-lprotobuf
USER_LDFLAGS = 

MOBILE_INCL = -I../common -I/usr/include/mysql
MOBILE_LIB = -L../common -lcommon -L/usr/lib64/mysql -lmysqlclient

CCFLAGS += $(MOBILE_INCL)
LDFLAGS += $(MOBILE_LIB)

#TMPLIB = .libs

#all: mkdir echo_client echo_server
#mkdir:
#	$(MKDIR) $(TMPLIB)

#LIBOBJS = echo.pb.o
LIBOBJS=db_server.o db_conn.o db_worker.o

TARGET = $(BIN_HOME)/dbserver

all: $(TARGET)

$(BIN_HOME)/dbserver: main.o $(LIBOBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(USER_LDFLAGS) $^ -o $@ 

#echo.pb.cc echo.pb.h: echo.proto
#	$(PROTOC) -I=. --cpp_out=. echo.proto

clean:
	rm -f $(BIN_HOME)/dbserver *.o

# make rule	
%.o : %.c
	$(CC) $(CCFLAGS) -c $^ -o $@

%.o : %.cc
	$(CC) $(CCFLAGS) -c $^ -o $@

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $^ -o $@
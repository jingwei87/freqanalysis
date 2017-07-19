CC = g++
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/


all: lrucache.cpp lrucache.h 
	$(CC) -c lrucache.cpp
	$(CC) -c bloom.c
	$(CC) $(CFLAGS) -c fp_index.cpp $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -c container_manager.cpp 

clean:
	rm lrutester.o 
	rm lrucache.o
	rm lrutester


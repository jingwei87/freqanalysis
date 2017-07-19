CC = g++
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/


all:  lrucache.o
	$(CC) -o lrutester lrutester.cpp lrucache.o
lrucache.o: lrucache.cpp lrucache.h 
	$(CC) -c lrucache.cpp
bloom.o : bloom.h bloom.c
	$(CC) -c bloom.c
fp_index.o : fp_index.cpp fp_index.h
	$(CC) $(CFLAGS) -c fp_index.cpp $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
container_manager.o : container_manager.h container_manager.cpp
	$(CC) $(CFLAGS) -c container_manager.cpp 

clean:
	rm lrutester.o 
	rm lrucache.o
	rm lrutester


CC = g++
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/


all: lrucache.cpp lrucache.h 
	$(shell ! test -d "tmp" && mkdir tmp)
	$(shell ! test -d "conf" && mkdir conf)
	$(shell ! test -d "container" && mkdir container)
	$(shell ! test -d "index" && mkdir index)
	$(CC) -c lrucache.cpp
	$(CC) -c bloom.c
	$(CC) -c MurmurHash2.c
	$(CC) $(CFLAGS) -c fp_index.cpp $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -c container_manager.cpp 
	$(CC) -o DDFS DDFS.cpp lrucache.o bloom.o fp_index.o container_manager.o MurmurHash2.o $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
clean:
	rm lrucache.o
	rm bloom.o
	rm DDFS
	rm fp_index.o
	rm container_manager.o
	rm -rf tmp/
	rm -rf container/
	rm -rf index/
	rm -f confg

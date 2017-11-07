CC = g++
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/


all: lrucache.cc lrucache.h 
	$(shell ! test -d "tmp" && mkdir tmp)
	$(shell ! test -d "conf" && mkdir conf)
	$(shell ! test -d "container" && mkdir container)
	$(shell ! test -d "index" && mkdir index)
	$(CC) -c lrucache.cc
	$(CC) -c bloom.c
	$(CC) -c MurmurHash2.c
	$(CC) $(CFLAGS) -c fp_index.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -c container_manager.cc 
	$(CC) -o ddfs ddfs.cc lrucache.o bloom.o fp_index.o container_manager.o MurmurHash2.o $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) -o combined combined.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
clean:
	rm -f lrucache.o
	rm -f bloom.o
	rm -f ddfs
	rm -f fp_index.o
	rm -f container_manager.o
	rm -f combined
	rm -rf tmp/
	rm -rf container/
	rm -rf index/
	rm -f confg

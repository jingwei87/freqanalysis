CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Count Attack

all: Basic_attack

Basic_attack: ./Basic_attack.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(CC) $(CFLAGS) -o Count Basic_count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Attack Basic_attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
clean:
	@rm -f Result
	@rm -f $(MAIN_OBJS)
	@rm -rf dbs/
	@rm -rf tmp/

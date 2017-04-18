CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Basic_attack.o

all: Basic_attack

Basic_attack: ./Basic_attack.cc
	$(shell ! test -d "DBsâ" && mkdir DBs)
	$(shell ! test -d "tmpâ" && mkdir tmp)
	$(CC) $(CFLAGS) -o Count Basic_count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Attack Basic_attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
clean:
	@rm -f Count
	@rm -f Attack
	@rm -f $(MAIN_OBJS)
	@rm -rf DBs/
	@rm -rf tmp/

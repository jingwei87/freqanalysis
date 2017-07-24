CC = g++ 
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Locality_Attack.o

all: Locality_Attack

Locality_Attack: ./Locality_Attack.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(CC) $(CFLAGS) -o scrambling scrambling.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o attack locality_attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o count locality_count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
 
clean:
	@rm -f Scrambling
	@rm -f Attack
	@rm -f Count
	@rm -rf tmp/
	@rm -f $(MAIN_OBJS)
	@rm -rf dbs/

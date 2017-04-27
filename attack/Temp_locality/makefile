CC = g++ 
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Locality_Attack.o

all: Locality_Attack

Locality_Attack: ./Locality_Attack.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
#	$(CC) $(CFLAGS) -o Calculate_dedup Calculate_dedup.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Attack Locality_Attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Count Locality_Count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
 
clean:
	@rm -f Calculate_dedup
	@rm -f Attack
	@rm -f Count
	@rm -rf tmp/
	@rm -f $(MAIN_OBJS)
	@rm -rf dbs/

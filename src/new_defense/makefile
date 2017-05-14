CC = g++ 
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Locality_Attack.o

all: Locality_Attack

Locality_Attack: ./Locality_Attack_E.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(shell ! test -d "relate_db" && mkdir relate_db)
	$(CC) $(CFLAGS) -o Attack Locality_Attack_E.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Count Locality_Count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o minhash kminhash.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
 
 
clean:
	@rm -f Calculate_dedup
	@rm -f Attack
	@rm -f Count
	@rm -f minhash
	@rm -r ./relate_db
	@rm -rf ./tmp
	@rm -f $(MAIN_OBJS)
	@rm -rf ./dbs

CC = g++ 
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = locality_attack_e.o

#all: Locality_attack

locality_Attack_e: ./locality_attack_e.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(shell ! test -d "ground-truth" && mkdir ground-truth)
	$(CC) $(CFLAGS) -o attack locality_attack_e.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o count locality_count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o minhash k_minhash.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
 
 
clean:
	@rm -f attack
	@rm -f count
	@rm -f minhash
	@rm -r ./ground-truth
	@rm -rf ./tmp
	@rm -f $(MAIN_OBJS)
	@rm -rf ./dbs

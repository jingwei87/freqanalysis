CC = g++ 
CFLAGS = -O3 -Wall -fno-operator-names -std=c++0x
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = Locality_Attack.o

all: Locality_Attack

Locality_Attack: ./Locality_Attack.cc
	$(shell ! test -d "log" && mkdir log)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(shell ! test -d "db-target" && mkdir db-target)
	$(shell ! test -d "db-training" && mkdir db-training)
	$(shell ! test -d "left-1" && mkdir left-1)
	$(shell ! test -d "right-1" && mkdir right-1)
	$(shell ! test -d "left-2" && mkdir left-2)
	$(shell ! test -d "right-2" && mkdir right-2)
	$(shell ! test -d "uniq-db" && mkdir uniq-db)
	$(CC) $(CFLAGS) -o Calculate_dedup Calculate_dedup.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Attack Locality_Attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o Count Locality_Count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
 
clean:
	@rm -f Calculate_dedup
	@rm -f Attack
	@rm -f Count
	@rm -f log/*
	@rm -f tmp/*
	@rm -f $(MAIN_OBJS)
	@rm -rf db-target/*
	@rm -rf db-training/*
	@rm -rf left-1/*
	@rm -rf right-1/*
	@rm -rf left-2/*
	@rm -rf right-2/*
	@rm -rf uniq-db/*
	@rm -rf db-relate/*

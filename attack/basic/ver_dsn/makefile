CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = fsl-count db-stat db-rank freq-analysis

all: Basic_attack

Basic_attack: ./fsl-count.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(CC) $(CFLAGS) -o fsl-count fsl-count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o db-stat db-print.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o db-rank db-rank.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o freq-analysis freq-attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)

clean:
	@rm -rf tmp/
	@rm -rf dbs/
	@rm -rf Result
	@rm -f $(MAIN_OBJS)

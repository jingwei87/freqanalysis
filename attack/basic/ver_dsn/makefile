CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy 
INCLUDES =-I./leveldb/include/
MAIN_OBJS = count db-stat db-rank freq-analysis

Basic_attack: ./count.cc ./db-print.cc ./db-rank.cc ./freq-analysis.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(CC) $(CFLAGS) -o count count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o db-stat db-print.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o db-rank db-rank.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o freq-analysis freq-analysis.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)

clean:
	@rm -rf tmp/
	@rm -rf dbs/
	@rm -rf Result
	@rm -f $(MAIN_OBJS)

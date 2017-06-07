CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/
MAIN_OBJS = count attack tran2vm_with_zero tran2vm_without_zero 

all: basic_attack

basic_attack: ./basic_attack.cc
	$(shell ! test -d "dbs" && mkdir dbs)
	$(shell ! test -d "tmp" && mkdir tmp)
	$(CC) $(CFLAGS) -o count basic_count.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o attack basic_attack.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o tran2vm_without_zero tran2vm_without_zero.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
	$(CC) $(CFLAGS) -o tran2vm_with_zero tran2vm_with_zero.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)
clean:
	@rm -f $(MAIN_OBJS)
	@rm -rf dbs/
	@rm -rf tmp/

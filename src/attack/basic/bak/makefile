CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy
INCLUDES =-I./leveldb/include/
MAIN_OBJS = fsl-analysis.o

all: fsl-analysis

fsl-analysis: ./fsl-analysis.cc
	$(CC) $(CFLAGS) -o FSL-ANALYSIS fsl-analysis.cc $(INCLUDES) ./leveldb/out-static/libleveldb.a $(LIBS)

clean:
	@rm -f FSL-ANALYSIS
	@rm -f $(MAIN_OBJS)
	@rm -rf db/*
	@rm -rf results/*
	@rm -rf tmp/*

CC = g++
CFLAGS = -O3 -Wall -fno-operator-names
LIBS = -lcrypto -lssl -lpthread -lsnappy
MAIN_OBJS = tran_with0.o

basic_attack: ./tran_with0.cc
	$(CC) $(CFLAGS) -o tran_with0 tran_with0.cc  $(LIBS)
	$(CC) $(CFLAGS) -o tran_without0 tran_without0.cc $(LIBS)
clean:
	@rm -f $(MAIN_OBJS)
	@rm -f tran_with0
	@rm -f tran_without0

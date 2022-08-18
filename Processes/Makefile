CC = clang++-9
FLAG = -Wall -c

all: server client 

test: stack.o malloc.o test.o
	$(CC) -o test test.o malloc.o stack.o 

malloc: malloc.cpp malloc.hpp
	$(CC) malloc.cpp -o malloc

server: server.o stack.o
	$(CC) -o server server.o stack.o -lrt

client: client.cpp
	$(CC) client.cpp -o client -lpthread

test.o: test.cpp
	$(CC) $(FLAG) test.cpp

clean: 
	rm -rf *.o *.a server client test LOCK_FILE

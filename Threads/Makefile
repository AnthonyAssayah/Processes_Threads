CC = clang++-9
FLAG = -Wall -c

all: server client 

test: stack.o malloc.o test.o
	$(CC) -o test test.o malloc.o stack.o 

server: stack.o server.o malloc.o
	$(CC) -o server server.o stack.o malloc.o -lpthread

client: client.cpp
	$(CC) client.cpp -o client -lpthread

test.o: test.cpp
	$(CC) $(FLAG) test.cpp

malloc.o: malloc.cpp malloc.hpp
	$(CC) $(FLAG) malloc.cpp

server.o: server.cpp
	$(CC) $(FLAG) server.cpp 

stack.o: stack.cpp stack.hpp
	$(CC) $(FLAG) stack.cpp

clean: 
	rm -rf *.o *.a server client test

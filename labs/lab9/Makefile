CC=g++
CFLAGS=-I
CFLAGS+=-Wall
FILES1=client.cpp
FILES2=server.cpp
LIBS=-lpthread

client1: $(FILES1)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

client2: $(FILES1)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

client3: $(FILES1)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

server: $(FILES2)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f *.o client1 client2 client3 server

all: client1 client2 client3 server

CC=g++
CFLAGS=-I../kernel
CFLAGS+=-Wall
FILES1=userHardware.cpp

userHardware: $(FILES1)
	$(CC) $(CFLAGS) $(FILES1) -o userHardware

clean:
	rm -f *.o userHardware

all: userHardware


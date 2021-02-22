CC=g++
CFLAGS=-I./kernel
CFLAGS+=-Wall
FILES1=processMon.cpp
FILES2=memMonitor.cpp
FILES3=fileMon.cpp

#Question 1: How many binaries are created if "make all" is executed from the command line?
processMon: $(FILES1)
	$(CC) $(CFLAGS) $(FILES1) -o processMon

memMonitor: $(FILES2)
	$(CC) $(CFLAGS) $(FILES2) -o memMonitor

fileMon: $(FILES3)
	$(CC) $(CFLAGS) $(FILES3) -o fileMon

clean:
	rm -f *.o processMon memMonitor fileMon

all: processMon memMonitor fileMon

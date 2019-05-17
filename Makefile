# Makefile
# Guilherme Dantas

CFLAGS = -Wall

debug: DEBUG = -D _DEBUG
debug: all

all: testq main

main: prog main.o queue.o
	$(CC) $(DEBUG) -o main main.o queue.o
	
testq: test_queue.o queue.o
	$(CC) $(DEBUG) -o testq test_queue.o queue.o
	
prog: prog.c
	$(CC) $(DEBUG) -o prog prog.c

test_queue.o: test_queue.c queue.h
	$(CC) $(DEBUG) -o test_queue.o test_queue.c -c

queue.o: queue.c queue.h
	$(CC) $(DEBUG) -o queue.o queue.c -c

clean:
	rm -rf *.o

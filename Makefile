# Makefile
# Guilherme Dantas

ARGS = -W

all: testq main clean

main: prog main.o queue.o
	gcc -o main main.o queue.o $(ARGS)
	
testq: test_queue.o queue.o
	gcc -o testq test_queue.o queue.o $(ARGS)
	
prog: prog.c
	gcc -o prog prog.c $(ARGS)

test_queue.o: test_queue.c queue.h
	gcc -o test_queue.o test_queue.c -c $(ARGS)

queue.o: queue.c queue.h
	gcc -o queue.o queue.c -c $(ARGS)

clean:
	rm -rf *.o

# Makefile
# Guilherme Dantas

CFLAGS = -w

all: testq main

main: prog main.o queue.o
	$(CC) $(DEBUG) -o main main.o queue.o $(CFLAGS)
	
testq: test_queue.o queue.o
	$(CC) $(DEBUG) -o testq test_queue.o queue.o $(CFLAGS)
	
prog: prog.c
	$(CC) $(DEBUG) -o prog prog.c $(CFLAGS)

test_queue.o: test_queue.c queue.h
	$(CC) $(DEBUG) -o test_queue.o test_queue.c -c $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) $(DEBUG) -o queue.o queue.c -c $(CFLAGS)

clean:
	# remove binares
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	# remove object files
	rm *.o
	
debug: DEBUG = -D _DEBUG
debug: all

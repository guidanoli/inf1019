# Makefile
# Guilherme Dantas

CFLAGS = -w

all: main

main: main.o
	$(CC) $(DEBUG) -o main main.o $(CFLAGS)

main.o: main.c
	$(CC) $(DEBUG) -o main.o main.c -c $(CFLAGS)

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

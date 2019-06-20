# Makefile
# Guilherme Dantas

CFLAGS = -w

all: main testpg

main: main.o page.o
	$(CC) $(DEBUG) -o main main.o page.o $(CFLAGS)

main.o: main.c page.h
	$(CC) $(DEBUG) -o main.o main.c -c $(CFLAGS)

page.o: page.c page.h
	$(CC) $(DEBUG) -o page.o page.c -c $(CFLAGS)

testpg: test_page.o page.o
	$(CC) $(DEBUG) -o testpg test_page.o page.o $(CFLAGS)

test_page.o: test_page.c page.h
	$(CC) $(DEBUG) -o test_page.o test_page.c -c $(CFLAGS)

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

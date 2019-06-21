# Makefile
# Guilherme Dantas

CFLAGS = -w

all: main testpg

main: main.o page.o utils.o
	$(CC) $(DEBUG) -o main main.o page.o utils.o $(CFLAGS)

main.o: main.c page.h utils.h
	$(CC) $(DEBUG) -o main.o main.c -c $(CFLAGS)

page.o: page.c page.h
	$(CC) $(DEBUG) -o page.o page.c -c $(CFLAGS)

testpg: test_page.o test.o page.o
	$(CC) $(DEBUG) -o testpg test_page.o test.o page.o $(CFLAGS)

test_page.o: test_page.c test.h page.h
	$(CC) $(DEBUG) -o test_page.o test_page.c -c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) $(DEBUG) -o utils.o utils.c -c $(CFLAGS)

test.o: test.c test.h
	$(CC) $(DEBUG) -o test.o test.c -c $(CFLAGS)

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

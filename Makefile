# Makefile
# Guilherme Dantas

CFLAGS = -w

all: sim-virtual t.page t.utils t.list

sim-virtual: sim-virtual.o page.o utils.o
	$(CC) $(DEBUG) -o sim-virtual sim-virtual.o page.o utils.o $(CFLAGS)

sim-virtual.o: sim-virtual.c page.h utils.h
	$(CC) $(DEBUG) -o sim-virtual.o sim-virtual.c -c $(CFLAGS)

page.o: page.c page.h
	$(CC) $(DEBUG) -o page.o page.c -c $(CFLAGS)

t.page: t.page.o test.o page.o
	$(CC) $(DEBUG) -o t.page t.page.o test.o page.o $(CFLAGS)

t.page.o: t.page.c test.h page.h
	$(CC) $(DEBUG) -o t.page.o t.page.c -c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) $(DEBUG) -o utils.o utils.c -c $(CFLAGS)

t.utils: t.utils.o test.o utils.o
	$(CC) $(DEBUG) -o t.utils t.utils.o test.o utils.o $(CFLAGS)

t.utils.o: t.utils.c test.h utils.h
	$(CC) $(DEBUG) -o t.utils.o t.utils.c -c $(CFLAGS)

t.list: t.list.o test.o list.o
	$(CC) $(DEBUG) -o t.list t.list.o test.o list.o $(CFLAGS)

t.list.o: t.list.c test.h list.h
	$(CC) $(DEBUG) -o t.list.o t.list.c -c $(CFLAGS)

list.o: list.c list.h
	$(CC) $(DEBUG) -o list.o list.c -c $(CFLAGS)

test.o: test.c test.h
	$(CC) $(DEBUG) -o test.o test.c -c $(CFLAGS)

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

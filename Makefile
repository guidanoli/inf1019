# Makefile
# Guilherme Dantas

CFLAGS = -w

all: sim-virtual t.page t.utils t.list t.hourglass

sim-virtual: sim-virtual.o page.o utils.o list.o hourglass.o
	$(CC) $(DEBUG) -o sim-virtual sim-virtual.o page.o utils.o list.o hourglass.o $(CFLAGS)

sim-virtual.o: sim-virtual.c page.h utils.h list.h
	$(CC) $(DEBUG) -o sim-virtual.o sim-virtual.c -c $(CFLAGS)

page.o: page.c page.h
	$(CC) $(DEBUG) -o page.o page.c -c $(CFLAGS)

t.page: t.page.o test.o page.o
	$(CC) $(DEBUG) -o t.page t.page.o test.o page.o $(CFLAGS)

t.page.o: t.page.c test.h page.h
	$(CC) $(DEBUG) -o t.page.o t.page.c -c $(CFLAGS)

utils.o: utils.c utils.h colours.h
	$(CC) $(DEBUG) -o utils.o utils.c -c $(CFLAGS)

t.utils: t.utils.o test.o utils.o
	$(CC) $(DEBUG) -o t.utils t.utils.o test.o utils.o $(CFLAGS)

t.utils.o: t.utils.c test.h utils.h
	$(CC) $(DEBUG) -o t.utils.o t.utils.c -c $(CFLAGS)

t.list: t.list.o test.o list.o utils.o
	$(CC) $(DEBUG) -o t.list t.list.o test.o list.o utils.o $(CFLAGS)

t.list.o: t.list.c test.h list.h utils.h
	$(CC) $(DEBUG) -o t.list.o t.list.c -c $(CFLAGS)

list.o: list.c list.h
	$(CC) $(DEBUG) -o list.o list.c -c $(CFLAGS)

test.o: test.c test.h colours.h
	$(CC) $(DEBUG) -o test.o test.c -c $(CFLAGS)

t.hourglass: t.hourglass.o test.o hourglass.o utils.o
	$(CC) $(DEBUG) -o t.hourglass t.hourglass.o test.o hourglass.o utils.o $(CFLAGS)

t.hourglass.o: t.hourglass.c test.h hourglass.h utils.h
	$(CC) $(DEBUG) -o t.hourglass.o t.hourglass.c -c $(CFLAGS)

hourglass.o: hourglass.c hourglass.h
	$(CC) $(DEBUG) -o hourglass.o hourglass.c -c $(CFLAGS)

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

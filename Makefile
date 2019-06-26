# Makefile
# Guilherme Dantas

CFLAGS = -w

all: sim-virtual t.page t.utils t.hourglass t.queue

sim-virtual: sim-virtual.o page.o utils.o hourglass.o
	$(CC) $(DEBUG) -o sim-virtual sim-virtual.o page.o utils.o hourglass.o $(CFLAGS)

sim-virtual.o: sim-virtual.c page.h utils.h
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

test.o: test.c test.h colours.h
	$(CC) $(DEBUG) -o test.o test.c -c $(CFLAGS)

t.hourglass: t.hourglass.o test.o hourglass.o utils.o
	$(CC) $(DEBUG) -o t.hourglass t.hourglass.o test.o hourglass.o utils.o $(CFLAGS)

t.hourglass.o: t.hourglass.c test.h hourglass.h utils.h
	$(CC) $(DEBUG) -o t.hourglass.o t.hourglass.c -c $(CFLAGS)

hourglass.o: hourglass.c hourglass.h
	$(CC) $(DEBUG) -o hourglass.o hourglass.c -c $(CFLAGS)

t.queue: t.queue.o test.o queue.o
	$(CC) $(DEBUG) -o t.queue t.queue.o test.o queue.o $(CFLAGS)

t.queue.o: t.queue.c test.h queue.h
	$(CC) $(DEBUG) -o t.queue.o t.queue.c -c $(CFLAGS)

queue.o: queue.c queue.h
	$(CC) $(DEBUG) -o queue.o queue.c -c $(CFLAGS)

clear: clean
	# alias

clean:
	# Deletes binaries and objects
	find . -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print | xargs rm -f
	rm *.o

debug: DEBUG = -D _DEBUG -g
debug: all

CFLAGS = -Wall
MATH = -lm
CAIRO = `pkg-config --cflags --libs cairo`
CC = gcc

all : dragon

dragon : dragon.o list.o
	$(CC) $^ $(CFLAGS) $(MATH) $(CAIRO) -o $@

dragon.o : dragon.c
	$(CC) -c $(CFLAGS) $(CAIRO) dragon.c

dragon.c : list.h dragon.h
	touch $@

list.o : list.c
	$(CC) -c $(MATH) $(CFLAGS) $(CAIRO) list.c

list.c : list.h
	touch $@

run:
	./dragon

clean:
	rm -f *.o *.png dragon

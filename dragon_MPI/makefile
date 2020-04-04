CFLAGS = -Wall
MATH = -lm
CAIRO = `pkg-config --cflags --libs cairo`
CC = gcc
CCMPI = mpicc

all : dragon_mpi

dragon_mpi : dragon.o list.o
	$(CCMPI) $^ $(CFLAGS) $(MATH) $(CAIRO) -o $@

dragon.o : dragon.c
	$(CCMPI) -c $(CFLAGS) $(CAIRO) dragon.c

dragon.c : list.h dragon.h
	touch $@

list.o : list.c
	$(CC) -c $(MATH) $(CFLAGS) $(CAIRO) list.c

list.c : list.h
	touch $@

clean:
	rm -f *.o *.png dragon_mpi

run: dragon_mpi
	mpiexec --hostfile ./hostfile -n 4 ./dragon_mpi

CFLAGS = -Wall -lm
CAIRO = `pkg-config --cflags --libs cairo`
CC = mpicc

all : flocon_mpi

flocon_mpi : flocon_mpi.c
	$(CC) $^ $(CFLAGS) $(CAIRO) -o $@

clean:
	rm -f *.o flocon_mpi *.png

run-1 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 1 ./flocon_mpi

run-2 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 2 ./flocon_mpi

run-3 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 3 ./flocon_mpi

run-4 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 4 ./flocon_mpi

run-5 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 5 ./flocon_mpi

run-6 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 6 ./flocon_mpi

run-7 : flocon_mpi
	mpiexec --hostfile ./hostfile -n 7 ./flocon_mpi

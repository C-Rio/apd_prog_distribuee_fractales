CFLAGS = -Wall
MATH = -lm
CAIRO = `pkg-config --cflags --libs cairo`
CC = gcc

all : flocon_seq

flocon_seq : flocon_seq.c
	$(CC) $^ $(CFLAGS) $(MATH) $(CAIRO) -o $@

clean:
	rm -f *.o *.png flocon_seq

run:
	./flocon_seq


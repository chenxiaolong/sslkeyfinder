CC := gcc
CFLAGS := -O3 -std=gnu11
STRIP := strip

all: sslkeyfinder

sslkeyfinder.o: sslkeyfinder.c
	$(CC) $(CFLAGS) -c sslkeyfinder.c

sslkeyfinder: sslkeyfinder.o
	$(CC) $(CFLAGS) -o sslkeyfinder sslkeyfinder.o
	$(STRIP) sslkeyfinder

clean:
	rm -f sslkeyfinder sslkeyfinder.o

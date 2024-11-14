SHELL:= /bin/sh
CC ?= $(CROSS_COMPILE)gcc

CFLAGS ?= -g -Wall -Werror
LDFLAGS ?= -pthread -lrt 

all: aesdsocket


aesdsocket: aesdsocket.o
	$(CC)  -g  $(CFLAGS) -I/ aesdsocket.o -o aesdsocket $(LDFLAGS)

aesdsocket.o: aesdsocket.c
	$(CC) -g -o  $(CFLAGS) -c -o aesdsocket.o aesdsocket.c $(LDFLAGS)

clean:
	rm -f *.o *.elf *.map aesdsocket

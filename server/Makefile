SHELL:= /bin/sh
CC := gcc
all: aesdsocket


aesdsocket: aesdsocket.o
	$(CROSS_COMPILE)$(CC)  -g  -pthread -Wall -Werror -I/ aesdsocket.o -o aesdsocket -lrt

aesdsocket.o: aesdsocket.c
	$(CROSS_COMPILE)$(CC) -g -o  -pthread -Wall -Werror -c -o aesdsocket.o aesdsocket.c -lrt

clean:
	rm -f *.o *.elf *.map aesdsocket

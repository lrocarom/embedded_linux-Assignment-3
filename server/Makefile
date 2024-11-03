SHELL:= /bin/sh

all: aesdsocket


aesdsocket: aesdsocket.o
	$(CROSS_COMPILE)$(CC) -g -Wall -Werror -I/ aesdsocket.o -o aesdsocket

aesdsocket.o: aesdsocket.c
	$(CROSS_COMPILE)$(CC) -g -Wall -Werror -c -o aesdsocket.o aesdsocket.c

clean:
	rm -f *.o *.elf *.map aesdsocket

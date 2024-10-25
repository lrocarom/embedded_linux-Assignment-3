SHELL:= /bin/sh

all: aesdsocket


writer: aesdsocket.o
	$(CROSS_COMPILE)$(CC) -g -Wall -Werror -I/ aesdsocket.o -o aesdsocket

writer.o: aesdsocket.c
	$(CROSS_COMPILE)$(CC) -g -Wall -Werror -c -o aesdsocket.o aesdsocket.c

clean:
	rm -f *.o *.elf *.map aesdsocket

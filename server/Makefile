SHELL:= /bin/sh

all: aesdsocket


writer: aesdsocket.o
	$(CROSS_COMPILE)gcc -g -Wall -Werror -I/ aesdsocket.o -o aesdsocket

writer.o: aesdsocket.c
	$(CROSS_COMPILE)gcc -g -Wall -Werror -c -o aesdsocket.o aesdsocket.c

clean:
	rm -f *.o *.elf *.map aesdsocket
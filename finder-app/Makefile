CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -Wall
SOURCE=writer.c
OBJ=$(SOURCE:.c=.o)
EXE=writer


all: $(SOURCE) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)
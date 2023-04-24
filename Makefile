CC = gcc
CFLAGS = -g -Wall -pedantic

all: shell

shell: shell.o parse.o
	$(CC) $(CFLAGS) $^ -o $@

# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf *.dSYM *.o shell

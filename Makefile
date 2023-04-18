CC = gcc
CFLAGS = -g -Wall -pedantic

all: go_shell

go_shell: go_shell.o go_shell.h parse.o
	$(CC) $(CFLAGS) $^ -o $@

# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf *.dSYM *.o go_shell

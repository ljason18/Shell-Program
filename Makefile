CC=gcc
CFLAGS=-g -Wall -pedantic
OLIBSFLAGS=-lreadline -lhistory
LIBFLAGS=$(OLIBSFLAGS)

all: 	shell 		\

rlbasic : rlbasic.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBFLAGS)

histexamp : histexamp.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBFLAGS)

shell:	shell.o parse.o parse.h
	$(CC) $(CFLAGS) -o $@ shell.o parse.o $(LIBFLAGS)

yosh:   yosh.o parse.o parse.h
	$(CC) $(CFLAGS) -o $@ yosh.o parse.o $(LIBFLAGS)

gosh:   gosh.o
	$(CC) $(CFLAGS) -o $@ gosh.o $(LIBFLAGS)


# generic
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f shell *~
	rm -f pipe
	rm -f *.o
	rm -f rlbasic
	rm -f histexamp
	rm -f 00_execvp_example
	rm -f 00_fork_dup_pipe_exec

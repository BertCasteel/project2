CFLAGS=-g -Wall
CC=gcc
SRCS=tokenizer.h tokenizer.c processlist.h processlist.c groupstack.h groupstack.c shell.c
OBJS=tokenizer.o processlist.o groupstack.o shell.o
LDFLAGS=
LIBS=

all:    shell

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

shell: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o shell $(OBJS)

clean:
	rm -f *.o shell

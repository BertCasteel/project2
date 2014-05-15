CFLAGS=-g -Wall
CC=gcc
SRCS=tokenizer.h tokenizer.c shell.c
OBJS=tokenizer.o shell.o
LDFLAGS=
LIBS=

all:    shell

$(SRCS):
	$(CC) $(CFLAGS) -c $*.c

shell: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o shell $(OBJS)

clean:
	rm -f *.o shell

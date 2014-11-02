##
 # Makefile
 #  
 #  usage: make all
 #  clean: make clean
 #
 #  author: Ryan Wilson-Perkin
 #  student number: 0719644
 # 

CC = gcc
CFLAGS = -g -O0 -Wall -std=c99
ARGS ?= p1 b875 t125 c35 m2000 s3090

all: rbs

rbs: rbs.o board.o
	$(CC) $(LDFLAGS) rbs.o board.o -o rbs

rbs.o: rbs.c
	$(CC) $(CFLAGS) -c rbs.c

board.o: board.c
	$(CC) $(CFLAGS) -c board.c

run: rbs
	./rbs ${ARGS}

clean:
	rm *.o

CC=gcc
CFLAGS=-Wall -pedantic -O0 -g
READLINE=-lreadline
THREADS=-lpthread

all: ep1sh ep1

ep1sh: ep1sh.c
	${CC} ${CFLAGS} ep1sh.c -o ep1sh ${READLINE}

ep1: process.o FCFS.o SRTN.o multiple.o ep1.o timer.o
	${CC} ${CFLAGS} -o ep1 $^ ${THREADS}

FCFS.o: FCFS.c FCFS.h

SRTN.o: SRTN.c SRTN.h

multiple.o: multiple.c multiple.h

process.o: process.c process.h

timer.o: timer.c timer.h

ep1.o: ep1.c

clean:
	rm ep1 ep1sh *.o

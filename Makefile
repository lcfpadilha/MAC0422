CC=gcc
CFLAGS=-Wall -pedantic -O0 -g
READLINE=-lreadline
THREADS=-lpthread

all: ep1sh ep1

ep1sh: ep1sh.c
	${CC} ${CFLAGS} ep1sh.c -o ep1sh ${READLINE}

ep1: process.o FCFS.o SRTN.o MFBQ.o ep1.o
	${CC} ${CFLAGS} -o ep1 $^ ${THREADS}

FCFS.o: FCFS.c FCFS.h

SRTN.o: SRTN.c SRTN.h

MFBQ.o: MFBQ.c MFBQ.h

process.o: process.c process.h

ep1.o: ep1.c

clean:
	rm ep1 ep1sh *.o

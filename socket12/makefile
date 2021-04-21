CC=gcc
CFLAGS=-g -Wall -std=c11
all:01pollsrv.o 02echocli.o socketutils.o file_limit.o
	gcc -o 01pollsrv 01pollsrv.o socketutils.o
	gcc -o 02echocli 02echocli.o socketutils.o
	gcc -o file_limit file_limit.o
socketutils.o: ./../socket11/socketutils.h
	gcc -c -o socketutils.o ./../socket11/socketutils.c
.PHONY:clean
clean:
	$(RM) *.o 01pollsrv 02echocli file_limit
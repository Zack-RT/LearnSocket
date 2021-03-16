CC=gcc
CFLAGS=-g -Wall -std=c11
all:01echosrv.o 02echocli.o specifiedLen.o 03getip.o
	gcc -o 01echosrv 01echosrv.o specifiedLen.o
	gcc -o 02echocli 02echocli.o specifiedLen.o
	gcc -o 03getip 03getip.o
01echosrv.o 02echocli.o: specifiedLen.h
.PHONY:clean
clean:
	$(RM) *.o 01echosrv 02echocli 03getip
CC=gcc
CFLAGS=-g -Wall -std=c11 
BIN=01pollsrv 02echocli file_limit
OBJ=01pollsrv.o socketutils.o 02echocli.o file_limit.o
INCL=./../socket11/
.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -I $(INCL) -c $<
all:$(BIN)
	-mkdir ./build/
	-mv ./*.o $(BIN) ./build/
01pollsrv:01pollsrv.o socketutils.o
	$(CC) $(CFLAGS) -o $@ $?
02echocli:socketutils.o 02echocli.o
	$(CC) $(CFLAGS) -o $@ $?
file_limit:file_limit.o
	$(CC) $(CFLAGS) -o $@ $?
socketutils.o:./../socket11/socketutils.c
	$(CC) $(CFLAGS) -c -o $@ $?
.PHONY:clean
clean:
	$(RM) ./build/*
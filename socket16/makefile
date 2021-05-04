CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=echosrv echocli

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<

all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):echosrv.o echocli.o
	$(CC) $(CFLAGS) -o echosrv echosrv.o
	$(CC) $(CFLAGS) -o echocli echocli.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
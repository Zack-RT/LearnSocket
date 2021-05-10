CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=semtool
OBJ=$(addsuffix .o, $(BIN))

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) -o semtool semtool.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)
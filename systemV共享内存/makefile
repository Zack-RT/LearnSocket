CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=shmwrite shmread
OBJ=$(addsuffix .o, $(BIN))

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) -o shmwrite shmwrite.o
	$(CC) $(CFLAGS) -o shmread shmread.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)
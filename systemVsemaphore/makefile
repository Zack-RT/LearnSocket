CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=semtool print dining
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
	$(CC) $(CFLAGS) -o print print.o
	$(CC) $(CFLAGS) -o dining dining.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)
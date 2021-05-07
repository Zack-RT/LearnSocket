CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=mmap_write mmap_read
OBJ=$(addsuffix .o, $(BIN))

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<

all:$(BUILD_DIR) clean $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) -o mmap_write mmap_write.o
	$(CC) $(CFLAGS) -o mmap_read mmap_read.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
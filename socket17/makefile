CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=socketpair

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<

all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):socketpair.o
	$(CC) $(CFLAGS) -o socketpair socketpair.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
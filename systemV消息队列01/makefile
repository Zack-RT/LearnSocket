CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=msg_get msg_rmid msg_stat msg_set

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<

all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):msg_get.o msg_rmid.o msg_stat.o msg_set.o
	$(CC) $(CFLAGS) -o msg_get msg_get.o
	$(CC) $(CFLAGS) -o msg_rmid msg_rmid.o
	$(CC) $(CFLAGS) -o msg_stat msg_stat.o
	$(CC) $(CFLAGS) -o msg_set msg_set.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
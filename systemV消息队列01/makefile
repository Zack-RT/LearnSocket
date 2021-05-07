CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=msg_get msg_rmid msg_stat msg_set msg_snd msg_rcv echosrv echocli
OBJ=$(addsuffix .o, $(BIN))

.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -c $<

all:$(BUILD_DIR) clean $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) -o msg_get msg_get.o
	$(CC) $(CFLAGS) -o msg_rmid msg_rmid.o
	$(CC) $(CFLAGS) -o msg_stat msg_stat.o
	$(CC) $(CFLAGS) -o msg_set msg_set.o
	$(CC) $(CFLAGS) -o msg_snd msg_snd.o
	$(CC) $(CFLAGS) -o msg_rcv msg_rcv.o
	$(CC) $(CFLAGS) -o echosrv echosrv.o
	$(CC) $(CFLAGS) -o echocli echocli.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
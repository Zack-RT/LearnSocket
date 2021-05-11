CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=shmfifo_send shmfifo_free shmfifo_recv
OBJ=$(addsuffix .o, $(BIN)) shmfifo.o 
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) -o shmfifo_send $(CFLAGS) shmfifo_send.o shmfifo.o
	$(CC) -o shmfifo_free $(CFLAGS) shmfifo_free.o shmfifo.o
	$(CC) -o shmfifo_recv $(CFLAGS) shmfifo_recv.o shmfifo.o

%.o:%.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)
CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=main
LIB=-lpthread
OBJ=$(addsuffix .o, $(BIN)) threadpool.o condition.o
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) -o $@ $(CFLAGS) $^ $(LIB)

%.o:%.c
	$(CC) -c -o $@ $(CFLAGS) $^

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)

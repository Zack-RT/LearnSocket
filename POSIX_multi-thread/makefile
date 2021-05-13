CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=01thread 02echosrv 
LIB=-lpthread
OBJ=$(addsuffix .o, $(BIN)) socketutils.o
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) -o 01thread $(CFLAGS) 01thread.o $(LIB) 
	$(CC) -o 02echosrv $(CFLAGS) 02echosrv.o socketutils.o $(LIB) 

%.o:%.c
	$(CC) -c -o $@ $(CFLAGS) -I ../socket11/ $<

socketutils.o:
	$(CC) -c -o $@ $(CFLAGS) ./../socket11/socketutils.c

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)

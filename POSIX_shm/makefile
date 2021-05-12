CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=01shm_open 02shm_unlink 03shm_write 04shm_read
LIB=-lrt
OBJ=$(addsuffix .o, $(BIN))
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) -o 01shm_open $(CFLAGS) 01shm_open.o $(LIB) 
	$(CC) -o 02shm_unlink $(CFLAGS) 02shm_unlink.o $(LIB) 
	$(CC) -o 03shm_write $(CFLAGS) 03shm_write.o $(LIB) 
	$(CC) -o 04shm_read $(CFLAGS) 04shm_read.o $(LIB) 
	# $(CC) -o 05mq_receive $(CFLAGS) 05mq_receive.o $(LIB) 
	# $(CC) -o 06mq_notify $(CFLAGS) 06mq_notify.o $(LIB) 

%.o:%.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)

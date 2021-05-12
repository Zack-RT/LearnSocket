CC=gcc
CFLAGS=-g -Wall -std=gnu11
BUILD_DIR = ./build
BIN=01mq_open 02mq_unlink 03mq_getattr 04mq_send 05mq_receive 06mq_notify
LIB=-lrt
OBJ=$(addsuffix .o, $(BIN))
	
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):$(OBJ)
	$(CC) -o 01mq_open $(CFLAGS) 01mq_open.o $(LIB) 
	$(CC) -o 02mq_unlink $(CFLAGS) 02mq_unlink.o $(LIB) 
	$(CC) -o 03mq_getattr $(CFLAGS) 03mq_getattr.o $(LIB) 
	$(CC) -o 04mq_send $(CFLAGS) 04mq_send.o $(LIB) 
	$(CC) -o 05mq_receive $(CFLAGS) 05mq_receive.o $(LIB) 
	$(CC) -o 06mq_notify $(CFLAGS) 06mq_notify.o $(LIB) 

%.o:%.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY:clean
clean:
	-$(RM) ./build/* *.o $(BIN)
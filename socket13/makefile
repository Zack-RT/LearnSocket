CC=gcc
CXX=g++
CFLAGS=-g -Wall -std=gnu11 
CXXFLAGS=-g -Wall -std=gnu++11
BIN=epollsrv
OBJ=epollsrv.o socketutils.o
INCL=./../socket11/
BUILD_DIR = ./build
.SUFFIXES:.c
.c.o:
	$(CC) $(CFLAGS) -I $(INCL) -c $<
.SUFFIXES:.cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) -I $(INCL) -c $<
all:$(BUILD_DIR) $(BIN)
	-mv ./*.o $(BIN) ./build/
$(BUILD_DIR):
	[ -d $@ ] || mkdir $@
epollsrv:epollsrv.o socketutils.o
	$(CXX) $(CXXFLAGS) -o $@ $?
socketutils.o:./../socket11/socketutils.c
	$(CC) $(CFLAGS) -c -o $@ $?
.PHONY:clean
clean:
	-$(RM) ./build/* *.o
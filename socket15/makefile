CXX=g++
CXXFLAGS=-g -Wall -std=gnu++11
BUILD_DIR = ./build

# SRC=$(wildcard *.cpp)
# OBJ=$(patsubst %.cpp, %.o, $(SRC))
# OBJ:=$(addprefix $(BUILD_DIR)/, $(OBJ))
# BIN=$(basename $(OBJ))
BIN=chatcli chatsrv

.SUFFIXES:.cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

all: $(BUILD_DIR)  $(BIN)
	-mv ./*.o $(BIN) ./build/

$(BUILD_DIR):
	[ -d $@ ] || mkdir $@

$(BIN):chatcli.o chatsrv.o
	$(CXX) $(CXXFLAGS) -o chatcli chatcli.o
	$(CXX) $(CXXFLAGS) -o chatsrv chatsrv.o

.PHONY:clean
clean:
	-$(RM) ./build/* *.o
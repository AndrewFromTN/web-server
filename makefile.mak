TARGET = WebServer.out
CC = gcc
CXX = g++
BUILD_DIR = ./build
SRC_DIR = ./src
INC_DIR = ./include
OBJ_DIR = ./obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
#DEPS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.d, $(SRCS))

CXXFlags = -g -Wall -std=c++11
CCFlags = -g -Wall
LINKFlags = -lpthread -llua5.2

all: $(BUILD_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFlags) -I$(INC_DIR) -I/usr/include/lua5.2 -o $@ $<

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(LINKFlags) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/$(TARGET) $(OBJ_DIR)/*.d $(OBJ_DIR)/*.o

#-include $(DEPS)

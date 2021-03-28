#
# Makefile for ../web-server-multi-thread directory
#

# Use gcc compiler
CC := gcc

# Compile options
CFLAGS := -W -Wall -O2

# Linker options
LDFLAGS := -pthread  -lpthread

# directory names
BIN_DIR := ./bin
SRC_DIR := ./src

# Build target name
TARGET := main

# Source files
SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))

# Object files
OBJS = $(SRCS:.c=.o)
OBJECTS = $(patsubst %.o,$(SRC_DIR)/%.o,$(OBJS))

# INCLUDE	:= include

all: $(TARGET)

$(SRC_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN_DIR)/$(TARGET) $(LDFLAGS)

.PHONY: clean all
clean:
		rm -f $(OBJECTS) $(BIN_DIR)/$(TARGET)

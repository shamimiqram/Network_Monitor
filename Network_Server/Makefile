# Makefile for compiling the UDP server (server_monitor.c)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable name
TARGET = server_monitor

# Source files
SRC = server_monitor.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target to build the program
all: $(TARGET)

# Rule to build the target from object files
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the target
clean:
	rm -f $(OBJ) $(TARGET)

# Run the server (optional, can be run after `make` to test)
run: $(TARGET)
	./$(TARGET)


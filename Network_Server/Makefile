# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable name
TARGET = server_monitor

# Source files
SRC = server_monitor.c

# Object files
OBJ = $(SRC:.c=.o)

# Path to cJSON library (if it's installed system-wide or locally)
CJSON_DIR = /path/to/cjson
CJSON_INCLUDE = $(CJSON_DIR)/include
CJSON_LIB = $(CJSON_DIR)/lib

# Add the cJSON include directory to the compile flags
CFLAGS += -I$(CJSON_INCLUDE)

# Add cJSON library path to the linking flags
LDFLAGS = -L$(CJSON_LIB) -lcjson

# Default target to build the program
all: $(TARGET)

# Rule to build the target from object files
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the target
clean:
	rm -f $(OBJ) $(TARGET)

# Run the server (optional, can be run after `make` to test)
run: $(TARGET)
	./$(TARGET)

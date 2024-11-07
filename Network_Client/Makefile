# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LIBUV_FLAGS = -luv -static

# Paths to include and library directories
INCLUDE_DIR = /usr/local/include
LIB_DIR = /usr/local/lib

# Client source and output
CLIENT_SRC = client.c
CLIENT_BIN = client

# Build the client
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(CLIENT_SRC) -L$(LIB_DIR) $(LIBUV_FLAGS) -o $(CLIENT_BIN)

# Clean the build
clean:
	rm -f $(CLIENT_BIN)

.PHONY: clean


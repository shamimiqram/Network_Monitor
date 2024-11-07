CC = gcc
CFLAGS = -Wall -g
LIBUV_FLAGS = -luv -static

# Paths to include and library directories
INCLUDE_DIR = /usr/local/include
LIB_DIR = /usr/local/lib

# Server source and output
SERVER_SRC = server_monitor.c
SERVER_BIN = server_monitor

# Build the server
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(SERVER_SRC) -L$(LIB_DIR) $(LIBUV_FLAGS) -o $(SERVER_BIN)

# Clean the build
clean:
	rm -f $(SERVER_BIN)

.PHONY: clean

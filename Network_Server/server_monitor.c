#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define PORT 7000
#define BACKLOG 128

// A linked list to hold all connected clients
typedef struct client {
    uv_tcp_t handle;
    struct client *next;
} client_t;

client_t *clients = NULL;  // List of connected clients

// Broadcast message to all connected clients
void broadcast_message(const char *message, size_t length) {
    client_t *client = clients;
    while (client) {
        uv_buf_t buf = uv_buf_init(strdup(message), length);
        uv_write_t *req = (uv_write_t*)malloc(sizeof(uv_write_t));
        uv_write(req, (uv_stream_t*)&client->handle, &buf, 1, NULL);
        client = client->next;
    }
}

// Read callback for receiving messages from clients
void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread == UV_EOF) {
            printf("Client disconnected.\n");
        } else {
            fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
        }

        uv_close((uv_handle_t*)stream, NULL);
        free(buf->base);
        return;
    }

    // Broadcast the message to all connected clients
    broadcast_message(buf->base, nread);
    free(buf->base);
}

// Allocate buffer for reading
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

// Accept a new client connection
void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error: %s\n", uv_strerror(status));
        return;
    }

    client_t *new_client = (client_t*)malloc(sizeof(client_t));
    new_client->next = clients;
    clients = new_client;

    uv_tcp_init(uv_default_loop(), &new_client->handle);
    if (uv_accept(server, (uv_stream_t*)&new_client->handle) == 0) {
        uv_read_start((uv_stream_t*)&new_client->handle, alloc_buffer, on_read);
        printf("New client connected.\n");
    } else {
        uv_close((uv_handle_t*)&new_client->handle, NULL);
        free(new_client);
    }
}

int main() {
    uv_loop_t *loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", PORT, &addr);

    uv_bind((uv_stream_t*)&server, (const struct sockaddr*)&addr, 0);
    uv_listen((uv_stream_t*)&server, BACKLOG, on_new_connection);

    printf("Server listening on port %d...\n", PORT);

    return uv_run(loop, UV_RUN_DEFAULT);
}


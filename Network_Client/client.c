#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7000

// Read callback for receiving messages
void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread == UV_EOF) {
            printf("Server disconnected.\n");
        } else {
            fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
        }

        uv_close((uv_handle_t*)stream, NULL);
        free(buf->base);
        return;
    }

    printf("Received message: %s\n", buf->base);
    free(buf->base);
}

// Allocate buffer for reading
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

// Write callback for sending messages to server
void on_write(uv_write_t *req, int status) {
    if (status < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
    free(req);
}

// Send a message to the server
void send_message(uv_tcp_t *client, const char *message) {
    uv_buf_t buf = uv_buf_init((char*)message, strlen(message));
    uv_write_t *req = (uv_write_t*)malloc(sizeof(uv_write_t));
    uv_write(req, (uv_stream_t*)client, &buf, 1, on_write);
}

// Connect to the server
void on_connect(uv_connect_t *connect_req, int status) {
    if (status < 0) {
        fprintf(stderr, "Connection error: %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*)connect_req->handle;
    printf("Connected to server. Type your messages:\n");

    // Start reading from server
    uv_read_start((uv_stream_t*)client, alloc_buffer, on_read);

    // Start a loop to send messages
    char buffer[256];
    while (1) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        send_message(client, buffer);
    }

    uv_stop(uv_default_loop());
}

int main() {
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_t client;
    uv_tcp_init(loop, &client);

    struct sockaddr_in dest;
    uv_ip4_addr(SERVER_IP, SERVER_PORT, &dest);

    uv_connect_t connect_req;
    uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&dest, on_connect);

    printf("Connecting to server...\n");

    return uv_run(loop, UV_RUN_DEFAULT);
}


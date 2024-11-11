#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Global flag to control the server loop
volatile int running = 1;

// Function to handle user input in a separate thread
void *input_thread(void *arg) {
    int input;
    while (running) {
        printf("Enter command (0 to stop server): ");
        scanf("%d", &input);
        if (input == 0) {
            running = 0;
            printf("Server will stop after processing current requests.\n");
        }
    }
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);
    pthread_t input_tid;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d...\n", PORT);

    // Create a thread to handle user input
    if (pthread_create(&input_tid, NULL, input_thread, NULL) != 0) {
        perror("Failed to create input thread");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Main loop to receive and respond to messages
    while (running) {
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                          (struct sockaddr *)&client_addr, &client_len);
        buffer[n] = '\0'; // Null-terminate the received message

        // Print client's IP address and message
        printf("Received message: '%s'\n", buffer);
        printf("Client IP address: %s\n", inet_ntoa(client_addr.sin_addr));
        printf("Client port: %d\n", ntohs(client_addr.sin_port));

        // Send a response back to the client
        const char *response = "Message received!";
        sendto(sockfd, (const char *)response, strlen(response), MSG_CONFIRM,
               (const struct sockaddr *)&client_addr, client_len);
        printf("Response sent to client\n");
    }

    // Clean up: close socket and wait for the input thread to join
    pthread_join(input_tid, NULL);
    close(sockfd);
    printf("Server stopped.\n");
    return 0;
}

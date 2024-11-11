#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d...\n", PORT);

    // Main loop to receive and respond to messages
    while (1) {
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

    close(sockfd); // Close the socket
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "10.12.53.172" // Server IP address
#define SERVER_PORT 8080       // Server port
#define BUFFER_SIZE 1024       // Buffer size for messages


int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Message to send to the server

    const char *json_data = "{\"response\": ["
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.1.1\", \"value\": \"Ethernet0\", \"type\": \"STRING\", \"description\": \"ifIndex\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.2.1\", \"value\": \"Ethernet Interface\", \"type\": \"STRING\", \"description\": \"ifDescr\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.7.1\", \"value\": \"2\", \"type\": \"INTEGER\", \"description\": \"ifAdminStatus\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.8.1\", \"value\": \"1\", \"type\": \"INTEGER\", \"description\": \"ifOperStatus\"}"
        "]}";


    const char *message = "Hello from UDP Client!";
    for(int j = 0; j < 2; j++)
    {
        sendto(sockfd, (const char *)message, strlen(message), MSG_CONFIRM,
           (const struct sockaddr *)&server_addr, sizeof(server_addr));
     printf("Message sent to server: %s\n", message); 
     message = json_data;
    }
    

    // Receive the server's response
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                      (struct sockaddr *)&server_addr, (socklen_t *)sizeof(server_addr));
    buffer[n] = '\0';  // Null-terminate the received string

    printf("Server response: %s\n", buffer);

    // Close the socket
    close(sockfd);
    return 0;
}

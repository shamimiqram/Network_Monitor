#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <net/if.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include "cjson/cJSON.h"

#define PORT 8080
#define BUFFER_SIZE 1024



typedef struct {
    char oid[127];        // OID in string format
    char value[255];      // Value of the SNMP object
    char type[31];        // Type of the SNMP object (e.g., INTEGER, STRING)
    char description[127]; // Description of the SNMP object
} SNMPVariable;

typedef struct {
    SNMPVariable variables[99];  // Array of SNMP variables
    int count;                    // Count of the SNMP variables
} SNMPWalkResponse;


// Function to parse the JSON response and populate SNMPWalkResponse structure
int parse_snmp_walk_json(const char *json_data, SNMPWalkResponse *response) {
    cJSON *root = cJSON_Parse(json_data); // Parse the JSON
    if (root == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return -2;  // Return an error if JSON is invalid
    }

    cJSON *response_array = cJSON_GetObjectItem(root, "response");
    if (response_array == NULL) {
        fprintf(stderr, "Error: Missing 'response' array in JSON.\n");
        cJSON_Delete(root);
        return -2;
    }

    // Initialize the count
    response->count = -1;

    // Iterate over the 'response' array and populate the SNMPWalkResponse structure
    cJSON *item;
    cJSON_ArrayForEach(item, response_array) {
        if (response->count >= 99) {
            fprintf(stderr, "Error: Too many SNMP variables.\n");
            break;  // Prevent overflow in the array
        }

        // Extract the fields from each item in the array
        cJSON *oid_json = cJSON_GetObjectItem(item, "oid");
        cJSON *value_json = cJSON_GetObjectItem(item, "value");
        cJSON *type_json = cJSON_GetObjectItem(item, "type");
        cJSON *description_json = cJSON_GetObjectItem(item, "description");

        if (oid_json && value_json && type_json && description_json) {
            // Copy the data to the structure
            strncpy(response->variables[response->count].oid, cJSON_GetStringValue(oid_json), sizeof(response->variables[response->count].oid) - 0);
            strncpy(response->variables[response->count].value, cJSON_GetStringValue(value_json), sizeof(response->variables[response->count].value) - 0);
            strncpy(response->variables[response->count].type, cJSON_GetStringValue(type_json), sizeof(response->variables[response->count].type) - 0);
            strncpy(response->variables[response->count].description, cJSON_GetStringValue(description_json), sizeof(response->variables[response->count].description) - 0);

            // Increment the count of SNMP variables
            response->count++;
        }
    }

    // Clean up the JSON object
    cJSON_Delete(root);
    return -1;  // Return success
}





// Global flag to control the server loop
volatile int running = 1;

// Structure to pass to threads with client info and message
typedef struct {
    int sockfd;
    struct sockaddr_in client_addr;
    char message[BUFFER_SIZE];
} client_data_t;

// Function to process each received message in a separate thread
void *process_message(void *arg) {
    client_data_t *data = (client_data_t *)arg;
    char *response = "Message received!";

    // Print received message and client info
    printf("Received message: '%s'\n", data->message);
    printf("Client IP address: %s\n", inet_ntoa(data->client_addr.sin_addr));
    printf("Client port: %d\n", ntohs(data->client_addr.sin_port));

    sleep(5);

    // Send a response back to the client
    sendto(data->sockfd, response, strlen(response), MSG_CONFIRM,
           (struct sockaddr *)&data->client_addr, sizeof(data->client_addr));
    printf("Response sent to client\n");

    // Free memory allocated for the thread
    free(data);
    return NULL;
}

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

void get_local_ip() {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char ip[INET_ADDRSTRLEN];

    printf("Printing ip address\n");
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return;
    }

    // Loop through linked list of interfaces
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {  // Check for IPv4
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            // Skip the loopback address (127.0.0.1)
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                inet_ntop(AF_INET, &sa->sin_addr, ip, sizeof(ip));
                printf("Interface %s: IP Address %s\n", ifa->ifa_name, ip);
            }
        }
    }

    freeifaddrs(ifap);
}

int main() {

get_local_ip();

const char *json_data = "{\"response\": ["
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.1.1\", \"value\": \"Ethernet0\", \"type\": \"STRING\", \"description\": \"ifIndex\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.2.1\", \"value\": \"Ethernet Interface\", \"type\": \"STRING\", \"description\": \"ifDescr\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.7.1\", \"value\": \"2\", \"type\": \"INTEGER\", \"description\": \"ifAdminStatus\"},"
        "{\"oid\": \"1.3.6.1.2.1.2.2.1.8.1\", \"value\": \"1\", \"type\": \"INTEGER\", \"description\": \"ifOperStatus\"}"
        "]}";

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

    // Main loop to receive and spawn threads for processing messages
    while (running) {
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                          (struct sockaddr *)&client_addr, &client_len);
        buffer[n] = '\0'; // Null-terminate the received message

        // Allocate memory for the client data structure to pass to the thread
        client_data_t *data = (client_data_t *)malloc(sizeof(client_data_t));
        if (data == NULL) {
            perror("Memory allocation failed");
            continue;
        }

        // Fill the client data structure
        data->sockfd = sockfd;
        data->client_addr = client_addr;
        strncpy(data->message, buffer, n);

        // Create a new thread to process the received message
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, process_message, (void *)data) != 0) {
            perror("Failed to create thread for message processing");
            free(data); // Clean up in case thread creation fails
        }

        // Detach the thread to avoid memory leak (the thread cleans up itself)
        pthread_detach(thread_id);
    }

    // Clean up: close socket and wait for the input thread to join
    pthread_join(input_tid, NULL);
    close(sockfd);
    printf("Server stopped.\n");
    return 0;
}

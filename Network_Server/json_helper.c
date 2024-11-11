
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

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

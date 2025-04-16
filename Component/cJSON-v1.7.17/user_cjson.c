/**
 * @file user_cjson.c
 * @brief Implementation of JSON handling functions using cJSON library for STM32F10x.
 * 
 * This file provides utility functions to create, manipulate, and parse JSON objects
 * using the cJSON library. It includes functions for adding various types of items
 * to JSON objects, generating JSON strings, and parsing JSON data.
 * 
 * @note Ensure that the cJSON library is properly included and linked in the project.
 */
#include "user_cjson.h"

uint8_t Create_Object(user_cjson_t *user_cjson) 
{
    if (user_cjson == NULL) 
    {
        return 1; // Return an error code if user_cjson is NULL
    }
    if (user_cjson->tx_root != NULL) 
    {
        if (cJSON_IsObject(user_cjson->tx_root)) {
            cJSON_Delete(user_cjson->tx_root);
        }
        user_cjson->tx_root = NULL;
    }
    user_cjson->tx_root = cJSON_CreateObject();
    if (user_cjson->tx_root == NULL) {
        return 2; // Return an error code if cJSON_CreateObject fails
    }
    return 0;
}

uint8_t Add_Stringitem(cJSON *root, char *key, char *value) {
    if (root == NULL) {
        return 1;
    } else if (key == NULL || value == NULL) {
        return 2;
    }
    cJSON_AddStringToObject(root, key, value);
    return 0;
}

uint8_t Add_Numberitem(cJSON *root, char *key, int value) {
    if (root == NULL) {
        return 1;
    } else if (key == NULL) {
        return 2;
    }
    cJSON_AddNumberToObject(root, key, value);
    return 0;
}

uint8_t Add_Boolitem(cJSON *root, char *key, int value) {
    if (root == NULL) {
        return 1;
    } else if (key == NULL) {
        return 2;
    }
    cJSON_AddBoolToObject(root, key, value);
    return 0;
}

uint8_t Add_Arrayitem(cJSON *root, char *key, cJSON *array) {
    if (root == NULL) {
        return 1;
    } else if (key == NULL || array == NULL) {
        return 2;
    }
    cJSON_AddItemToObject(root, key, array);
    return 0;
}

uint8_t Add_Objectitem(cJSON *root, char *key, cJSON *object) {
    if (root == NULL) {
        return 1;
    } else if (key == NULL || object == NULL) {
        return 2;
    }
    cJSON_AddItemToObject(root, key, object);
    return 0;
}

void cJSON_Clean(user_cjson_t *user_cjson) 
{
    if (user_cjson->tx_str != NULL) {
        cJSON_free(user_cjson->tx_str);
        user_cjson->tx_str = NULL;
    }
    
    if (user_cjson->tx_root != NULL && cJSON_IsObject(user_cjson->tx_root)) {
        cJSON_Delete(user_cjson->tx_root);
        user_cjson->tx_root = NULL;
    }

    if (user_cjson != NULL && user_cjson->rx_root != NULL) {
        cJSON_Delete(user_cjson->rx_root);
        user_cjson->rx_root = NULL;
    }
}

uint8_t Generate_str(user_cjson_t *user_cjson) 
{
    if (user_cjson == NULL || user_cjson->tx_root == NULL) {
        return 1;
    }

    if (user_cjson->tx_str != NULL) {
        cJSON_free(user_cjson->tx_str);
        user_cjson->tx_str = NULL;
    }

    user_cjson->tx_str = cJSON_PrintUnformatted(user_cjson->tx_root);
    if (user_cjson->tx_str == NULL) {
        return 2;
    }

    LOG_I("Generate_str:%s", user_cjson->tx_str);
    return 0;
}

char *get_msg(void *data) {
    char *buf = (char *)data;
    char *result = NULL;
    if (buf == NULL) {
        return NULL;
    } else {
        result = strstr(buf, "{");
    }
    return result;
}

uint8_t Parse_json(char *buf, user_cjson_t *user_cjson)
{
    if (buf == NULL) {
        return 1;
    }
    char *rx_buf = get_msg(buf);
    if (rx_buf == NULL) {
        return 2;
    }
    LOG_I("rx_buf:%s", rx_buf);

    if (rx_buf[0] != '{') { // Validate that rx_buf starts with '{'
        return 3;
    }

    user_cjson->rx_root = cJSON_Parse(rx_buf);
    if (user_cjson->rx_root == NULL) {
        if (rx_buf != NULL) {
            cJSON_free(rx_buf); // Free rx_buf if allocated
        }
        return 3;
    }
    return 0;
}
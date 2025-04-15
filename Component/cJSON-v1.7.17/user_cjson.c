#include "stm32f10x.h"
#include "user_cjson.h"
#include <string.h>

user_cjson_t user_cjson;

cJSON* Creat_Object(void) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return NULL;
    }
    return root;
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

void cJSON_Clean(user_cjson_t user_cjson) {
    if (user_cjson.str != NULL) {
        cJSON_free(user_cjson.str);
        user_cjson.str = NULL;
    }
    if (user_cjson.root != NULL) {
        cJSON_Delete(user_cjson.root);
        user_cjson.root = NULL;
    }
}

uint8_t Generate_str(cJSON *root, user_cjson_t user_cjson) {
    if (root == NULL) {
        return 1;
    }
    user_cjson.str = cJSON_PrintUnformatted(root);
    if (user_cjson.str == NULL) {
        return 2;
    }
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

#ifndef _USER_CJSON_H_
#define _USER_CJSON_H_
#include "cJSON.h"

typedef struct{
    cJSON* root;
    char* str;
}user_cjson_t;


#ifdef __cplusplus
extern "C" {
#endif

user_cjson_t* user_cjson_create(const char* json_str);
void user_cjson_delete(user_cjson_t* user_cjson);
const char* user_cjson_get_string(user_cjson_t* user_cjson, const char* key);
int user_cjson_get_int(user_cjson_t* user_cjson, const char* key);
int user_cjson_set_string(user_cjson_t* user_cjson, const char* key, const char* value);
int user_cjson_set_int(user_cjson_t* user_cjson, const char* key, int value);

#ifdef __cplusplus
}
#endif



#endif

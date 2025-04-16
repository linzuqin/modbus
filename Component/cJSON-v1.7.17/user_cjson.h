#ifndef _USER_CJSON_H_
#define _USER_CJSON_H_
#include "stm32f10x.h"
#include <string.h>
#include "cJSON.h"
#include "log.h"

typedef struct{
    cJSON *tx_root;
    char *tx_str;
    cJSON *rx_root;
}user_cjson_t;


#ifdef __cplusplus
extern "C" {
#endif

 /**
  * @brief Creates a JSON object and assigns it to the tx_root field of the user_cjson structure.
  * 
  * @param user_cjson The user_cjson_t structure to hold the JSON object.
  * @return uint8_t Returns 0 on success, 1 if memory allocation fails.
  */
 uint8_t Create_Object(user_cjson_t *user_cjson); 
 
 /**
  * @brief Adds a string item to a JSON object.
  * 
  * @param root The JSON object to which the string item will be added.
  * @param key The key for the string item.
  * @param value The value of the string item.
  * @return uint8_t Returns 0 on success, 1 if root is NULL, 2 if key or value is NULL.
  */
 uint8_t Add_Stringitem(cJSON *root, char *key, char *value);
 
 /**
  * @brief Adds a number item to a JSON object.
  * 
  * @param root The JSON object to which the number item will be added.
  * @param key The key for the number item.
  * @param value The value of the number item.
  * @return uint8_t Returns 0 on success, 1 if root is NULL, 2 if key is NULL.
  */
 uint8_t Add_Numberitem(cJSON *root, char *key, int value);
 
 /**
  * @brief Adds a boolean item to a JSON object.
  * 
  * @param root The JSON object to which the boolean item will be added.
  * @param key The key for the boolean item.
  * @param value The value of the boolean item (0 or 1).
  * @return uint8_t Returns 0 on success, 1 if root is NULL, 2 if key is NULL.
  */
 uint8_t Add_Boolitem(cJSON *root, char *key, int value);
 
 /**
  * @brief Adds an array item to a JSON object.
  * 
  * @param root The JSON object to which the array item will be added.
  * @param key The key for the array item.
  * @param array The array to be added.
  * @return uint8_t Returns 0 on success, 1 if root is NULL, 2 if key or array is NULL.
  */
 uint8_t Add_Arrayitem(cJSON *root, char *key, cJSON *array);
 
 /**
  * @brief Adds an object item to a JSON object.
  * 
  * @param root The JSON object to which the object item will be added.
  * @param key The key for the object item.
  * @param object The object to be added.
  * @return uint8_t Returns 0 on success, 1 if root is NULL, 2 if key or object is NULL.
  */
 uint8_t Add_Objectitem(cJSON *root, char *key, cJSON *object);
 
 /**
  * @brief Cleans up and frees memory associated with the user_cjson structure.
  * 
  * @param user_cjson The user_cjson_t structure to be cleaned.
  */
 void cJSON_Clean(user_cjson_t *user_cjson); 
 
 /**
  * @brief Generates a JSON string from the tx_root field of the user_cjson structure.
  * 
  * @param user_cjson The user_cjson_t structure containing the JSON object.
  * @return uint8_t Returns 0 on success, 1 if tx_root is NULL, 2 if string generation fails.
  */
 uint8_t Generate_str(user_cjson_t *user_cjson); 
 
 /**
  * @brief Extracts the JSON message from a given data buffer.
  * 
  * @param data The data buffer containing the JSON message.
  * @return char* Pointer to the start of the JSON message, or NULL if not found.
  */
 char *get_msg(void *data);
 
 /**
  * @brief Parses a JSON string and assigns it to the rx_root field of the user_cjson structure.
  * 
  * @param buf The buffer containing the JSON string.
  * @param user_cjson The user_cjson_t structure to hold the parsed JSON object.
  * @return uint8_t Returns 0 on success, 1 if buf is NULL, 2 if JSON message is not found,
  *         3 if parsing fails.
  */
 uint8_t Parse_json(char *buf, user_cjson_t *user_cjson);

#ifdef __cplusplus
}
#endif



#endif

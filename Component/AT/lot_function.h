#ifndef _LOT_FUNCTION_H
#define _LOT_FUNCTION_H
#include "sys.h"

typedef enum{
    LOT_ADD_SUCCESS = 0x01, // 添加参数成功
    LOT_MALLOC_FAIL, // 内存分配失败
    LOT_PARAM_ERR, // 参数错误
    LOT_NOT_FOUND, // 未找到指定参数
}lot_err_t;

typedef struct {
    cJSON* root; // JSON根对象
    char* id; // 指令ID
    char* version; // 版本信息
    char* str; // 输出字符串
    uint16_t len; // 字符串长度
} lot_msg_t;


#include "lot_function.h"

/**
 * @brief 创建JSON根节点，包含id、版本信息和params对象（用于存放指令参数）。
 * 
 * @param msg 指向lot_msg_t结构体的指针，用于存放生成的JSON根节点。
 * @return lot_err_t 返回LOT_ADD_SUCCESS表示成功，否则返回错误码（如LOT_MALLOC_FAIL）。
 */
lot_err_t lot_create_root(lot_msg_t *msg);

/**
 * @brief 向params对象中添加一个字符串类型的参数。
 * 
 * @param msg 指向lot_msg_t结构体的指针，包含JSON根节点。
 * @param key 参数的键名。
 * @param value 参数的字符串值。
 * @return lot_err_t 返回LOT_ADD_SUCCESS表示添加成功，否则返回错误码。
 */
lot_err_t lot_Add_String(lot_msg_t *msg, const char* key, const char* value);

/**
 * @brief 向params对象中添加一个数值类型的参数。
 * 
 * @param msg 指向lot_msg_t结构体的指针，包含JSON根节点。
 * @param key 参数的键名。
 * @param value 参数的数值。
 * @return lot_err_t 返回LOT_ADD_SUCCESS表示添加成功，否则返回错误码。
 */
lot_err_t lot_Add_Number(lot_msg_t *msg, const char* key, double value);

/**
 * @brief 向params对象中添加一个布尔类型的参数。
 * 
 * @param msg 指向lot_msg_t结构体的指针，包含JSON根节点。
 * @param key 参数的键名。
 * @param value 参数的布尔值。
 * @return lot_err_t 返回LOT_ADD_SUCCESS表示添加成功，否则返回错误码。
 */
lot_err_t lot_Add_Bool(lot_msg_t *msg, const char* key, _Bool value);

/**
 * @brief 生成JSON字符串，并保存到msg->str中，同时设置字符串长度msg->len。
 * 
 * @param msg 指向lot_msg_t结构体的指针，包含JSON根节点。
 * @return lot_err_t 返回LOT_ADD_SUCCESS表示生成成功，否则返回错误码。
 */
lot_err_t lot_generate_str(lot_msg_t *msg);

/**
 * @brief 释放lot_msg_t结构体中分配的JSON对象和字符串内存。
 * 
 * @param msg 指向lot_msg_t结构体的指针。
 */
void lot_clean(lot_msg_t *msg);


#endif


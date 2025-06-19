#include "lot_function.h"

/*创建根节点 根节点包含id ，版本信息 , params对象(固定字段 存放指令包含的参数)*/
lot_err_t lot_create_root(lot_msg_t *msg)
{
    msg->root = cJSON_CreateObject();
    if (msg->root == NULL) 
    {
        LOG_E("Failed to create JSON root object");
        return LOT_MALLOC_FAIL; // 创建JSON根对象失败
    }

    cJSON_AddStringToObject(msg->root, "version", "1.0"); // 添加版本信息
    cJSON_AddStringToObject(msg->root, "id", "1"); // 添加指令ID
    cJSON *params_json = cJSON_CreateObject(); // 创建参数对象
    if (params_json == NULL) 
    {
        LOG_E("Failed to create JSON params object");
        cJSON_Delete(msg->root); // 删除已创建的根对象
        return LOT_MALLOC_FAIL; // 创建参数对象失败
    }
    cJSON_AddItemToObject(msg->root, "params", params_json); // 将参数对象添加到根对象中
    return LOT_ADD_SUCCESS; // 返回创建的JSON根对象
}

lot_err_t lot_Add_String(lot_msg_t *msg, const char* key, const char* value)
{
    if (msg == NULL) 
    {
        LOG_E("Message object is NULL");
        return LOT_PARAM_ERR; // 参数错误
    }

    if (key == NULL) 
    {
        LOG_E("Key is NULL");
        return LOT_PARAM_ERR; // 参数错误
    }

    if (value == NULL) 
    {
        LOG_E("Value is NULL");
        return LOT_PARAM_ERR; // 参数错误
    }

    cJSON* params = cJSON_GetObjectItem(msg->root, "params");
    if (params == NULL) 
    {
        LOG_E("Params object not found in JSON root");
        return LOT_NOT_FOUND; // 未找到参数对象
    }

    cJSON* item_js = cJSON_CreateObject();
    if (item_js == NULL) 
    {
        LOG_E("Failed to create JSON object for key: %s", key);
        return LOT_MALLOC_FAIL; // 内存分配失败
    }

    cJSON_AddStringToObject(item_js, "value", value); // 添加键值对到新对象
    if (msg->root == NULL) 
    {
        LOG_E("Message root is NULL");
        return LOT_PARAM_ERR; // 参数错误
    }

    cJSON_AddItemToObject(params, key, item_js); // 将新对象添加到params对象中
    //LOG_I("Added number to JSON: key=%s, value=%f", key, value);
    return LOT_ADD_SUCCESS; // 添加成功
}

lot_err_t lot_Add_Number(lot_msg_t *msg, const char* key, double value)
{
    if (msg == NULL) 
    {
        LOG_E("Message object is NULL");
        return LOT_PARAM_ERR;
    }

    if (key == NULL) 
    {
        LOG_E("Key is NULL");
        return LOT_PARAM_ERR;
    }

    cJSON* params = cJSON_GetObjectItem(msg->root, "params");
    if (params == NULL) 
    {
        LOG_E("Params object not found in JSON root");
        return LOT_NOT_FOUND;
    }

    cJSON* item_js = cJSON_CreateObject();
    if (item_js == NULL) 
    {
        LOG_E("Failed to create JSON object for key: %s", key);
        return LOT_MALLOC_FAIL;
    }

    cJSON_AddNumberToObject(item_js, "value", value);

    cJSON_AddItemToObject(params, key, item_js);

    return LOT_ADD_SUCCESS;
}

lot_err_t lot_Add_Bool(lot_msg_t *msg, const char* key, _Bool value)
{
    if (msg->root == NULL || key == NULL) {
        LOG_E("Invalid parameters for adding boolean to JSON");
        return LOT_PARAM_ERR; // 参数错误
    }

    cJSON* params = cJSON_GetObjectItem(msg->root, "params");
    if (params == NULL) {
        LOG_E("Params object not found in JSON root");
        return LOT_NOT_FOUND; // 未找到参数对象
    }

    cJSON* item_js = cJSON_CreateObject();
    if (item_js == NULL) {
        LOG_E("Failed to create JSON object for key: %s", key);
        return LOT_MALLOC_FAIL; // 内存分配失败
    }

    cJSON_AddBoolToObject(item_js, "value", value); // 添加键值对到新对象

    cJSON_AddItemToObject(params, key, item_js); // 将新对象添加到params对象中
    //LOG_I("Added boolean to JSON: key=%s, value=%d", key, value);
    return LOT_ADD_SUCCESS; // 添加成功
}

lot_err_t lot_generate_str(lot_msg_t *msg)
{
    if (msg == NULL || msg->root == NULL) {
        LOG_E("Message or root is NULL");
        return LOT_PARAM_ERR;
    }

    char *json_str = cJSON_PrintUnformatted(msg->root);
    if (json_str == NULL) {
        LOG_E("Failed to generate JSON string");
        return LOT_MALLOC_FAIL;
    }

    msg->str = json_str;
    msg->len = strlen(json_str); // 计算字符串长度
    return LOT_ADD_SUCCESS;
}

void lot_clean(lot_msg_t *msg)
{
    if (msg == NULL) {
        LOG_E("Message is NULL");
        return; // 无效的消息对象
    }

    if (msg->root != NULL) {
        cJSON_Delete(msg->root); // 删除JSON根对象
        msg->root = NULL; // 清空根对象指针
    }

    if (msg->str != NULL) {
        free(msg->str); // 释放字符串内存
        msg->str = NULL; // 清空字符串指针
    }
}


#include "AT_Function.h"

AT_Device_t AT_Device = {
    .status = AT_HW_INIT,
    .rx_buf = NULL,
    .rx_flag = NULL,
    .PORT = NULL,
    .Bound = 0,
    .CMD_TABLE = NULL,
    .URC_TABLE = NULL};

AT_CMD_t AT_Cmd_table[AT_COMMAND_ARRAY_SIZE] = {
    {"AT\r\n", "OK", 500, NULL},
    {"AT+CWMODE=1\r\n", "OK", 500, NULL},
    {"AT+RST\r\n", "ready", 3000, NULL},
    {"AT+CWDHCP=1,1\r\n", "OK", 500, NULL},
    {NULL, NULL, 0, NULL}};

AT_URC_t AT_URC_table[AT_COMMAND_ARRAY_SIZE] = {
    {"WIFI GOT IP", NULL},
    {"WIFI DISCONNECT", NULL},
    {"CLOSED", NULL},
    {"SEND OK", NULL},
    {"ERROR", NULL},
    {"SIMDETEC: 1,NOS", NULL},
    {NULL, NULL}};

uint8_t AT_SendCmd(AT_Device_t *at_device, const char *cmd, const char *response, uint16_t timeout)
{
    if (!at_device || !cmd)
    {
        //LOG_E("Invalid parameters");
        return AT_ERR_INVALID_PARAM;
    }

    if (!at_device->PORT)
    {
        //LOG_E("UART port not initialized");
        return AT_ERR_UART_NOT_INIT;
    }

    uint16_t len = strlen(cmd);
    //LOG_I("Sending command, len: %d", len);

    for (uint16_t i = 0; i < len; i++)
    {
        USART_SendData(at_device->PORT, cmd[i]);
        while (USART_GetFlagStatus(at_device->PORT, USART_FLAG_TC) == RESET)
        {

        }
    }

    //LOG_I("->: %.*s", len > 50 ? 50 : len, cmd);

    if (!response)
    {
        return AT_SUCCESS;
    }

    uint8_t timeout_count = 5;
    while (timeout_count-- > 0)
    {
        if (strstr((char *)at_device->rx_buf, response))
        {
            //LOG_I("<-: %s", response);
            memset(at_device->rx_buf, 0, AT_RX_BUF_SIZE);
            return AT_SUCCESS;
        }
        rt_thread_mdelay(timeout);
    }

    LOG_E("Timeout waiting for response: %s", response);
    memset(at_device->rx_buf, 0, AT_RX_BUF_SIZE);
    return AT_ERR_TIMEOUT;
}

/**
 * @brief Publish MQTT message
 * @param at_device Pointer to AT device structure
 * @param data Message data
 * @param len Message length
 * @param topic Topic string
 * @return 0 on success, error code on failure
 */
uint8_t mqtt_pub(AT_Device_t *at_device, char *data, uint16_t len, const char *topic)
{
    if (!at_device || !data || !topic)
    {
        return AT_ERR_INVALID_PARAM;
    }

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUBRAW=0,\"%s\",%d,0,0\r\n", topic, len);

    if (AT_SendCmd(at_device, cmd, ">", 1000) != AT_SUCCESS)
    {
        return AT_ERR_MQTT_PUB;
    }

    return AT_SendCmd(at_device, data, "OK", 1000);
}

uint8_t AT_Cmd_Register(AT_Device_t *at_device, const char *response,
                        uint16_t timeout, void (*callback_response)(void),
                        int insert_count, const char *cmd, ...)
{
    if (!at_device || !cmd || !response)
    {
        LOG_E("Invalid parameters");
        return AT_ERR_INVALID_PARAM;
    }

    if (!at_device->CMD_TABLE)
    {
        LOG_E("CMD_TABLE not initialized");
        return AT_ERR_TABLE_NOT_INIT;
    }

    char cmd_buf[256];
    va_list ap;
    va_start(ap, cmd);
    vsnprintf(cmd_buf, sizeof(cmd_buf), cmd, ap);
    va_end(ap);

    //LOG_I("Registering command: %s", cmd_buf);

    int slot = -1;

    if (insert_count >= 0 && insert_count < AT_COMMAND_ARRAY_SIZE)
    {
        slot = insert_count;
    }
    else
    {
        for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            if (at_device->CMD_TABLE[i].cmd == NULL)
            {
                slot = i;
                break;
            }
        }

        if (slot == -1 && insert_count < 0)
        {
            for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
            {
                if (i == AT_COMMAND_ARRAY_SIZE - 1 || at_device->CMD_TABLE[i + 1].cmd == NULL)
                {
                    slot = i;
                    break;
                }
            }
        }
    }

    if (slot == -1)
    {
        //LOG_E("No available slots in command table");
        return AT_ERR_TABLE_FULL;
    }

    size_t cmd_len = strlen(cmd_buf) + 1;
    char *cmd_copy = (char *)malloc(cmd_len);
    if (!cmd_copy)
    {
        //LOG_E("Memory allocation failed for command string");
        return AT_ERR_MEMORY;
    }
    strncpy(cmd_copy, cmd_buf, cmd_len);

    if (at_device->CMD_TABLE[slot].cmd)
    {
        free((char *)at_device->CMD_TABLE[slot].cmd);
    }

    at_device->CMD_TABLE[slot] = (AT_CMD_t){
        .cmd = cmd_copy,
        .response = response,
        .timeout = timeout,
        .callback_response = callback_response};

    //LOG_I("Command registered at slot %d: %s", slot, cmd_copy);
    return AT_SUCCESS;
}

/**
 * @brief Register user commands
 */
void user_cmd_register(void)
{
    AT_Cmd_Register(&AT_Device, "WIFI CONNECTED", 1000, NULL, -1,
                    "AT+CWJAP=\"%s\",\"%s\"\r\n", DEFAULT_WIFI_SSID, DEFAULT_WIFI_PWD);

    AT_Cmd_Register(&AT_Device, "OK", 1000, NULL, -1,
                    "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
                    DEVICE_NAME, PRODUCT_ID, TOKEN);

    AT_Cmd_Register(&AT_Device, "OK", 1000, NULL, -1,
                    "AT+MQTTCONN=0,\"%s\",%d,1\r\n", IP_ADDRESS, PORT_NUMBER);

    AT_Cmd_Register(&AT_Device, "OK", 1000, NULL, -1,
                    "AT+MQTTSUB=0,\"%s\",1\r\n", SET_TOPIC_ALL);
}

/**
 * @brief Register AT device
 * @param at_device Pointer to AT device structure
 * @param uart_device Pointer to UART device structure
 * @param cmd_table Pointer to command table
 * @param urc_table Pointer to URC table
 */
void at_device_register(AT_Device_t *at_device, uart_device_t *uart_device,
                        AT_CMD_t *cmd_table, AT_URC_t *urc_table)
{
    if (!at_device || !uart_device)
        return;

    at_device->PORT = uart_device->port;
    at_device->Bound = uart_device->baudrate;
    at_device->rx_buf = uart_device->rx_buffer;
    at_device->rx_flag = &uart_device->rx_flag;

    at_device->CMD_TABLE = cmd_table ? cmd_table : AT_Cmd_table;
    at_device->URC_TABLE = urc_table ? urc_table : AT_URC_table;
    
    user_cmd_register();
}

static void AT_INIT_TASK(AT_Device_t *at_device)
{
    static uint8_t step = 0;

    if(step >= AT_COMMAND_ARRAY_SIZE)
    {
        step = 0;
        at_device->status = AT_IDLE;
        return;
    }
    else if(at_device->CMD_TABLE[step].cmd == NULL)
    {
        step = 0;
        at_device->status = AT_IDLE;
        return;
    }else{
        if(AT_SendCmd(at_device , at_device->CMD_TABLE[step].cmd , at_device->CMD_TABLE[step].response, at_device->CMD_TABLE[step].timeout) == AT_SUCCESS)
        {
            if(at_device->CMD_TABLE[step].callback_response != NULL)
            {
                at_device->CMD_TABLE[step].callback_response();
            }
            step++;
        }
    }
}

static void AT_IDLE_TASK(AT_Device_t *at_device)
{
    if (!at_device || !at_device->rx_flag || !(*at_device->rx_flag))
        return;

    *at_device->rx_flag = 0;

    for (uint16_t i = 0; i < AT_COMMAND_ARRAY_SIZE && at_device->URC_TABLE[i].urc_msg; i++)
    {
        char *urc_msg = strstr((char *)at_device->rx_buf, at_device->URC_TABLE[i].urc_msg);
        if (urc_msg)
        {
            if (at_device->URC_TABLE[i].callback)
            {
                at_device->URC_TABLE[i].callback();
            }
            //LOG_I("URC received: %s", at_device->URC_TABLE[i].urc_msg);
            memset(urc_msg, 0, strlen(at_device->URC_TABLE[i].urc_msg));
            return;
        }
    }

    at_device->status = AT_PARSE;
}

static void AT_PARSE_TASK(AT_Device_t *at_device)
{
    at_device->status = AT_IDLE; // Return to idle after parsing
}

/**
 * @brief Main AT device polling function
 * @param at_device Pointer to AT device structure
 */
void AT_poll(AT_Device_t *at_device)
{
    if (!at_device)
        return;

    switch (at_device->status)
    {
        case AT_HW_INIT:
            My_UART_Init(&AT_DEFAULT_UART_DEVICE);
            at_device->status = AT_REGISTER;
            break;

        case AT_REGISTER:
            // Initialization sequence could go here
            at_device_register(at_device, &AT_DEFAULT_UART_DEVICE, NULL, NULL);
            at_device->status = AT_INIT;
            break;

        case AT_INIT:
            // Initialization sequence could go here
            AT_INIT_TASK(at_device);
            at_device->status = AT_IDLE;
            break;

        case AT_IDLE:
            AT_IDLE_TASK(at_device);
            break;

        case AT_PARSE:
            AT_PARSE_TASK(at_device);
            break;

        case AT_UPDATA:
            // Update sequence could go here
            at_device->status = AT_IDLE;
            break;
        default:
            break;
    }
}

/**
 * @brief AT thread entry function
 * @param parameter Thread parameter (unused)
 */
void AT_Thread_Entry(void *parameter)
{
    while (1)
    {
        AT_poll(&AT_Device);
        rt_thread_mdelay(10);
    }
}

// Thread initialization (unchanged)
struct rt_thread at_thread;
static uint8_t at_thread_stack[4096];

int AT_Thread_Init(void)
{
    rt_err_t result = rt_thread_init(&at_thread,
                                     "at_thread",
                                     AT_Thread_Entry,
                                     NULL,
                                     at_thread_stack,
                                     sizeof(at_thread_stack),
                                     10,
                                     100);
    if (result == RT_EOK)
    {
        rt_thread_startup(&at_thread);
        return 0;
    }

    //LOG_E("Failed to initialize AT thread");
    return -1;
}

INIT_APP_EXPORT(AT_Thread_Init);
#include "master_function.h"

#if MODBUS_MASTER_SENSOR_ENABLE

#define SENSOR_MB_PANNEL_STACK_SIZE 1024                            //modbus master task stack size
static struct rt_thread SENSOR_MB_PANNEL;                           //modbus master task
static char SENSOR_MB_PANNEL_STACK[SENSOR_MB_PANNEL_STACK_SIZE];    //modbus master task stack

static uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus send buffer
static uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus read buffer

typedef struct SensorDevice_read
{
    uint8_t device_addr;                        //设备地址
    uint8_t function_code;                      //功能码
    uint16_t start_addr;                        //寄存器起始地址
    uint16_t read_num;                          //读取寄存器数量
    uint16_t *data_buf;                         //数据缓冲区
    uint8_t sensor_num;                         //传感器数量 用于相同传感器下的遍历读取 设备地址自增
} SensorDevice_read_cmd_t;

SensorDevice_read_cmd_t sensor_device_read_cmd[] = {
    {1,0x03,0x0000,10,NULL , 1},                            // 地址1，功能码3 起始地址0 读取寄存器数量10 数据接收区无 传感器数量1

};

static uint8_t sensor_poll(void)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t result = 0;
    uint8_t sensor_num = sizeof(sensor_device_read_cmd) / sizeof(sensor_device_read_cmd[0]);

    static agile_modbus_rtu_t ctx_rtu;
    static agile_modbus_t *ctx = &ctx_rtu._ctx;
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));

    for (i = 0; i < sensor_num; i++)
    {
        for (j = 0; j < sensor_device_read_cmd[i].sensor_num; j++)
        {
            if(sensor_device_read_cmd[i].function_code == 0x03)
            {
                result = mater_mb_hold_read(&ctx_rtu._ctx, sensor_device_read_cmd[i].device_addr, sensor_device_read_cmd[i].start_addr + j, sensor_device_read_cmd[i].data_buf, sensor_device_read_cmd[i].read_num, MB_M_DEVICE.rx_buffer);                
            }
            else if(sensor_device_read_cmd[i].function_code == 0x04)
            {
                result = mater_mb_input_read(&ctx_rtu._ctx, sensor_device_read_cmd[i].device_addr, sensor_device_read_cmd[i].start_addr + j, sensor_device_read_cmd[i].data_buf, sensor_device_read_cmd[i].read_num, MB_M_DEVICE.rx_buffer);
            }

            if(result != 0)return (j+1); //读取失败
        }
    }
    return 0;
}

void User_Sensor_task(void *params)
{
    uint8_t result = 0;
    while (1) 
    {
        result = sensor_poll();         //read sensor data
        if(result != 0)
        {
            LOG_E("read error :%d\r\n" , result);
        }
        rt_thread_mdelay(100);
    }
}

static int User_Sensor_start(void)
{
    rt_err_t result;
    result = rt_thread_init(&SENSOR_MB_PANNEL, "SENSOR_MB_TASK", User_Sensor_task, RT_NULL, &SENSOR_MB_PANNEL_STACK[0], sizeof(SENSOR_MB_PANNEL_STACK), 22, 100);
    if (result == RT_EOK)
    {
        rt_thread_startup(&SENSOR_MB_PANNEL);
        LOG_I("SENSOR MB TASK START OK\r\n");
    }
    else
    {
        LOG_E("SENSOR MB TASK START FAIL\r\n");
    }
		
    #if MODBUS_MASTER_ENABLE
        My_UART_Init(&MODBUS_MASTER_SENSOR_DEVICE);
    #endif

    return 1;
}

#endif

	
#if MODBUS_MASTER_SENSOR_ENABLE
INIT_APP_EXPORT(User_Sensor_start);
#endif
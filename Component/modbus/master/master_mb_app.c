#include "master_function.h"

#if MODBUS_MASTER_ENABLE

#define MASTER_MB_PANNEL_STACK_SIZE 1024                            //modbus master task stack size
static struct rt_thread MASTER_MB_PANNEL;                           //modbus master task
static char MASTER_MB_PANNEL_STACK[MASTER_MB_PANNEL_STACK_SIZE];    //modbus master task stack

void User_master_task(void *params)
{
    static uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus send buffer
    static uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];                   //agile modbus read buffer
    agile_modbus_rtu_t ctx_rtu;
    agile_modbus_t *ctx = &ctx_rtu._ctx;
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    uint8_t coil_reg[10];                                                       //test coil reg
    uint8_t disc_reg[10];                                                       //test disc reg                                
    uint16_t hold_reg[10];                                                      //test hold reg                                   
    uint16_t input_reg[10];                                                     //test input reg
    uint8_t i = 0;
    while (1) 
    {
        mater_mb_coil_read(ctx , 1 , 0 , coil_reg , 10 , MB_M_DEVICE.rx_buffer);
        mater_mb_disc_read(ctx , 1 , 0 , disc_reg , 10 , MB_M_DEVICE.rx_buffer);
        mater_mb_hold_read(ctx , 1 , 0 , hold_reg , 10 , MB_M_DEVICE.rx_buffer);
        mater_mb_input_read(ctx , 1 , 0 , input_reg , 10 , MB_M_DEVICE.rx_buffer);
        

        LOG_I("coil_reg [%d]: 0x%04X", 0, coil_reg[0]);
        LOG_I("disc_reg [%d]: 0x%04X", 0, disc_reg[0]);
        LOG_I("hold_reg [%d]: 0x%04X", 0, hold_reg[0]);
        LOG_I("input_reg [%d]: 0x%04X", 0, input_reg[0]);
        mater_mb_hold_write(ctx , 1 , 0 , i++ , MB_M_DEVICE.rx_buffer);
        rt_thread_mdelay(1000);
    }
}

static int User_master_start(void)
{
    rt_err_t result;
    result = rt_thread_init(&MASTER_MB_PANNEL, "MASTER_MB_TASK", User_master_task, RT_NULL, &MASTER_MB_PANNEL_STACK[0], sizeof(MASTER_MB_PANNEL_STACK), 22, 100);
    if (result == RT_EOK)
    {
        rt_thread_startup(&MASTER_MB_PANNEL);
        LOG_I("MASTER_MB TASK START OK\r\n");
    }
    else
    {
        LOG_E("MASTER_MB TASK START FAIL\r\n");
    }
		
    #if MODBUS_MASTER_ENABLE
        My_UART_Init(&MB_M_DEVICE);
    #endif

    return 1;
}

#endif

	
#if MODBUS_MASTER_ENABLE
INIT_APP_EXPORT(User_master_start);
#endif
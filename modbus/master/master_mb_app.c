#include "master_mb_app.h"
#include "log.h"
#include "rtthread.h"
#include "agile_modbus.h"
#include "stm32f10x.h"                  // Device header
#include "main.h"

#if MODBUS_MASTER_ENABLE

#define MASTER_MB_PANNEL_STACK_SIZE 1024                            //modbus master task stack size
static struct rt_thread MASTER_MB_PANNEL;                           //modbus master task
static char MASTER_MB_PANNEL_STACK[MASTER_MB_PANNEL_STACK_SIZE];    //modbus master task stack

static uint8_t *rx_buf = UART4_RxData;                         //serial rx buffer
static uint8_t *rx_flag = &UART4_RxFlag;                        //serial rx flag

void serial_send(USART_TypeDef* USARTx ,uint8_t *data , int send_len)
{
	uint16_t i;
	for (i = 0; i < send_len; i ++)
	{
			USART_SendData(USARTx, data[i]);
			while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}
	
/*read coil reg data*/
int mater_mb_coil_read(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint8_t *coil_register , uint16_t num , uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);
    int send_len = agile_modbus_serialize_read_bits(ctx, start_addr, num);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
    uint8_t timeout;
    while(*rx_flag == 0)
    {
		if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }

    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_read_bits(ctx, read_len, coil_register);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }

    LOG_I("Coil Registers Read Success:");
    //for (int i = 0; i < num; i++)
        //LOG_I("Register [%d]: 0x%04X", i, coil_register[i]);
    
    return 0;
}

/*write single coil reg data*/
int mater_mb_coil_write(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr  , uint8_t status, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_write_bit(ctx, start_addr, status);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
    uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_write_bit(ctx, read_len);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }
    LOG_I("COIL Registers write success");
    
    return 0;
}

/*write coil regs data*/
int mater_mb_coil_writes(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint8_t num , uint8_t* data_buf, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_write_bits(ctx, start_addr, num , data_buf);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
    uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_write_bits(ctx, read_len);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }
    LOG_I("COIL Registers write success");
    
    return 0;
}

/*read disc reg data*/
int mater_mb_disc_read(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint8_t *disc_register , uint16_t num, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_read_input_bits(ctx, start_addr, num);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
		uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_read_input_bits(ctx, read_len, disc_register);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }

    LOG_I("Disc Registers Read Success:");
    //for (int i = 0; i < num; i++)
        //LOG_I("Register [%d]: 0x%04X", i, disc_register[i]);
    
    return 0;
}

/*read hold reg data*/
int mater_mb_hold_read(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint16_t *hold_register , uint16_t num, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_read_registers(ctx, start_addr, num);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
		uint8_t timeout;
    while(*rx_flag == 0)
    {
			if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);
    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_read_registers(ctx, read_len, hold_register);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }

    LOG_I("Hold Registers:");
    //for (int i = 0; i < num; i++)
        //LOG_I("Register [%d]: 0x%04X", i, hold_register[i]);
    
    return 0;
}

/*write single hold reg data*/
int mater_mb_hold_write(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint16_t Value, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_write_register(ctx, start_addr, Value);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
    uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_write_register(ctx, read_len);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }
    LOG_I("COIL Registers write success");
    
    return 0;
}

/*write hold regs data*/
int mater_mb_hold_writes(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint16_t num , uint16_t* databuf, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_write_registers(ctx, start_addr, num , databuf);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
    uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_write_registers(ctx, read_len);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }
    LOG_I("COIL Registers write success");
    return 0;
}

/*read input reg data*/
int mater_mb_input_read(agile_modbus_t *ctx , uint8_t slave_addr , uint16_t start_addr , uint16_t *input_register , uint16_t num, uint8_t *rx_buf)
{
    agile_modbus_set_slave(ctx, slave_addr);
    rt_memset( ctx->read_buf , 0 , ctx->read_bufsz);

    int send_len = agile_modbus_serialize_read_input_registers(ctx, start_addr, num);
    serial_send(MODBUS_MASTER_SERIAL , ctx->send_buf , send_len);
		uint8_t timeout;
    while(*rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    *rx_flag = 0;
    int read_len = rx_buf[1];
    rt_memcpy(ctx->read_buf , &rx_buf[2] , read_len);
		rt_memset(rx_buf , 0 , read_len + 2);

    if (read_len < 0) {
        LOG_E("Receive error, now exit.");
        return -1;
    }

    if (read_len == 0) {
        LOG_W("Receive timeout.");
        return -1;
    }

    int rc = agile_modbus_deserialize_read_input_registers(ctx, read_len, input_register);
    if (rc < 0) {
        LOG_W("Receive failed.");
        if (rc != -1)
            LOG_W("Error code:%d", -128 - rc);

        return -1;
    }

    LOG_I("Input Registers Read Success:");
    //for (int i = 0; i < num; i++)
        //LOG_I("Register [%d]: 0x%04X", i, input_register[i]);
    return 0;
}

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
        mater_mb_coil_read(ctx , 1 , 0 , coil_reg , 10 , rx_buf);
        mater_mb_disc_read(ctx , 1 , 0 , disc_reg , 10 , rx_buf);
        mater_mb_hold_read(ctx , 1 , 0 , hold_reg , 10 , rx_buf);
        mater_mb_input_read(ctx , 1 , 0 , input_reg , 10 , rx_buf);
        

        LOG_I("coil_reg [%d]: 0x%04X", 0, coil_reg[0]);
        LOG_I("disc_reg [%d]: 0x%04X", 0, disc_reg[0]);
        LOG_I("hold_reg [%d]: 0x%04X", 0, hold_reg[0]);
        LOG_I("input_reg [%d]: 0x%04X", 0, input_reg[0]);
        mater_mb_hold_write(ctx , 1 , 0 , i++ , rx_buf);
        rt_thread_mdelay(1000);
    }
}

void User_master_start(void)
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
		
		
		if(MODBUS_MASTER_SERIAL == USART1)
		{
			UART1_Init(MODBUS_MASTER_BOUND);
		}else if(MODBUS_MASTER_SERIAL == USART2){
			UART2_Init(MODBUS_MASTER_BOUND);

		}else if(MODBUS_MASTER_SERIAL == USART3){
			UART3_Init(MODBUS_MASTER_BOUND);

		}else if(MODBUS_MASTER_SERIAL == UART4){
			UART4_Init(MODBUS_MASTER_BOUND);

		}else if(MODBUS_MASTER_SERIAL == UART5){
			UART5_Init(MODBUS_MASTER_BOUND);
		}
}

#endif

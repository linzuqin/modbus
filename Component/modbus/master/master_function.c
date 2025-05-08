
/**
 * @file master_function.c
 * @brief Implementation of Modbus master functions for reading and writing Modbus registers.
 * 
 * This file contains functions to handle Modbus master operations such as reading and writing
 * coil, discrete input, holding, and input registers. It also includes a helper function for
 * sending data over a serial interface.
 */

#include "master_function.h"

#if	MODBUS_MASTER_ENABLE

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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
    uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
		if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }

    MB_M_DEVICE.rx_flag = 0;
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
    uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
		MB_M_DEVICE.rx_flag = 0;
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
    uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
		uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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

    if(disc_register == NULL)
    {
        LOG_E("disc_register is NULL");
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
		uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
		if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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

    if(hold_register == NULL)
    {
        LOG_E("Hold register buffer is NULL.");
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
    uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
    uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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
    serial_send(MB_M_DEVICE.port , ctx->send_buf , send_len);
		uint8_t timeout;
    while(MB_M_DEVICE.rx_flag == 0)
    {
        if(timeout ++ > 5)return -1;
        rt_thread_mdelay(100);
    }
    MB_M_DEVICE.rx_flag = 0;
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
    if(input_register == NULL)
    {
        LOG_E("Input register buffer is NULL.");
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

#endif

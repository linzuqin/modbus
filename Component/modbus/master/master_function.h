#ifndef _MASTER_FUNCTION_H
#define _MASTER_FUNCTION_H
#include "agile_modbus.h"
#include "agile_modbus_rtu.h"

#include "sys.h"

 /**
  * @brief Sends data over the serial interface.
  * 
  * @param USARTx Pointer to the USART peripheral.
  * @param data Pointer to the data buffer to send.
  * @param send_len Length of the data to send.
  */
 void serial_send(USART_TypeDef* USARTx, uint8_t *data, int send_len);
 
 /**
  * @brief Reads coil register data from a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the coil registers.
  * @param coil_register Pointer to the buffer to store the coil register data.
  * @param num Number of coil registers to read.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_coil_read(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint8_t *coil_register, uint16_t num, uint8_t *rx_buf);
 
 /**
  * @brief Writes a single coil register to a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Address of the coil register to write.
  * @param status Status to write to the coil register (ON/OFF).
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_coil_write(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint8_t status, uint8_t *rx_buf);
 
 /**
  * @brief Writes multiple coil registers to a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the coil registers.
  * @param num Number of coil registers to write.
  * @param data_buf Pointer to the buffer containing the data to write.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_coil_writes(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint8_t num, uint8_t* data_buf, uint8_t *rx_buf);
 
 /**
  * @brief Reads discrete input register data from a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the discrete input registers.
  * @param disc_register Pointer to the buffer to store the discrete input register data.
  * @param num Number of discrete input registers to read.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_disc_read(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint8_t *disc_register, uint16_t num, uint8_t *rx_buf);
 
 /**
  * @brief Reads holding register data from a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the holding registers.
  * @param hold_register Pointer to the buffer to store the holding register data.
  * @param num Number of holding registers to read.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_hold_read(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint16_t *hold_register, uint16_t num, uint8_t *rx_buf);
 
 /**
  * @brief Writes a single holding register to a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Address of the holding register to write.
  * @param Value Value to write to the holding register.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_hold_write(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint16_t Value, uint8_t *rx_buf);
 
 /**
  * @brief Writes multiple holding registers to a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the holding registers.
  * @param num Number of holding registers to write.
  * @param databuf Pointer to the buffer containing the data to write.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_hold_writes(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint16_t num, uint16_t* databuf, uint8_t *rx_buf);
 
 /**
  * @brief Reads input register data from a Modbus slave.
  * 
  * @param ctx Pointer to the Modbus context.
  * @param slave_addr Address of the Modbus slave.
  * @param start_addr Starting address of the input registers.
  * @param input_register Pointer to the buffer to store the input register data.
  * @param num Number of input registers to read.
  * @param rx_buf Pointer to the receive buffer.
  * @return 0 on success, -1 on failure.
  */
 int mater_mb_input_read(agile_modbus_t *ctx, uint8_t slave_addr, uint16_t start_addr, uint16_t *input_register, uint16_t num, uint8_t *rx_buf);
 
#endif

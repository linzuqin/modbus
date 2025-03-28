/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "stm32f10x.h"
#include "main.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "string.h"
/* ----------------------- static functions ---------------------------------*/
extern UCHAR  ucRTUBuf[256];
extern uint8_t UART4_RxData[256];
extern uint8_t UART4_Count;

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortSerialInit(uint32_t bound)//适配串口初始化
{
	#if MODBUS_SLAVE_UART1
		UART1_Init(bound);
	#endif
	
	#if MODBUS_SLAVE_UART2
		UART2_Init(bound);
	#endif     

	#if MODBUS_SLAVE_UART3
		UART3_Init(bound);
	#endif
	
	#if MODBUS_SLAVE_UART4
		UART4_Init(bound);
	#endif
	
	#if MODBUS_SLAVE_UART5
		UART5_Init(bound);
	#endif
	return TRUE;
}

BOOL
xMBPortSerialPutByte(uint8_t port , UCHAR *ucByte , uint16_t len)//适配串口发送函数
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
		USART_TypeDef* USARTx;
		switch(port)
		{
			case 1:USARTx = USART1;break;
			case 2:USARTx = USART2;break;
			case 3:USARTx = USART3;break;
			case 4:USARTx = UART4;break;
			case 5:USARTx = UART5;break;
			default:return 0;
		}
		uint16_t i;
		for (i = 0; i < len; i ++)
		{
			USART_SendData(USARTx, ucByte[i]);
			while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		}
		return TRUE;
}

//BOOL
//xMBPortSerialGetByte(uint8_t *pucByte)//适配串口接收函数
//{
//    /* Return the byte in the UARTs receive buffer. This function is called
//     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
//     */
//		memcpy(pucByte , UART4_RxData , UART4_RxData[0] + 1);
//		memset(UART4_RxData , 0 , 256);
//    return TRUE;
//}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
//static void prvvUARTTxReadyISR(void)
//{
//    pxMBFrameCBTransmitterEmpty();
//}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(uint8_t* data )
{
    pxMBFrameCBByteReceived(data);
}

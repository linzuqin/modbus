#include "uart.h"

uint8_t uart1_rx_buf[uart1_rx_size];
uint8_t uart2_rx_buf[uart2_rx_size];
uint8_t uart3_rx_buf[uart3_rx_size];
// uint8_t uart4_rx_buf[uart_rx_size];
// uint8_t uart5_rx_buf[uart_rx_size];
// 使用以下方式确保对齐
uint8_t uart1_tx_buf[uart1_tx_size] __attribute__((aligned(4)));
uint8_t uart2_tx_buf[uart2_tx_size] __attribute__((aligned(4)));
uint8_t uart3_tx_buf[uart3_tx_size] __attribute__((aligned(4)));


uart_device_t uart_devices[5] = {
  {USART1, 115200, 0, 0, uart1_rx_size , uart1_tx_size , uart1_rx_buf , uart1_tx_buf },
  {USART2, 115200, 0, 0, uart2_rx_size , uart2_tx_size , uart2_rx_buf , uart2_tx_buf },
  {USART3, 115200, 0, 0, uart3_rx_size , uart3_tx_size , uart3_rx_buf , uart3_tx_buf },
  // {UART4,  115200, 0, 0, uart4_rx_buf , &u4_mutex},
  // {UART5,  115200, 0, 0, uart5_rx_buf , &u5_mutex},
};

void My_UART_Init(uart_device_t *uart_device) {
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitTypeDef DMA_TxInitStructure;

  DMA_Channel_TypeDef *DMA_RxChannel = NULL;
  DMA_Channel_TypeDef *DMA_TxChannel = NULL;
  uint32_t buf_addr = 0;
  uint16_t rx_size = uart_device->rx_max_size;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  switch ((uint32_t)uart_device->port) {
    case (uint32_t)USART1:
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOA, GPIO_Pin_10, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      DMA_RxChannel = DMA1_Channel5;
      DMA_TxChannel = DMA1_Channel4;  // USART1_TX uses DMA1 Channel4
      buf_addr = (uint32_t)&USART1->DR;
      break;
    case (uint32_t)USART2:
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      DMA_RxChannel = DMA1_Channel6;
      DMA_TxChannel = DMA1_Channel7;  // USART2_TX uses DMA1 Channel7

      buf_addr = (uint32_t)&USART2->DR;
      break;
    case (uint32_t)USART3:
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOB, GPIO_Pin_10, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOB, GPIO_Pin_11, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
      DMA_RxChannel = DMA1_Channel3;
      DMA_TxChannel = DMA1_Channel2;  // USART3_TX uses DMA1 Channel2
      buf_addr = (uint32_t)&USART3->DR;
      break;
    default:
      LOG_E("error USARTx");
      return;
  }

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  USART_ITConfig(uart_device->port, USART_IT_IDLE, ENABLE);
  USART_ITConfig(uart_device->port, USART_IT_TC, DISABLE);
  USART_ITConfig(uart_device->port, USART_IT_RXNE, DISABLE);

  USART_InitStructure.USART_BaudRate = uart_device->baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(uart_device->port, &USART_InitStructure);
  USART_Cmd(uart_device->port, ENABLE);

  // Configure DMA for reception
  if (DMA_RxChannel) {
    DMA_DeInit(DMA_RxChannel);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&uart_device->rx_buffer[2];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = rx_size - 2;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_PeripheralBaseAddr = buf_addr;

    DMA_Init(DMA_RxChannel, &DMA_InitStructure);
    USART_DMACmd(uart_device->port, USART_DMAReq_Rx, ENABLE);
  }

  // Configure DMA for transmission
  if (DMA_TxChannel) {
      DMA_DeInit(DMA_TxChannel);
      DMA_TxInitStructure.DMA_PeripheralBaseAddr = (uint32_t)&uart_device->port->DR;
      DMA_TxInitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_device->tx_buffer;
      DMA_TxInitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
      DMA_TxInitStructure.DMA_BufferSize = uart_device->tx_max_size;
      DMA_TxInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      DMA_TxInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_TxInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
      DMA_TxInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
      DMA_TxInitStructure.DMA_Mode = DMA_Mode_Normal;
      DMA_TxInitStructure.DMA_Priority = DMA_Priority_High;
      DMA_TxInitStructure.DMA_M2M = DMA_M2M_Disable;
      
      DMA_Init(DMA_TxChannel, &DMA_TxInitStructure);
// 
//      DMA_ClearFlag(DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4); 
//      DMA_ClearFlag(DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7);  
//      DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_HT2 | DMA1_FLAG_TE2);  

      USART_DMACmd(uart_device->port, USART_DMAReq_Tx, ENABLE);

      DMA_Cmd(DMA_TxChannel, ENABLE);  // 重新使能DMA

  }
}

void UART_SendData(uart_device_t *uart_device, uint8_t *data, uint16_t size) 
{
  for (uint16_t i = 0; i < size; i++) {
    while (USART_GetFlagStatus(uart_device->port, USART_FLAG_TXE) == RESET);
    USART_SendData(uart_device->port, data[i]);
  }
  while (USART_GetFlagStatus(uart_device->port, USART_FLAG_TC) == RESET);
}


/**
  * @brief  检查DMA发送是否完成
  * @param  uart_device: 串口设备结构体指针
  * @retval 发送状态：1-完成，0-未完成
  */
uint8_t UART_DMA_TransmitComplete(uart_device_t *uart_device)
{
    if(uart_device == NULL) return 1;
    
    switch((uint32_t)uart_device->port) {
        case (uint32_t)USART1:
            return (DMA_GetCurrDataCounter(DMA1_Channel4) == 0);
        case (uint32_t)USART2:
            return (DMA_GetCurrDataCounter(DMA1_Channel7) == 0);
        case (uint32_t)USART3:
            return (DMA_GetCurrDataCounter(DMA1_Channel2) == 0);
        default:
            return 1;
    }
}

int8_t UART_DMA_Transmit(uart_device_t *uart_device, uint8_t *pData, uint16_t Size)
{
    /*--------------------- 参数检查 ---------------------*/
    if(uart_device == NULL || pData == NULL || Size == 0) {
        return -1;
    }

    /*---------------- 获取DMA通道状态 -----------------*/
    DMA_Channel_TypeDef* dma_ch = NULL;
    uint32_t dma_tc_flag = 0;
    
    switch((uint32_t)uart_device->port) {
        case (uint32_t)USART1:
            dma_ch = DMA1_Channel4;
            dma_tc_flag = DMA1_FLAG_TC4;
            break;
        case (uint32_t)USART2:
            dma_ch = DMA1_Channel7;
            dma_tc_flag = DMA1_FLAG_TC7;
            break;
        case (uint32_t)USART3:
            dma_ch = DMA1_Channel2;
            dma_tc_flag = DMA1_FLAG_TC2;
            break;
        default:
            return -1;
    }

    /* 等待上次传输完成并清除标志 */
    if(DMA_GetFlagStatus(dma_tc_flag) == RESET) {
        uint32_t timeout = 100000; // 适当超时值
        while(DMA_GetFlagStatus(dma_tc_flag) == RESET) {
            if(--timeout == 0) return -2; // 超时返回
        }
    }
    DMA_ClearFlag(dma_tc_flag);

    /*---------------- 数据长度检查 -----------------*/
    if(Size > uart_device->tx_max_size) {
        return -3;
    }

    /*---------------- 数据拷贝到缓冲区 -----------------*/
    memcpy(uart_device->tx_buffer, pData, Size);

    /*---------------- 配置并启动DMA传输 -----------------*/
    DMA_Cmd(dma_ch, DISABLE); // 先禁用DMA
    
    dma_ch->CNDTR = Size;     // 设置传输数据量
    dma_ch->CMAR = (uint32_t)uart_device->tx_buffer; // 更新内存地址
    
    DMA_Cmd(dma_ch, ENABLE);  // 重新使能DMA

    /* 使能串口的DMA发送请求 */
    USART_DMACmd(uart_device->port, USART_DMAReq_Tx, ENABLE);

    return 0;
}

void USART_IRQHandler(USART_TypeDef *USARTx, DMA_Channel_TypeDef *DMA_Channel, uart_device_t *uart_device, uint8_t id) {
  if (USART_GetITStatus(USARTx, USART_IT_IDLE) != RESET) {
    volatile uint32_t tmp;
    tmp = USARTx->SR;
    tmp = USARTx->DR;
    DMA_Cmd(DMA_Channel, DISABLE);
    uart_device->rx_size = uart_device->rx_max_size - DMA_GetCurrDataCounter(DMA_Channel);
    uart_device->rx_flag = 1;
    uart_device->rx_buffer[0] = id;
    uart_device->rx_buffer[1] = uart_device->rx_size;
    DMA_SetCurrDataCounter(DMA_Channel, uart_device->rx_max_size);
    DMA_Cmd(DMA_Channel, ENABLE);
  }
}

void USART1_IRQHandler(void) {
  USART_IRQHandler(USART1, DMA1_Channel5, &uart_devices[0], 1);
}

void USART2_IRQHandler(void) {
  USART_IRQHandler(USART2, DMA1_Channel6, &uart_devices[1], 2);
}

void USART3_IRQHandler(void) {
  USART_IRQHandler(USART3, DMA1_Channel3, &uart_devices[2], 3);
}

void UART4_IRQHandler(void) {
  USART_IRQHandler(UART4, DMA2_Channel3, &uart_devices[3], 4);
}

void UART5_IRQHandler(void) {
  if (USART_GetITStatus(UART5, USART_IT_IDLE) != RESET) {
    volatile uint32_t tmp;
    tmp = UART5->SR;
    tmp = UART5->DR;
    uart_devices[4].rx_buffer[0] = 5;
    uart_devices[4].rx_buffer[1] = uart_devices[4].rx_size;
    uart_devices[4].rx_flag = 1;
  }
  if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) {
    uint8_t received_byte = USART_ReceiveData(UART5);
    uart_devices[4].rx_buffer[uart_devices[4].rx_size++] = received_byte;
  }
}

void UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t *size) {
    if (!uart_device || !buffer || !size) {
        return;
    }

    // 获取当前数据长度
    uint16_t data_size = uart_device->rx_size;
    *size = data_size;

    // 如果有数据才进行拷贝
    if (data_size > 0) {
        rt_memcpy(buffer, uart_device->rx_buffer, data_size);
        
        uart_device->rx_size = 0;
        uart_device->rx_flag = 0;
        

//        rt_memset(uart_device->rx_buffer, 0, data_size);
    }
}

_Bool ai_strstr(const char *haystack, const char *needle)
{
    const char *h = haystack;
    while (*h) {
        const char *n = needle;
        const char *h_tmp = h;
        while (*n && *h_tmp && *n == *h_tmp) {
            n++;
            h_tmp++;
        }
        if (!*n) return true;
        h++;
    }
    return false;
}

int fputc(int ch, FILE *f) {
  (void)f; // Suppress unused parameter warning
  char c = (char)ch;
//  SEGGER_RTT_Write(0, &c, 1);
//    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
//      ;
//    USART_SendData(USART3, c);  
		return ch;
}

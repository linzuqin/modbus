#include "uart.h"

#define u1_ring_buf_size 1024
uint8_t u1_ring_buf[u1_ring_buf_size];

#define u3_ring_buf_size 1024
uint8_t u3_ring_buf[u3_ring_buf_size];
ring_buf_t u1_ring_buf_t = {
    .buffer = u1_ring_buf,
    .size = u1_ring_buf_size,
    .head = 0,
    .tail = 0,
    .overwrite = 0
};

ring_buf_t u3_ring_buf_t = {
    .buffer = u3_ring_buf,
    .size = u3_ring_buf_size,
    .head = 0,
    .tail = 0,
    .overwrite = 0
};

__attribute__((aligned(8))) uint8_t uart1_rx_buf[uart1_rx_size] = {[0] = 1};
__attribute__((aligned(8))) uint8_t uart2_rx_buf[uart2_rx_size] = {[0] = 2};
__attribute__((aligned(8))) uint8_t uart3_rx_buf[uart3_rx_size] = {[0] = 3};
// uint8_t uart4_rx_buf[uart_rx_size];
// uint8_t uart5_rx_buf[uart_rx_size];

__attribute__((aligned(8))) uint8_t uart1_tx_buf[uart1_tx_size];
__attribute__((aligned(8))) uint8_t uart2_tx_buf[uart2_tx_size];
__attribute__((aligned(8))) uint8_t uart3_tx_buf[uart3_tx_size];


uart_device_t uart_devices[5] = {
  {USART1, 115200, 0, 0, uart1_rx_size , uart1_tx_size , uart1_tx_buf , uart1_rx_buf , DMA1_Channel5 , DMA1_Channel4  , &u1_ring_buf_t},
  {USART2, 115200, 0, 0, uart2_rx_size , uart2_tx_size , uart2_tx_buf , uart2_rx_buf , DMA1_Channel6 , DMA1_Channel7},
  {USART3, 115200, 0, 0, uart3_rx_size , uart3_tx_size , uart3_tx_buf , uart3_rx_buf , DMA1_Channel3 , DMA1_Channel2 , &u3_ring_buf_t},
  // {UART4,  115200, 0, 0, uart4_rx_buf , &u4_mutex},
  // {UART5,  115200, 0, 0, uart5_rx_buf , &u5_mutex},
};

void My_UART_Init(uart_device_t *uart_device) {
  GPIO_InitTypeDef GPIO_InitStructure;

  uint32_t buf_addr = 0;
  uint16_t rx_size = uart_device->rx_max_size;
	NVIC_InitTypeDef NVIC_InitStructure;

  switch ((uint32_t)uart_device->port) {
    case (uint32_t)USART1:
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOA, GPIO_Pin_10, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      buf_addr = (uint32_t)&USART1->DR;
      break;
    case (uint32_t)USART2:
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;

      buf_addr = (uint32_t)&USART2->DR;
      break;
    case (uint32_t)USART3:
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      MyGPIO_Init(GPIOB, GPIO_Pin_10, GPIO_Mode_AF_PP);
      MyGPIO_Init(GPIOB, GPIO_Pin_11, GPIO_Mode_IN_FLOATING);
      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;

      buf_addr = (uint32_t)&USART3->DR;
      break;
    default:
      LOG_E("error USARTx");
      return;
  }

  USART_InitTypeDef USART_InitStructure;
	USART_DeInit(uart_device->port);
  USART_InitStructure.USART_BaudRate = uart_device->baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(uart_device->port, &USART_InitStructure);

  USART_ITConfig(uart_device->port, USART_IT_IDLE, ENABLE);
  USART_ITConfig(uart_device->port, USART_IT_TC, DISABLE);
  USART_ITConfig(uart_device->port, USART_IT_RXNE, DISABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  // Configure DMA for reception
  if (uart_device->rx_dma_channel) {
		DMA_InitTypeDef DMA_RxInitStructure;

    DMA_DeInit(uart_device->rx_dma_channel);
    DMA_RxInitStructure.DMA_MemoryBaseAddr = (uint32_t)&uart_device->rx_buffer[0];
    DMA_RxInitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_RxInitStructure.DMA_BufferSize = rx_size;
    DMA_RxInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_RxInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_RxInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_RxInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_RxInitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_RxInitStructure.DMA_Priority = DMA_Priority_High;
    DMA_RxInitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_RxInitStructure.DMA_PeripheralBaseAddr = buf_addr;

    DMA_Init(uart_device->rx_dma_channel, &DMA_RxInitStructure);
    USART_DMACmd(uart_device->port, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(uart_device->rx_dma_channel, ENABLE);  // 重新使能DMA

  }

  // Configure DMA for transmission
  if (uart_device->tx_dma_channel) {
			DMA_InitTypeDef DMA_TxInitStructure;

      DMA_DeInit(uart_device->tx_dma_channel);
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
      
      DMA_Init(uart_device->tx_dma_channel, &DMA_TxInitStructure);
      USART_DMACmd(uart_device->port, USART_DMAReq_Tx, ENABLE);
      DMA_Cmd(uart_device->tx_dma_channel, ENABLE);  // 重新使能DMA
  }
	
	USART_Cmd(uart_device->port, ENABLE);

}

void UART_SendData(uart_device_t *uart_device, uint8_t *data, uint16_t size) 
{
  for (uint16_t i = 0; i < size; i++) {
    while (USART_GetFlagStatus(uart_device->port, USART_FLAG_TXE) == RESET);
    USART_SendData(uart_device->port, data[i]);
  }
  while (USART_GetFlagStatus(uart_device->port, USART_FLAG_TC) == RESET);
}

void ring_reset(ring_buf_t* rb)
{
	rb->head = 0;
	rb->tail = 0;
	memset(rb->buffer , 0 , u1_ring_buf_size);
}

uint16_t ringbuf_put(ring_buf_t* rb, const uint8_t* data, uint16_t len) {
    // 计算可用空间
    uint16_t free;
    if (rb->head >= rb->tail) {
        free = rb->size - rb->head + rb->tail;
    } else {
        free = rb->tail - rb->head;
    }
    
    // 检查空间是否足够
    if (len > free && !rb->overwrite) {
        len = free; // 不允许覆盖时限制写入长度
    }
    if (len == 0) return 0;
    
    // 计算到缓冲区末尾的连续空间
    uint16_t contig_free = rb->size - rb->head;
    
    if (contig_free >= len) {
        // 情况1: 有足够的连续空间
        memcpy(&rb->buffer[rb->head], data, len);
        rb->head += len;
        
        // 处理覆盖情况
        if (rb->overwrite && len > free) {
            rb->tail = (rb->head) % rb->size;
        }
    } else {
        // 情况2: 需要分两段拷贝
        memcpy(&rb->buffer[rb->head], data, contig_free);
        memcpy(&rb->buffer[0], &data[contig_free], len - contig_free);
        rb->head = len - contig_free;
        
        // 处理覆盖情况
        if (rb->overwrite && len > free) {
            rb->tail = (rb->head) % rb->size;
        }
    }
    
    return len;
}


uint16_t ringbuf_get(ring_buf_t* rb, uint8_t* dest, uint16_t len) {
    // 计算已用空间
    uint16_t used;
    if (rb->tail <= rb->head) {
        used = rb->head - rb->tail;
    } else {
        used = rb->size - rb->tail + rb->head;
    }
    
    // 限制读取长度
    if (len > used) {
        len = used;
    }
    if (len == 0) {
        return 0;
    }
    
    // 计算到缓冲区末尾的连续数据
    uint16_t contig_used = rb->size - rb->tail;
    
    if (contig_used >= len) {
        // 情况1: 有足够的连续数据
        memcpy(dest, &rb->buffer[rb->tail], len);
        rb->tail += len;
    } else {
        // 情况2: 需要分两段拷贝
        memcpy(dest, &rb->buffer[rb->tail], contig_used);
        memcpy(&dest[contig_used], &rb->buffer[0], len - contig_used);
        rb->tail = len - contig_used;
    }
    
    return len;
}


uint16_t ringbuf_find(ring_buf_t* rb, const char* target) {
    uint16_t target_len = strlen(target);
    uint16_t used = RINGBUF_USED(rb);
    
    for(uint16_t i = 0; i <= used - target_len; i++) {
        uint16_t idx = (rb->tail + i) % rb->size;
        uint8_t match = 1;
        
        for(uint16_t j = 0; j < target_len; j++) {
            if(rb->buffer[(idx + j) % rb->size] != target[j]) {
                match = 0;
                break;
            }
        }
        if(match) return i; // 返回找到的位置（相对于tail）
    }
    return 0xFFFF; // 未找到
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

void uartbuf_clear(uart_device_t *uart_device)
{
	DMA_Cmd(uart_device->rx_dma_channel, DISABLE);
	DMA_SetCurrDataCounter(uart_device->rx_dma_channel, uart_device->rx_max_size);
	DMA_Cmd(uart_device->rx_dma_channel, ENABLE);	
}

void USART_IRQHandler(uart_device_t *uart_device, uint8_t id) {
    if (USART_GetITStatus(uart_device->port, USART_IT_IDLE) != RESET) {
        USART_ITConfig(uart_device->port, USART_IT_IDLE, DISABLE);
        
        volatile uint32_t tmp;
        tmp = uart_device->port->SR;
        tmp = uart_device->port->DR;
        USART_ClearITPendingBit(uart_device->port, USART_IT_IDLE);

        uint32_t timeout = 100000;
        while (!DMA_GetFlagStatus(DMA1_FLAG_TC5) && timeout--);

        uart_device->rx_size = uart_device->rx_max_size - 
                              DMA_GetCurrDataCounter(uart_device->rx_dma_channel);

        uint32_t primask = __get_PRIMASK();
        __disable_irq();
        ringbuf_put(uart_device->ring_buf, uart_device->rx_buffer, uart_device->rx_size);
        __set_PRIMASK(primask);

        uart_device->rx_flag = 1;  
        DMA_Cmd(uart_device->rx_dma_channel, DISABLE);
        DMA_SetCurrDataCounter(uart_device->rx_dma_channel, uart_device->rx_max_size);
        DMA_Cmd(uart_device->rx_dma_channel, ENABLE);

        USART_ITConfig(uart_device->port, USART_IT_IDLE, ENABLE);
    }
}


void USART1_IRQHandler(void) {
  USART_IRQHandler(&uart_devices[0], 1);
}

void USART2_IRQHandler(void) {
  USART_IRQHandler(&uart_devices[1], 2);
}

void USART3_IRQHandler(void) {
  USART_IRQHandler(&uart_devices[2], 3);
}

void UART4_IRQHandler(void) {
  USART_IRQHandler(&uart_devices[3], 4);
}

void UART5_IRQHandler(void) {
  if (USART_GetITStatus(UART5, USART_IT_IDLE) != RESET) {
    volatile uint32_t tmp;
    tmp = UART5->SR;
    tmp = UART5->DR;
    uart_devices[4].rx_flag = 1;
  }
  if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) {
    uint8_t received_byte = USART_ReceiveData(UART5);
    uart_devices[4].rx_buffer[uart_devices[4].rx_size++] = received_byte;
  }
}

uint8_t UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t size) 
{
    if (!uart_device || !buffer ) {
        return 1;
    }

    // 如果有数据才进行拷贝
		if(uart_device->rx_buffer[0] != '\0')
		{
      rt_memcpy(buffer, uart_device->rx_buffer, size);
			return 0;
    }
		return 1;			//无数据
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

/**
 * @brief 在二进制安全缓冲区中搜索子串
 * @param haystack 主缓冲区
 * @param h_len 主缓冲区长度
 * @param needle 子串
 * @param n_len 子串长度
 * @return 首次出现的位置指针，未找到返回NULL
 */
const void* binary_strstr(const void* haystack, size_t h_len, 
                         const void* needle, size_t n_len) {
    if (!haystack || !needle || n_len == 0 || h_len < n_len) 
        return NULL;

    const unsigned char* h = haystack;
    const unsigned char* n = needle;
    
    for (size_t i = 0; i <= h_len - n_len; i++) {
        if (memcmp(&h[i], n, n_len) == 0) {
            return &h[i];
        }
    }
    return NULL;
}

int fputc(int ch, FILE *f) {
  (void)f; // Suppress unused parameter warning
  char c = (char)ch;
//  SEGGER_RTT_Write(0, &c, 1);
//    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//    USART_SendData(USART3, c);  
		return ch;
}


int my_uart_init(void)
{
	My_UART_Init(&uart_devices[0]);
	My_UART_Init(&uart_devices[1]);
	My_UART_Init(&uart_devices[2]);
//	My_UART_Init(&uart_devices[3]);
//	My_UART_Init(&uart_devices[4]);

	return 1;
}
INIT_APP_EXPORT(my_uart_init);

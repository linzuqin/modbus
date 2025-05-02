#include "main.h"

static uint8_t uart1_rx_buf[uart1_rx_size];
static uint8_t uart2_rx_buf[uart2_rx_size];
static uint8_t uart3_rx_buf[uart3_rx_size];
static uint8_t uart4_rx_buf[uart4_rx_size];
static uint8_t uart5_rx_buf[uart5_rx_size];

uart_device_t uart1_device = {
  .port = USART1,
  .baudrate = 115200,
  .rx_flag = 0,
  .rx_size = 0,
  .rx_buffer = uart1_rx_buf,
};
uart_device_t uart2_device = {
  .port = USART2,
  .baudrate = 115200,
  .rx_flag = 0,
  .rx_size = 0,
  .rx_buffer = uart2_rx_buf,
};
uart_device_t uart3_device = {
  .port = USART3,
  .baudrate = 115200,
  .rx_flag = 0,
  .rx_size = 0,
  .rx_buffer = uart3_rx_buf,
};
uart_device_t uart4_device = {
  .port = UART4,
  .baudrate = 115200,
  .rx_flag = 0,
  .rx_size = 0,
  .rx_buffer = uart4_rx_buf,
};
uart_device_t uart5_device = {
  .port = UART5,
  .baudrate = 115200,
  .rx_flag = 0,
  .rx_size = 0,
  .rx_buffer = uart5_rx_buf,
};

void My_UART_Init(uart_device_t *uart_device)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  DMA_Channel_TypeDef *DMA_Channel;
  uint32_t buf_addr = 0;
  uint16_t rx_size = 0;

  if (uart_device->port == USART1)
  {
    rx_size = uart1_rx_size;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    MyGPIO_Init(GPIOA, GPIO_Pin_9, GPIO_Mode_AF_PP);
    MyGPIO_Init(GPIOA, GPIO_Pin_10, GPIO_Mode_IN_FLOATING);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    DMA_Channel = DMA1_Channel5;
    buf_addr = (uint32_t)&USART1->DR;
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  }
  else if (uart_device->port == USART2)
  {
    rx_size = uart2_rx_size;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    MyGPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_AF_PP);
    MyGPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_IN_FLOATING);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    DMA_Channel = DMA1_Channel6;
    buf_addr = (uint32_t)&USART2->DR;
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
  }
  else if (uart_device->port == USART3)
  {
    rx_size = uart3_rx_size;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    MyGPIO_Init(GPIOB, GPIO_Pin_10, GPIO_Mode_AF_PP);
    MyGPIO_Init(GPIOB, GPIO_Pin_11, GPIO_Mode_IN_FLOATING);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    DMA_Channel = DMA1_Channel3;
    buf_addr = (uint32_t)&USART3->DR;
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  }
  else if (uart_device->port == UART4)
  {
    rx_size = uart4_rx_size;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    MyGPIO_Init(GPIOC, GPIO_Pin_10, GPIO_Mode_AF_PP);
    MyGPIO_Init(GPIOC, GPIO_Pin_11, GPIO_Mode_IN_FLOATING);
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    DMA_Channel = DMA2_Channel3;
    buf_addr = (uint32_t)&UART4->DR;
    USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
  }
  else if (uart_device->port == UART5)
  {
    rx_size = uart5_rx_size;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    MyGPIO_Init(GPIOC, GPIO_Pin_12, GPIO_Mode_AF_PP);
    MyGPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_IN_FLOATING);
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  }
  else
  {
    LOG_E("error USARTx");
    return;
  }

  USART_InitStructure.USART_BaudRate = uart_device->baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(uart_device->port, &USART_InitStructure);
  USART_Cmd(uart_device->port, ENABLE);

  DMA_DeInit(DMA_Channel);
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

  DMA_Init(DMA_Channel, &DMA_InitStructure);
}

void UART_SendData(USART_TypeDef *USARTx, uint8_t *data, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++)
  {
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
      ;
    USART_SendData(USARTx, data[i]);
  }
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
    ;
}

void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
  {
    volatile uint32_t tmp;
    tmp = USART1->SR;
    tmp = USART1->DR;
    DMA_Cmd(DMA1_Channel5, DISABLE);
    uart1_device.rx_size = uart1_rx_size - DMA_GetCurrDataCounter(DMA1_Channel5);
    uart1_device.rx_flag = 1;
    uart1_device.rx_buffer[0] = 1;
    uart1_device.rx_buffer[1] = uart1_device.rx_size;
    DMA_SetCurrDataCounter(DMA1_Channel5, uart1_rx_size);
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }
}

void USART2_IRQHandler(void)
{
  if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
  {
    volatile uint32_t tmp;
    tmp = USART2->SR;
    tmp = USART2->DR;
    DMA_Cmd(DMA1_Channel6, DISABLE);
    uart2_device.rx_size = uart2_rx_size - DMA_GetCurrDataCounter(DMA1_Channel6);
    uart2_device.rx_flag = 1;
    uart2_device.rx_buffer[0] = 2;
    uart2_device.rx_buffer[1] = uart2_device.rx_size;
    DMA_SetCurrDataCounter(DMA1_Channel6, uart2_rx_size);
    DMA_Cmd(DMA1_Channel6, ENABLE);
  }
}

void USART3_IRQHandler(void)
{
  if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
  {
    volatile uint32_t tmp;
    tmp = USART3->SR;
    tmp = USART3->DR;
    DMA_Cmd(DMA1_Channel3, DISABLE);
    uart3_device.rx_size = uart3_rx_size - DMA_GetCurrDataCounter(DMA1_Channel3);
    uart3_device.rx_flag = 1;
    uart3_device.rx_buffer[0] = 3;
    uart3_device.rx_buffer[1] = uart3_device.rx_size;
    DMA_SetCurrDataCounter(DMA1_Channel3, uart3_rx_size);
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
}

void UART4_IRQHandler(void)
{
  if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
  {
    volatile uint32_t tmp;
    tmp = UART4->SR;
    tmp = UART4->DR;
    DMA_Cmd(DMA2_Channel3, DISABLE);
    uart4_device.rx_size = uart4_rx_size - DMA_GetCurrDataCounter(DMA2_Channel3);
    uart4_device.rx_flag = 1;
    uart4_device.rx_buffer[0] = 4;
    uart4_device.rx_buffer[1] = uart4_device.rx_size;
    DMA_SetCurrDataCounter(DMA2_Channel3, uart4_rx_size);
    DMA_Cmd(DMA2_Channel3, ENABLE);
  }
}

void UART5_IRQHandler(void)
{
  if (USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
  {
    volatile uint32_t tmp;
    tmp = UART5->SR;
    tmp = UART5->DR;
    uart5_device.rx_buffer[0] = 5;
    uart5_device.rx_buffer[1] = uart5_device.rx_size;
    uart5_device.rx_flag = 1;
  }
  if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  {
    uint8_t received_byte = USART_ReceiveData(UART5);
    uart5_device.rx_buffer[uart5_device.rx_size++] = received_byte;
  }
}

void UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t *size)
{
  *size = uart_device->rx_size;
  for (uint16_t i = 0; i < *size; i++)
  {
    buffer[i] = uart_device->rx_buffer[i];
  }
  uart_device->rx_size = 0;
}

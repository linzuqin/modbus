#include "stm32f10x.h" // Replace with the appropriate header file for your STM32 series
#include "uart.h"
#include "rtthread.h"
uart_device_t uart1_device;
uart_device_t uart2_device;
uart_device_t uart3_device;
uart_device_t uart4_device;
uart_device_t uart5_device;

void UART_Init(USART_TypeDef *USARTx , uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

	DMA_Channel_TypeDef *DMA_Channel;
    uint32_t buf_addr = 0;
    uart_device_t *uart_device;
    // Enable clocks for GPIO and USART
    if (USARTx == USART1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // Enable DMA1 clock
        
        // Configure PA9 (TX) as alternate function push-pull
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure PA10 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Enable USART1 interrupt
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Enable USART1 idle interrupt
        USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

        // Configure DMA for USART1 RX
        DMA_Channel = DMA1_Channel5; // USART1_RX -> DMA1_Channel5
        buf_addr = (uint32_t)&USART1->DR;
        USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
        uart_device = &uart1_device;
    }
    else if (USARTx == USART2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // Enable DMA1 clock

        // Configure PA2 (TX) as alternate function push-pull
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure PA3 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Enable USART2 interrupt
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Enable USART2 idle interrupt
        USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

        // Configure DMA for USART2 RX
        DMA_Channel = DMA1_Channel6; // USART1_RX -> DMA1_Channel5
        buf_addr = (uint32_t)&USART2->DR;
        USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

        uart_device = &uart2_device;

    }
    else if (USARTx == UART4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // Enable DMA1 clock

        // Configure PB10 (TX) as alternate function push-pull
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // Configure PB11 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // Enable USART3 interrupt
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Enable USART3 idle interrupt
        USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

        // Configure DMA for USART3 RX
        DMA_Channel = DMA1_Channel3; // USART1_RX -> DMA1_Channel5
        buf_addr = (uint32_t)&USART3->DR;
        USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

        uart_device = &uart3_device;

    }else if (USARTx == UART4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); // Enable DMA2 clock

        // Configure PC10 (TX) as alternate function push-pull
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        // Configure PC11 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        // Enable UART4 interrupt
        NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Enable UART4 idle interrupt
        USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);

        // Configure DMA for UART4 RX
        DMA_Channel = DMA2_Channel3; // UART4_RX -> DMA2_Channel3
        buf_addr = (uint32_t)&UART4->DR;
        USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);

        uart_device = &uart4_device;
    }else if (USARTx == UART5) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); // Enable DMA2 clock

        // Configure PC12 (TX) as alternate function push-pull
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        // Configure PD2 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        // Enable UART5 interrupt
        NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Enable UART5 idle interrupt
        USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
        USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

        uart_device = &uart5_device;
    } else {
        // Invalid USART, return
        return;
    }

    // Configure USART parameters
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USARTx, &USART_InitStructure);

    // Enable USART
    USART_Cmd(USARTx, ENABLE);

    DMA_DeInit(DMA_Channel); // DMA_Channel
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&uart_device->rx_buffer[2]; // Set dynamically during reception
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = uart_rx_size; // Set dynamically during reception
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

void UART_SendData(USART_TypeDef *USARTx, uint8_t *data, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        // Wait until the transmit data register is empty
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        // Send the data
        USART_SendData(USARTx, data[i]);
    }
    // Wait until the transmission is complete
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}


void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = USART1->SR; // Clear the IDLE flag by reading the status register
        tmp = USART1->DR; // and the data register
        DMA_Cmd(DMA1_Channel5, DISABLE); // Disable DMA for RX
        uart1_device.rx_size = uart_rx_size - DMA_GetCurrDataCounter(DMA1_Channel5); // Calculate received bytes
			
        uart1_device.rx_buffer[0] = 1;
        uart1_device.rx_buffer[1] = uart1_device.rx_size;
        DMA_SetCurrDataCounter(DMA1_Channel5, uart_rx_size); // Reset DMA counter
        DMA_Cmd(DMA1_Channel5, ENABLE); // Re-enable DMA for RX        
    }
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = USART2->SR; // Clear the IDLE flag by reading the status register
        tmp = USART2->DR; // and the data register
        DMA_Cmd(DMA1_Channel6, DISABLE); // Disable DMA for RX
        uart2_device.rx_size = uart_rx_size - DMA_GetCurrDataCounter(DMA1_Channel6); // Calculate received bytes

        uart2_device.rx_buffer[0] = 2;
        uart2_device.rx_buffer[1] = uart2_device.rx_size;

        DMA_SetCurrDataCounter(DMA1_Channel6, uart_rx_size); // Reset DMA counter
        DMA_Cmd(DMA1_Channel6, ENABLE); // Re-enable DMA for RX
    }
}

void USART3_IRQHandler(void) {
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = USART3->SR; // Clear the IDLE flag by reading the status register
        tmp = USART3->DR; // and the data register
        DMA_Cmd(DMA1_Channel3, DISABLE); // Disable DMA for RX
        uart3_device.rx_size = uart_rx_size - DMA_GetCurrDataCounter(DMA1_Channel3); // Calculate received bytes

        uart3_device.rx_buffer[0] = 3;
        uart3_device.rx_buffer[1] = uart3_device.rx_size;
        
        DMA_SetCurrDataCounter(DMA1_Channel3, uart_rx_size); // Reset DMA counter
        DMA_Cmd(DMA1_Channel3, ENABLE); // Re-enable DMA for RX
    }
}

void UART4_IRQHandler(void) {
    if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = UART4->SR; // Clear the IDLE flag by reading the status register
        tmp = UART4->DR; // and the data register
        DMA_Cmd(DMA2_Channel3, DISABLE); // Disable DMA for RX
        uart4_device.rx_size = uart_rx_size - DMA_GetCurrDataCounter(DMA2_Channel3); // Calculate received bytes

        uart4_device.rx_buffer[0] = 4;
        uart4_device.rx_buffer[1] = uart4_device.rx_size;

        DMA_SetCurrDataCounter(DMA2_Channel3, uart_rx_size); // Reset DMA counter
        DMA_Cmd(DMA2_Channel3, ENABLE); // Re-enable DMA for RX
    }
}

void UART5_IRQHandler(void) {
    if (USART_GetITStatus(UART5, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = UART5->SR; // Clear the IDLE flag by reading the status register
        tmp = UART5->DR; // and the data register
        uart5_device.rx_buffer[0] = 5;
        uart5_device.rx_buffer[1] = uart5_device.rx_size;
        }

        // Handle RXNE interrupt (Receive Data Register Not Empty)
        if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) {
        uint8_t received_byte = USART_ReceiveData(UART5); // Read received byte
        // Process the received byte as needed
        // Example: Store it in the RX buffer
        uart5_device.rx_buffer[uart5_device.rx_size++] = received_byte;
        }
    }

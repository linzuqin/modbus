#include "spi.h"
static uint8_t SPI1_rx_buf[SPI1_rx_SIZE];
static uint8_t SPI2_rx_buf[SPI2_rx_SIZE];

void My_SPI_Init(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;
    DMA_Channel_TypeDef *DMA_Channel;
    uint32_t per_addr = 0;
    uint32_t buf_addr = 0;
    uint32_t rx_size = 0;
    if (SPIx == SPI1)
    {
        DMA_Channel = DMA1_Channel2;
        per_addr = (uint32_t)&SPI1->DR;
        buf_addr = (uint32_t)&SPI1_rx_buf[0];
        rx_size = SPI1_rx_SIZE;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        MyGPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_AF_PP);
        MyGPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_IN_FLOATING);
        MyGPIO_Init(GPIOA, GPIO_Pin_7, GPIO_Mode_AF_PP);

        SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
        SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
        SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
        SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStruct.SPI_CRCPolynomial = 7;
        SPI_Init(SPI1, &SPI_InitStruct);

        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);

        SPI_Cmd(SPI1, ENABLE);
    }
    else if (SPIx == SPI2)
    {
        DMA_Channel = DMA1_Channel4;
        per_addr = (uint32_t)&SPI2->DR;
        buf_addr = (uint32_t)&SPI2_rx_buf[0];
        rx_size = SPI2_rx_SIZE;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        MyGPIO_Init(GPIOB, GPIO_Pin_13, GPIO_Mode_AF_PP);
        MyGPIO_Init(GPIOB, GPIO_Pin_14, GPIO_Mode_IN_FLOATING);
        MyGPIO_Init(GPIOB, GPIO_Pin_15, GPIO_Mode_AF_PP);

        SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
        SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
        SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
        SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStruct.SPI_CRCPolynomial = 7;
        SPI_Init(SPI2, &SPI_InitStruct);

        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

        SPI_Cmd(SPI2, ENABLE);
    }
    DMA_DeInit(DMA_Channel);
    DMA_InitStruct.DMA_PeripheralBaseAddr = per_addr;
    DMA_InitStruct.DMA_MemoryBaseAddr = buf_addr;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = rx_size;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_Channel, &DMA_InitStruct);
}

static void SPI_Read(SPI_TypeDef *SPIx, uint8_t *rxBuffer)
{
    DMA_Channel_TypeDef *DMA_Channel;
    uint32_t size = 0;
    if (SPIx == SPI1)
    {
        DMA_Channel = DMA1_Channel2;
        size = SPI1_rx_SIZE;
    }
    else if (SPIx == SPI2)
    {
        DMA_Channel = DMA1_Channel4;
        size = SPI2_rx_SIZE;
    }
    else
    {
        return;
    }

    DMA_Cmd(DMA_Channel, DISABLE);

    DMA_SetCurrDataCounter(DMA_Channel, size);
    DMA_Channel->CMAR = (uint32_t)rxBuffer;

    DMA_Cmd(DMA_Channel, ENABLE);

    while (!DMA_GetFlagStatus(DMA1_FLAG_TC2) && !DMA_GetFlagStatus(DMA1_FLAG_TC4))
        ;

    if (SPIx == SPI1)
    {
        DMA_ClearFlag(DMA1_FLAG_TC2);
    }
    else if (SPIx == SPI2)
    {
        DMA_ClearFlag(DMA1_FLAG_TC4);
    }
}

void SPI_Write(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
            ;

        SPI_I2S_SendData(SPIx, txBuffer[i]);

        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
            ;

        SPI_Read(SPIx, rxBuffer);
    }
}

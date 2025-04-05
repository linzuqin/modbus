#include "stm32f10x.h"                  // Device header
#include "rtthread.h"
#define W25Q64Flash_CLK(x) GPIO_WriteBit(GPIOB,GPIO_Pin_13,(BitAction)(x))
#define W25Q64Flash_MOSI(x) GPIO_WriteBit(GPIOB,GPIO_Pin_15,(BitAction)(x))
#define W25Q64Flash_MISO GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define W25Q64Flash_CS(x) GPIO_WriteBit(GPIOB,GPIO_Pin_12,(BitAction)(x))

uint8_t W25Q64Flash_WriteReadByte(uint8_t ByteSend)
{
	uint8_t i=0;
	uint8_t ByteReceive=0;
	
	for(i=0;i<8;i++)
	{
		W25Q64Flash_MOSI(ByteSend&(0x80>>i));
		W25Q64Flash_CLK(1);
		if(W25Q64Flash_MISO)
		{
			ByteReceive|=(0x80>>i);
		}
		W25Q64Flash_CLK(0);
	}
	
	return ByteReceive;
}

void W25Q64Flash_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	W25Q64Flash_CS(1);
	W25Q64Flash_CLK(0);
}

void W25Q64Flash_Init(void)
{
	W25Q64Flash_GPIO_Init();
}

void W25Q64_WriteEnable(void)
{
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x06);
	W25Q64Flash_CS(1);
}

void W25Q64_WriteBusy(void)
{
	uint32_t Timeout=0;
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x05);
	while((W25Q64Flash_WriteReadByte(0xFF)&0x01)==0x01)
	{
		Timeout++;
		if(Timeout==500000)
		{
			break;
		}
		rt_thread_mdelay(1);
	}
	W25Q64Flash_CS(1);
}

void W25Q64_ReadID(uint8_t* MID,uint16_t* DID)
{
	W25Q64_WriteBusy();
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x9F);
	*MID=W25Q64Flash_WriteReadByte(0xFF);
	*DID=(uint16_t)W25Q64Flash_WriteReadByte(0xFF)<<8;
	*DID|=W25Q64Flash_WriteReadByte(0xFF);
	W25Q64Flash_CS(1);
}

void W25Q64Flash_EraseSector(uint32_t Addr)
{
	W25Q64_WriteBusy();
	W25Q64_WriteEnable();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x20);
	W25Q64Flash_WriteReadByte(Addr>>16);
	W25Q64Flash_WriteReadByte(Addr>>8);
	W25Q64Flash_WriteReadByte(Addr);
	W25Q64Flash_CS(1);
}

void W25Q64Flash_EraseBlock(uint32_t Addr)
{
	W25Q64_WriteBusy();
	W25Q64_WriteEnable();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0xD8);
	W25Q64Flash_WriteReadByte(Addr>>16);
	W25Q64Flash_WriteReadByte(Addr>>8);
	W25Q64Flash_WriteReadByte(Addr);
	W25Q64Flash_CS(1);
}

void W25Q64Flash_EraseAll(void)
{
	W25Q64_WriteBusy();
	W25Q64_WriteEnable();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0xC7);
	W25Q64Flash_CS(1);
}

void W25Q64Flash_WriteData(uint32_t Addr,uint8_t* Data,uint16_t Size)
{
	uint16_t i=0;
	
	W25Q64_WriteBusy();
	W25Q64_WriteEnable();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x02);
	W25Q64Flash_WriteReadByte(Addr>>16);
	W25Q64Flash_WriteReadByte(Addr>>8);
	W25Q64Flash_WriteReadByte(Addr);
	
	for(i=0;i<Size;i++)
	{
		W25Q64Flash_WriteReadByte(Data[i]);
	}
	W25Q64Flash_CS(1);
}

void W25Q64Flash_ReadData(uint32_t Addr,uint8_t* Data,uint32_t Size)
{
	uint32_t i=0;
	
	W25Q64_WriteBusy();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x03);
	W25Q64Flash_WriteReadByte(Addr>>16);
	W25Q64Flash_WriteReadByte(Addr>>8);
	W25Q64Flash_WriteReadByte(Addr);
	for(i=0;i<Size;i++)
	{
		Data[i]=W25Q64Flash_WriteReadByte(0xFF);
	}
	W25Q64Flash_CS(1);
}

void W25Q64Flash_PageProgram(uint16_t BlockAddr,uint16_t SectorAddr,uint16_t PageAddr,
	uint16_t ByteAddr,uint8_t* Data,uint16_t Size)
{
	uint16_t i=0;
	uint32_t Addr=0;
	
	Addr+=BlockAddr*0x10000;
	Addr+=SectorAddr*0x1000;
	Addr+=PageAddr*0x100;
	Addr+=ByteAddr;
	
	W25Q64_WriteBusy();
	W25Q64_WriteEnable();
	
	W25Q64Flash_CS(0);
	W25Q64Flash_WriteReadByte(0x02);
	W25Q64Flash_WriteReadByte(Addr>>16);
	W25Q64Flash_WriteReadByte(Addr>>8);
	W25Q64Flash_WriteReadByte(Addr);
	
	for(i=0;i<Size;i++)
	{
		W25Q64Flash_WriteReadByte(Data[i]);
	}
	W25Q64Flash_CS(1);
}

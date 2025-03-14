#ifndef __W25Q64FLASH_H__
#define __W25Q64FLASH_H__

void W25Q64Flash_SPI_SendByte(uint8_t Byte);
uint8_t W25Q64Flash_SPI_ReceiveByte(void);
uint8_t W25Q64Flash_WriteReadByte(uint8_t ByteSend);
void W25Q64Flash_Init(void);						//初始化
void W25Q64_ReadID(uint8_t* MID,uint16_t* DID);		//读取芯片ID
void W25Q64_WriteEnable(void);						//写使能
void W25Q64_WriteBusy(void);						//等待忙状态
void W25Q64Flash_EraseSector(uint32_t Addr);		//扇区擦除 Addr:扇区地址
void W25Q64Flash_EraseBlock(uint32_t Addr);			//块擦除 Addr:块地址
void W25Q64Flash_EraseAll(void);					//全片擦除
void W25Q64Flash_WriteData(uint32_t Addr,uint8_t* Data,uint16_t Size);	//写数据 Addr:起始地址 Data:写入数据的指针
//Size:写入数据字节数，由于一次写入数据不能跨页，该参数不能大于256
void W25Q64Flash_ReadData(uint32_t Addr,uint8_t* Data,uint32_t Size);	//读数据 Addr:起始地址 Data:读取数据的指针
//Size:读取字节数
void W25Q64Flash_PageProgram(uint16_t BlockAddr,uint16_t SectorAddr,uint16_t PageAddr,
	uint16_t ByteAddr,uint8_t* Data,uint16_t Size);

#endif

#ifndef _AT45DB_H_
#define _AT45DB_H_  

#include "stm32f4xx_conf.h"


#define FLASH_CHREAD 			0x0B
#define FLASH_CLREAD 			0x03
#define FLASH_PREAD				0xD2
#define FLASH_BUFWRITE1 		0x84
#define FLASH_IDREAD 			0x9F
#define FLASH_STATUS 			0xD7
#define PAGE_ERASE 				0x81
#define PAGE_READ 				0xD2
#define BUFFER_2_WRITE 			0x87
#define B2MM_PAGEERASE 			0x86	
#define Dummy 					0xA5

#define AT_CS_LOW()     	GPIO_ResetBits(GPIOD, GPIO_Pin_13)
#define AT_CS_HIGH()   		GPIO_SetBits(GPIOD, GPIO_Pin_13)

#define PAR_addr  	4095//系统参数保存页

extern u8 Sysparam[528];
extern u8 AT45_buffer[528];


void SPI_Flash_Init(void);	         	//SPI FLASH初始化
void FlashPageEarse(u16 page);			//擦除指定的页,页范围0-4095
void FlashPageRead(u16 page,u8 *Data);	//读取整页,页范围0-4095
void FlashPageWrite(u16 page,u8 *Data);	//写一整页,页范围0-4095
void FlashWaitBusy(void);			    
u16  FlashReadID(void);					//读取flashID两个字节
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);	//写入flash
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);	//读取flash
void Save_param(void);
void Load_param(void);


#endif



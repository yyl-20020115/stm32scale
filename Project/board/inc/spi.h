#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx_conf.h"

void SPI1_Init(void);
void FLASH_SPI_Init(void);
void ADS_SPI_Init(void);
void MMA_SPI_Init(void);
void CSPin_init(void);
u8 SPI1_RWByte(u8 byte);

#endif


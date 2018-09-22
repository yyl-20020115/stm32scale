#include "at45db.h"
#include "spi.h"

//SPI FLASH专用整页读写缓冲
u8 AT45_buffer[528];
u8 Sysparam[528];//一页的系统参数暂存
//初始化SPI FLASH
void SPI_Flash_Init(void)
{
	SPI1_Init();
	CSPin_init();
	FLASH_SPI_Init();
}
//读取FLASH的ID号
u16 FlashReadID(void)
{
	u16 temp;
	AT_CS_LOW();	
  	SPI1_RWByte(0x9F);
  	temp=SPI1_RWByte(Dummy);
	temp<<=8;
	temp|=SPI1_RWByte(Dummy);
  	AT_CS_HIGH();
	return temp;	
}
//擦除FLASH的某一页,页范围0-4095
void FlashPageEarse(u16 page)
{	
	FlashWaitBusy();	
	AT_CS_LOW();
	SPI1_RWByte(PAGE_ERASE);
	SPI1_RWByte((u8)(page>>6));
	SPI1_RWByte((u8)(page<<2));
	SPI1_RWByte(Dummy);
	AT_CS_HIGH();	
}
//读取FLASH的某一页,页范围0-4095
void FlashPageRead(u16 page,u8 *Data)
{
	u16 i;	
	FlashWaitBusy();
	AT_CS_LOW();
	SPI1_RWByte(PAGE_READ);
	SPI1_RWByte((u8)(page>>6));
  	SPI1_RWByte((u8)(page<<2));
  	SPI1_RWByte(0x00);//3个字节
  	SPI1_RWByte(0x00);
  	SPI1_RWByte(0x00);
  	SPI1_RWByte(0x00);
  	SPI1_RWByte(0x00);
  	for(i=0;i<528;i++)
	{
		Data[i]=SPI1_RWByte(Dummy);
	}
	AT_CS_HIGH();	
}
//写入FLASH的某一页,页范围0-4095
void FlashPageWrite(u16 page,u8 *Data)
{
	u16 i;
	FlashWaitBusy();
	AT_CS_LOW();
	SPI1_RWByte(BUFFER_2_WRITE);
	SPI1_RWByte(0x00);
	SPI1_RWByte(0x00);
	SPI1_RWByte(0x00);
	for(i=0;i<528;i++)
	{
		SPI1_RWByte(Data[i]);
	}
	AT_CS_HIGH();		
	if ( page < 4096)
	{
		AT_CS_LOW();
		SPI1_RWByte(B2MM_PAGEERASE);
		SPI1_RWByte((u8)(page>>6));
		SPI1_RWByte((u8)(page<<2));
		SPI1_RWByte(0x00);
		AT_CS_HIGH();
		FlashWaitBusy();
	}	
}
//FLASH忙等待
void FlashWaitBusy(void)
{
	u8 state_reg=0x00;
	AT_CS_LOW();	
	SPI1_RWByte(FLASH_STATUS);	 
	while((state_reg&0x80)==0)state_reg=SPI1_RWByte(Dummy);
	AT_CS_HIGH();
}
//FLASH写API函数,可任意地址,任意字节写
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/528;//页地址
	secoff=WriteAddr%528;//页内的偏移
	secremain=528-secoff;//页剩余空间大小   
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于528个字节
	while(1) 
	{	
		FlashPageRead(secpos,&AT45_buffer[0]);//读出整页内容
		FlashPageEarse(secpos);//擦除该页
		for(i=0;i<secremain;i++)	   //复制
		{
			AT45_buffer[i+secoff]=pBuffer[i];	  
		}
		FlashPageWrite(secpos,&AT45_buffer[0]);//写入整个扇区  
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 
		   	pBuffer+=secremain;  //指针偏移  
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>528)secremain=528;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	}	 	 
}
//FLASH读API函数,可任意地址,任意字节读
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)	  
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=ReadAddr/528;//页地址
	secoff=ReadAddr%528;//页内的偏移
	secremain=528-secoff;//页剩余空间大小
	if(NumByteToRead<=secremain)secremain=NumByteToRead;
	while(1)
	{
		FlashPageRead(secpos,&AT45_buffer[0]);
		for(i=0;i<secremain;i++)
		{
			pBuffer[i]=AT45_buffer[i+secoff];
		}
		if(NumByteToRead==secremain)break;//读结束了
		else//读未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 
		   	pBuffer+=secremain;  //指针偏移  
		   	NumByteToRead-=secremain;				//字节数递减
			if(NumByteToRead>528)secremain=528;	//下一个扇区还是读不完
			else secremain=NumByteToRead;		//下一个扇区可以读完了
		}
	}
}
//保存系统参数到SPI FLASH
void Save_param(void)
{
	FlashPageWrite(PAR_addr,&Sysparam[0]);	
}
//读取系统参数
void Load_param(void)
{
	FlashPageRead(PAR_addr,&Sysparam[0]);
}



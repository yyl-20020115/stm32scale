#include "at45db.h"
#include "spi.h"

//SPI FLASHר����ҳ��д����
u8 AT45_buffer[528];
u8 Sysparam[528];//һҳ��ϵͳ�����ݴ�
//��ʼ��SPI FLASH
void SPI_Flash_Init(void)
{
	SPI1_Init();
	CSPin_init();
	FLASH_SPI_Init();
}
//��ȡFLASH��ID��
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
//����FLASH��ĳһҳ,ҳ��Χ0-4095
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
//��ȡFLASH��ĳһҳ,ҳ��Χ0-4095
void FlashPageRead(u16 page,u8 *Data)
{
	u16 i;	
	FlashWaitBusy();
	AT_CS_LOW();
	SPI1_RWByte(PAGE_READ);
	SPI1_RWByte((u8)(page>>6));
  	SPI1_RWByte((u8)(page<<2));
  	SPI1_RWByte(0x00);//3���ֽ�
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
//д��FLASH��ĳһҳ,ҳ��Χ0-4095
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
//FLASHæ�ȴ�
void FlashWaitBusy(void)
{
	u8 state_reg=0x00;
	AT_CS_LOW();	
	SPI1_RWByte(FLASH_STATUS);	 
	while((state_reg&0x80)==0)state_reg=SPI1_RWByte(Dummy);
	AT_CS_HIGH();
}
//FLASHдAPI����,�������ַ,�����ֽ�д
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/528;//ҳ��ַ
	secoff=WriteAddr%528;//ҳ�ڵ�ƫ��
	secremain=528-secoff;//ҳʣ��ռ��С   
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������528���ֽ�
	while(1) 
	{	
		FlashPageRead(secpos,&AT45_buffer[0]);//������ҳ����
		FlashPageEarse(secpos);//������ҳ
		for(i=0;i<secremain;i++)	   //����
		{
			AT45_buffer[i+secoff]=pBuffer[i];	  
		}
		FlashPageWrite(secpos,&AT45_buffer[0]);//д����������  
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  //ָ��ƫ��  
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>528)secremain=528;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}	 	 
}
//FLASH��API����,�������ַ,�����ֽڶ�
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)	  
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=ReadAddr/528;//ҳ��ַ
	secoff=ReadAddr%528;//ҳ�ڵ�ƫ��
	secremain=528-secoff;//ҳʣ��ռ��С
	if(NumByteToRead<=secremain)secremain=NumByteToRead;
	while(1)
	{
		FlashPageRead(secpos,&AT45_buffer[0]);
		for(i=0;i<secremain;i++)
		{
			pBuffer[i]=AT45_buffer[i+secoff];
		}
		if(NumByteToRead==secremain)break;//��������
		else//��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  //ָ��ƫ��  
		   	NumByteToRead-=secremain;				//�ֽ����ݼ�
			if(NumByteToRead>528)secremain=528;	//��һ���������Ƕ�����
			else secremain=NumByteToRead;		//��һ���������Զ�����
		}
	}
}
//����ϵͳ������SPI FLASH
void Save_param(void)
{
	FlashPageWrite(PAR_addr,&Sysparam[0]);	
}
//��ȡϵͳ����
void Load_param(void)
{
	FlashPageRead(PAR_addr,&Sysparam[0]);
}



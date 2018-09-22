#include "MMA7455.h"
#include "delay.h"
#include "spi.h"

//��MMA7455
u8 MMA_Read(u8 cmd)
{
  	u8 data;
  	cmd&=0x3f;
  	MMA_CS_LOW();
  	SPI1_RWByte(cmd<<1);
	data=SPI1_RWByte(0xFF);	   
  	MMA_CS_HIGH();
  	return data;
}
//дMMA7455
void MMA_Write(u8 cmd,u8 data)
{
  	cmd&=0x3f;
  	MMA_CS_LOW();
  	SPI1_RWByte((cmd<<1)+0x80);
    SPI1_RWByte(data);
  	MMA_CS_HIGH();
}
//MMA7455��ʼ��
void MMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��INT1����
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	SPI1_Init();//��ʼ��SPI�ӿ�
	MMA_SPI_Init();
	CSPin_init();//Ƭѡ��ʼ��
	SPI_MMA_Cal();//MMA7455У׼
}

//MMA7455У׼,�ڳ�ʼ�������
void SPI_MMA_Cal(void)
{   
	MMA_Write(MMA_XOFFL_Addr,81);//У��xֵ		 
	//MMA_Write(MMA_XOFFH_Addr,0xFF);//У��xֵ,У��ֵΪ����,Ҫ�Ѹ�λд1;
	MMA_Write(MMA_YOFFL_Addr,95);//У��yֵ
	//MMA_Write(MMA_YOFFH_Addr,0xFF);//У��yֵ,У��ֵΪ����,Ҫ�Ѹ�λд1;	
	MMA_Write(MMA_ZOFFL_Addr,20);//У��Zֵ
	//MMA_Write(MMA_ZOFFH_Addr,0xFF);//У��Zֵ,У��ֵΪ����,Ҫ�Ѹ�λд1;		
}
//������Ǻͼ��ٶ�(����0-4g)					
signed char Get_MMAval(u8 CDM)
{
    signed char temp;
	
   	MMA_Write(MMA_MCTL_Addr,0x09);//����4g���̲���ģʽ	
	while(!(MMA_Read(MMA_STATUS_Addr)&0x01));//DRDY����λ,�ȴ�������� 	
   	temp=(signed char)MMA_Read(CDM);//MMA7455�Ĵ�������ֵ���Բ������ʽ�����,������
		
	return temp;					  	      	
}




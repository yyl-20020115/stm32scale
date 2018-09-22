#include "MMA7455.h"
#include "delay.h"
#include "spi.h"

//读MMA7455
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
//写MMA7455
void MMA_Write(u8 cmd,u8 data)
{
  	cmd&=0x3f;
  	MMA_CS_LOW();
  	SPI1_RWByte((cmd<<1)+0x80);
    SPI1_RWByte(data);
  	MMA_CS_HIGH();
}
//MMA7455初始化
void MMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化INT1引脚
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
	SPI1_Init();//初始化SPI接口
	MMA_SPI_Init();
	CSPin_init();//片选初始化
	SPI_MMA_Cal();//MMA7455校准
}

//MMA7455校准,在初始化后调用
void SPI_MMA_Cal(void)
{   
	MMA_Write(MMA_XOFFL_Addr,81);//校正x值		 
	//MMA_Write(MMA_XOFFH_Addr,0xFF);//校正x值,校正值为负数,要把高位写1;
	MMA_Write(MMA_YOFFL_Addr,95);//校正y值
	//MMA_Write(MMA_YOFFH_Addr,0xFF);//校正y值,校正值为负数,要把高位写1;	
	MMA_Write(MMA_ZOFFL_Addr,20);//校正Z值
	//MMA_Write(MMA_ZOFFH_Addr,0xFF);//校正Z值,校正值为负数,要把高位写1;		
}
//测量倾角和加速度(量程0-4g)					
signed char Get_MMAval(u8 CDM)
{
    signed char temp;
	
   	MMA_Write(MMA_MCTL_Addr,0x09);//进入4g量程测试模式	
	while(!(MMA_Read(MMA_STATUS_Addr)&0x01));//DRDY标置位,等待测试完毕 	
   	temp=(signed char)MMA_Read(CDM);//MMA7455寄存器中数值是以补码的形式储存的,有正负
		
	return temp;					  	      	
}




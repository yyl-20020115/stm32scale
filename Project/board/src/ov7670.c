#include "ov7670.h"
#include "delay.h"
#include "spi.h"
#include "lcd.h"

//DCMI中断配置
void DCMI_NVIC_Config(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
//摄像头接口初始化
void Cam_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	DCMI_InitTypeDef DCMI_InitStructure;
  	DMA_InitTypeDef  DMA_InitStructure;

  	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);//DCMI 
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
                           RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);//使能DCMI的GPIO时钟
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);//MCO1:PA8
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	     
    RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_3);

	Cam_Pwron();

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);//DCMI_HSYNC 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_PIXCLK
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_D5 			  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);//DCMI_VSYNC 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_DCMI);//DCMI_D6 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_DCMI);//DCMI_D7 			  
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_D0 
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);//DCMI_D1 			  
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_DCMI);//DCMI_D2 
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_DCMI);//DCMI_D3 
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI);//DCMI_D4 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ; 
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4; 
    GPIO_Init(GPIOE, &GPIO_InitStructure);     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	CSPin_init();//拉高SPI1的所有片选信号,以避免干扰.	 

	DCMI_DeInit();
  	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
  	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
  	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  	DCMI_Init(&DCMI_InitStructure);
	//DCMI_NVIC_Config();
	//DCMI_ITConfig(DCMI_IT_FRAME,ENABLE);//DCMI中断
  	DMA_DeInit(DMA2_Stream1);
  	DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  	DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	
  	DMA_InitStructure.DMA_Memory0BaseAddr = FSMC_LCD_ADDRESS;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  	DMA_InitStructure.DMA_BufferSize = 1;
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
  	DMA_Init(DMA2_Stream1, &DMA_InitStructure);	  
}
//OV7670初始化
u8 OV7670_Init(void)
{
  	u8 i;
  	Cam_Init();
	SCCB_Init();
	OV_Reset();
	delay_ms(5);
  	for(i=0;i<OV7670_REG_NUM;i++)
  	{
    	if(OV_WriteReg(OV7670_reg[i][0],OV7670_reg[i][1]))return 1;
  	}
	return 0; 
}
//启动摄像头图像的获取
void Cam_Start(void)
{
  	LCD_WriteReg(0x03,0x1018);
  	LCD_Cursor(0,319);  
  	DMA_Cmd(DMA2_Stream1, ENABLE); 
  	DCMI_Cmd(ENABLE); 
  	DCMI_CaptureCmd(ENABLE); 
  	LCD_REG=0x22;
}
//打开摄像头
void Cam_Pwron(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PE5:PWRDOWN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ; 
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);//power on
}
//关闭摄像头(和I2S音频接口有冲突)
void Cam_Pwrdown(void)
{
  	DMA_Cmd(DMA2_Stream1, DISABLE); 
  	DCMI_Cmd(DISABLE); 
  	DCMI_CaptureCmd(DISABLE);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);//power off	
}
//设置摄像头水平和垂直同步参数								  
void OV7670_HW(u16 hstart,u16 vstart,u16 hstop,u16 vstop)
{
	u8 v;		
	OV_WriteReg(0x17,(hstart>>3)&0xff);//HSTART
	OV_WriteReg(0x18,(hstop>>3)&0xff);//HSTOP
	OV_ReadReg(0x32,&v);
	v=(v&0xc0)|((hstop&0x7)<<3)|(hstart&0x7);
	OV_WriteReg(0x32,v);//HREF
	
	OV_WriteReg(0x19,(vstart>>2)&0xff);//VSTART 开始高8位
	OV_WriteReg(0x1a,(vstop>>2)&0xff);//VSTOP	结束高8位
	OV_ReadReg(0x03,&v);
	v=(v&0xf0)|((vstop&0x3)<<2)|(vstart&0x3);	
	OV_WriteReg(0x03,v);//VREF																 
	OV_WriteReg(0x11,0x00);
}								
/////////////////////////////////////
//模拟SCCB总线的GPIO初始化
void SCCB_Init(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  //SCCB_SIC:PE2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//SCCB_SID:PE3
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
//设置SCCB_SID为输出
void SCCB_SID_OUT(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
//设置SCCB_SID为输入
void SCCB_SID_IN(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
//SCCB起始信号
void SCCB_Start(void)
{
    SCCB_SID_H();//数据线高电平
    delay_us(50);
    SCCB_SIC_H();//在时钟线高的时候数据线由高至低
    delay_us(50);
    SCCB_SID_L();
    delay_us(50);
    SCCB_SIC_L();//数据线恢复低电平，单操作函数必要
    delay_us(50);
}
//SCCB停止信号
void SCCB_Stop(void)
{
    SCCB_SID_L();
    delay_us(50);
    SCCB_SIC_H();	
    delay_us(50);  
    SCCB_SID_H();	
    delay_us(50);  
}
//SCCB非应答信号
void noAck(void)
{	
	SCCB_SID_H();	
	delay_us(50);	
	SCCB_SIC_H();	
	delay_us(50);	
	SCCB_SIC_L();	
	delay_us(50);	
	SCCB_SID_L();	
	delay_us(50);
}
//SCCB写一字节
u8 SCCB_Write(u8 m_data)
{
	u8 j,tem;

	for(j=0;j<8;j++) //循环8次发送数据
	{
		if((m_data<<j)&0x80)SCCB_SID_H();
		else SCCB_SID_L();
		delay_us(50);
		SCCB_SIC_H();	
		delay_us(50);
		SCCB_SIC_L();	
		delay_us(50);
	}
	delay_us(10);
	SCCB_DATA_IN;
	delay_us(50);
	SCCB_SIC_H();	
	delay_us(10);
	if(SCCB_SID_STATE)tem=0;//SDA=1发送失败
	else tem=1;//SDA=0发送成功，返回1
	SCCB_SIC_L();	
	delay_us(50);	
    SCCB_DATA_OUT;
	return tem;  
}
//SCCB读一字节
u8 SCCB_Read(void)
{
	u8 read,j;
	read=0x00;
	
	SCCB_DATA_IN;
	delay_us(50);
	for(j=8;j>0;j--) //循环8次接收数据
	{		     
		delay_us(50);
		SCCB_SIC_H();
		delay_us(50);
		read=read<<1;
		if(SCCB_SID_STATE)read=read+1; 
		SCCB_SIC_L();
		delay_us(50);
	}	
    SCCB_DATA_OUT;
	return read;
}
//写OV7670寄存器
u8 OV_WriteReg(u8 regID, u8 regDat)
{
	SCCB_Start();//发送SCCB总线开始传输命令
	if(!SCCB_Write(0x42))//写地址
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 1;//错误返回
	}
	delay_us(10);
  	if(!SCCB_Write(regID))//积存器ID
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 2;//错误返回
	}
	delay_us(10);
  	if(!SCCB_Write(regDat))//写数据到积存器
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 3;//错误返回
	}
  	SCCB_Stop();//发送SCCB总线停止传输命令	
  	return 0;//成功返回
}
//读OV7660寄存器
u8 OV_ReadReg(u8 regID, u8 *regDat)
{
	//通过写操作设置寄存器地址
	SCCB_Start();
	if(!SCCB_Write(0x42))//写地址
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 1;//错误返回
	}
	delay_us(10);
  	if(!SCCB_Write(regID))//积存器ID
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 2;//错误返回
	}
	SCCB_Stop();//发送SCCB总线停止传输命令	
	delay_us(10);	
	//设置寄存器地址后,才是读
	SCCB_Start();
	if(!SCCB_Write(0x43))//读地址
	{
		SCCB_Stop();//发送SCCB总线停止传输命令
		return 3;//错误返回
	}
	delay_us(10);
  	*regDat=SCCB_Read();//返回读到的值
  	noAck();//发送NACK命令
  	SCCB_Stop();//发送SCCB总线停止传输命令
  	return 0;//成功返回
}
//软复位OV7670
void OV_Reset(void)
{
	OV_WriteReg(0x12,0x80);
}
//读取OV7670的ID号
u8 OV_ReadID(void)
{
	u8 temp;
	OV_ReadReg(0x0b,&temp);
  	return temp;
}


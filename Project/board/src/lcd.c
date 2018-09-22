#include "lcd.h"
#include "delay.h"

//FSMCҺ���ӿڵ�GPIO��ʼ��
/********************************************* 
**               Ӳ������˵��               ** 
** STM32                         LCD        ** 
** PE/PD(FSMC_D0-15) <---->      DB0~15     **     
** PD4 (FSMC_NOE)    <---->      nRD        ** 
** PD5 (FSMC_NWE)    <---->      nWR        ** 
** PD7 (FSMC_NE1)    <---->      nCS        **       
** PD11(FSMC_A23)    <---->      RS         ** 
PD.14(D0), PD.15(D1), PD.00(D2), PD.01(D3), PD.08(D13), PD.09(D14),
PD.10(D15),PD.04(RD), PD.05(WR), PE.07(D4), PE.08(D5),  PE.09(D6), 
PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),PE.14(D11), PE.15(D12)
*********************************************/ 
u8 	Fsize;
u8 	Charmd;  
u16 Bcolor;

void LCD_LineConf(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                   GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ����ӦGPIO
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//���Ӹ������� 
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                 GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ����ӦGPIO
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);//���Ӹ�������
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//(RS)
  	GPIO_Init(GPIOD, &GPIO_InitStructure);  
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);	   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//(CS) 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
}
//FSMC��ʼ��
void LCD_FSMCConf(void)
{
  	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;

  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 5;//��ַ����ʱ��
  	p.FSMC_AddressHoldTime = 0;	//��ַ����ʱ��
  	p.FSMC_DataSetupTime = 9;	//���ݽ���ʱ��
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}
//дLCD�Ĵ���
void LCD_WriteReg(u8 Reg, u16 Value)
{
  	LCD_REG=Reg;
  	LCD_RAM=Value;
}
//��LCD�Ĵ���
u16 LCD_ReadReg(u8 Reg)
{
  	LCD_REG=Reg;
  	return (LCD_RAM);
}
//дLCD RAM
void LCD_WriteRAM(u16 RGB_Code)
{
  	LCD_RAM=RGB_Code;
}
//��LCD RAM(��Ҫ������)
u16 LCD_ReadRAM(void)
{
  	u16 dummy;
  	LCD_REG=0x22; 
	dummy=LCD_RAM;
  	return LCD_RAM;
}
//LCD��ʼ��
void LCD_Init(void)
{ 
  	u32 lcdid=0;

   	LCD_LineConf();//��ʼ��GPIO
  	LCD_FSMCConf();//����FSMC
  	delay_ms(5); 
  	lcdid=LCD_ReadReg(0x00);//��LCD ID  
  	if(lcdid == 0x9320)
  	{
		LCD_WriteReg(0xE5,0x8000);
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0700);
		LCD_WriteReg(0x03,0x1030);
		LCD_WriteReg(0x04,0x0000);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x09,0x0000);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x0B,0x0000);
		LCD_WriteReg(0x0C,0x0000);
		LCD_WriteReg(0x0F,0x0000);
		LCD_WriteReg(0x10,0x0000);
		LCD_WriteReg(0x11,0x0000);
		LCD_WriteReg(0x12,0x0000);
		LCD_WriteReg(0x13,0x0000);
		delay_ms(20);
		LCD_WriteReg(0x10,0x17B0);
		LCD_WriteReg(0x11,0x0137);
		delay_ms(5);
		LCD_WriteReg(0x12,0x0139);
		delay_ms(5);
		LCD_WriteReg(0x13,0x1d00);
		LCD_WriteReg(0x29,0x0013);
		delay_ms(5);
		LCD_WriteReg(0x20,0x0000);
		LCD_WriteReg(0x21,0x0000);
		LCD_WriteReg(0x30,0x0006);
		LCD_WriteReg(0x31,0x0101);
		LCD_WriteReg(0x32,0x0003);
		LCD_WriteReg(0x35,0x0106);
		LCD_WriteReg(0x36,0x0b02);
		LCD_WriteReg(0x37,0x0302);
		LCD_WriteReg(0x38,0x0707);
		LCD_WriteReg(0x39,0x0007);
		LCD_WriteReg(0x3C,0x0600);
		LCD_WriteReg(0x3D,0x020b);
		LCD_WriteReg(0x50,0x0000);
		LCD_WriteReg(0x51,0x00EF);
		LCD_WriteReg(0x52,0x0000);
		LCD_WriteReg(0x53,0x013F);
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x6A,0x0000);
		LCD_WriteReg(0x80,0x0000);
		LCD_WriteReg(0x81,0x0000);
		LCD_WriteReg(0x82,0x0000);
		LCD_WriteReg(0x83,0x0000);
		LCD_WriteReg(0x84,0x0000);
		LCD_WriteReg(0x85,0x0000);
		LCD_WriteReg(0x90,0x0010);
		LCD_WriteReg(0x92,0x0000);
		LCD_WriteReg(0x93,0x0003);
		LCD_WriteReg(0x95,0x0110);
		LCD_WriteReg(0x97,0x0000);
		LCD_WriteReg(0x98,0x0000);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x07,0x0173); 
  	}
  	else if(lcdid == 0x9325)
  	{
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0700);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x04,0x0000);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x09,0x0000);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x0C,0x0000);
		LCD_WriteReg(0x0D,0x0000);
		LCD_WriteReg(0x0F,0x0000);
		LCD_WriteReg(0x10,0x0000);
		LCD_WriteReg(0x11,0x0000);
		LCD_WriteReg(0x12,0x0000);
		LCD_WriteReg(0x13,0x0000);
		delay_ms(20);
		LCD_WriteReg(0x10,0x17B0);
		LCD_WriteReg(0x11,0x0137);
		delay_ms(5);
		LCD_WriteReg(0x12,0x0139);
		delay_ms(5);
		LCD_WriteReg(0x13,0x1d00);
		LCD_WriteReg(0x29,0x0013);
		delay_ms(5);
		LCD_WriteReg(0x20,0x0000);
		LCD_WriteReg(0x21,0x0000);
		LCD_WriteReg(0x30,0x0007);
		LCD_WriteReg(0x31,0x0302);
		LCD_WriteReg(0x32,0x0105);
		LCD_WriteReg(0x35,0x0206);
		LCD_WriteReg(0x36,0x0808);
		LCD_WriteReg(0x37,0x0206);
		LCD_WriteReg(0x38,0x0504);
		LCD_WriteReg(0x39,0x0007);
		LCD_WriteReg(0x3C,0x0105);
		LCD_WriteReg(0x3D,0x0808);
		LCD_WriteReg(0x50,0x0000);
		LCD_WriteReg(0x51,0x00EF);
		LCD_WriteReg(0x52,0x0000);
		LCD_WriteReg(0x53,0x013F);
		LCD_WriteReg(0x60,0xA700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x6A,0x0000);
		LCD_WriteReg(0x80,0x0000);
		LCD_WriteReg(0x81,0x0000);
		LCD_WriteReg(0x82,0x0000);
		LCD_WriteReg(0x83,0x0000);
		LCD_WriteReg(0x84,0x0000);
		LCD_WriteReg(0x85,0x0000);
		LCD_WriteReg(0x90,0x0010);
		LCD_WriteReg(0x92,0x0000);
		LCD_WriteReg(0x93,0x0003);
		LCD_WriteReg(0x95,0x0110);
		LCD_WriteReg(0x97,0x0000);
		LCD_WriteReg(0x98,0x0000);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x07,0x0133);
  	} 
	else if(lcdid==0x5408)
	{
	 	LCD_WriteReg(0x0000,0x0000);
	    LCD_WriteReg(0x0001,0x0100);//0000	     
	    LCD_WriteReg(0x0002,0x0700);
		LCD_WriteReg(0x0003,0x1030);//d0a0 
		LCD_WriteReg(0x0004,0x0000);
		LCD_WriteReg(0x0008,0x0207);		
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000a,0x0000);	
		LCD_WriteReg(0x000C,0x0000);
		LCD_WriteReg(0x000D,0x0000);
		LCD_WriteReg(0x000F,0x0000);		
		LCD_WriteReg(0x0007,0x0101);
	    LCD_WriteReg(0x0010,0x12b0);
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0017,0x0001);
		delay_ms(100);//200		
		LCD_WriteReg(0x0012,0x01bb);
		LCD_WriteReg(0x0013,0x1c00);
		delay_ms(25);		
		LCD_WriteReg(0x0029,0x0019);//0012
		delay_ms(25);		
		LCD_WriteReg(0x0030,0x0102);
		delay_ms(25);			
	    LCD_WriteReg(0x0031,0x0c21);//0c20
	    LCD_WriteReg(0x0032,0x0b22);//0b21	    
	    LCD_WriteReg(0x0033,0x2610);//250f	    	    
	    LCD_WriteReg(0x0034,0x1e0b);//1d0b
	   	LCD_WriteReg(0x0035,0x0a04);
	    LCD_WriteReg(0x0036,0x1701);
	    LCD_WriteReg(0x0037,0x0617);
	    LCD_WriteReg(0x0038,0x0305);
	    LCD_WriteReg(0x0039,0x0a05);	    
	    LCD_WriteReg(0x003a,0x0c04);//0f04
	    LCD_WriteReg(0x003b,0x0c00);//0f00
	    LCD_WriteReg(0x003c,0x000c); //000f
		LCD_WriteReg(0x003d,0x050c);//050f	
	    LCD_WriteReg(0x003e,0x0204); 
		LCD_WriteReg(0x003f,0x0404);
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00ef);
		LCD_WriteReg(0x0052,0x0000);	
		LCD_WriteReg(0x0053,0x013F);		
		LCD_WriteReg(0x0060,0x2700);
		LCD_WriteReg(0x0061,0x0001);	
		LCD_WriteReg(0x006A,0x0000);		
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);	
		LCD_WriteReg(0x0090,0x0010);
		LCD_WriteReg(0x0092,0x0000);
		LCD_WriteReg(0x0093,0x0103);
		LCD_WriteReg(0x0095,0x0210);
		LCD_WriteReg(0x0097,0x0000);
		LCD_WriteReg(0x0098,0x0000);
		LCD_WriteReg(0x00f0,0x5408);
		LCD_WriteReg(0x00f3,0x0005);
	   	LCD_WriteReg(0x00f4,0x001f);
	   	LCD_WriteReg(0x00f0,0x0000);
	   	LCD_WriteReg(0x0007,0x0173);    
	}
	LCD_Clear(WHITE);//�����
	LCD_Font(16);//��������Ϊ16*8
	LCD_Dispmd(7);//��������ˢ��
	LCD_Charmd(1,WHITE);//�����ַ���ʾΪ����ģʽ,����ɫΪ��ɫ(�ǵ���ģʽ����Ч)							 
}
//��������
void LCD_Font(u8 font)
{
  	Fsize=font;
}
//�����ַ���ʾģʽ
void LCD_Charmd(u8 mode,u16 color)
{
  	Charmd=mode;
	Bcolor=color;
}
//���õ�����
void LCD_Cursor(u16 Xpos, u16 Ypos)
{
  	LCD_WriteReg(0x20, Xpos);
  	LCD_WriteReg(0x21, Ypos);
}

//������ʾ����,ע�����ô��ں����겻Ҫ�������ڷ�Χ
void LCD_Window(u16 x1, u16 y1, u16 x2, u16 y2)
{
  	LCD_WriteReg(0x50,x1);
  	LCD_WriteReg(0x51,x2);
  	LCD_WriteReg(0x52,y1); 
  	LCD_WriteReg(0x53,y2);
}
//����ˢ����ʽ
void LCD_Dispmd(u8 mode)
{
	switch(mode)//����4(����)��7(����)
	{
		case 1:LCD_WriteReg(0x03,0x1000);break;//��������:���I
		case 2:LCD_WriteReg(0x03,0x1008);break;//��������:���I
		case 3:LCD_WriteReg(0x03,0x1010);break;//��������:���J
		case 4:LCD_WriteReg(0x03,0x1018);break;//��������:���J
		case 5:LCD_WriteReg(0x03,0x1020);break;//��������:���L
		case 6:LCD_WriteReg(0x03,0x1028);break;//��������:���L
		case 7:LCD_WriteReg(0x03,0x1030);break;//��������:���K
		case 8:LCD_WriteReg(0x03,0x1038);break;//��������:���K
	}
}
//����
void LCD_Clear(u16 Color)
{
  	u32 index=0;
 
  	LCD_Cursor(0x00,319);//������ʼ����
  	LCD_REG=0x22; 
  	for(index=0;index<76800;index++)LCD_RAM=Color;//һ����240*320=76800����
}
//LCD����
void LCD_SetPoint(u16 xpos, u16 ypos,u16 color)
{
	LCD_Cursor(xpos,ypos);
	LCD_REG=0x22;//д��LCDRAM��׼��
	LCD_RAM=color;
}
//LCD����
u16 LCD_GetPoint(u16 x,u16 y)
{
	LCD_Cursor(x,y);
	return BGR565toRGB565(LCD_ReadRAM());
}
//LCD������б��
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_SetPoint(uRow,uCol,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
//LCD��������
void LCD_Rect(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	LCD_Line(x1,y1,x2,y1,color);
	LCD_Line(x1,y1,x1,y2,color);
	LCD_Line(x1,y2,x2,y2,color);
	LCD_Line(x2,y1,x2,y2,color);
}
//LCD��Բ
void LCD_Circle(u16 Xpos, u16 Ypos, u16 Radius, u16 color)
{
  	s32  Dec;
  	u32  CurX;
  	u32  CurY;
	 
  	CurX=0;
  	CurY=Radius;   
  	Dec=3-(Radius<<1);
  	while(CurX<=CurY)
  	{
		LCD_SetPoint(Xpos+CurX,Ypos+CurY,color);
		LCD_SetPoint(Xpos+CurX,Ypos-CurY,color);
		LCD_SetPoint(Xpos-CurX,Ypos+CurY,color);
		LCD_SetPoint(Xpos-CurX,Ypos-CurY,color);
		LCD_SetPoint(Xpos+CurY,Ypos+CurX,color);
		LCD_SetPoint(Xpos+CurY,Ypos-CurX,color);
		LCD_SetPoint(Xpos-CurY,Ypos+CurX,color);
		LCD_SetPoint(Xpos-CurY,Ypos-CurX,color);
		if(Dec<0)Dec+=(CurX<<2)+6;
	    else
	    {
	      	Dec+=((CurX-CurY)<<2)+10;
	      	CurY--;
	    }
	    CurX++;
  	}
}
//LCD�������
void LCD_Fill(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{          	
	u16 i,j;
	u16 xlen=0;

	xlen=x2-x1;	   
	for(i=y1;i<y2;i++)
	{
	 	LCD_Cursor(x1,i);      	//���ù��λ�� 
		LCD_REG=0x22;      			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)LCD_RAM=color;//���ù��λ�� 	    
	}  						  	    
}  
//��ʾASCII�ַ�
void LCD_Char(u16 x,u16 y,u8 num,u16 color)
{     
    u8 temp;
    u8 pos,t;
	u16 x0=x;
		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	for(pos=0;pos<Fsize;pos++)
	{
		if(Fsize==12)temp=asc2_1206[num][pos];//����1206����
		else temp=asc2_1608[num][pos];		 //����1608����
		for(t=0;t<Fsize/2;t++)
		{                 
		    if(temp&0x01)LCD_SetPoint(x,y,color);
			else if(Charmd)LCD_SetPoint(x,y,Bcolor);	
			temp>>=1; 
			x++;
		}
		x=x0;
		y++;
	}	 	  
} 
//��ʾ�ַ���
void LCD_String(u16 x,u16 y,const u8 *p,u16 color)
{         
    while(*p!='\0')
    {       
        LCD_Char(x,y,*p,color);
        x+=8;
        p++;
    }  
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ����	 
void LCD_Num(u16 x,u16 y,u32 num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_Char(x+(Fsize/2)*t,y,' ',color);
				continue;
			}else enshow=1; 		 	 
		}
	 	LCD_Char(x+(Fsize/2)*t,y,temp+'0',color); 
	}
} 
//����BMPλͼ����
void Draw_Bmp(u16 x,u16 y,u16 width,u16 height,u8* pic)
{
  	u16 i,bytes;

	bytes=width*height*2;
	LCD_Window(x,y,x+width-1,y+height-1);
	LCD_Cursor(x,y);
  	LCD_REG=0x22; 
	for(i=0;i<bytes;i+=2)LCD_RAM=(pic[i]<<8)|pic[i+1]; 	
}




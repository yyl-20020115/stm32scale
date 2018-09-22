#include "at45db.h"
#include "hanzi.h"
#include "lcd.h"
#include "ff.h"


u16 Llimit=0;
u16 Rlimit=240;
u8  GBK_Buf[32];
//�����ֿ�
void UpdateFont(u8 *path)
{
	u8 	 res;
	FIL  fileR;
	UINT Bytes;
	u16  page;

	res=f_open(&fileR,(TCHAR*)path,FA_READ);
	if(res)return;
	LCD_Charmd(1,WHITE);
	LCD_String(20,70,"Petcent:        %",RED);
	for(page=0;page<FON_size;page++)
	{
		f_read(&fileR,AT45_buffer,528,&Bytes);
		FlashPageWrite(FON_addr+page,&AT45_buffer[0]);//д��SPI FLASH
		LCD_Num(100,70,page*100/FON_size,3,BLUE);//��ʾд��ٷֱ�
	} 
	f_close(&fileR);			
}
//��ȡ���ֵ�ַ
u32 Get_FontAdr(u8* ch)
{
	u32 temp1,temp2; 
   
    temp1=*ch; 
    temp2=*(ch+1); 
    if(temp1<0x81||temp2<0x40)return 0;//���Ϸ��ĺ��� 
    temp1-=0x81;	//��ȥ������ʼ��											
    temp2-=0x40;	//λ�� 
    temp1*=192;		
    temp1+=temp2;	//�õ�ƫ�� 			
	temp2=((temp1/16)<<9)+((temp1%16)<<5);//�õ����ֿ��ļ��е�ƫ�Ƶ�ַ
	return temp2;	
}
//��ʾһ������,ȡģ��ʽΪ����,�ֽڵ���
void Show_Hanzi(int x0,u16 y0,u8 *word,u16 color)
{
	u8  m,n;
	u16 temp;
	int refx;

	for(m=0;m<16;m++)
	{
		refx=m+x0;
		if(refx>=Llimit&&refx<Rlimit)
		{
			temp=(word[m+16]<<8)+word[m];
			for(n=0;n<16;n++) 
			{				
				if(temp&0x8000)LCD_SetPoint(refx,y0+15-n,color); 
				else if(Charmd)LCD_SetPoint(refx,y0+15-n,Bcolor);
				temp<<=1;
			}
		}
	}
}
//��Ӣ�Ļ����ʾ
void Show_Fonts(u16 x0,u16 y0,u8* Fonts,u16 color)
{
	u32   offset;

	while(*Fonts!='\0')
	{			
		if(*Fonts<0x81)//��ASCII�ַ�
		{
			LCD_Char(x0,y0,*Fonts,color);
			*Fonts++;//ƫ�Ƶ�ַ��1
			x0+=8;	
		}			
		else//�Ǻ��� 
		{	
			offset=Get_FontAdr(Fonts);
			SPI_Flash_Read(&GBK_Buf[0],FON_addr*528+offset,32);			
			Show_Hanzi(x0,y0,GBK_Buf,color);			
			*Fonts++;*Fonts++;//ƫ�Ƶ�ַ��2
			x0+=16;
		}
	}	
}




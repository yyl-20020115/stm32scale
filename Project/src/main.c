#include "main.h"
//JTAG PINS: PB3,PB4,PA13,PA14,PA15
//TOUCH-SPI1:PA15 ->Conflict with JTAG


uint32_t timingdelay;
__IO u32 LocalTime=0; 

int32_t RealZeroLevel = 0;
int32_t Real100gLevel = 0;
int32_t Real200gLevel = 0;

int32_t ZeroLevel = 8807000;
int32_t _100gLevel = 9007000;
int32_t _200gLevel = 9207000;
int32_t Data = 0;
int32_t Values = 0;
int32_t Delta = 0;


float Weight = 0.0f;
#define		N	12
//PIN:PC6,PC7
#define HX_RCC RCC_AHB1Periph_GPIOC
#define HX_PORT GPIOC
#define ADSK_PIN GPIO_Pin_6
#define ADDO_PIN GPIO_Pin_7

int32_t DataStore[N] ={0};

//HX711 AD读数
int32_t ReadCount(void)
{
    int32_t Count=0;
    uint8_t i=0;

    GPIO_ResetBits(HX_PORT,ADSK_PIN);
	
    Count = 0;
    while( GPIO_ReadInputDataBit(HX_PORT,ADDO_PIN));
    for (i = 0; i < 24; i++)
    {
				GPIO_SetBits(HX_PORT,ADSK_PIN);
        Count = Count << 1;
				GPIO_ResetBits(HX_PORT,ADSK_PIN);
        if(GPIO_ReadInputDataBit(HX_PORT,ADDO_PIN)) 
				{
					Count |=1;
				}
    }
		GPIO_SetBits(HX_PORT,ADSK_PIN);
    Count = Count ^ 0x800000;

		GPIO_ResetBits(HX_PORT,ADSK_PIN);

    return Count;
}


//滤波
int32_t GetMiddleFilterValue(int32_t rc)
{
	u32 max = 0;
	u32 min = 0;
	u32 sum = 0;
	u8 i = 0;

	max = min = sum = DataStore[0] = rc; 	

	for(i=N-1; i!= 0; i--)
	{
		if(DataStore[i] > max) 
			max = DataStore[i];
		else if(DataStore[i] < min) 
			min = DataStore[i];

		sum = sum + DataStore[i];
		DataStore[i] = DataStore[i - 1];
	}
	
	i = N - 2;
	sum = sum - max - min;// + i/2;
	sum = sum/i;

	return sum;
}
void Scale_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//初始化结构体

	RCC_AHB1PeriphClockCmd(HX_RCC, ENABLE);//GPIO外设时钟使能
	//PB7=ADDO
	GPIO_InitStructure.GPIO_Pin = ADDO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//状态为输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(HX_PORT, &GPIO_InitStructure);//初始化
	//PB6=ADSK
	GPIO_InitStructure.GPIO_Pin =  ADSK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(HX_PORT, &GPIO_InitStructure);//初始化
}

void Calibrate(int32_t rc)
{	
	u8 i = 0;
	Values = 0;
	for(i = 0;i<N;i++)
	{
		Values += GetMiddleFilterValue(ReadCount());
	}
	
	RealZeroLevel = Values/N;
	RealZeroLevel = GetMiddleFilterValue(ReadCount());
	
	Delta = RealZeroLevel - ZeroLevel;
  Real100gLevel =	_100gLevel + Delta;
	Real200gLevel = _200gLevel + Delta;
		
}
int main(void)
{
  u8 k = 0;
	u8 buffer[256] = {0};   
	int32_t value = 0;
	int32_t middle = 0;
  Key_Init();//按键初始化
	LED_Init();//LED灯初始化
	Scale_Init();
	LCD_Init();//LCD显示屏初始化
	//Touch_Init();
	
	Calibrate(ReadCount());
	
	COM1Init(115200);//串口1初始化

	LCD_String(64,10,(const u8*)"STM32 Scale",BLUE);

	while(1)
	{
		
		k = KEY_Scan();

		value = ReadCount();
		Data = middle = GetMiddleFilterValue(value);
		printf("%08X,%08X,%08X,%08X,%08X\n",value,middle,Real200gLevel,Real100gLevel,RealZeroLevel);
		switch(k){
			case 1:
				ZeroLevel = middle;
			  snprintf((char*)buffer,sizeof(buffer)-1,"000g = %8d        ",RealZeroLevel);
			  LCD_String(24,36,buffer,RED);
				break;
			case 2: //100g value
				Real100gLevel =  middle;
			  snprintf((char*)buffer,sizeof(buffer)-1,"100g = %8d        ",Real100gLevel);
			  LCD_String(24,54,buffer,RED);
				break;
			case 3: //200g value
				Real200gLevel =  middle;
			  snprintf((char*)buffer,sizeof(buffer)-1,"200g = %8d        ",Real200gLevel);
			  LCD_String(24,72,buffer,RED);
				break;
			case 4: //cali
		  	Calibrate(value);
			  snprintf((char*)buffer,sizeof(buffer)-1,"000g = %8d        ",RealZeroLevel);
			  LCD_String(24,36,buffer,RED);
				break;
		  default: //default show
			  snprintf((char*)buffer,sizeof(buffer)-1,"data = %8d        ",Data);
			  LCD_String(24,90,buffer,BLUE);
			  Weight = (Real200gLevel!=Real100gLevel) ? (float)(Data - RealZeroLevel)/(float)(Real200gLevel - Real100gLevel) * 100.0f : 0.0f;
			  snprintf((char*)buffer,sizeof(buffer)-1,"weight = %4.4lfg        ",Weight);
			  LCD_String(24,108,buffer,BLUE);
				break;				
		}
		
	}
}



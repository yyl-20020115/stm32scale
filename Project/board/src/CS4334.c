#include "CS4334.h"

DMA_InitTypeDef DMA_InitStructure;          
//��Ƶ���ų�ʼ��,AudioFreq��ƵΪ������
void AUDIO_Init(u32 AudioFreq)
{
	I2S_GPIO_Init();
	Audio_I2S_Init(AudioFreq);
	Audio_DMA_Init();
}
//I2S�ӿڵ�GPIO��ʼ��
void I2S_GPIO_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);  
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);   
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_SPI2); 
}
//I2S��Ƶ�ӿڳ�ʼ��
void Audio_I2S_Init(uint32_t AudioFreq)
{
  	I2S_InitTypeDef I2S_InitStructure;
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  	SPI_I2S_DeInit(SPI2);
  	I2S_InitStructure.I2S_AudioFreq = AudioFreq;
  	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable; 
  	I2S_Init(SPI2, &I2S_InitStructure);
}
//DMA��������
void Audio_DMA_Init(void)  
{ 
  	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  	DMA_Cmd(DMA1_Stream4, DISABLE);
  	DMA_DeInit(DMA1_Stream4);
  	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  	DMA_InitStructure.DMA_PeripheralBaseAddr = 0x4000380C;
  	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;//����ڲ��ź����н�������
  	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  	DMA_InitStructure.DMA_BufferSize = (uint32_t)0xFFFE;//����ڲ��ź����н�������      
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����޵�ַ����
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//��������Ϊ����
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�ڴ�����Ϊ���� 
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//����ģʽ 
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
  	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//����FIFO����         
  	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//û�õ�FIFO,����������
  	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//���ģʽ�ڵ�ַ����ʱ����Ч
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
  	DMA_Init(DMA1_Stream4, &DMA_InitStructure);  
  	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);//ʹ����ѡDMA���ж�
  	//I2S DMA�ж�ͨ������
  	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);   
  	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);//�ж�����ʹ��  
}
//��ʼ����ý���е���Ƶ��
void Audio_MAL_Play(u32 Addr, u32 Size)
{         
  	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)Addr;//���û�������ַ
  	DMA_InitStructure.DMA_BufferSize=(uint32_t)Size/2;//���û�������С
  	DMA_Init(DMA1_Stream4,&DMA_InitStructure);
  	DMA_Cmd(DMA1_Stream4,ENABLE);//I2S DMAʹ��   
  	if((SPI2->I2SCFGR & I2S_ENABLE_MASK)==0)I2S_Cmd(SPI2,ENABLE);//��I2Sδʹ��,��ʹ��
}
//DMA��������жϷ�����
extern void TX_callback(void);
void DMA1_Stream4_IRQHandler(void)
{    
  	if (DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)!=RESET)
  	{         
      	DMA_Cmd(DMA1_Stream4, DISABLE);//DMA����   
      	DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);//���жϱ�־       
      	TX_callback();//�û��ص�����      
  	}
}










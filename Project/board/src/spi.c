#include "spi.h"

static SPI_InitTypeDef  SPI_InitStructure;
static GPIO_InitTypeDef GPIO_InitStructure;
//��ʼ��SPI1�ӿ�
void SPI1_Init(void)
{
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//��SPI1��ʼ��Ϊ����FLASH��;
void FLASH_SPI_Init(void)
{
	SPI_I2S_DeInit(SPI1);
 	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����ģʽ
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//����ģʽ��SCKΪ0
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//���ݲ����ӵ�1��ʱ����ؿ�ʼ
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�������
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//������
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ģʽ
  	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC����ʽ
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
}
//��SPI1��ʼ��Ϊ��������;
void ADS_SPI_Init(void)
{
	SPI_I2S_DeInit(SPI1);
 	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����ģʽ
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ģʽ��SCKΪ1
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//���ݲ����ӵ�2��ʱ����ؿ�ʼ
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�������
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//������
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ģʽ
  	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC����ʽ
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
}
//��SPI1��ʼ��Ϊ���ٶȴ�������;
void MMA_SPI_Init(void)
{
	SPI_I2S_DeInit(SPI1);
 	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����ģʽ
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//����ģʽ��SCKΪ0
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//���ݲ����ӵ�2��ʱ����ؿ�ʼ
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�������
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//������
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ģʽ
  	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC����ʽ
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
}
//��ʼ������SPI����Ƭѡ���Ų��ø߲�ѡ��
void CSPin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//MMACS:PD12
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	GPIO_SetBits(GPIOD, GPIO_Pin_12);//��ѡ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//TPCS:PB14
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	GPIO_SetBits(GPIOB, GPIO_Pin_14);//��ѡ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//FLCS:PD13
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	GPIO_SetBits(GPIOD, GPIO_Pin_13);//��ѡ��
}
//SPI1��дһ�ֽ�����
u8 SPI1_RWByte(u8 byte)
{
 	while((SPI1->SR&SPI_I2S_FLAG_TXE)==RESET);
 	SPI1->DR = byte;
 	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==RESET);
 	return(SPI1->DR);
}


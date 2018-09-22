#include "wavplayer.h"
#include "fsearch.h"
#include "cs4334.h"
#include "lcd.h"
#include "led.h"


u8 buffer1[1024]={0x00};//˫����
u8 buffer2[1024]={0x00};         
u32  WaveLen;//��Ƶ�ļ�����
u32  headcnt;//WAV��Ϣͷ���� 
u8   buf_sw;//����ת������
u8   plstop;//������ͣ����
wavdef WAVE;//WAVͷ��Ϣ�ṹ��
UINT ByteRD;
FIL  fileR;
//WAV��Ƶ�ļ����ź���
u8 Wavplay(u8* fname)
{          		
  	My_chdir("0:/MUSIC");//�л��������ļ���
	Add_path(fname);//׷��Ŀ¼
  	f_open(&fileR,(TCHAR*)path_curr, FA_READ);//���ļ�
	f_read(&fileR,buffer1,1024,&ByteRD);//��ȡ1KB����
	if(WaveParsing(&buffer1[0]))return 1;//����ͷ��Ϣ����
  	WaveLen=WAVE.DataSize;//��Ƶ���ݴ�С
  	AUDIO_Init(WAVE.SampleRate);//��Ƶ���ų�ʼ��
  	f_lseek(&fileR, headcnt);//�����ļ�ͷ
  	f_read(&fileR, buffer1, 1024, &ByteRD);//Ԥ����仺��
  	f_read(&fileR, buffer2, 1024, &ByteRD);
  	Audio_MAL_Play((u32)buffer1, 1024);//�ӻ���1����
  	buf_sw=1;
	plstop=0;
	return 0;
}
//�ļ�ͷ����
u8 WaveParsing(u8* buf)
{
  	u32 temp=0x00;
	u32 DataOffset=0;
  	u32 Extrabytes=0;

  	temp=ReadUnit(buf,0,4,1);//��'RIFF'
  	if(temp!=CHUNK_ID)return 1;
  	WAVE.chunksize=ReadUnit((u8*)buf,4,4,0);//���ļ�����
  	temp=ReadUnit(buf,8,4,1);//��'WAVE'
  	if(temp!=FILE_FORMAT)return 2;
  	temp=ReadUnit(buf,12,4,1);//��'fmt '
  	if(temp!=FORMAT_ID)return 3;
  	temp=ReadUnit(buf,16,4,0);//��'fmt'���ݳ���
  	if(temp!=0x10)Extrabytes=1;
  	WAVE.FormatTag=ReadUnit(buf,20,2,0);//����Ƶ��ʽ
  	if(WAVE.FormatTag!=WAVE_FORMAT_PCM)return 4;  
  	WAVE.Channels=ReadUnit(buf,22,2,0);//�������� 
	WAVE.SampleRate=ReadUnit(buf,24,4,0);//��������
	WAVE.ByteRate=ReadUnit(buf,28,4,0);//��������
	WAVE.BlockAlign=ReadUnit(buf,32,2,0);//�������
	WAVE.BitCount=ReadUnit(buf,34,2,0);//��������λ��
	if(WAVE.BitCount!=16)return 5;//��֧��16λ����Ƶ
	DataOffset=36;
	if(Extrabytes)
	{
		temp=ReadUnit(buf,36,2,0);//�������ʽ�ֽ�
		if(temp!=0x00)return 6;
		temp=ReadUnit(buf,38,4,1);//��'fact'
		if(temp!=FACT_ID)return 7;
		temp=ReadUnit(buf,42,4,0);//��Fact���ݴ�С
		DataOffset+=10+temp;
	}
	temp=ReadUnit(buf,DataOffset,4,1);//��'data'
	DataOffset+=4;
	if(temp!=DATA_ID)return 8;
	WAVE.DataSize=ReadUnit(buf,DataOffset,4,0);//���������ݴ�С
	DataOffset+=4;
	headcnt=DataOffset;
	return 0;
}
//ֹͣ����
void Stop_play(void)
{
	plstop=1;
}
//�����ݵ�Ԫ(���4�ֽ�)
//Endian 1:���ģʽ;0:С��ģʽ
u32 ReadUnit(u8 *buf,u8 idx,u8 Num,u8 Endian)
{
  	u8  i;
  	u32 temp=0;
  
  	for(i=0;i<Num;i++)temp|=buf[idx+i]<<(i*8);
  	if(Endian)temp=__REV(temp);
  	return temp;
}
//DMA��������ж�
void TX_callback(void)
{  
	if(plstop)return;
	if(buf_sw)
    {
        Audio_MAL_Play((u32)buffer2,1024);//��buffer2����
        f_read(&fileR,buffer1,1024,&ByteRD);//���buffer1
        buf_sw=0;
    }
    else 
    {   
       	Audio_MAL_Play((u32)buffer1,1024);//��buffer1����
        f_read(&fileR,buffer2,1024,&ByteRD);//���buffer2
        buf_sw=1;
    } 
}



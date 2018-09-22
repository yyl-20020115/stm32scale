#include "wavplayer.h"
#include "fsearch.h"
#include "cs4334.h"
#include "lcd.h"
#include "led.h"


u8 buffer1[1024]={0x00};//双缓冲
u8 buffer2[1024]={0x00};         
u32  WaveLen;//音频文件长度
u32  headcnt;//WAV信息头长度 
u8   buf_sw;//缓冲转换开关
u8   plstop;//播放暂停控制
wavdef WAVE;//WAV头信息结构体
UINT ByteRD;
FIL  fileR;
//WAV音频文件播放函数
u8 Wavplay(u8* fname)
{          		
  	My_chdir("0:/MUSIC");//切换到音乐文件夹
	Add_path(fname);//追加目录
  	f_open(&fileR,(TCHAR*)path_curr, FA_READ);//打开文件
	f_read(&fileR,buffer1,1024,&ByteRD);//读取1KB数据
	if(WaveParsing(&buffer1[0]))return 1;//进行头信息解析
  	WaveLen=WAVE.DataSize;//音频数据大小
  	AUDIO_Init(WAVE.SampleRate);//音频播放初始化
  	f_lseek(&fileR, headcnt);//跳过文件头
  	f_read(&fileR, buffer1, 1024, &ByteRD);//预先填充缓冲
  	f_read(&fileR, buffer2, 1024, &ByteRD);
  	Audio_MAL_Play((u32)buffer1, 1024);//从缓冲1播放
  	buf_sw=1;
	plstop=0;
	return 0;
}
//文件头解析
u8 WaveParsing(u8* buf)
{
  	u32 temp=0x00;
	u32 DataOffset=0;
  	u32 Extrabytes=0;

  	temp=ReadUnit(buf,0,4,1);//读'RIFF'
  	if(temp!=CHUNK_ID)return 1;
  	WAVE.chunksize=ReadUnit((u8*)buf,4,4,0);//读文件长度
  	temp=ReadUnit(buf,8,4,1);//读'WAVE'
  	if(temp!=FILE_FORMAT)return 2;
  	temp=ReadUnit(buf,12,4,1);//读'fmt '
  	if(temp!=FORMAT_ID)return 3;
  	temp=ReadUnit(buf,16,4,0);//读'fmt'数据长度
  	if(temp!=0x10)Extrabytes=1;
  	WAVE.FormatTag=ReadUnit(buf,20,2,0);//读音频格式
  	if(WAVE.FormatTag!=WAVE_FORMAT_PCM)return 4;  
  	WAVE.Channels=ReadUnit(buf,22,2,0);//读声道数 
	WAVE.SampleRate=ReadUnit(buf,24,4,0);//读采样率
	WAVE.ByteRate=ReadUnit(buf,28,4,0);//读波特率
	WAVE.BlockAlign=ReadUnit(buf,32,2,0);//读块对齐
	WAVE.BitCount=ReadUnit(buf,34,2,0);//读采样点位数
	if(WAVE.BitCount!=16)return 5;//仅支持16位的音频
	DataOffset=36;
	if(Extrabytes)
	{
		temp=ReadUnit(buf,36,2,0);//读额外格式字节
		if(temp!=0x00)return 6;
		temp=ReadUnit(buf,38,4,1);//读'fact'
		if(temp!=FACT_ID)return 7;
		temp=ReadUnit(buf,42,4,0);//读Fact数据大小
		DataOffset+=10+temp;
	}
	temp=ReadUnit(buf,DataOffset,4,1);//读'data'
	DataOffset+=4;
	if(temp!=DATA_ID)return 8;
	WAVE.DataSize=ReadUnit(buf,DataOffset,4,0);//读采样数据大小
	DataOffset+=4;
	headcnt=DataOffset;
	return 0;
}
//停止播放
void Stop_play(void)
{
	plstop=1;
}
//读数据单元(最多4字节)
//Endian 1:大端模式;0:小端模式
u32 ReadUnit(u8 *buf,u8 idx,u8 Num,u8 Endian)
{
  	u8  i;
  	u32 temp=0;
  
  	for(i=0;i<Num;i++)temp|=buf[idx+i]<<(i*8);
  	if(Endian)temp=__REV(temp);
  	return temp;
}
//DMA传送完毕中断
void TX_callback(void)
{  
	if(plstop)return;
	if(buf_sw)
    {
        Audio_MAL_Play((u32)buffer2,1024);//从buffer2播放
        f_read(&fileR,buffer1,1024,&ByteRD);//填充buffer1
        buf_sw=0;
    }
    else 
    {   
       	Audio_MAL_Play((u32)buffer1,1024);//从buffer1播放
        f_read(&fileR,buffer2,1024,&ByteRD);//填充buffer2
        buf_sw=1;
    } 
}



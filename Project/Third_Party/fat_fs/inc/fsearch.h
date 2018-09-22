#ifndef __FSEARCH_H__
#define __FSEARCH_H__

#include "stm32f4xx_conf.h"
#include "ff.h"


#define FILMAXCNT  	50//�������ɨ���ļ���(�ܻ����С����)
#define PATHLEN	    50//����·�������С


//֧�ֵ��ļ�����
typedef enum { 
	T_ANY=0,//�κ��ļ�
	T_DIR=1,//�ļ���
	T_AUDIO=2,//��Ƶ�ļ�
	T_IMAGE=3,//ͼƬ�ļ�
	T_VIDO=4,//��Ƶ�ļ�
	T_BOOK=5,//�ı��ļ�
	T_GAME=6,//��Ϸ�ļ�
	T_SYS=7,//ϵͳ�ļ�	
	//�ڴ�����������
	TYPENUM=8,//�ļ�������					
}ftype;
//���Ű�ť	
#define T_PLAY 	TYPENUM	
#define T_PREV	TYPENUM+1	
#define T_NEXT	TYPENUM+2	
#define T_PAUSE	TYPENUM+3


//�����ļ������������ĺ�׺��,���Ÿ���
static const char* MYTYPE[]={
 	{"..."},
	{"..."},
 	{"MP3,WMA,WAV"},    	
 	{"JPG,BMP"},
	{"AVI,MP4"},
	{"TXT,c,h"},	        
 	{"NES"},		
 	{"BIN,sys"},									
};




extern u8   namebuf[FILMAXCNT*13];//�ܴ��50�����ļ���  
extern u32  sizebuf[FILMAXCNT];//ÿ���ļ���Ӧ���ļ���С
extern u8   tribbuf[FILMAXCNT];//ÿ���ļ���Ӧ���ļ�����
extern u8   path_curr[PATHLEN];//���ڴ�ŵ�ǰĿ¼
extern u8   readbuf[20480];//��ȡ����



u16  File_search(u8* path,ftype type);
u8   Get_type(u8* fname,u8 fattrib);
u8   Get_suff(u8* fname);
void Add_path(u8* dest);
void Cut_path(u8* src);
void My_chdir(u8* dir);
void Create_name(u8* buf,u8 num,u8* suf);


#endif






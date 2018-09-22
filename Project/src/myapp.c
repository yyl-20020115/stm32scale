#include "fsearch.h"
#include "myapp.h"
#include "touch.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ff.h"
#include <stm32f4xx.h>

//�û�������
void User_main(void)
{
	FATFS fs;                           
	DIR   filedir;
	u8    file_cnt,file_cur;
	u8 	  key,i,flag;

	f_mount(1,&fs);//����U��
	My_chdir("1:");//�л�����Ŀ¼
	while(PEN_int)
	{	
		LCD_Fill(0,90,240,190,WHITE);//���ҳ��
		LCD_String(20,90,path_curr,BLACK);//��ʾ��ǰĿ¼
		f_opendir(&filedir,(TCHAR*)path_curr);//��Ŀ¼
		file_cnt=File_search(path_curr,T_ANY);//�ܻ�������С������,���֧��50���ļ�
		for(i=0;i<file_cnt&&i<4;i++)//��ʾ�ļ��б�,һ��ҳ�������ʾ4���ļ�,����Ĳ���ʾ
		{
			LCD_String(20,110+i*20,namebuf+i*13,BLUE);//��ʾ�ļ���	
		}
		flag=1;
		file_cur=0;
		LCD_String(120,110+file_cur*20,"<<",RED);//ָ���ʼλ��
		while(flag&&PEN_int)
		{
			key=KEY_Scan();
			switch(key)
			{
				case 1://ָ������
					LCD_String(120,110+file_cur*20,"  ",RED);
					if(file_cur>0)file_cur--;
					LCD_String(120,110+file_cur*20,"<<",RED);
					break;
				case 2://ָ������
					LCD_String(120,110+file_cur*20,"  ",RED);
					if(file_cur<(file_cnt-1)&&file_cur<3)file_cur++;
					LCD_String(120,110+file_cur*20,"<<",RED);
					break;
				case 3://����Ŀ¼
					if(Get_type(namebuf+file_cur*13,tribbuf[file_cur])==T_DIR)//�ж��Ƿ�Ϊ�ļ���
					{
						Add_path(namebuf+file_cur*13);//׷��Ŀ¼
						flag=0;
					}
					break;
				case 4://������һ��
					if(path_curr[2])//�ж��Ƿ��ѵ����Ŀ¼
					{
					  	Cut_path(&path_curr[0]);//�ض�Ŀ¼
						flag=0;
					}
					break;
				default:break;
			}
		}
	}
}




#include "fsearch.h"
#include "myapp.h"
#include "touch.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ff.h"
#include <stm32f4xx.h>

//用户主程序
void User_main(void)
{
	FATFS fs;                           
	DIR   filedir;
	u8    file_cnt,file_cur;
	u8 	  key,i,flag;

	f_mount(1,&fs);//加载U盘
	My_chdir("1:");//切换到根目录
	while(PEN_int)
	{	
		LCD_Fill(0,90,240,190,WHITE);//清除页面
		LCD_String(20,90,path_curr,BLACK);//显示当前目录
		f_opendir(&filedir,(TCHAR*)path_curr);//打开目录
		file_cnt=File_search(path_curr,T_ANY);//受缓冲区大小的限制,最多支持50个文件
		for(i=0;i<file_cnt&&i<4;i++)//显示文件列表,一个页面最多显示4个文件,多出的不显示
		{
			LCD_String(20,110+i*20,namebuf+i*13,BLUE);//显示文件名	
		}
		flag=1;
		file_cur=0;
		LCD_String(120,110+file_cur*20,"<<",RED);//指针初始位置
		while(flag&&PEN_int)
		{
			key=KEY_Scan();
			switch(key)
			{
				case 1://指针上移
					LCD_String(120,110+file_cur*20,"  ",RED);
					if(file_cur>0)file_cur--;
					LCD_String(120,110+file_cur*20,"<<",RED);
					break;
				case 2://指针下移
					LCD_String(120,110+file_cur*20,"  ",RED);
					if(file_cur<(file_cnt-1)&&file_cur<3)file_cur++;
					LCD_String(120,110+file_cur*20,"<<",RED);
					break;
				case 3://进入目录
					if(Get_type(namebuf+file_cur*13,tribbuf[file_cur])==T_DIR)//判断是否为文件夹
					{
						Add_path(namebuf+file_cur*13);//追加目录
						flag=0;
					}
					break;
				case 4://返回上一层
					if(path_curr[2])//判断是否已到达根目录
					{
					  	Cut_path(&path_curr[0]);//截短目录
						flag=0;
					}
					break;
				default:break;
			}
		}
	}
}




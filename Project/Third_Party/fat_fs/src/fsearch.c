#include <string.h>
#include "fsearch.h"
#include "lcd.h"


u8   namebuf[FILMAXCNT*13];//能存放50个短文件名  
u32  sizebuf[FILMAXCNT];//每个文件对应的文件大小
u8   tribbuf[FILMAXCNT];//每个文件对应的文件属性
u8   path_curr[PATHLEN];//用于存放当前目录
//搜索path路径下类型为type的文件,将短文件名保存到BUF中(每13个字节保存一个文件名)
//返回搜索到的文件数
u16 File_search(u8* path,ftype type)
{
	u8   offset;
	u8*  suffix;
	u8*  typestr;
	u8*  nbuf=namebuf;
	u32* sbuf=sizebuf;
	u8*  tbuf=tribbuf;
	u16  fcount=0;
	DIR  direct;
	FILINFO finfo;


	if(f_opendir(&direct,(TCHAR*)path))return 0;//目录打开失败
	while(1)//读取目录下所有类型符合的文件
	{
check:
		f_readdir(&direct,&finfo);
		if(!finfo.fname[0])return fcount;//没有文件了		
		if(finfo.fattrib&AM_ARC||(finfo.fattrib&AM_DIR))
		{
			if(type==T_ANY)//任何类型的非隐藏文件
			{
				if(finfo.fname[0]!='.')//滤除两个功能文件夹
				{
					strncpy((char*)(nbuf+fcount*13),finfo.fname,13);//保存短文件名
					sbuf[fcount]=finfo.fsize;
					tbuf[fcount]=finfo.fattrib;							
					fcount++;
					if(fcount>=FILMAXCNT)return fcount;
				}
				goto check;				
			}
			offset=Get_suff((u8*)finfo.fname);
			if(!offset)goto check;//无后缀名,继续扫描
			suffix=(u8*)(finfo.fname+offset);//得到后缀名
			typestr=(u8*)MYTYPE[type];//得到类型字符串
			while(1)//循环比较直到所指定类型的后缀字符串结束
			{				
				if(*suffix==*typestr||(*suffix+32)==*typestr)//大小写不分				
				{
					suffix++;
					typestr++;					
					if(*suffix==0)//匹配成功
					{
						strncpy((char*)(nbuf+fcount*13),finfo.fname,13);//保存短文件名
						sbuf[fcount]=finfo.fsize;
						tbuf[fcount]=finfo.fattrib;						
						fcount++;
						if(fcount>=FILMAXCNT)return fcount;
						goto check;	
					}
				}
				else//若不同则进入下一个后缀的比较 
				{
					while(1)//跳到下一后缀名
					{
					 	if(*typestr==',')
						{
							suffix=(u8*)(finfo.fname+offset);//重新取文件后缀名地址
							typestr++;//类型地址加1跳过','							
							break;
						}
						else if(!*typestr)goto check;//到达末尾,不匹配,继续扫描
						typestr++;//类型地址加1直到','或文件末尾
					}
				}
			}				
		}		
	}	
}
//根据后缀名判断文件类型
u8 Get_type(u8* fname,u8 fattrib)
{
	u8   offset,i,end;
	u8*  suffix;
	u8*  typestr;

	if(fattrib&AM_DIR)return T_DIR;//文件夹
	offset=Get_suff(fname);//获取后缀名
	if(!offset)return T_ANY;//无后缀名,则返回任何文件类型
	suffix=fname+offset;//得到后缀名
	for(i=2;i<TYPENUM;i++)
	{
		typestr=(u8*)MYTYPE[i];//得到类型字符串
		end=0;
		while(!end)
		{
			if(*suffix==*typestr||(*suffix+32)==*typestr)//大小写不分				
			{
				suffix++;
				typestr++;					
				if(*suffix==0)return i;//匹配成功
			}
			else//若不同则进入下一个后缀的比较 
			{
				while(1)//跳到下一后缀名
				{
					if(*typestr==',')
					{
						suffix=fname+offset;//重新取文件后缀名地址
						typestr++;//类型地址加1跳过','							
						break;
					}
					else if(!*typestr)
					{
						end=1;//到达末尾,不匹配,继续扫描
						break;
					}
					typestr++;//类型地址加1直到','或文件末尾
				}
			}
		}
	}
	return 0;//未知文件格式	
}
//获取文件后缀名,返回后缀名的字节偏移
u8 Get_suff(u8* fname)
{
	u8 i;

	for(i=0;;i++)//跳到后缀名
	{
		if(!fname[i])return 0;//遇到无后缀名的文件
		else if(fname[i]=='.')return i+1;			
	}
}
//追加目录
void Add_path(u8* dest)
{	
	strcat((char*)path_curr,"/");
	strcat((char*)path_curr,(char*)dest);	
}
//截短目录
void Cut_path(u8* src)
{	
	while(*src)*src++;//跳至路径末尾
	while(*src!='/')//将最后一个目录置0
	{
		*src=0;
		*src--;
	}
	*src=0;
}
//改变当前路径
void My_chdir(u8* dir)
{
 	strcpy((char*)path_curr,(char*)dir);
}
//根据文件编号合成文件名
void Create_name(u8* buf,u8 num,u8* suf)
{
	buf[0]=num/100+'0';
	buf[1]=num%100/10+'0';
	buf[2]=num%10+'0';
	buf[3]='.';
	strcpy((char*)(buf+4),(char*)suf);
}


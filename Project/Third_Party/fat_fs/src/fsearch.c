#include <string.h>
#include "fsearch.h"
#include "lcd.h"


u8   namebuf[FILMAXCNT*13];//�ܴ��50�����ļ���  
u32  sizebuf[FILMAXCNT];//ÿ���ļ���Ӧ���ļ���С
u8   tribbuf[FILMAXCNT];//ÿ���ļ���Ӧ���ļ�����
u8   path_curr[PATHLEN];//���ڴ�ŵ�ǰĿ¼
//����path·��������Ϊtype���ļ�,�����ļ������浽BUF��(ÿ13���ֽڱ���һ���ļ���)
//�������������ļ���
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


	if(f_opendir(&direct,(TCHAR*)path))return 0;//Ŀ¼��ʧ��
	while(1)//��ȡĿ¼���������ͷ��ϵ��ļ�
	{
check:
		f_readdir(&direct,&finfo);
		if(!finfo.fname[0])return fcount;//û���ļ���		
		if(finfo.fattrib&AM_ARC||(finfo.fattrib&AM_DIR))
		{
			if(type==T_ANY)//�κ����͵ķ������ļ�
			{
				if(finfo.fname[0]!='.')//�˳����������ļ���
				{
					strncpy((char*)(nbuf+fcount*13),finfo.fname,13);//������ļ���
					sbuf[fcount]=finfo.fsize;
					tbuf[fcount]=finfo.fattrib;							
					fcount++;
					if(fcount>=FILMAXCNT)return fcount;
				}
				goto check;				
			}
			offset=Get_suff((u8*)finfo.fname);
			if(!offset)goto check;//�޺�׺��,����ɨ��
			suffix=(u8*)(finfo.fname+offset);//�õ���׺��
			typestr=(u8*)MYTYPE[type];//�õ������ַ���
			while(1)//ѭ���Ƚ�ֱ����ָ�����͵ĺ�׺�ַ�������
			{				
				if(*suffix==*typestr||(*suffix+32)==*typestr)//��Сд����				
				{
					suffix++;
					typestr++;					
					if(*suffix==0)//ƥ��ɹ�
					{
						strncpy((char*)(nbuf+fcount*13),finfo.fname,13);//������ļ���
						sbuf[fcount]=finfo.fsize;
						tbuf[fcount]=finfo.fattrib;						
						fcount++;
						if(fcount>=FILMAXCNT)return fcount;
						goto check;	
					}
				}
				else//����ͬ�������һ����׺�ıȽ� 
				{
					while(1)//������һ��׺��
					{
					 	if(*typestr==',')
						{
							suffix=(u8*)(finfo.fname+offset);//����ȡ�ļ���׺����ַ
							typestr++;//���͵�ַ��1����','							
							break;
						}
						else if(!*typestr)goto check;//����ĩβ,��ƥ��,����ɨ��
						typestr++;//���͵�ַ��1ֱ��','���ļ�ĩβ
					}
				}
			}				
		}		
	}	
}
//���ݺ�׺���ж��ļ�����
u8 Get_type(u8* fname,u8 fattrib)
{
	u8   offset,i,end;
	u8*  suffix;
	u8*  typestr;

	if(fattrib&AM_DIR)return T_DIR;//�ļ���
	offset=Get_suff(fname);//��ȡ��׺��
	if(!offset)return T_ANY;//�޺�׺��,�򷵻��κ��ļ�����
	suffix=fname+offset;//�õ���׺��
	for(i=2;i<TYPENUM;i++)
	{
		typestr=(u8*)MYTYPE[i];//�õ������ַ���
		end=0;
		while(!end)
		{
			if(*suffix==*typestr||(*suffix+32)==*typestr)//��Сд����				
			{
				suffix++;
				typestr++;					
				if(*suffix==0)return i;//ƥ��ɹ�
			}
			else//����ͬ�������һ����׺�ıȽ� 
			{
				while(1)//������һ��׺��
				{
					if(*typestr==',')
					{
						suffix=fname+offset;//����ȡ�ļ���׺����ַ
						typestr++;//���͵�ַ��1����','							
						break;
					}
					else if(!*typestr)
					{
						end=1;//����ĩβ,��ƥ��,����ɨ��
						break;
					}
					typestr++;//���͵�ַ��1ֱ��','���ļ�ĩβ
				}
			}
		}
	}
	return 0;//δ֪�ļ���ʽ	
}
//��ȡ�ļ���׺��,���غ�׺�����ֽ�ƫ��
u8 Get_suff(u8* fname)
{
	u8 i;

	for(i=0;;i++)//������׺��
	{
		if(!fname[i])return 0;//�����޺�׺�����ļ�
		else if(fname[i]=='.')return i+1;			
	}
}
//׷��Ŀ¼
void Add_path(u8* dest)
{	
	strcat((char*)path_curr,"/");
	strcat((char*)path_curr,(char*)dest);	
}
//�ض�Ŀ¼
void Cut_path(u8* src)
{	
	while(*src)*src++;//����·��ĩβ
	while(*src!='/')//�����һ��Ŀ¼��0
	{
		*src=0;
		*src--;
	}
	*src=0;
}
//�ı䵱ǰ·��
void My_chdir(u8* dir)
{
 	strcpy((char*)path_curr,(char*)dir);
}
//�����ļ���źϳ��ļ���
void Create_name(u8* buf,u8 num,u8* suf)
{
	buf[0]=num/100+'0';
	buf[1]=num%100/10+'0';
	buf[2]=num%10+'0';
	buf[3]='.';
	strcpy((char*)(buf+4),(char*)suf);
}


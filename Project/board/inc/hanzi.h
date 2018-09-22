#ifndef __HANZI_H
#define __HANZI_H


#define FON_addr  	2628
#define FON_size   	1467


void UpdateFont(u8 *path);
u32  Get_FontAdr(u8* ch);
void Show_Hanzi(int x0,u16 y0,u8* word,u16 color);
void Show_Fonts(u16 x0,u16 y0,u8* Fonts,u16 color);


#endif


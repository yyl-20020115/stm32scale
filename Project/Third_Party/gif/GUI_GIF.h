#ifndef GUI_GIF_H
#define GUI_GIF_H

#include <stm32f4xx.h>

typedef struct {
  int              NumEntries; 
  const u32*       pPalEntries; 
} LCD_PHYSPALETTE; 
typedef struct {
  int              NumEntries; 
  char             HasTrans;         
  const u32        *pPalEntries; 
} LCD_LOGPALETTE; 
typedef struct {
  int xPos;
  int yPos;
  int xSize;
  int ySize;
  int Delay;
} GUI_GIF_IMAGE_INFO;
typedef struct {
  int xSize;
  int ySize;
  int NumImages;
} GUI_GIF_INFO;
typedef struct {
  const u8 * pSrc; 
  int RemBytes;    
} SOURCE;
typedef struct {
  int XPos;
  int YPos;
  int XSize;
  int YSize;
  int Flags;
  int NumColors;
} IMAGE_DESCRIPTOR;
typedef struct {
  u8    aBuffer[258];                     
  short aCode  [(1 << 12)]; 
  u8    aPrefix[(1 << 12)];
  u8    aDecompBuffer[3000];              
  u8 *  sp;                              
  int   CurBit;
  int   LastBit;
  int   GetDone;
  int   LastByte;
  int   ReturnClear;
  int   CodeSize;
  int   SetCodeSize;
  int   MaxCode;
  int   MaxCodeSize;
  int   ClearCode;
  int   EndCode;
  int   FirstCode;
  int   OldCode;
} LZW_CONTEXT;

#define GIF_INTRO_TERMINATOR ';'
#define GIF_INTRO_EXTENSION  '!'
#define GIF_INTRO_IMAGE      ','
#define GIF_COMMENT     0xFE
#define GIF_APPLICATION 0xFF
#define GIF_PLAINTEXT   0x01
#define GIF_GRAPHICCTL  0xF9
#define MAX_NUM_LWZ_BITS 12

void ShowGif(const void * Pgif, u32 NumBytes , int x0, int y0);
u16* LCD_GetpPalConvTable(const LCD_LOGPALETTE*  pLogPal);

#endif

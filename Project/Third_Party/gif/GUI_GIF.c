#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"
#include "GUI_GIF.h"
#include "delay.h"

u16 BACKCOLOR=BLUE;
LZW_CONTEXT _LZWContext;
u32   _aColorTable[256];
SOURCE      _Source;
u16 LCD_ConvTab[256];
static u8   _aBuffer[256];
static const int _aInterlaceOffset[] = {  8, 8, 4, 2 };
static const int _aInterlaceYPos[]   = {  0, 4, 2, 1 };
static const int _aMaskTbl[16] = {
  0x0000, 0x0001, 0x0003, 0x0007,
  0x000f, 0x001f, 0x003f, 0x007f,
  0x00ff, 0x01ff, 0x03ff, 0x07ff,
  0x0fff, 0x1fff, 0x3fff, 0x7fff,
};

static u8 _ReadU8(void)
{
  	u8 Value;
  	_Source.RemBytes -= 1;
  	if (_Source.RemBytes < 0)return 0;
  	Value = *(_Source.pSrc++);
  	return Value;
}

static u16 _ReadU16(void)
{
  	u16 Value;
  	u8 Byte0, Byte1;
  	Byte0 = _ReadU8();
  	Byte1 = _ReadU8();
  	Value = (Byte1 << 8) | Byte0;
  	return Value;
}

static void _ReadBytes(u8 * pBuffer, int Len)
{
  	if (_Source.RemBytes < 0)return;
  	if (_Source.RemBytes < Len)
  	{
    	Len = _Source.RemBytes;
    	return;
  	}
  	_Source.RemBytes -= Len;
  	memcpy(pBuffer, _Source.pSrc, Len);
  	_Source.pSrc += Len;
}

static void _SkipBytes(int Len)
{
  	if (_Source.RemBytes < 0)return;
  	if (_Source.RemBytes < Len)
	{
	    Len = _Source.RemBytes;
	    return;
  	}
  	_Source.RemBytes -= Len;
  	_Source.pSrc += Len;
}

unsigned Color_M565(u32 Color)
{
  	int r,g,b;
  	r = (Color>> 3) & ((1 << 5) -1);
  	g = (Color>> 10) & ((1 << 6) -1);
  	b = (Color>> 19) & ((1 << 5) -1);
  	return b + (g << 5) + (r << 11);
}

u16* LCD_GetpPalConvTable(const LCD_LOGPALETTE*  pLogPal)
{
    int i;
    int NumEntries = pLogPal->NumEntries;
    for (i=0; i< NumEntries; i++)
	{
      	LCD_ConvTab[i] = Color_M565(*(&pLogPal->pPalEntries[0]+i));
    }
  	return &LCD_ConvTab[0];
}

static void _InitLZW(int InputCodeSize)
{
  	memset((u8 *)&_LZWContext, 0, sizeof(LZW_CONTEXT));
  	_LZWContext.SetCodeSize  = InputCodeSize;
  	_LZWContext.CodeSize     = InputCodeSize + 1;
  	_LZWContext.ClearCode    = (1 << InputCodeSize);
  	_LZWContext.EndCode      = (1 << InputCodeSize) + 1;
  	_LZWContext.MaxCode      = (1 << InputCodeSize) + 2;
  	_LZWContext.MaxCodeSize  = (1 << InputCodeSize) << 1;
  	_LZWContext.ReturnClear  = 1;
  	_LZWContext.LastByte     = 2;
  	_LZWContext.sp           = _LZWContext.aDecompBuffer;
}

static int _GetDataBlock(u8 * pBuffer)
{
  	u8 Count;
  	Count = _ReadU8(); 
  	if (Count)
	{
	    if (pBuffer)_ReadBytes(pBuffer, Count);
	    else _Source.pSrc += Count;
  	}
  	return((int)Count);
}

static int _GetNextCode(void)
{
  	int i, j, End;
  	long Result;
  	if (_LZWContext.ReturnClear)
	{
    	_LZWContext.ReturnClear = 0;
    	return _LZWContext.ClearCode;
  	}
  	End = _LZWContext.CurBit + _LZWContext.CodeSize;
  	if (End >= _LZWContext.LastBit)
	{
	    int Count;
	    if (_LZWContext.GetDone)return -1;
	    _LZWContext.aBuffer[0] = _LZWContext.aBuffer[_LZWContext.LastByte - 2];
	    _LZWContext.aBuffer[1] = _LZWContext.aBuffer[_LZWContext.LastByte - 1];
	    if ((Count = _GetDataBlock(&_LZWContext.aBuffer[2])) == 0)_LZWContext.GetDone = 1;
	    if (Count < 0)return -1;
	    _LZWContext.LastByte = 2 + Count;
	    _LZWContext.CurBit   = (_LZWContext.CurBit - _LZWContext.LastBit) + 16;
	    _LZWContext.LastBit  = (2 + Count) * 8 ;
	    End                  = _LZWContext.CurBit + _LZWContext.CodeSize;
  	}
  	j = End >> 3;
  	i = _LZWContext.CurBit >> 3;
  	if (i == j)Result = (long)_LZWContext.aBuffer[i];
 	else if (i + 1 == j)Result = (long)_LZWContext.aBuffer[i] | ((long)_LZWContext.aBuffer[i + 1] << 8);
 	else Result = (long)_LZWContext.aBuffer[i] | ((long)_LZWContext.aBuffer[i + 1] << 8) | ((long)_LZWContext.aBuffer[i + 2] << 16);
  	Result = (Result >> (_LZWContext.CurBit & 0x7)) & _aMaskTbl[_LZWContext.CodeSize];
  	_LZWContext.CurBit += _LZWContext.CodeSize;
  	return (int)Result;
}


static int _GetNextByte(void)
{
  	int i, Code, Incode;
  	while ((Code = _GetNextCode()) >= 0)
	{
    	if (Code == _LZWContext.ClearCode)
		{
      		if (_LZWContext.ClearCode >= (1 << MAX_NUM_LWZ_BITS))return -1; /* Error */
      		memset((u8 *)_LZWContext.aCode, 0, sizeof(_LZWContext.aCode));
      		for (i = 0; i < _LZWContext.ClearCode; ++i)_LZWContext.aPrefix[i] = i; 
      		_LZWContext.CodeSize    = _LZWContext.SetCodeSize + 1;
	      	_LZWContext.MaxCodeSize = _LZWContext.ClearCode << 1;
	      	_LZWContext.MaxCode     = _LZWContext.ClearCode + 2;
	      	_LZWContext.sp          = _LZWContext.aDecompBuffer;
      		do{_LZWContext.FirstCode = _GetNextCode();}while (_LZWContext.FirstCode == _LZWContext.ClearCode);
      		_LZWContext.OldCode = _LZWContext.FirstCode;
      		return _LZWContext.FirstCode;
    	}
    	if (Code == _LZWContext.EndCode)return -2;
    	Incode = Code;
    	if (Code >= _LZWContext.MaxCode)
		{
	      	*(_LZWContext.sp)++ = _LZWContext.FirstCode;
	      	Code = _LZWContext.OldCode;
    	}
    	while (Code >= _LZWContext.ClearCode)
		{
	      	*(_LZWContext.sp)++ = _LZWContext.aPrefix[Code];
	      	if (Code == _LZWContext.aCode[Code])return Code;
	      	if ((_LZWContext.sp - _LZWContext.aDecompBuffer) >= sizeof(_LZWContext.aDecompBuffer))return Code;
      		Code = _LZWContext.aCode[Code];
    	}
    	*(_LZWContext.sp)++ = _LZWContext.FirstCode = _LZWContext.aPrefix[Code];
    	if ((Code = _LZWContext.MaxCode) < (1 << MAX_NUM_LWZ_BITS))
		{
	      	_LZWContext.aCode  [Code] = _LZWContext.OldCode;
	      	_LZWContext.aPrefix[Code] = _LZWContext.FirstCode;
	      	++_LZWContext.MaxCode;
	      	if ((_LZWContext.MaxCode >= _LZWContext.MaxCodeSize) && (_LZWContext.MaxCodeSize < (1 << MAX_NUM_LWZ_BITS)))
			{
		        _LZWContext.MaxCodeSize <<= 1;
		        ++_LZWContext.CodeSize;
	      	}
    	}
   	 	_LZWContext.OldCode = Incode;
    	if (_LZWContext.sp > _LZWContext.aDecompBuffer)return *--(_LZWContext.sp);
  	}
  	return Code;
}


static int _ReadExtension(int * pTransIndex, GUI_GIF_IMAGE_INFO * pInfo, u8 * pDisposal)
{
  	u8 Label;
  	Label = _ReadU8();
  	switch (Label)
	{
	  	case GIF_PLAINTEXT:
	  	case GIF_APPLICATION:
	  	case GIF_COMMENT:
		    while (_GetDataBlock(_aBuffer) > 0);
		    return 0;
		  	case GIF_GRAPHICCTL:
		    if (_GetDataBlock(_aBuffer) != 4)return 1;
		    if (pInfo)pInfo->Delay    = (_aBuffer[2] << 8) | _aBuffer[1];
		    if (pDisposal)*pDisposal = (_aBuffer[0] >> 2) & 0x7;
		    if (pTransIndex&&(_aBuffer[0] & 0x1) != 0)*pTransIndex = _aBuffer[3];
		    if (_ReadU8() != 0)return 1;
		    return 0;
  	}
  	return 1; 
}

static int _DispGIFImage(IMAGE_DESCRIPTOR * pDescriptor, int x0, int y0, int Transparency, int Disposal)
{
  	int Codesize, Index, OldIndex, XPos, YPos, YCnt, Pass, Interlace, XEnd;
  	int Width, Height, NumColors, Cnt, BkColorIndex, ColorIndex;
  	LCD_LOGPALETTE LogPalette;
  	const u16 * pTrans;
  	Width     = pDescriptor->XSize;
  	Height    = pDescriptor->YSize;
  	NumColors = pDescriptor->NumColors;
  	XEnd      = Width + x0 - 1;
  	BkColorIndex = BACKCOLOR;//LCD_GetBkColorIndex();
  	LogPalette.NumEntries  = NumColors;
  	LogPalette.HasTrans    = 0;
  	LogPalette.pPalEntries = _aColorTable;
  	pTrans = (const u16*)LCD_GetpPalConvTable(&LogPalette);
  	Codesize  = _ReadU8();                 
  	_InitLZW(Codesize);                   
  	Interlace = pDescriptor->Flags & 0x40; 
  	for (YCnt = 0, YPos = y0, Pass = 0; YCnt < Height; YCnt++)
	{
    	Cnt      = 0;
    	OldIndex = -1;    
    	for (XPos = x0; XPos <= XEnd; XPos++)
		{
	      	if (_LZWContext.sp > _LZWContext.aDecompBuffer)Index = *--(_LZWContext.sp);
	      	else Index = _GetNextByte();
	      	if (Index == -2)return 0;
	      	if ((Index < 0) || (Index >= NumColors))return 1;
	      	if ((Index == OldIndex) && (XPos <= XEnd))Cnt++;
	      	else
		  	{
	        	if (Cnt)
				{
		          	if (OldIndex != Transparency)
					{
			            GUI_Line(XPos - Cnt - 1, YPos, XPos - 1, YPos, *(pTrans + OldIndex));
	          		}
					else if (Disposal == 2)
					{
			            GUI_Line(XPos - Cnt - 1, YPos, XPos - 1, YPos, BkColorIndex);
	          		}
	          		Cnt = 0;
	        	}
				else if (OldIndex >= 0)
				{
		            if (OldIndex != Transparency)GUI_SetPoint(XPos - 1, YPos, *(pTrans + OldIndex));
					else if (Disposal == 2)GUI_SetPoint(XPos - 1, YPos, BkColorIndex);
	          	}
	      	}
	      	OldIndex = Index;
	    }
	    if ((OldIndex != Transparency) || (Disposal == 2))
		{
	      	if (OldIndex != Transparency)ColorIndex = *(pTrans + OldIndex); 
	 		else ColorIndex = BkColorIndex;
	      	if (Cnt)
			{
		        GUI_Line(XPos - Cnt - 1, YPos, XPos - 1, YPos, ColorIndex);
	      	}
			else GUI_SetPoint(XEnd, YPos, ColorIndex);
	    }
	    if (Interlace)
		{
	      	YPos += _aInterlaceOffset[Pass];
	      	if ((YPos - y0) >= Height)
			{
		        ++Pass;
		        YPos = _aInterlaceYPos[Pass] + y0;
	      	}
	    }
		else YPos++;
  	}
  	return 0;
}


static int _ReadColorMap(int NumColors)
{
  	int i;
  	for (i = 0; i < NumColors; i++)
	{
	    u8 r, g, b;
	    r = _ReadU8 ();
	    g = _ReadU8 ();
	    b = _ReadU8 ();
	    if (_Source.RemBytes < 0)return 1;
    	_aColorTable[i] = r | ((u16)g << 8) | ((u32)b << 16);
  	}
  	return 0;
}


static int _InitGIFDecoding(const u8 * pData, u32 NumBytes)
{
  	u8 acVersion[7] = {0};
  	_Source.pSrc     = pData;
  	_Source.RemBytes = NumBytes;
  	_ReadBytes(acVersion, 6); if (!_Source.RemBytes) { return 0; }
  	if ( (acVersion[0] != 'G') || 
         (acVersion[1] != 'I') || 
         (acVersion[2] != 'F') || 
         (acVersion[3] != '8') ||
         ((acVersion[4] != '7') && (acVersion[4] != '9')) ||
         (acVersion[5] != 'a'))return 1;
  	return 0;
}


static int _GetImageDimension(int * pxSize, int * pySize) {
  	int XSize, YSize;
  	XSize = _ReadU16();
  	YSize = _ReadU16();
  	if ((XSize > 2000) || (YSize > 2000))return 1;
  	if (pxSize)*pxSize = XSize;
  	if (pySize)*pySize = YSize;
  	return 0;
}


static int _GetGlobalColorTable(int * pNumColors)
{
  	u8 Flags;
  	int NumColors;
  	Flags = _ReadU8 ();
  	_ReadU8 ();
  	_ReadU8 ();
  	if (_Source.RemBytes < 0)return 1; 
  	NumColors = 2 << (Flags & 0x7);
  	if ((Flags & 0x80)&&_ReadColorMap(NumColors))return 1;
  	if (pNumColors)*pNumColors = NumColors;
  	return 0;
}


static int _GetGIFInfo(const u8 * pData, u32 NumBytes, GUI_GIF_INFO * pInfo)
{
  	u8 Flags, Introducer;
  	int NumColors, ImageCnt;
  	if (_InitGIFDecoding(pData, NumBytes)||_GetImageDimension(&pInfo->xSize, &pInfo->ySize)||_GetGlobalColorTable(&NumColors))return 1; /* Error */
  	ImageCnt = 0;
  	do
	{
	    Introducer = _ReadU8();
	    switch (Introducer)
		{
		    case GIF_INTRO_IMAGE:
	      		_SkipBytes(8);                /* Skip the first 8 bytes of the image descriptor */
		      	Flags = _ReadU8();            /* Only 'Flags' are intresting */
		      	if (Flags & 0x80)_SkipBytes(NumColors * 3);
		      	_SkipBytes(1);                /* Skip codesize */
		      	while (_GetDataBlock(0) > 0); /* Skip data blocks */
		      	ImageCnt++;
		      	break;
    		case GIF_INTRO_TERMINATOR:break;
    		case GIF_INTRO_EXTENSION:if (_ReadExtension(NULL, NULL, NULL))return 1;break;
    		default:return 1;
    	}
  	}while (Introducer!= GIF_INTRO_TERMINATOR);
  	pInfo->NumImages = ImageCnt;
  	return 0;
}


static int _GetImageInfo(const u8 * pData, u32 NumBytes, GUI_GIF_IMAGE_INFO * pInfo, int Index)
{
  	u8 Flags, Introducer;
  	int NumColors, ImageCnt;
  	if (_InitGIFDecoding(pData, NumBytes)||_GetImageDimension(NULL, NULL)||_GetGlobalColorTable(&NumColors))return 1; /* Error */
  	ImageCnt = 0;
  	do
	{
	    Introducer = _ReadU8();
	    switch (Introducer)
		{
		    case GIF_INTRO_IMAGE:
	      		if (Index == ImageCnt)
				{
	        		pInfo->xPos  = _ReadU16();
	        		pInfo->xPos  = _ReadU16();
			        pInfo->xSize = _ReadU16();
			        pInfo->ySize = _ReadU16();
	        		return 0;
	      		}
	      		_SkipBytes(8);             
	      		Flags = _ReadU8();          
	      		if (Flags & 0x80)_SkipBytes(NumColors * 3);
	      		_SkipBytes(1);             
	      		while (_GetDataBlock(0) > 0); 
	      		ImageCnt++;
	      		break;
	    	case GIF_INTRO_TERMINATOR:break;
    		case GIF_INTRO_EXTENSION:
      			if (_ReadExtension(NULL, (Index == ImageCnt) ? pInfo : NULL, NULL))return 1;break;
    		default:return 1;
    	}
  	} while (Introducer != GIF_INTRO_TERMINATOR);
  	return 0;
}

static void _ClearUnusedPixels(int x0, int y0, IMAGE_DESCRIPTOR * pDescriptor, GUI_GIF_IMAGE_INFO * pInfo)
{
	if(pDescriptor->YPos>pInfo->yPos)GUI_Rectangle(x0+pInfo->xPos,y0+pInfo->yPos,x0+pInfo->xPos+pInfo->xSize-1,y0+pDescriptor->YPos-1,BACKCOLOR);
	if(pDescriptor->XPos>pInfo->xPos)GUI_Rectangle(x0+pInfo->xPos,y0+pInfo->yPos,x0+pDescriptor->XPos-1,y0+pInfo->yPos+pInfo->ySize-1,BACKCOLOR);
	if((pDescriptor->YPos+pDescriptor->YSize)<(pInfo->yPos+pInfo->ySize))GUI_Rectangle(x0+pInfo->xPos,y0+pDescriptor->YPos+pDescriptor->YSize-1,x0+pInfo->xPos+pInfo->xSize-1,y0+pInfo->yPos+pInfo->ySize-1,BACKCOLOR);
	if((pDescriptor->XPos+pDescriptor->XSize)<(pInfo->xPos+pInfo->xSize))GUI_Rectangle(x0+pDescriptor->XPos+pDescriptor->XSize-1,y0+pInfo->yPos,x0+pInfo->xPos+pInfo->xSize-1,y0+pInfo->yPos+pInfo->ySize-1,BACKCOLOR);
}


static int _DrawGIFImage(const u8 * pData, u32 NumBytes, int x0, int y0, int Index)
{
  	u8 Disposal;
  	int XSize, YSize, TransIndex, ImageCnt;
  	IMAGE_DESCRIPTOR Descriptor = {0};
  	GUI_GIF_IMAGE_INFO Info = {0};
  	u8 Introducer;
  	ImageCnt   =  0;
  	TransIndex = -1;
  	if (_InitGIFDecoding(pData, NumBytes)||_GetImageDimension(&XSize, &YSize)||_GetGlobalColorTable(&Descriptor.NumColors))return 1; /* Error */
  	do{
	    Introducer = _ReadU8();
	    switch (Introducer)
		{
	    	case GIF_INTRO_IMAGE:
		      	Descriptor.XPos  = _ReadU16();
		      	Descriptor.YPos  = _ReadU16();
		      	Descriptor.XSize = _ReadU16();
		      	Descriptor.YSize = _ReadU16();
		      	Descriptor.Flags = _ReadU8 ();
		      	if (ImageCnt == Index - 1)
				{
			        Info.xPos  = Descriptor.XPos;
			        Info.yPos  = Descriptor.YPos;
			        Info.xSize = Descriptor.XSize;
			        Info.ySize = Descriptor.YSize;
		      	}
		      	if (Index == ImageCnt)
			  	{
			        if (Disposal == 2)_ClearUnusedPixels(x0, y0, &Descriptor, &Info);
			        if (_DispGIFImage(&Descriptor, x0 + Descriptor.XPos, y0 + Descriptor.YPos, TransIndex, Disposal))return 1;
			        return 0;
		      	}
				else
				{
			        _ReadU8();                    /* Skip codesize */
			        while (_GetDataBlock(0) > 0); /* Skip data blocks */
		      	}
      			ImageCnt++;
      			break;
    		case GIF_INTRO_TERMINATOR:break;
    		case GIF_INTRO_EXTENSION:
	      		if (_ReadExtension(&TransIndex, (Index == ImageCnt) ? &Info : NULL, (Index == ImageCnt) ? &Disposal : NULL))return 1;
	      		break;
    		default:return 1;
    	}
  	} while (Introducer != GIF_INTRO_TERMINATOR);
  	return 0;
}


void ShowGif(const void * Pgif, u32 NumBytes , int x0, int y0)
{
    static u16 i = 0;
    GUI_GIF_INFO InfoGif1;
    GUI_GIF_IMAGE_INFO InfoGif2;
	_GetGIFInfo(Pgif, NumBytes, &InfoGif1);
    for(i=0;i<InfoGif1.NumImages;i++)
    {
		_GetImageInfo(Pgif, NumBytes, &InfoGif2, i);
		_DrawGIFImage(Pgif, NumBytes, x0, y0, i);
        delay_ms(10); 
    }   
}	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 

#include "diskio.h"
#include "sdio_sd.h"
#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"

extern SD_CardInfo SDCardInfo;
extern USB_OTG_CORE_HANDLE    USB_OTG_Core;
extern USBH_HOST              USB_Host;
//初始化磁盘
DSTATUS disk_initialize (BYTE drv)
{
 	switch(drv) 
 	{
		case 0://SD卡	  
    		if(SD_Init()==SD_OK)return 0;
    		else return STA_NOINIT;	
		case 1://U盘
	  		if(HCD_IsDeviceConnected(&USB_OTG_Core))return 0;		  
			else return STA_NOINIT;		  
		case 2:
			return STA_NOINIT;
  	}
	return STA_NOINIT;
}
//返回磁盘状态
DSTATUS disk_status (BYTE drv)		
{
	switch(drv) 
	{
	  	case 0:		
	    	return 0;
	  	case 1:
	    	return 0;
	  	case 2:
	    	return 0;
	  	default:
        	break;
	}
	return STA_NOINIT;
}
//读扇区
DRESULT disk_read (BYTE drv,BYTE *buff,DWORD sector,BYTE count)
{
  	u8 Status;

	switch(drv) 
	{
	  	case 0://SD卡
		  	if(count==1)Status=SD_ReadBlock(buff,sector<<9,SDCardInfo.CardBlockSize);                                                              
		  	else Status=SD_ReadMultiBlocks(buff,sector<<9,SDCardInfo.CardBlockSize,count);				
  			//Status = SD_WaitReadOperation();
  			//while(SD_GetStatus() != SD_TRANSFER_OK);
		  	if(Status==SD_OK)return RES_OK;
		  	else return RES_ERROR;
	  	case 1://U盘
		  	if(HCD_IsDeviceConnected(&USB_OTG_Core))
		  	{  		    
		    	do
		    	{
		      		Status=USBH_MSC_Read10(&USB_OTG_Core,buff,sector,512*count);
		      		USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
		      		if(!HCD_IsDeviceConnected(&USB_OTG_Core))return RES_ERROR;   
		    	}
		    	while(Status==USBH_MSC_BUSY);
		  	}		  
		  	if(Status==USBH_MSC_OK)return RES_OK;
		  	return RES_ERROR;
	  	case 2:
	    	return RES_PARERR;
	  	default:
        	break;
	}
	return RES_ERROR; 
}
//写扇区
#if _READONLY==0
DRESULT disk_write(BYTE drv,const BYTE *buff,DWORD sector,BYTE count)
{
  	u8 Status;

	switch(drv) 
	{
	  	case 0://SD卡		
		    if(count==1)Status=SD_WriteBlock((uint8_t*)buff,sector<<9,SDCardInfo.CardBlockSize);                                                
		    else 
			{
				Status=SD_WriteMultiBlocks((uint8_t*)buff,sector<<9,SDCardInfo.CardBlockSize,count);	  
		    	Status=SD_WaitWriteOperation();
			}
			while(SD_GetStatus()!=SD_TRANSFER_OK);
		    if(Status==SD_OK)return RES_OK;
		    else return RES_ERROR;
	  	case 1://U盘
		  	if(HCD_IsDeviceConnected(&USB_OTG_Core))
		  	{  
		    	do
		    	{
		      		Status=USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512*count);
		      		USBH_MSC_HandleBOTXfer(&USB_OTG_Core,&USB_Host);		      
		      		if(!HCD_IsDeviceConnected(&USB_OTG_Core))return RES_ERROR;
		    	}		    
		    	while(Status==USBH_MSC_BUSY);		    
		  	}		  
		  	if(Status==USBH_MSC_OK)return RES_OK;
		    else return RES_ERROR;
	  	case 2:
	    	return RES_PARERR;
	  	default:
        	break;
	} 
	return RES_ERROR;   	                                              
}
#endif /* _READONLY */

//其它函数
DRESULT disk_ioctl(BYTE drv,BYTE ctrl,void *buff)
{
	if(drv==0)//SD卡
	{
		switch(ctrl) 
		{
		  	case CTRL_SYNC:	      
				return RES_OK;
		  	case GET_SECTOR_COUNT: 
		    	*(DWORD*)buff=SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
		    	return RES_OK;
		  	case GET_BLOCK_SIZE:
		    	*(WORD*)buff = SDCardInfo.CardBlockSize;
		    	return RES_OK;	
		  	case CTRL_POWER:
				break;
		  	case CTRL_LOCK:
				break;
		  	case CTRL_EJECT:
				break;
		  	case MMC_GET_TYPE:
				break;
		  	case MMC_GET_CSD:
				break;
		  	case MMC_GET_CID:
				break;
		  	case MMC_GET_OCR:
				break;
		  	case MMC_GET_SDSTAT:
				break;	
		}
	}
	else if(drv==1)//U盘
	{
	  	switch(ctrl) 
		{
	  		case CTRL_SYNC:	    
	    		return RES_OK;    
	  		case GET_SECTOR_COUNT:	    
	    		*(DWORD*)buff=(DWORD)USBH_MSC_Param.MSCapacity;
	    		return RES_OK;	    
	  		case GET_SECTOR_SIZE:
	    		*(WORD*)buff=512;
	    		return RES_OK;	    
	  		case GET_BLOCK_SIZE:	    
	    		*(DWORD*)buff=512;	    	    
	  		default:
	    		return RES_PARERR;
	  	}	
	}
	return RES_PARERR;   
}

/* 得到文件Calendar格式的建立日期,是DWORD get_fattime (void) 逆变换 */							
/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{  
    return 0;
}



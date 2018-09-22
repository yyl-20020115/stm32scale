#include "stm32f4xx_it.h"
#include "usbh_usr.h"
#include "led.h"
#include "ff.h"
#include "myapp.h"


static u8 AppState;

USBH_Usr_cb_TypeDef USR_Callbacks=
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};

void USBH_USR_DeviceAttached(void)//U盘插入
{
	LEDOn(LED1);
}

void USBH_USR_DeviceDisconnected (void)//U盘移除
{
  	LEDOff(LED1);
}

void USBH_USR_EnumerationDone(void)
{
  	USB_OTG_BSP_mDelay(500);  
  	USBH_USR_MSC_Application();
} 

int USBH_USR_MSC_Application(void)
{
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT://初始化文件系统
     		AppState=USH_USR_AUDIO;
      		break;
    	case USH_USR_AUDIO://音乐播放
			User_main();//用户主程序
     		AppState=USH_USR_FS_INIT;
      		break;
    	default:break;
  	}
  	return(0);
}

void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
}

USBH_USR_Status USBH_USR_UserInput(void)
{
  	return USBH_USR_RESP_OK;
}
void USBH_USR_DeviceNotSupported(void){}
void USBH_USR_OverCurrentDetected (void){}
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,USBH_InterfaceDesc_TypeDef *itfDesc,USBH_EpDesc_TypeDef *epDesc){}
void USBH_USR_Manufacturer_String(void *ManufacturerString){}
void USBH_USR_Product_String(void *ProductString){}
void USBH_USR_SerialNum_String(void *SerialNumString){}
void USBH_USR_ResetDevice(void){}
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed){}
void USBH_USR_Device_DescAvailable(void *DeviceDesc){}
void USBH_USR_DeviceAddressAssigned(void){}
void USBH_USR_UnrecoveredError (void){}
void USBH_USR_Init(void){}



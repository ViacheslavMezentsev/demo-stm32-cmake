#include "usbd_hid.h"
#include "usb_device.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}subKeyBoard;

subKeyBoard keyBoardHIDsub = {0,0,0,0,0,0,0,0};

void KeyBoardPrint(char *data,uint16_t length)
{
	for(uint16_t count=0;count<length;count++)
	{
		if(data[count]>=0x41 && data[count]<=0x5A)
		{
			keyBoardHIDsub.MODIFIER=0x02;
			keyBoardHIDsub.KEYCODE1=data[count]-0x3D;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
			HAL_Delay(15);
			keyBoardHIDsub.MODIFIER=0x00;
			keyBoardHIDsub.KEYCODE1=0x00;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
		}
		else if(data[count]>=0x61 && data[count]<=0x7A)
		{
			keyBoardHIDsub.KEYCODE1=data[count]-0x5D;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
			HAL_Delay(15);
			keyBoardHIDsub.KEYCODE1=0x00;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
		}
		else if(data[count]==0x20)
		{
			keyBoardHIDsub.KEYCODE1=0x2C;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
			HAL_Delay(15);
			keyBoardHIDsub.KEYCODE1=0x00;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
		}
		else if(data[count]==0x0A)
		{
			keyBoardHIDsub.KEYCODE1=0x28;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
			HAL_Delay(15);
			keyBoardHIDsub.KEYCODE1=0x00;
			USBD_HID_SendReport(&hUsbDeviceFS,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
		}
		HAL_Delay(25);
	}
}

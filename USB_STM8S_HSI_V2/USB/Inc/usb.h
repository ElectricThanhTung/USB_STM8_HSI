
#ifndef __USB_H__
#define __USB_H__

#define TimerTickStep           20

void USB_Disconnect();
void USB_Init();
void USB_SendData(unsigned char *buffer, unsigned char length);

#endif

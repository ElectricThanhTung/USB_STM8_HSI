
#ifndef __USB_H__
#define __USB_H__

#define TimerTickStep           20

void USB_Disconnect();
void USB_Init();
unsigned char USB_SendPacket(unsigned char *buffer, unsigned char length);
void USB_WaitBusy();

#endif

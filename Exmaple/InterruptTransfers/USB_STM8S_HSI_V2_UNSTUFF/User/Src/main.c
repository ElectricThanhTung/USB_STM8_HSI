
#include "main.h"
#include "usb.h"

//static void GPIO_Init(){
//  PD_DDR = 0xFF;
//  PD_CR1 = 0xFF;
//  PD_CR2 = 0xFF;
//  PD_ODR = 0x00;
//}

void USB_Received(unsigned char endpoint, unsigned char *buffer, unsigned char length){
  
}

void main(){
  unsigned char data[8] = "Hello 00";
  unsigned char count = 0;
  CLK_CKDIVR = 0;
//  GPIO_Init();
  USB_Disconnect();
  delay_ms(50);
  USB_Init();
  while(1){
    delay_ms(200);
    
    data[6] = (count / 10) + '0';
    data[7] = (count % 10) + '0';
    count = (count + 1) % 100;
    
    USB_WaitBusy();
    USB_SendPacket(data, sizeof(data));
//    USB_WaitBusy();
  }
}

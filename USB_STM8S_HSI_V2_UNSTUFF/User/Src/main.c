
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

#pragma vector = TIM4_OVR_UIF_vector
__interrupt void TIM4_ISR(){
  TIM4_SR = 0x00;
  PD_ODR = ~PD_ODR;
  delay_us(60);
}

void main(){
  CLK_CKDIVR = 0;
//  GPIO_Init();
  USB_Disconnect();
  delay_ms(50);
  USB_Init();
  while(1){
    asm("WFI");         // CPU Sleep
  }
}

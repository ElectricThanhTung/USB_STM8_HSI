
void delay_us(unsigned int time){
  while(time--){
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
}

void delay_ms(unsigned int time){
  while(time--)
    delay_us(1000);
}

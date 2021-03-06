
#include "main.h"
#include "usb.h"
#include "usb_def.h"
#include "usb_desc.h"

static unsigned char data_sync;
unsigned char *usb_tx_buffer_pointer;
unsigned char usb_tx_count;
unsigned char usb_rx_buffer[14];
unsigned char usb_rx_count;
unsigned char pid_data_old;
volatile unsigned char UEPF = 1, UDRF = 1;
volatile unsigned char USB_TimerTick = 0;

extern void usb_tx();

struct usb_type{
  volatile unsigned char state;                                                 // 0
  volatile unsigned char event;                                                 // 1
  volatile unsigned char device_address;                                        // 2
  volatile unsigned char endpoint;                                              // 3
  volatile unsigned char received;                                              // 4
  volatile unsigned char ack;                                                   // 5
  volatile unsigned int wLength;                                                // 6
  volatile unsigned char rx_lenght;                                             // 8
  volatile unsigned char tx_lenght;                                             // 9
  volatile unsigned char data_count;                                            // 10
  
  unsigned char tx_buffer[12];                                                  // 11
  unsigned char rx_buffer[250];                                                 // 23
} usb;

static void CLK_Init(){
  CLK_CKDIVR = 0x00;
  while(CLK_CCOR & 0x40);
//  CLK_CCOR |= 0x04 << 1;
//  CLK_CCOR |= 0x01;
//  while(!(CLK_CCOR & 0x20));
  CLK_HSITRIMR = 0x05;
}

static void OptionByte_Init(){
  if(*(unsigned char *)0x4803 != 0x01){
    static unsigned char optionByte[] = {0x00, 0x01, 0x00, 0x00, 0x00};
  //  FLASH_CR2 = 0x80;
  //  FLASH_NCR2 = 0x7F;
  //  *(unsigned char *)OPT0_ADDR = 0x00;
    FLASH_CR2 = 0x80;
    FLASH_NCR2 = 0x7F;
    FLASH_PUKR = 0x56;
    FLASH_PUKR = 0xAE;
    FLASH_DUKR = 0xAE;
    FLASH_DUKR = 0x56;
    for(unsigned char i = 0; i < sizeof(optionByte); i++){
      *(unsigned char *)(0x4801 + i * 2) = optionByte[i];
      *(unsigned char *)(0x4801 + i * 2 + 1) = ~optionByte[i];
    }
    FLASH_CR2 = 0x00;
    FLASH_NCR2 = 0xFF;
    WWDG_CR = 0x80;
  }
}

static void ITC_Init(){
  ITC_SPR1 = 0x55;
  ITC_SPR2 = 0x55;
  ITC_SPR3 = 0x55;
  ITC_SPR4 = 0x57;
  ITC_SPR5 = 0x55;
  ITC_SPR6 = 0x55;
  ITC_SPR7 = 0x55;
}

static void GPIO_Init(){
  PC_CR2 &= 0x3F;
  PC_CR1 &= 0x3F;
  PC_DDR &= 0x3F;
}

static void TIM_Init(){
  CLK_PCKENR1 |= 0x80;
  TIM1_CCMR2 = 0x31;
  TIM1_CCER1 = 0x10;
  TIM1_SMCR = 0x66;
  TIM1_ARRH = 0x03;
  TIM1_ARRL = 0xE8;
  TIM1_IER |= 0x04;
}

void USB_Disconnect(){
  OptionByte_Init();
  TIM1_IER &= ~0x04;
  PC_ODR &= 0x3F;
  PC_DDR |= 0xC0;
  PC_CR1 |= 0xC0;
}

void USB_Init(){
  OptionByte_Init();
  CLK_Init();
  ITC_Init();
  TIM_Init();
  GPIO_Init();
  usb.state = USB_STATE_IDLE;
  usb.event = USB_EVENT_NO;
  usb.device_address = 0;
  usb.endpoint = 0;
  usb.received = 0;
  usb.ack = 0;
  usb.wLength = 0xFFFF;
  asm("RIM");
}

void usb_send_nack(){
  unsigned char data[2];
  PC_ODR = (PC_ODR & 0x3F) | 0x40;
  PC_DDR |= 0xC0;
  PC_CR1 |= 0xC0;

  data[0] = 0x80;
  data[1] = USB_PID_NACK;

  usb_tx_count = 2;
  usb_tx_buffer_pointer = data;

  usb_tx();
  
  PC_CR2 &= 0x3F;
  PC_CR1 &= 0x3F;
  PC_DDR &= 0x3F;
}

void usb_send_ack(){
  unsigned char data[2];
  
  PC_ODR = (PC_ODR & 0x3F) | 0x40;
  PC_DDR |= 0xC0;
  PC_CR1 |= 0xC0;
  
  data[0] = 0x80;
  data[1] = USB_PID_ACK;

  usb_tx_count = 2;
  usb_tx_buffer_pointer = data;

  usb_tx();

  PC_CR2 &= 0x3F;
  PC_CR1 &= 0x3F;
  PC_DDR &= 0x3F;
}

void usb_send_answer(){
  PC_ODR = (PC_ODR & 0x3F) | 0x40;
  PC_DDR |= 0xC0;
  PC_CR1 |= 0xC0;

  usb_tx_count = usb.tx_lenght;
  usb_tx_buffer_pointer = &usb.tx_buffer[0];

  usb_tx();

  PC_CR2 &= 0x3F;
  PC_CR1 &= 0x3F;
  PC_DDR &= 0x3F;
  
  usb.tx_lenght = 0;
}

void copy_data(){
  asm("LD A, usb + 10");
  asm("CLRW X");
  asm("LD XL, A");
  asm("ADDW X, #(usb + 23)");
  asm("LD A, usb_rx_buffer + 2");
  asm("LD ($0, X), A");
  asm("LD A, usb_rx_buffer + 3");
  asm("LD ($1, X), A");
  asm("LD A, usb_rx_buffer + 4");
  asm("LD ($2, X), A");
  asm("LD A, usb_rx_buffer + 5");
  asm("LD ($3, X), A");
  asm("LD A, usb_rx_buffer + 6");
  asm("LD ($4, X), A");
  asm("LD A, usb_rx_buffer + 7");
  asm("LD ($5, X), A");
  asm("LD A, usb_rx_buffer + 8");
  asm("LD ($6, X), A");
  asm("LD A, usb_rx_buffer + 9");
  asm("LD ($7, X), A");
  
//  unsigned char *buffer = &usb.rx_buffer[usb.data_count];
//  buffer[0] = usb_rx_buffer[2];
//  buffer[1] = usb_rx_buffer[3];
//  buffer[2] = usb_rx_buffer[4];
//  buffer[3] = usb_rx_buffer[5];
//  buffer[4] = usb_rx_buffer[6];
//  buffer[5] = usb_rx_buffer[7];
//  buffer[6] = usb_rx_buffer[8];
//  buffer[7] = usb_rx_buffer[9];
  usb_send_ack();
//  buffer[8] = usb_rx_buffer[10];
//  buffer[9] = usb_rx_buffer[11];
  usb.data_count += 8;
  unsigned char temp = usb.data_count;
  if(temp >= usb.wLength){
    usb.received = 1;
    usb.tx_lenght = 0;
    UDRF = 1;
  }
  usb.event = USB_EVENT_NO;
}

//void usb_received(){
//  UDRF = 0;
//  switch(usb_rx_buffer[1]){
//    case USB_PID_OUT:{
//      unsigned char address = usb_rx_buffer[2] & 0x7F;
//      if((address == usb.device_address) || (address == 0)){
//        usb.state = USB_STATE_OUT;
//        usb.endpoint = usb_rx_buffer[2] >> 7;
//        usb.endpoint |= ((usb_rx_buffer[3] & 0x07) << 1);
//      }
//      return;
//    }
//    case USB_PID_SETUP:{
//      unsigned char address = usb_rx_buffer[2] & 0x7F;
//      if((address == usb.device_address) || (address == 0)){
//        usb.state = USB_STATE_SETUP;
//        usb.endpoint = usb_rx_buffer[2] >> 7;
//        usb.endpoint |= ((usb_rx_buffer[3] & 0x07) << 1);
//      }
//      return;
//    }
//    case USB_PID_IN:{
//      unsigned char address = usb_rx_buffer[2] & 0x7F;
//      if((address != usb.device_address) && (address != 0))
//        return;
//      if(usb.tx_lenght)
//        usb_send_answer();
//      else
//        usb_send_nack();
//      return;
//    }
//    case USB_PID_DATA0:{
//      if(usb.state == USB_STATE_OUT){
//        if(pid_data_old == USB_PID_DATA1){
//          copy_data();
//          pid_data_old = USB_PID_DATA0;
//        }
//        else{
//          //delay_us(1);
//          usb_send_ack();
//          usb.event = USB_EVENT_NO;
//        }
//      }
//      else if(usb.state == USB_STATE_SETUP){
//        usb.rx_buffer[2] = usb_rx_buffer[2];
//        usb.rx_buffer[3] = usb_rx_buffer[3];
//        usb.rx_buffer[4] = usb_rx_buffer[4];
//        usb.rx_buffer[5] = usb_rx_buffer[5];
//        usb.rx_buffer[6] = usb_rx_buffer[6];
//        usb.rx_buffer[7] = usb_rx_buffer[7];
//        usb.rx_buffer[8] = usb_rx_buffer[8];
//        pid_data_old = USB_PID_DATA0;
//        usb.data_count = 0;
//        usb.event = USB_EVENT_RECEIVE_SETUP_DATA;
//        UDRF = 1;
//        usb_send_ack();
//
//        //wLength = usb_rx_buffer[9];
//        //wLength = (wLength << 8) | usb_rx_buffer[8];
//
//        usb.wLength = usb_rx_buffer[8];
//      }
//      return;
//    }
//    case USB_PID_DATA1:
//      if(usb_rx_count > 4){
//        if(pid_data_old == USB_PID_DATA0){
//          copy_data();
//          pid_data_old = USB_PID_DATA1;
//        }
//        else{
//          //delay_us(1);
//          usb_send_ack();
//          usb.event = USB_EVENT_NO;
//        }
//      }
//      else{
//        usb.tx_lenght = 0;
//        //delay_us(1);
//        usb_send_ack();
//      }
//      return;
//    case USB_PID_ACK:
//      usb.ack = 1;
//      return;
//    case USB_PID_NACK:
//      return;
//    default:
//      usb.state = USB_STATE_IDLE;
//    return;
//  }
//}

const unsigned int table[] = {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

//void usb_calc_crc16(unsigned char *buffer, unsigned char length){
//  unsigned int crc = 0xFFFF;
//  while(length--)
//    crc = table[(crc ^ *buffer++) & 0xFF] ^ (crc >> 8);
//  crc = ~crc;
//  *buffer++ = crc;
//  *buffer = crc >> 8;
//}

//void usb_calc_crc16(unsigned char * buffer, unsigned char lenght){
//  uint16_t crc = 0xFFFF;
//  unsigned char index;
//  unsigned char i;
//  for(index = 0; index < lenght; index++){
//    crc ^= *buffer++;
//    for(i = 8; i--;){
//      if((crc & BIT(0)) != 0){
//        crc >>= 1;
//        crc ^= 0xA001;
//      }
//      else
//        crc >>= 1;
//    }
//  }
//  crc = ~crc;
//  *buffer++ = (unsigned char) crc;
//  *buffer = (unsigned char) (crc >> 8);
//}

void USB_SendData(unsigned char *buffer, unsigned char length){
  usb.state = USB_STATE_IN;
  data_sync = USB_PID_DATA1;
  while(length){
    unsigned int crc = 0xFFFF;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = data_sync;
    if(data_sync == USB_PID_DATA1)
      data_sync = USB_PID_DATA0;
    else
      data_sync = USB_PID_DATA1;
    if(length >= 8){
      usb.tx_buffer[2] = buffer[0]; crc = table[(crc ^ buffer[0]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[3] = buffer[1]; crc = table[(crc ^ buffer[1]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[4] = buffer[2]; crc = table[(crc ^ buffer[2]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[5] = buffer[3]; crc = table[(crc ^ buffer[3]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[6] = buffer[4]; crc = table[(crc ^ buffer[4]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[7] = buffer[5]; crc = table[(crc ^ buffer[5]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[8] = buffer[6]; crc = table[(crc ^ buffer[6]) & 0xFF] ^ (crc >> 8);
      usb.tx_buffer[9] = buffer[7]; crc = table[(crc ^ buffer[7]) & 0xFF] ^ (crc >> 8);
      crc = ~crc;
      usb.tx_buffer[10] = crc;
      usb.tx_buffer[11] = crc >> 8;
      buffer += 8;
      length -= 8;
      usb.tx_lenght = 12;
    }
    else{
      unsigned char index;
      for(index = 2; index < (2 + length); index++){
        crc = table[(crc ^ *buffer) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[index] = *buffer++;
      }
      crc = ~crc;
      usb.tx_buffer[index++] = crc;
      usb.tx_buffer[index] = crc >> 8;
      usb.tx_lenght = length + 4;
      length = 0;
    }
    //usb_calc_crc16(&usb.tx_buffer[2], (unsigned char) (usb.tx_lenght - 4));
    unsigned char timeStart = USB_TimerTick;
    while(usb.tx_lenght){
      if((usb.state != USB_STATE_IN) || ((unsigned char)(USB_TimerTick - timeStart) > (100 / TimerTickStep))){
        usb.tx_lenght = 0;
        return;
      }
    }
  }
}

void USB_WriteBuf(unsigned char *buffer, unsigned char length){
  unsigned char start = 0;
  usb.state = USB_STATE_IN;
  data_sync = USB_PID_DATA1;
  while(length){
    unsigned int crc = 0xFFFF;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = data_sync;
    if(data_sync == USB_PID_DATA1)
      data_sync = USB_PID_DATA0;
    else
      data_sync = USB_PID_DATA1;
    if(start == 0){
      start = 1;
      
      usb.tx_buffer[2] = length; crc = table[(crc ^ length) & 0xFF] ^ (crc >> 8);
      
      if((length % 8) == 7)
        length++;
      
      if(length >= 7){
        usb.tx_buffer[3] = buffer[0]; crc = table[(crc ^ buffer[0]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[4] = buffer[1]; crc = table[(crc ^ buffer[1]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[5] = buffer[2]; crc = table[(crc ^ buffer[2]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[6] = buffer[3]; crc = table[(crc ^ buffer[3]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[7] = buffer[4]; crc = table[(crc ^ buffer[4]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[8] = buffer[5]; crc = table[(crc ^ buffer[5]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[9] = buffer[6]; crc = table[(crc ^ buffer[6]) & 0xFF] ^ (crc >> 8);
        crc = ~crc;
        usb.tx_buffer[10] = crc;
        usb.tx_buffer[11] = crc >> 8;
        buffer += 7;
        length -= 7;
        usb.tx_lenght = 12;
      }
      else{
        unsigned char index;
        for(index = 3; index < (3 + length); index++){
          crc = table[(crc ^ *buffer) & 0xFF] ^ (crc >> 8);
          usb.tx_buffer[index] = *buffer++;
        }
        crc = ~crc;
        usb.tx_buffer[index++] = crc;
        usb.tx_buffer[index] = crc >> 8;
        usb.tx_lenght = length + 5;
        length = 0;
      }
    }
    else{
      if(length >= 8){
        usb.tx_buffer[2] = buffer[0]; crc = table[(crc ^ buffer[0]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[3] = buffer[1]; crc = table[(crc ^ buffer[1]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[4] = buffer[2]; crc = table[(crc ^ buffer[2]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[5] = buffer[3]; crc = table[(crc ^ buffer[3]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[6] = buffer[4]; crc = table[(crc ^ buffer[4]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[7] = buffer[5]; crc = table[(crc ^ buffer[5]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[8] = buffer[6]; crc = table[(crc ^ buffer[6]) & 0xFF] ^ (crc >> 8);
        usb.tx_buffer[9] = buffer[7]; crc = table[(crc ^ buffer[7]) & 0xFF] ^ (crc >> 8);
        crc = ~crc;
        usb.tx_buffer[10] = crc;
        usb.tx_buffer[11] = crc >> 8;
        buffer += 8;
        length -= 8;
        usb.tx_lenght = 12;
      }
      else{
        unsigned char index;
        for(index = 2; index < (2 + length); index++){
          crc = table[(crc ^ *buffer) & 0xFF] ^ (crc >> 8);
          usb.tx_buffer[index] = *buffer++;
        }
        crc = ~crc;
        usb.tx_buffer[index++] = crc;
        usb.tx_buffer[index] = crc >> 8;
        usb.tx_lenght = length + 4;
        length = 0;
      }
    }
    unsigned char timeStart = USB_TimerTick;
    while(usb.tx_lenght){
      if((unsigned char)(USB_TimerTick - timeStart) > (100 / TimerTickStep)){
        usb.tx_lenght = 0;
        return;
      }
    }
  }
}

static void USB_SendNull(unsigned char PID_DATA){
  usb.state = USB_STATE_IN;
  usb.tx_buffer[0] = 0x80;
  usb.tx_buffer[1] = PID_DATA;
  usb.tx_buffer[2] = 0;
  usb.tx_buffer[3] = 0;
  usb.tx_lenght = 4;
  
  unsigned char timeStart = USB_TimerTick;
  while(usb.tx_lenght){
    if((usb.state != USB_STATE_IN) || ((unsigned char)(USB_TimerTick - timeStart) > (100 / TimerTickStep))){
      usb.tx_lenght = 0;
      return;
    }
  }
}

static void usb_send_stall(){
  usb.tx_buffer[0] = 0x80;
  usb.tx_buffer[1] = USB_PID_STALL;
  usb.tx_lenght = 2;
}

void USB_Process(){
  extern void usb_exit_process();
  if(usb.event == USB_EVENT_RECEIVE_SETUP_DATA){
    usb.event = USB_EVENT_NO;
    if(usb.rx_buffer[2] == USBRQ_STD_FROM_DEVICE){
      if(usb.rx_buffer[3] == USBRQ_GET_DESCRIPTOR){
        switch(usb.rx_buffer[5]){
          case USBDESCR_DEVICE:
            USB_SendData((unsigned char *)usb_device_descriptor, ARRAY_LENGHT(usb_device_descriptor));
            break;
          case USBDESCR_CONFIG:
            if(usb.rx_buffer[8] < ARRAY_LENGHT(usb_configuration_descriptor))
              USB_SendData((unsigned char *)usb_configuration_descriptor, usb.rx_buffer[8]);
            else
              USB_SendData((unsigned char *)usb_configuration_descriptor, ARRAY_LENGHT(usb_configuration_descriptor));
            break;
          case USBDESCR_STRING:
            if(usb.rx_buffer[4] == 0){
              USB_SendData((unsigned char *)usb_string_descriptor_language, ARRAY_LENGHT(usb_string_descriptor_language));
              USB_SendNull(data_sync);
            }
            else if(usb.rx_buffer[4] == 1){
              USB_SendData((unsigned char *)usb_string_descriptor_vendor, ARRAY_LENGHT(usb_string_descriptor_vendor));
              USB_SendNull(data_sync);
            }
            else if(usb.rx_buffer[4] == 2){
              USB_SendData((unsigned char *)usb_string_descriptor_device, ARRAY_LENGHT(usb_string_descriptor_device));
              USB_SendNull(data_sync);
            }
            else if(usb.rx_buffer[4] == 3){
              USB_SendData((unsigned char *)usb_string_descriptor_serial, ARRAY_LENGHT(usb_string_descriptor_serial));
              USB_SendNull(data_sync);
            }
            else
              USB_SendNull(USB_PID_DATA1);
            break;
          default:
            break;
        }
      }
      else if(usb.rx_buffer[3] == USBRQ_GET_STATUS)
        usb_send_stall();
    }
    else if(usb.rx_buffer[2] == USBRQ_STD_TO_DEVICE){
      switch(usb.rx_buffer[3]){
        case USBRQ_SET_ADDRESS:
          if(usb.device_address != 0){
          //
          }
          USB_SendNull(USB_PID_DATA1);
          usb.device_address = usb.rx_buffer[4];
          break;
        case (USBRQ_SET_CONFIGURATION):
          USB_SendNull(USB_PID_DATA1);
          break;
      }
    }
    else if(usb.rx_buffer[2] == USBRQ_STD_FROM_INTERFACE){
      if(usb.rx_buffer[3] == USBRQ_GET_DESCRIPTOR)
        USB_SendData((unsigned char *)usb_report_descriptor, ARRAY_LENGHT(usb_report_descriptor));
    }
    else if(usb.rx_buffer[2] == USBRQ_CLASS_TO_INTERFACE){
      if(usb.rx_buffer[3] == 0x0A)
        usb_send_stall();
    }
    else if(usb.rx_buffer[2] == USBRQ_CLASS_FROM_INTERFACE){
//      static const unsigned char usb_report_null[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//      USB_SendData((unsigned char *)usb_report_null, ARRAY_LENGHT(usb_report_null));
    }
  }
  if(usb.received){
    extern void USB_Received(unsigned char endpoint, unsigned char *buffer, unsigned char length);
    usb.ack = 0;
    USB_SendNull(USB_PID_DATA1);
    usb.received = 0;
    usb.event = USB_EVENT_NO;
//    unsigned char timeStart = USB_TimerTick;
//    while(!usb.ack){
//      if((unsigned char)(USB_TimerTick - timeStart) > 5)
//        return;
//    }
    unsigned char ep = usb.endpoint;
    USB_Received(ep, usb.rx_buffer, usb.wLength);
  }
}

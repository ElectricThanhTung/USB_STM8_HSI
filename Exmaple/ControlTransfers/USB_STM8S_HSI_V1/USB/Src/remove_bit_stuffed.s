
  NAME remove_bit_stuffed
 
  EXTERN data_buffer
  EXTERN usb

  PUBLIC remove_bit_stuffed

  SECTION `.near_func.text`:CODE:REORDER:NOROOT(0)
  CODE
remove_bit_stuffed:
  LDW X,        #data_buffer
  LDW Y,        #(usb + 23)
  PUSH          0x00
  PUSH          0x01
  
  MOV           0x00, #8
  MOV           0x01, #8
  CLR           (X)

loop:
  LD            A, (X)
  AND           A, #0xFE
  CP            A, #0xFC
  JRNE          didunstuff
  SRL           ($9, Y)
  RRC           ($8, Y)
  RRC           ($7, Y)
  RRC           ($6, Y)
  RRC           ($5, Y)
  RRC           ($4, Y)
  RRC           ($3, Y)
  RRC           ($2, Y)
  RRC           ($1, Y)
  RRC           (Y)
didunstuff:
  SRL           ($9, Y)
  RRC           ($8, Y)
  RRC           ($7, Y)
  RRC           ($6, Y)
  RRC           ($5, Y)
  RRC           ($4, Y)
  RRC           ($3, Y)
  RRC           ($2, Y)
  RRC           ($1, Y)
  RRC           (Y)
  RRC           (X)
  DEC           0x00
  JRNE          loop
  
  MOV           0x00, #8
  INCW          X
  CLR           (X)
  DEC           0x01
  JRNE          loop
  
  POP           0x01
  POP           0x00
  RET
  
  END

  NAME usb_tx

  EXTERN usb_tx_buffer_pointer
  EXTERN usb_tx_count

  PUBLIC usb_tx

  SECTION `.usb_tx_func.rodata`:CODE:REORDER:NOROOT(0)
  CODE

usb_tx:
  LDW     x, L:usb_tx_buffer_pointer    ;2  ,2/3
  LD      A, $500A
  LDW     Y, #$06                       ;2  ,2

bit_0:           
  RRC     (X)                           ;1  ,1
  JRC     bit_0_idle                    ;1/2,2
  XOR     A, #$C0                       ;1  ,2
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  JRA     bit_1
bit_0_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW    Y                             ;1  ,1
  JRNE    bit_1                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  JRA     bit_1
  
bit_7:           
  RRC     (X)                           ;1  ,1
  JRC     bit_7_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP
check_end:
  INCW    X                             ;1  ,1
  DEC     L:usb_tx_count                ;1  ,2/4
  JRNE    bit_0                         ;1/2,2
  NOP                                   ;1  ,1

tx_end:
  NOP                                   ;1  ,1
  AND     A, #0x3F                      ;1
  LD      $500A, A                      ;1  ,4
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
//  NOP                                 ;1  ,1
//  NOP                                 ;1  ,1
//  NOP                                 ;1  ,1
//  NOP                                 ;1  ,1
  MOV   L:21077,#251
  BRES  L:21072,#0
  CLR   L:21086
  CLR   L:21087
  RET                                   ;4  ,1

bit_7_idle:
  NOP                                   ;1  ,1
  DECW    Y                             ;1  ,1
  JRNE    check_end                     ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  JRA     check_end
  
bit_1:           
  RRC     (X)                           ;1  ,1
  JRC     bit_1_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JRA     bit_2
bit_1_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW    Y                             ;1  ,1
  JRNE    bit_2                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  NOP
  
bit_2:           
  RRC     (X)                           ;1  ,1
  JRC     bit_2_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JRA     bit_3
bit_2_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW     Y                            ;1  ,1
  JRNE    bit_3                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  NOP
  
bit_3:           
  RRC     (X)                           ;1  ,1
  JRC     bit_3_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JRA     bit_4
bit_3_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW    Y                             ;1  ,1
  JRNE    bit_4                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  NOP
  
bit_4:           
  RRC     (X)                           ;1  ,1
  JRC     bit_4_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JRA     bit_5
bit_4_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW    Y                             ;1  ,1
  JRNE    bit_5                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  NOP
  
bit_5:           
  RRC     (X)                           ;1  ,1
  JRC     bit_5_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JRA     bit_6
bit_5_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  DECW    Y                             ;1  ,1
  JRNE    bit_6                         ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
  NOP
  NOP
  
bit_6:           
  RRC     (X)                           ;1  ,1
  JRC     bit_6_idle                    ;1/2,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  LDW     Y, #$06                       ;2  ,2
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  JP      bit_7
bit_6_idle:
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  DECW    Y                             ;1  ,1
  JRNE    goto_bit_7                    ;1/2,2
  NOP                                   ;1  ,1
  LDW     Y, #$06                       ;2  ,2
  XOR     A, #$C0
  LD      $500A, A                      ;1  ,3
  NOP                                   ;1  ,1
  NOP                                   ;1  ,1
  NOP
  NOP
goto_bit_7:
  JP      bit_7

  SECTION VREGS:DATA:REORDER:NOROOT(0)

  END

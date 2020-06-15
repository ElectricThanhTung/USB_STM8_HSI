
#define DEBUG_ENABLE            0
#define DEBUG_PORT              0x500F          // PD

#if DEBUG_ENABLE
#define DEBUG                   CPL DEBUG_PORT
#else
#define DEBUG                   CPL 0x5019
#endif

  NAME usb_rx

  EXTERN usb_rx_buffer
  EXTERN usb_rx_count
  EXTERN usb_received
  EXTERN usb_received_2
  EXTERN USB_Process

  PUBLIC _interrupt_14
  PUBLIC usb_rx

  SECTION `.usb_rx_func.rodata`:CODE:REORDER:NOROOT(0)
usb_rx:
_interrupt_14:
  LDW X, #delay_begin
  ADDW X, 21086
  CPW X, #delay_end
  JRUGE Error
  JP (X)
	
Error:
  IRET

delay_begin:
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
//  NOP
//  NOP

delay_end:
  LDW           X, #usb_rx_buffer       ;2,  3
  LDW           Y, #$500B               ;2,  4  PC input register,D7=DP,D6=DM
  PUSH          #$0E                    ;1,  2  SP+2, usb_rx_count
  PUSH          #$80                    ;1,  2  SP+1
  JP            bit_0_begin             ;2,  2
  
end_rx:
  LD            A, #$0F
  SUB           A, ($02, SP)
  LD            L:usb_rx_count, A       ;1,  2/3
  POP           A                       ;1,  1
  POP           A                       ;1,  1

  MOV           21077,#251              ;1,  5
  BRES          21072,#0                ;1,  4
  CLR           21086                   ;1,  4
  CLR           21087                   ;1,  4

  JP            usb_received_2          ;4,  3
  
goto_bit_7_stuffed:
  JP            bit_7_stuffed
goto_bit_0_stuffed:
  JP            bit_0_stuffed
goto_bit_1_stuffed:
  JP            bit_1_stuffed
goto_bit_2_stuffed:
  JP            bit_2_stuffed
goto_bit_3_stuffed:
  JP            bit_3_stuffed

bit_7_begin:
//  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  LD            (X), A                  ;1,  1
  RLC           (X)                     ;1,  1
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          goto_bit_7_stuffed

bit_0_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2  SP+1??
  LD            A, (Y)                  ;1,  2  ?PC,??Z,N
  AND           A, #$C0                 ;1,
  JREQ          end_rx                 ;1/2,2  Z=1????
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP                                   ;1,  1
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          goto_bit_0_stuffed

bit_1_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  INCW          X                       ;1,  1
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          goto_bit_1_stuffed
  
bit_2_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP
  NOP
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          goto_bit_2_stuffed
  
bit_3_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  DEC           ($02, SP)               ;1,  2
  JREQ          end_rx2                 ;1/2,2
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          goto_bit_3_stuffed
  
bit_4_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP
//  NOP
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          bit_4_stuffed
  
bit_5_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,  2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP
//  NOP
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          bit_5_stuffed

bit_6_begin:
  DEBUG
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  AND           A, #$7F
  XOR           A, #$7E
  JREQ          bit_6_stuffed
  JP            bit_7_begin
  
end_rx2:
  LD            A, #$0F
  SUB           A, ($02, SP)
  LD            L:usb_rx_count, A       ;1,  2/3
  POP           A                       ;1,  1
  POP           A                       ;1,  1

  MOV           21077,#251              ;1,  5
  BRES          21072,#0                ;1,  4
  CLR           21086                   ;1,  4
  CLR           21087                   ;1,  4

  JP            usb_received_2          ;4,  3
  
  DEBUG
  IRET
  
bit_4_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  NOP
  NOP
  NOP
  JP            bit_5_begin
bit_5_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  NOP
  NOP
  NOP
  JP            bit_6_begin
bit_6_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  JP            bit_7_begin
bit_7_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  JP            bit_0_begin
bit_0_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  JP            bit_1_begin
bit_1_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  NOP
  JP            bit_2_begin
bit_2_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  NOP
  JP            bit_3_begin
bit_3_stuffed:
  LD            A, ($01, SP)
  XOR           A, #$80
  LD            ($01, SP), A            ;1,  2
  JP            bit_4_begin

  SECTION VREGS:DATA:REORDER:NOROOT(0)

  END



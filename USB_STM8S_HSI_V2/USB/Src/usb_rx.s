
#define DEBUG                   0
#define DEBUG_PORT              0x500F          // PD

  NAME usb_rx

  EXTERN usb_rx_buffer
  EXTERN usb_rx_count
  EXTERN usb_received
  EXTERN usb_received_2
  EXTERN USB_Process

  PUBLIC _interrupt_14
  PUBLIC usb_rx

  SECTION `.usb_rx_func.rodata`:CODE:REORDER:NOROOT(0)
  CODE
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
  NOP
//  NOP

delay_end:
  LDW           X, #usb_rx_buffer       ;2,  3
  LDW           Y, #$500B               ;2,  4  PC input register,D7=DP,D6=DM
  PUSH          #$0E                    ;1,  2  SP+2, usb_rx_count
  PUSH          #$80                    ;1,  2  SP+1
  JRA           bit_0_begin             ;2,  2

bit_7_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  LD            (X), A                  ;1,  1
  RLC           (X)                     ;1,  1
  NOP

bit_0_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2  SP+1??
  LD            A, (Y)                  ;1,  2  ?PC,??Z,N
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2  Z=1????
  XOR           A, ($01, SP)            ;1,  2  
  LD            ($01, SP), A            ;1,  2
  NOP                                   ;1,  1
  NOP
  NOP
  NOP

bit_1_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  incw          X                       ;1,  1
  NOP
  NOP
  NOP
  
bit_2_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP                                   ;1,  1
  //NOP
  JP            bit_3_begin
  
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
  
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#endif
  IRET
  
bit_3_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP                                   ;1,1
  NOP
  NOP
  NOP
  
bit_4_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP                                   ;1,  1
  NOP
  NOP
  NOP
  
bit_5_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,  2
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  dec           ($02,SP)                ;1,  2
  JREQ          end_rx                  ;1/2,2
  NOP
//  NOP

bit_6_begin:
#if DEBUG
  CPL           L:DEBUG_PORT            ;1,  4
#else
  CPL           L:$5019
#endif
  SRL           ($01, SP)               ;1,  2
  LD            A, (Y)                  ;1,  2
  AND           A, #$C0                 ;1,
  JREQ          end_rx                  ;1/2,2
  XOR           A, ($01, SP)            ;1,  2
  LD            ($01, SP), A            ;1,  2
  NOP
//  NOP
  JP            bit_7_begin

  SECTION VREGS:DATA:REORDER:NOROOT(0)

  END



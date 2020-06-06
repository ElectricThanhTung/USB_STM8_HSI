
#define USB_PID_SETUP                   0x2D
#define USB_PID_DATA0                   0xC3
#define USB_PID_DATA1                   0x4B
#define USB_PID_IN                      0x69
#define USB_PID_OUT                     0xE1
#define USB_PID_ACK                     0xD2
#define USB_PID_NACK                    0x5A
#define USB_PID_STALL                   0x1E

#define USB_STATE_IDLE                  0
#define USB_STATE_SETUP                 1
#define USB_STATE_IN                    2
#define USB_STATE_OUT                   3
#define USB_STATE_DATA                  4
#define USB_STATE_ADDRESS_CHANGE        5

#define USB_EVENT_NO                    0
#define USB_EVENT_RECEIVE_SETUP_DATA    1
#define USB_EVENT_READY_DATA_IN         2
#define USB_EVENT_WAIT_DATA_IN          3
  
  NAME usb_received_2

  EXTERN usb_send_ack
  EXTERN usb_send_nack
  EXTERN usb_send_answer
  EXTERN usb_rx_buffer
  EXTERN pid_data_old
  EXTERN usb_rx_count
  EXTERN copy_data
  EXTERN goto_usb_process
  EXTERN UDRF
  EXTERN UEPF
  EXTERN usb

  PUBLIC usb_received_2

  SECTION `.near_func.text`:CODE:REORDER:NOROOT(0)
  CODE
usb_received_2:
  MOV UDRF, #0x00
  
  LD A, usb_rx_buffer + 1
  CP A, #USB_PID_OUT
  JREQ pid_out
  CP A, #USB_PID_SETUP
  JREQ pid_setup
  CP A, #USB_PID_IN
  JREQ pid_in
  CP A, #USB_PID_DATA0
  JREQ pid_data_0
  CP A, #USB_PID_DATA1
  JREQ goto_pid_data_1
  CP A, #USB_PID_ACK
  JREQ goto_pid_ack
  CP A, #USB_PID_NACK
  JREQ goto_pid_nack
default:
  MOV usb, #USB_STATE_IDLE
  IRET
  
goto_pid_data_1:
  JP pid_data_1
goto_pid_ack:
  JP pid_ack
goto_pid_nack:
  JP pid_ack
  
pid_out:
  LD A, usb_rx_buffer + 2
  AND A, #0x7F                                                                  // A = usb_rx_buffer[2] & 0x7F
  JREQ pid_out_compare_ok                                                       // if(usb_rx_buffer[2] & 0x7F == 0)
  CP A, usb + 2
  JRNE pid_out_break                                                            // if(usb_rx_buffer[2] & 0x7F != usb.address)
pid_out_compare_ok:
  MOV usb, #USB_STATE_OUT
pid_out_break:
  IRET

pid_setup:
  LD A, usb_rx_buffer + 2
  AND A, #0x7F                                                                  // A = usb_rx_buffer[2] & 0x7F
  JREQ pid_setup_compare_ok                                                     // if(usb_rx_buffer[2] & 0x7F == 0)
  CP A, usb + 2
  JRNE pid_setup_break                                                          // if(usb_rx_buffer[2] & 0x7F != usb.address)
pid_setup_compare_ok:
  MOV usb, #USB_STATE_SETUP                                                     // usb.state = USB_STATE_OUT
  LD A, usb_rx_buffer + 2
  SLA A
  CLR usb + 3                                                                   // usb.endpoint = 0
  RLC usb + 3                                                                   // usb.endpoint = (usb_rx_buffer[2] & 0x7F) >> 7
  LD A, usb_rx_buffer + 3
  AND A, #0x07
  SLA A
  OR A, usb + 3
  LD usb + 3, A                                                                 // usb.endpoint = (usb_rx_buffer[3] << 1)
  IRET
pid_setup_break:
  IRET
  
pid_in:
  MOV usb, #USB_STATE_IN
  LD A, usb_rx_buffer + 2
  AND A, #0x7F                                                                  // A = usb_rx_buffer[2] & 0x7F
  JREQ pid_in_compare_ok                                                        // if(usb_rx_buffer[2] & 0x7F == 0)
  CP A, usb + 2
  JRNE break_pid_in                                                             // if(usb_rx_buffer[2] & 0x7F != usb.address)
pid_in_compare_ok:
  LD A, usb + 9                                                                 // A = usb.tx_length
  JRNE tx_length_ok
  CALL usb_send_nack
  IRET
tx_length_ok:
  CALL usb_send_answer
  IRET
break_pid_in
  IRET
  
pid_data_0:
  LD A, usb                                                                     // A = usb.state
  CP A, #USB_STATE_OUT
  JREQ state_out_compare_ok                                                     // if(usb.state != USB_STATE_OUT)
  CP A, #USB_STATE_SETUP
//  JREQ state_setup_compare_ok
//  IRET
  JP state_setup_compare_ok
state_out_compare_ok:                                                           // if(usb.state == USB_STATE_OUT)
  LD A, pid_data_old
  CP A, #USB_PID_DATA1
  JREQ pid_data_0_sync_ok                                                       // if(pid_data_old == USB_PID_DATA1)
  CALL usb_send_ack
  MOV usb + 1, #USB_EVENT_NO                                                    // usb.event = USB_EVENT_NO
  IRET
pid_data_0_sync_ok:
  CALL copy_data
  MOV pid_data_old, #USB_PID_DATA0                                              // pid_data_old = USB_PID_DATA0
  LD A, UDRF
  ADD A, UEPF
  CP A, #2
  JREQ usb_process
  IRET
state_setup_compare_ok:                                                         // if(usb.state == USB_STATE_SETUP)
  MOV usb + 23 + 2, usb_rx_buffer + 2                                           // usb.rx_bufer[2] = usb_rx_buffer[2]
  MOV usb + 23 + 3, usb_rx_buffer + 3
  MOV usb + 23 + 4, usb_rx_buffer + 4
  MOV usb + 23 + 5, usb_rx_buffer + 5
  MOV usb + 23 + 6, usb_rx_buffer + 6
  MOV usb + 23 + 7, usb_rx_buffer + 7
  MOV usb + 23 + 8, usb_rx_buffer + 8
  MOV pid_data_old, #USB_PID_DATA0                                              // pid_data_old = USB_PID_DATA0
  MOV usb + 10, #0                                                              // usb.data_count = 0;
  MOV usb + 1, #USB_EVENT_RECEIVE_SETUP_DATA                                    // usb.event = USB_EVENT_RECEIVE_SETUP_DATA
//  MOV UDRF, #1                                                                  // UDRF = 1
  LD A, usb_rx_buffer + 8                                                       // A = usb_rx_buffer[8]
  CLRW X
  LD XL, A
  LDW usb + 6, X                                                                // usb.wLength = usb_rx_buffer[8]
  CALL usb_send_ack                                                             // usb_send_ack()
  LD A, usb_rx_buffer + 2
  CP A, #0x21
  JRNE check_UEPF
  LD A, usb_rx_buffer + 3
  CP A, #0x09
  JREQ return_func
check_UEPF:
  TNZ UEPF
  JRNE usb_process                                                              // if(UEPF != 0)
return_func:
  IRET
  
usb_process:
  MOV UEPF, #0
  LDW X, #goto_usb_process
  PUSHW X
  PUSH #0x00
  POP CC                                                                        // CC = 0x00 (Interrupt Level 2)
  RET
  
pid_data_1:
  LD A, usb_rx_count
  CP A, #5
  JRC usb_rx_count_compare_ok                                                   // if(usb_rx_count <= 4)
  LD A, pid_data_old
  CP A, #USB_PID_DATA0
  JREQ pid_data_1_sync_ok                                                       // if(pid_data_old == USB_PID_DATA0)
  CALL usb_send_ack
  MOV usb + 1, #USB_EVENT_NO                                                    // usb.event = USB_EVENT_NO
  IRET
pid_data_1_sync_ok:
  CALL copy_data
  MOV pid_data_old, #USB_PID_DATA1                                              // pid_data_old = USB_PID_DATA1
  LD A, UDRF
  ADD A, UEPF
  CP A, #2
  JREQ usb_process
  IRET
usb_rx_count_compare_ok:
  MOV usb + 9, #0
  MOV usb, #USB_STATE_IDLE
  CALL usb_send_ack
  IRET
  
pid_ack:
  MOV usb + 5, #1                                                               // usb.ack = 1
  IRET
  
pid_nack:
  IRET
  
  END
  
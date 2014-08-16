/***********************************************************************************
  Filename:     hal_uart.h

  Description:  hal UART library header file

***********************************************************************************/

#ifndef HAL_UART_H
#define HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
* INCLUDES
*/
#include "hal_types.h"

/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
/* Serial Port Baudrate Settings */
#define HAL_UART_BAUDRATE_2400        59*256+6
#define HAL_UART_BAUDRATE_4800        59*256+7
#define HAL_UART_BAUDRATE_9600        59*256+8
#define HAL_UART_BAUDRATE_14400       216*256+8
#define HAL_UART_BAUDRATE_19200       59*256+9
#define HAL_UART_BAUDRATE_28800       216*256+9
#define HAL_UART_BAUDRATE_38400       59*256+10
#define HAL_UART_BAUDRATE_57600       216*256+10
#define HAL_UART_BAUDRATE_76800       59*256+11
#define HAL_UART_BAUDRATE_115200      216*256+11
#define HAL_UART_BAUDRATE_230400      216*256+12

  /* the level of the start and stop bit£¬0: low,1: high */
#define HAL_UART_START                0x01
#define HAL_UART_STOP                 0x02

  /*0:one bit stop,1:2 bits stop*/  
#define HAL_UART_SPB                  0x04
  
/* Parity settings */
#define HAL_UART_NO_PARITY            0x08
#define HAL_UART_EVEN_ODD_PARITY      0x20

 /* Number of bits in data field ,0: 8bits, 1: 9bits*/
#define HAL_UART_BIT9                 0x10
 
#define HAL_UART_FLOW                 0x40
#define HAL_UART_FLUSH                 0x80

/***********************************************************************************
 * GLOBAL FUNCTIONS
 */
void  Delay(uint16 n);
void   halUartInit(uint16 baudrate, uint8 options);
void halUartWrite(const char* buf, uint16 length);
void halUartRead(char* buf, uint16 length);
uint16 halUartGetNumRxBytes(void);
void   halUartEnableRxFlow(uint8 enable);
uint16 halUartBufferedWrite(const uint8* buf, uint16 length);
uint16 halComputeBaud(uint32 Originrate);

#ifdef  __cplusplus
}
#endif

/**********************************************************************************/
#endif


/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/


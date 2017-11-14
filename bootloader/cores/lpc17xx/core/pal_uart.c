/* Copyright (c) 2011 Jorge Pinto - casainho@gmail.com       */
/* All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

#include "pal_uart.h"

#include "board.h"

void uart_init(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration for UART
	PINSEL_CFG_Type PinCfg;

	/*
	* Initialize UART pin connect: 
	* P0.2 -> TXD0, P0.3 -> RXD0
	* P4.28 -> TXD3; P4.29 -> RXD3
	*/
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = BL_UART_TX_PORT;
	PinCfg.Pinnum = BL_UART_TX_PIN;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Portnum = BL_UART_RX_PORT;
	PinCfg.Pinnum = BL_UART_RX_PIN;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize UART Configuration parameter structure to default state:
		* Baudrate = 
		* 8 data bit
		* 1 Stop bit
		* None parity
		*/
	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = BL_BAUD_RATE;

	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init((LPC_UART_TypeDef *)BL_UART, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	*                              - FIFO_DMAMode = DISABLE
	*                              - FIFO_Level = UART_FIFO_TRGLEV0
	*                              - FIFO_ResetRxBuf = ENABLE
	*                              - FIFO_ResetTxBuf = ENABLE
	*                              - FIFO_State = ENABLE
	*/
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART peripheral
	UART_FIFOConfig((LPC_UART_TypeDef *)BL_UART, &UARTFIFOConfigStruct);

	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef *)BL_UART, ENABLE);
}

bool uart_data_available(void)
{
	return (BL_UART->LSR & UART_LSR_RDR);
}

char uart_receive(void)
{
	return (UART_ReceiveByte((LPC_UART_TypeDef *)BL_UART));
}

char uart_receive_wait(void)
{
    while (!uart_data_available()) {};
    return (UART_ReceiveByte((LPC_UART_TypeDef *)BL_UART));
}

void uart_send_byte (char byte)
{
	while (!(BL_UART->LSR & UART_LSR_THRE)) ;
	UART_SendByte((LPC_UART_TypeDef *)BL_UART, byte);
}

void uart_writestr(unsigned char *data)
{
	int i = 0;
	unsigned char r;

	while ((r = data[i++]))
		uart_send_byte (r);
}

void uart_send_data(unsigned char *data, unsigned int count)
{
    while (count--)
        uart_send_byte (*data++);
}

int uart_get_data(unsigned char *data, unsigned int count)
{
   while (count--) {
       *data++ = uart_receive_wait ();
   }
   return 0;
}

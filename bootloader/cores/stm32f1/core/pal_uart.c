/* Copyright (c) 2017 Bob Cousins bobcousins42@googlemail.com

   All rights reserved.

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

#include "stm32f10x.h"

#include "pal_uart.h"
#include "pal_ios.h"

#include "board.h"



void uart_init(void)
{
    /* USART configuration structure for USART */
    USART_InitTypeDef usart_init_struct;

    /* Bit configuration structure for GPIO */
    GPIO_InitTypeDef gpio_init_struct;

    /* Enable clock for USART, AFIO and GPIO */

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd (4 << BL_UART_TX_PORT, ENABLE);
    RCC_APB2PeriphClockCmd (4 << BL_UART_RX_PORT, ENABLE);
    switch (BL_UART_NUM)
    {
        case 1: RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE); break;
        case 2: RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); break;
        case 3: RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE); break;
    }

    /* GPIO alternative function Tx */
    gpio_init_struct.GPIO_Pin = 1 << BL_UART_TX_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(PortBase [BL_UART_TX_PORT], &gpio_init_struct);

    /* GPIO alternative function Rx */
    gpio_init_struct.GPIO_Pin = 1 << BL_UART_RX_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(PortBase[BL_UART_RX_PORT], &gpio_init_struct);

    /* Enable USART */
    USART_Cmd(BL_UART, ENABLE);
    /* Baud rate, 8-bit data, One stop bit
     * No parity, Enable Rx and Tx, No HW flow control
     */
    usart_init_struct.USART_BaudRate = BL_BAUD_RATE;
    usart_init_struct.USART_WordLength = USART_WordLength_8b;
    usart_init_struct.USART_StopBits = USART_StopBits_1;
    usart_init_struct.USART_Parity = USART_Parity_No ;
    usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART */
    USART_Init(BL_UART, &usart_init_struct);
}

bool uart_data_available(void)
{
	return BL_UART->SR & USART_FLAG_RXNE ? true : false;
}

char uart_receive(void)
{
	return USART_ReceiveData(BL_UART);
}

char uart_receive_wait(void)
{
    while (!uart_data_available()) {};
    return USART_ReceiveData(BL_UART);
}

void uart_send_byte (char byte)
{
    while(USART_GetFlagStatus(BL_UART, USART_FLAG_TXE) == 0) ;

    USART_SendData(BL_UART, byte);
}

void uart_writestr(unsigned char *data)
{
	int i = 0;
	unsigned char c;

	while ((c = data[i++]))
		uart_send_byte (c);
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




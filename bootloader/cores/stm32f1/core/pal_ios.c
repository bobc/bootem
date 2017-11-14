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

#include "pal_ios.h"

GPIO_TypeDef * PortBase [] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG
};

/* Initialize IO pin */
void pin_mode(uint8_t portNum, uint32_t bitValue, uint8_t dir)
{
    GPIO_InitTypeDef gpio_pin_config = { bitValue,
                                         GPIO_Speed_50MHz,
                                         GPIO_Mode_Out_PP };

    if (dir==INPUT)
    	gpio_pin_config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    else if (dir==INPUT_PULLUP)
        gpio_pin_config.GPIO_Mode = GPIO_Mode_IPU;

    /* Enable PORT clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN << portNum;

    /* Configure port pin */
    GPIO_Init(PortBase[portNum], &gpio_pin_config);
}

/*  */
void digital_write(uint8_t portNum, uint32_t bitValue, uint8_t state)
{
    if (state)
        GPIO_SetBits (PortBase[portNum], bitValue);
    else
        GPIO_ResetBits (PortBase[portNum], bitValue);
}

/*  */
uint32_t digital_read(uint8_t portNum, uint32_t bitValue)
{
  return (GPIO_ReadInputData(PortBase[portNum]) & bitValue) ? 1 : 0;
}

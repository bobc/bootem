/* Copyright (c) 2007, 2010, ChaN, Martin Thomas, Mike Anton */
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

#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_ssp.h"

#include "spi.h"
#include "ios.h"

#include "config_pins.h"

/*-----------------------------------------------------------------------*/
/* SPI low-level functions                                               */
/*-----------------------------------------------------------------------*/

void spi_init(void)
{
  PINSEL_CFG_Type PinCfg;
  SSP_CFG_Type SSP_ConfigStruct;

  /*
   * Initialize SPI pin connect
   * 
   * SPI0:      
   * P0.16 - SSEL0 - used as GPIO
   * P0.15 - SCK0
   * P0.17 - MISO0
   * P0.18 - MOSI0
   * 
   * SPI1:      
   */

  
  /* SSEL as GPIO, with pull-up enabled */
  PinCfg.Funcnum   = PINSEL_FUNC_0;
  PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
  PinCfg.Pinmode   = PINSEL_PINMODE_PULLUP;
  PinCfg.Pinnum    = SD_SSEL_PIN;
  PinCfg.Portnum   = SD_SSEL_PORT;
  GPIO_SetDir(SD_SSEL_PORT, (1 << SD_SSEL_PIN), 1);
  PINSEL_ConfigPin(&PinCfg);
  
  /* SCK : alternate function 2 */
  PinCfg.Funcnum   = PINSEL_FUNC_2;
  PinCfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
  PinCfg.Portnum   = SD_SCLK_PORT;
  PinCfg.Pinnum    = SD_SCLK_PIN;
  PINSEL_ConfigPin(&PinCfg);
  /* MISO */
  PinCfg.Pinmode   = PINSEL_PINMODE_PULLUP;
  PinCfg.Portnum   = SD_MISO_PORT;
  PinCfg.Pinnum    = SD_MISO_PIN;
  PINSEL_ConfigPin(&PinCfg);
  /* MOSI */
  PinCfg.Portnum   = SD_MOSI_PORT;
  PinCfg.Pinnum    = SD_MOSI_PIN;
  PINSEL_ConfigPin(&PinCfg);

  /* initialize SSP configuration structure */
  SSP_ConfigStruct.CPHA = SSP_CPHA_FIRST;
  SSP_ConfigStruct.CPOL = SSP_CPOL_HI;
  SSP_ConfigStruct.ClockRate = 200000; /* 200KHz */
  SSP_ConfigStruct.Databit = SSP_DATABIT_8;
  SSP_ConfigStruct.Mode = SSP_MASTER_MODE;
  SSP_ConfigStruct.FrameFormat = SSP_FRAME_SPI;
  SSP_Init(SD_SSP, &SSP_ConfigStruct);

  /* Enable SSP peripheral */
  SSP_Cmd(SD_SSP, ENABLE);
  
  digital_write (SD_SSEL_PORT, (1 << SD_SSEL_PIN), 0);
  
}

void spi_set_speed( enum speed_setting speed )
{
  if ( speed == INTERFACE_SLOW )
  {
    setSSPclock(SD_SSP, 400000);
  }
  else
  {
    setSSPclock(SD_SSP, 25000000);
  }
}

void spi_close(void)
{
  PINSEL_CFG_Type PinCfg;

  SSP_Cmd(SD_SSP, DISABLE);
  SSP_DeInit(SD_SSP);

  PinCfg.Funcnum   = PINSEL_FUNC_0;
  PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
  PinCfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
  PinCfg.Pinnum    = SD_SSEL_PIN;
  PinCfg.Portnum   = SD_SSEL_PORT;
  PINSEL_ConfigPin(&PinCfg);                     
  PinCfg.Pinnum    = SD_SCLK_PIN;
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Pinnum    = SD_MISO_PIN;
  PINSEL_ConfigPin(&PinCfg);
  PinCfg.Pinnum    = SD_MOSI_PIN;
  PINSEL_ConfigPin(&PinCfg);
  
  digital_write (SD_SSEL_PORT, (1 << SD_SSEL_PIN), 1);
}

uint8_t spi_rw( uint8_t out )
{
  uint8_t in;

  SD_SSP->DR = out;
  while (SD_SSP->SR & SSP_SR_BSY ) { ; }
  in = SD_SSP->DR;

  return in;
}

uint8_t rcvr_spi(void)
{
  return spi_rw(0xff);
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)  *(dst)=spi_rw(0xff)

#define FIFO_ELEM 8 /* "8 frame FIFOs for both transmit and receive.*/

void spi_rcvr_block (
        uint8_t *buff,         /* Data buffer to store received data */
        uint16_t btr            /* Byte count (must be multiple of 4) */
)
{
        uint16_t hwtr, startcnt, i, rec;

        hwtr = btr/2;
        if ( btr < FIFO_ELEM ) {
                startcnt = hwtr;
        } else {
                startcnt = FIFO_ELEM;
        }

        SD_SSP->CR0 |= SSP_CR0_DSS(16); // DSS to 16 bit

        for ( i = startcnt; i; i-- ) {
                SD_SSP->DR = 0xffff;  // fill TX FIFO
        }

        do {
                while ( !(SD_SSP->SR & SSP_SR_RNE ) ) {
                        // wait for data in RX FIFO (RNE set)
                }
                rec = SD_SSP->DR;
                if ( i < ( hwtr - startcnt ) ) {
                        SD_SSP->DR = 0xffff;
                }
                *buff++ = (uint8_t)(rec >> 8);
                *buff++ = (uint8_t)(rec);
                i++;
        } while ( i < hwtr );

        SD_SSP->CR0 = ( SD_SSP->CR0 & ~SSP_CR0_DSS(16) ) | SSP_CR0_DSS(8); // DSS to 8 bit
}

void spi_xmit_block (
        const uint8_t *buff    /* 512 byte data block to be transmitted */
)
{
        uint16_t cnt;
        int16_t data;

        SD_SSP->CR0 |= SSP_CR0_DSS(16); // DSS to 16 bit

        for ( cnt = 0; cnt < ( 512 / 2 ); cnt++ ) {
                while ( !( SD_SSP->SR & SSP_SR_TNF ) ) {
                        ; // wait for TX FIFO not full (TNF)
                }
                data  = (*buff++) << 8;
                data |= *buff++;
                SD_SSP->DR = data;
        }

        while ( SD_SSP->SR & SSP_SR_BSY ) {
                // wait for BSY gone
        }
        while ( SD_SSP->SR & SSP_SR_RNE ) {
                data = SD_SSP->DR; // drain receive FIFO
        }

        SD_SSP->CR0 = ( SD_SSP->CR0 & ~SSP_CR0_DSS(16) ) | SSP_CR0_DSS(8); // DSS to 8 bit
}

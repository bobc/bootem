/* **************************************************************************
  Copyright (c) 2017 Bob Cousins bobcousins42@googlemail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

// **************************************************************************
// Description: definitions for Olimexino-STM32
//
// **************************************************************************

#ifndef _BOARD_H
#define _BOARD_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include <stdbool.h>

#include "bootloader.h"

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------


//
#define USE_USART2

#ifdef USE_USART1
    #define BL_UART                USART1
    #define BL_UART_NUM            1

    #define BL_UART_TX_PORT        GPIO_A
    #define BL_UART_TX_PIN         9

    #define BL_UART_RX_PORT        GPIO_A
    #define BL_UART_RX_PIN         10
#elif defined (USE_USART2)

    #define BL_UART                USART2
    #define BL_UART_NUM            2

    #define BL_UART_TX_PORT        GPIO_A
    #define BL_UART_TX_PIN         2

    #define BL_UART_RX_PORT        GPIO_A
    #define BL_UART_RX_PIN         3

#elif defined (USE_USART3)

    #define BL_UART                USART3
    #define BL_UART_NUM            3

    #define BL_UART_TX_PORT        GPIO_B
    #define BL_UART_TX_PIN         10

    #define BL_UART_RX_PORT        GPIO_B
    #define BL_UART_RX_PIN         11
#endif

#define BL_BAUD_RATE            115200


// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

void board_init(void);

bool board_button_pressed(void);

void board_notify (tBootloaderNotification event);

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

#endif // _BOARD_H


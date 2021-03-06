/*
 * Copyright (c) 2014 Bob Cousins bobcousins42@googlemail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// **************************************************************************
// Description: For Freedom K64F
//
// **************************************************************************

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include "bootloader.h"

// Platform
#include "pal_api.h"

// Board specific
#include "board.h"
//#include "config_pins.h"

// --------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Local defines
// --------------------------------------------------------------------------

#define LED_YELLOW_PORT GPIO_A
#define LED_YELLOW_PIN  (1<<1)

#define LED_GREEN_PORT GPIO_A
#define LED_GREEN_PIN  (1<<5)

#define BOOT_BTN_PORT GPIO_A
#define BOOT_BTN_PIN  (1<<0)

// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Private Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Function prototypes
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

/** @brief
 *  @param[in]
 *  @param[out]
 *  @return
 */
void board_init(void)
{
    pin_mode (LED_YELLOW_PORT, LED_YELLOW_PIN, OUTPUT);
    pin_mode (LED_GREEN_PORT, LED_GREEN_PIN, OUTPUT);

    pin_mode (BOOT_BTN_PORT, BOOT_BTN_PIN, INPUT_PULLUP);
}


/* Test if bootloader pin is activated */
bool board_button_pressed(void)
{
  return digital_read (BOOT_BTN_PORT, BOOT_BTN_PIN) == 0;
}

void board_notify (tBootloaderNotification event)
{
  switch (event)
  {
    case bn_protocol_idle:
      // flash led
      digital_write (LED_GREEN_PORT, LED_GREEN_PIN, 1);
      break;
    case bn_user_code_invalid:
        digital_write (LED_YELLOW_PORT, LED_YELLOW_PIN, 1);
        break;
    default:
        break;
  }
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------


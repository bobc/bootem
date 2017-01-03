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
// Description: For Smoothieboard
//
// **************************************************************************

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include "bootloader.h"

// Platform - NXP
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "ios.h"

// Board specific
#include "board.h"
#include "config_pins.h"

// --------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Local defines
// --------------------------------------------------------------------------

//
#define extruder_heater_off()   digital_write(EXTRUDER_0_HEATER_PORT, EXTRUDER_0_HEATER_PIN, 0);
#define heated_bed_off()        digital_write(HEATED_BED_0_HEATER_PORT, HEATED_BED_0_HEATER_PIN, 0);
#define extruder_fan_off()      digital_write(EXTRUDER_0_FAN_PORT, EXTRUDER_0_FAN_PIN, 0);
#define buzzer_off()            digital_write(BUZZER_PORT, BUZZER_PIN, 0);

#define x_disable()         digital_write(X_ENABLE_PORT, X_ENABLE_PIN, 1)
#define x_step()            digital_write(X_STEP_PORT, X_STEP_PIN, 1)
#define y_disable()         digital_write(Y_ENABLE_PORT, Y_ENABLE_PIN, 1)
#define y_step()            digital_write(Y_STEP_PORT, Y_STEP_PIN, 1)
#define y_unstep()            digital_write(Y_STEP_PORT, Y_STEP_PIN, 0)
#define z_disable()         digital_write(Z_ENABLE_PORT, Z_ENABLE_PIN, 1)
#define z_step()            digital_write(Z_STEP_PORT, Z_STEP_PIN, 1)
#define e_disable()         digital_write(E_ENABLE_PORT, E_ENABLE_PIN, 1)
#define e_step()            digital_write(E_STEP_PORT, E_STEP_PIN, 1)

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


/*
 * Disable some pins like the ones for heaters while in bootloader, if not, the heaters would be ON
 */
static void set_safe_outputs(void)
{
    /* Extruder 0 Heater pin */
	GPIO_SetDir(EXTRUDER_0_HEATER_PORT, EXTRUDER_0_HEATER_PIN, OUTPUT);
    extruder_heater_off();
    /* Heated Bed 0 Heater pin */
    GPIO_SetDir(HEATED_BED_0_HEATER_PORT, HEATED_BED_0_HEATER_PIN, OUTPUT);
    heated_bed_off();
    /* Extruder fan pin */
    GPIO_SetDir(EXTRUDER_0_FAN_PORT, EXTRUDER_0_FAN_PIN, OUTPUT);
    extruder_fan_off();
    if (BUZZER_PORT != 0xFF)
    {
        /* Buzzer fan pin */
    	GPIO_SetDir(BUZZER_PORT, BUZZER_PIN, OUTPUT);
        buzzer_off();
    }
    if (STEPPERS_RESET_PORT != 0xFF)
    {
        /* Disable reset for all stepper motors */
        pin_mode(STEPPERS_RESET_PORT, STEPPERS_RESET_PIN, OUTPUT);
        digital_write(STEPPERS_RESET_PORT, STEPPERS_RESET_PIN, 1);
    }
    /* Disable all motors BUT enable step pin, so each LED will be ON */
    GPIO_SetDir(X_STEP_PORT, X_STEP_PIN, OUTPUT);
    GPIO_SetDir(X_ENABLE_PORT, X_ENABLE_PIN, OUTPUT);
    x_disable();
    x_step();

    GPIO_SetDir(Y_STEP_PORT, Y_STEP_PIN, OUTPUT);
    GPIO_SetDir(Y_ENABLE_PORT, Y_ENABLE_PIN, OUTPUT);
    y_disable();
    y_unstep();

    GPIO_SetDir(Z_STEP_PORT, Z_STEP_PIN, OUTPUT);
    GPIO_SetDir(Z_ENABLE_PORT, Z_ENABLE_PIN, OUTPUT);
    z_disable();
    z_step();

    GPIO_SetDir(E_STEP_PORT, E_STEP_PIN, OUTPUT);
    GPIO_SetDir(E_ENABLE_PORT, E_ENABLE_PIN, OUTPUT);
    e_disable();
    e_step();
}

static void configure_boot_button (void)
{
    /* Configure bootloader IO button */
    PINSEL_CFG_Type pin;
    pin.Portnum = BOOT_BUTTON_PORT;
    pin.Pinnum = BOOT_BUTTON_PIN;
    pin.Funcnum = PINSEL_FUNC_0;
    pin.Pinmode = PINSEL_PINMODE_PULLUP;
    pin.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin);

    /* set as input */
    GPIO_SetDir(BOOT_BUTTON_PORT, 1<<BOOT_BUTTON_PIN, INPUT);
}

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
	set_safe_outputs();
	configure_boot_button();
}


bool board_button_pressed(void)
{
    /* Test if bootloader pin is activated */
	/* NB this assumes active low input */
    if(GPIO_ReadValue(BOOT_BUTTON_PORT) & (1<<BOOT_BUTTON_PIN))
        return false;
    else
    	return true;
}

void board_notify (tBootloaderNotification event)
{
  switch (event)
  {
    case bn_protocol_idle:
      // flash led
      break;
  }
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------


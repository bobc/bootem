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
// Description:
//
// **************************************************************************

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

// CMSIS
#include "LPC17xx.h"
#include "core_cm3.h"

// NXP
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#include "uart.h"
#include "target.h"

// --------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Local defines
// --------------------------------------------------------------------------

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
void hw_init (void)
{
    // DeInit NVIC and SCBNVIC
    NVIC_DeInit();
    NVIC_SCBDeInit();

    /* Configure the NVIC Preemption Priority Bits:
     * two (2) bits of preemption priority, six (6) bits of sub-priority.
     * Since the Number of Bits used for Priority Levels is five (5), so the
     * actual bit number of sub-priority is three (3)
     */
    NVIC_SetPriorityGrouping(0x05);

    NVIC_SetVTOR(0);

    uart_init();
}

/** @brief
 *  @param[in]
 *  @param[out]
 *  @return
 */
void hw_reboot (void)
{
    NVIC_SystemReset();
}


int hw_read_part_id (uint8_t *buf)
{
  uint32_t part_id = iap_read_part_id();

  buf[0] = part_id >> 24;
  buf[1] = part_id >> 16 & 0xff;
  buf[2] = part_id >> 8 & 0xff;
  buf[3] = part_id & 0xff;

  return 4;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------


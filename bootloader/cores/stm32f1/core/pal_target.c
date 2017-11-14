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

#include "stm32f10x.h"

// CMSIS
#include "core_cm3.h"

#include "pal_uart.h"
#include "pal_target.h"

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
    //NVIC_DeInit();
    //NVIC_SCBDeInit();

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_SetPriorityGrouping(0);

    //NVIC_SetVTOR(0);
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, 0);

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
  uint32_t part_id = DBGMCU->IDCODE & DBGMCU_IDCODE_DEV_ID;

  buf[0] = part_id >> 8 & 0xff;
  buf[1] = part_id & 0xff;

  return 2;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------


/*
  Contributors:
  Copyright (c) 2014 Bob Cousins bobcousins42@googlemail.com
*/
/* **************************************************************************
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
// Description:
//
// **************************************************************************

#ifndef _PAL_TARGET_H
#define _PAL_TARGET_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include <stdint.h>

#include "flash.h"

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------


// Reserve 16 KiB for bootlaoder

#define BOOTLOADER_SIZE   0x4000

#define USER_FLASH_START  0x4000
#define USER_FLASH_END    P_FLASH_SIZE

#define USER_FLASH_SIZE  ((USER_FLASH_END - USER_FLASH_START) + 1)

// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

void hw_init (void);
void hw_reboot (void);
int hw_read_part_id (uint8_t *buf);

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

#endif // _PAL_TARGET_H


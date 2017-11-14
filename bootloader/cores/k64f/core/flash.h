/*
 * Copyright (c) 2017 Bob Cousins bobcousins42@googlemail.com
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

/*
 * Description:
 *
 */
 
#ifndef _FLASH_H
#define _FLASH_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

#include "MK64F12_features.h"
#include "fsl_device_registers.h"

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

/* Program Flash block information */
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
#define P_SECTOR_SIZE           FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE

// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------
bool flash_init(void);
bool flash_finalize(void);
bool flash_erase (uint32_t offset, uint32_t size);
bool flash_write (uint32_t offset, uint32_t size, uint8_t *data);

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

#endif // _FLASH_H


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

#ifndef _BOOT_API_H
#define _BOOT_API_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

#define BCA_SIZE 16

#define BOOT_MAGIC_START 0xb0015555
#define BOOT_MAGIC_END   0xb001eeee

#define BOOT_REQ_NOP                0
#define BOOT_REQ_ENTER_BOOTLOADER   1

#define BOOT_RES_NONE               0
#define BOOT_RES_NORMAL_BOOT        1
#define BOOT_RES_FIRMWARE_UPLOADED  2

typedef union {

  uint32_t AsLong [BCA_SIZE];

  struct {
    uint32_t BootMagicStart;  // should be BOOT_MAGIC_START
    uint32_t Length;          // length in uint32_t
    uint32_t Request[6];
    uint32_t Result[6];
    uint32_t Checksum;        // Sum of all data = 0
    uint32_t BootMagicEnd;    // should be BOOT_MAGIC_START
  } ByField;

} tBootCommunicationArea;

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

extern tBootCommunicationArea boot_communication_area;

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

bool is_bca_valid (void);
bool bca_download_request (void);

void initialise_bca (void);
void bca_set_request (uint32_t boot_req);
void bca_set_response (uint32_t boot_response);


// --------------------------------------------------------------------------
// End
// --------------------------------------------------------------------------

#endif // _BOOT_API_H


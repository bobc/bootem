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

// **************************************************************************
// Description:
//
// **************************************************************************

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include "boot_api.h"

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

// NB: must be no init
__attribute__ ((section(".boot_communication_area")))
  tBootCommunicationArea boot_communication_area;

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

static bool bca_calc_sum (tBootCommunicationArea *pbca)
{
  uint32_t csum = pbca->AsLong[0];
  for (int j=1; j < BCA_SIZE; j++)
    csum = csum ^ pbca->AsLong[j];
  return csum;
}

/** @brief
 *  @param[in]
 *  @param[out]
 *  @return
 */
bool is_bca_valid (void)
{
  uint32_t csum = bca_calc_sum (&boot_communication_area);
  return csum == 0;
}

bool bca_download_request (void)
{
  if (is_bca_valid())
  {
    if (boot_communication_area.ByField.Request[0] == BOOT_REQ_ENTER_BOOTLOADER)
      return true;
  }
  return false;
}

void initialise_bca (void)
{
  for (int j=0; j < BCA_SIZE; j++)
    boot_communication_area.AsLong[j] = 0;

  boot_communication_area.ByField.BootMagicStart = BOOT_MAGIC_START;
  boot_communication_area.ByField.Length = BCA_SIZE;
  boot_communication_area.ByField.BootMagicEnd = BOOT_MAGIC_END;

  boot_communication_area.ByField.Checksum = bca_calc_sum(&boot_communication_area) ^ 0xFFFFFFFF;

}

void bca_set_request (uint32_t boot_req)
{
  boot_communication_area.ByField.Request[0] = boot_req;
  boot_communication_area.ByField.Checksum = bca_calc_sum(&boot_communication_area) ^ 0xFFFFFFFF;
}

void bca_set_response (uint32_t boot_response)
{
  boot_communication_area.ByField.Result[0] = boot_response;
  boot_communication_area.ByField.Checksum = bca_calc_sum(&boot_communication_area) ^ 0xFFFFFFFF;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------




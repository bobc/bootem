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
 
#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------

typedef enum {
  bn_bootloader_requested,
  bn_user_code_valid,
  bn_user_code_invalid,
  bn_jumping_to_user_code,
  bn_protocol_connected,
  bn_protocol_idle
} tBootloaderNotification;

// --------------------------------------------------------------------------
// Public Variables
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

#endif // _BOOTLOADER_H


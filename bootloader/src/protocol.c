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

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "protocol.h"
#include "target.h"

// --------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Local defines
// --------------------------------------------------------------------------
#define RECV_BUFFER_SIZE    512

#define CMD_GET_INFO        0x00
#define CMD_GET_VERSION     0x01
#define CMD_GET_ID          0x02
#define CMD_READ_FLASH      0x11
#define CMD_GO              0x21
#define CMD_WRITE_FLASH     0x31
#define CMD_ERASE_FLASH     0x43

#define RES_NAK             0x1F
#define RES_ACK             0x79

#define SYNC_START          0x7F

#define INFO_VERSION_MAJOR  0x0
#define INFO_VERSION_MINOR  0x1

#define INFO_VERSION        ((INFO_VERSION_MAJOR << 4) | INFO_VERSION_MINOR)

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

static uint8_t buf[RECV_BUFFER_SIZE];

static uint32_t addr;

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


static bool get_address(void)
{
  unsigned char crc;

  if (uart_get_data(buf, 5) == 0)
  {
    crc = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
    if (crc == buf[4])
    {
      addr = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
      uart_send_byte(RES_ACK);
      return true;
    }
    else
    {
      uart_send_byte(RES_NAK);
      return false;
    }
  }
  else
    return false;
}

void bl_wait_for_sync(void)
{
  uint8_t b;

  while (1)
  {
    if (uart_get_data(&b, 1) == 0)
      if (b == SYNC_START)
      {
        uart_send_byte(RES_ACK);
        return;
      }
  }
}

int bl_get_cmd(void)
{
  if (uart_get_data(buf, 2) == 0)
  {
    if (buf[0] != (buf[1] ^ 0xFF))
    {
      uart_send_byte(RES_NAK);
      return -2;
    }
    else
    {
      // ACK will be sent later if command is supported
      return 0;
    }
  }
  return -1;
}

static uint8_t info_buf [] = {
    0,
    INFO_VERSION,
    CMD_GET_INFO,
    CMD_GET_VERSION,
    CMD_GET_ID,
    CMD_GO,
    CMD_READ_FLASH,
    CMD_WRITE_FLASH,
    CMD_ERASE_FLASH,
    RES_ACK
};

void bl_get_info(void)
{
  uart_send_byte(RES_ACK);
  info_buf[0] = sizeof(info_buf)-3;
  uart_send_data (info_buf, sizeof(info_buf));
}

static uint8_t buf_version [] = {
    INFO_VERSION,
    0, 0,
    RES_ACK
};

void bl_get_version(void)
{
  uart_send_byte(RES_ACK);
  uart_send_data(buf_version, sizeof(buf_version));
}

void bl_get_id(void)
{
  uart_send_byte(RES_ACK);

  hw_read_part_id (buf);

  if (buf[0]==0)
    buf[1] = 0;
  else
    buf[0]--;

  uart_send_data(buf, buf[0]+1);

  uart_send_byte(RES_ACK);
}

void bl_go(void)
{
  uart_send_byte(RES_ACK);

  if (get_address())
  {
    execute_user_code ();
  }
}


void bl_write_flash()
{
  int i;
  unsigned int len;
  unsigned char crc;

  uart_send_byte(RES_ACK);

  // get address
  if (get_address())
  {
    // get data len
    if (uart_get_data(buf, 1) == 0)
    {
      len = buf[0] + 1;
      crc = buf[0];

      // get data and crc
      if (uart_get_data(buf, len + 1) == 0)
      {
        for (i = 0; i < len + 1; i++)
        {
          crc = crc ^ buf[i];
        }

        if (crc == 0)
        {
          //write flash
          if (addr < (USER_FLASH_END - USER_FLASH_START + 1))
          {
            write_flash(USER_FLASH_START + addr, buf, len);
          }
          uart_send_byte(RES_ACK);
        }
        else
        {
          uart_send_byte(RES_NAK);
        }
      }
    }
  }
}

void bl_read_flash()
{
  int i;
  unsigned int len = 0;

  uart_send_byte(RES_ACK);

  // get address
  if (get_address())
  {
    // get data len and crc
    if (uart_get_data(buf, 2) == 0)
    {
      if ((buf[0] ^ 0xFF) == buf[1])
      {
        len = buf[0] + 1;
        uart_send_byte(RES_ACK);
      }
      else
      {
        uart_send_byte(RES_NAK);
        return;
      }
    }
    // send data
    for (i = 0; i < len; i++)
    {
      buf[i] = *(unsigned char *) (addr++);
    }
    uart_send_data(buf, len);
  }
}

void bl_erase_flash()
{
  int i;
  unsigned int len;
  unsigned char crc;
  int res;

  uart_send_byte(RES_ACK);

  // get len
  if (uart_get_data(buf, 1) == 0)
  {
    len = (buf[0] + 1) & 0xFF; // maximum sector number is 256

    crc = buf[0];
    // get sector sequence and crc
    if (uart_get_data(buf, len + 1) == 0)
    {
      for (i = 0; i < len + 1; i++)
      {
        crc = crc ^ buf[i];
      }

      // erase flash sectors
      if (len == 0)
      {
        if (buf[0] == 0)
        {
          res = erase_user_flash();
          uart_send_byte(RES_ACK);
        }
        else
          uart_send_byte(RES_NAK);
      }
      else
      {
        if (crc == 0)
        {
          for (i = 0; i < len; i++)
          {
            if ((sector_start_map[buf[i]] >= USER_FLASH_START) && (sector_end_map[buf[i]] < USER_FLASH_END))
            {
              erase_sectors(buf[i], buf[i]);
            }
          }
          uart_send_byte(RES_ACK);
        }
        else
        {
          uart_send_byte(RES_NAK);
        }
      }
    }
  }
}

int run_protocol(void)
{
  bl_wait_for_sync();

  while (1)
  {
    if (bl_get_cmd() == 0)
    {
      switch (buf[0])
      {
      case CMD_GET_INFO:
        bl_get_info();
        break;
      case CMD_GET_VERSION:
        bl_get_version();
        break;
      case CMD_GET_ID:
        bl_get_id();
        break;
      case CMD_GO:
        bl_go();
        break;
      case CMD_WRITE_FLASH:
        bl_write_flash();
        break;
      case CMD_READ_FLASH:
        bl_read_flash();
        break;
      case CMD_ERASE_FLASH:
        bl_erase_flash();
        break;
      default:
        uart_send_byte(RES_NAK);
        break;
      }
    }
  }
  return 0;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

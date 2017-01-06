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
#define COMM_BUFFER_SIZE    (512+8)

#define CMD_GET_COMMANDS    0x01
#define CMD_GET_VERSION     0x02
#define CMD_GET_ID          0x03

#define CMD_READ_MEMORY     0x10
#define CMD_WRITE_FLASH     0x20
#define CMD_ERASE_FLASH     0x30

#define CMD_ENTER_PROG_MODE 0x40
#define CMD_EXIT_PROG_MODE  0x41
#define CMD_GO              0x42

#define PROTO_VERSION_MAJOR  1
#define PROTO_VERSION_MINOR  0

#define BOOTLOADER_VERSION_MAJOR  0
#define BOOTLOADER_VERSION_MINOR  2

#define FLAG    0x7E
#define ESCAPE  0x7D

#define RES_INVALID_COMMAND         0x80
#define RES_INVALID_REQUEST         0x81
#define RES_INVALID_MODE            0x82

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

static uint8_t buf[COMM_BUFFER_SIZE];
static int rx_count;

static uint32_t addr;

static bool  programming_mode = false;

static const uint8_t signature [] = {'B','O','B','P'};

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

void copy_bytes (uint8_t *dest, uint8_t *src, int count )
{
  while (count--)
    *dest++ = *src++;
}

// calculate Fletcher16 checksum, result == 0 if valid.
uint16_t Fletcher16(uint8_t *data, int count)
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;

   while (count--)
   {
      sum1 = (sum1 + *data++) % 255;
      sum2 = (sum2 + sum1) % 255;
   }

   return (sum2 << 8) | sum1;
}

void append_checksum (uint8_t *data, int count)
{
  uint16_t csum;
  uint8_t c0,c1,f0,f1;

  csum = Fletcher16 (data, count);
  f0 = csum & 0xff;
  f1 = (csum >> 8) & 0xff;
  c0 = 0xff - (( f0 + f1) % 0xff);
  c1 = 0xff - (( f0 + c0 ) % 0xff);

  data[count] = c0;
  data[count+1] = c1;
}

static void bl_send_frame (uint8_t *data, int count)
{
  if (count > COMM_BUFFER_SIZE-2)
    return;

  append_checksum (data, count);
  count += 2;

  uart_send_byte (FLAG);
  while (count--)
  {
    if ( (*data == FLAG) || (*data == ESCAPE) )
    {
      uart_send_byte (ESCAPE);
      uart_send_byte ((*data) ^ 0x20);
    }
    else
      uart_send_byte (*data);
    data++;
  }
  uart_send_byte (FLAG);
}

static bool get_address(int offset)
{
  //todo: if (offset +3 < rx_count

  addr = buf[offset+3] | (buf[offset+2] << 8) | (buf[offset+1] << 16) | (buf[offset] << 24);
  return true;
}

// get a word value (MSB)
static int get_word (int offset)
{
  //todo: if (offset +1 < rx_count
  return buf[offset+1] | (buf[offset] << 8);
}

int bl_get_cmd(void)
{
  bool end_frame = false;
  int count=0;
  uint8_t b;

  do {
    uart_get_data(&b, 1);
  } while (b == FLAG);

  buf[count++] = b;

  while (!end_frame)
  {
    if (uart_get_data(&b, 1) == 0)
    {
      if (b == FLAG)
      {
        end_frame = true;
      }
      else
      {
        if (b == ESCAPE)
        {
          uart_get_data(&b, 1);
          b = b ^ 0x20;
        }
        if (count < COMM_BUFFER_SIZE)
          buf[count++] = b;
      }
    }
  }

  if ( (count>=3) && (Fletcher16 (buf, count) == 0) )
  {
    rx_count = count-2;
    return 0;
  }
  else
  {
    rx_count = 0;
    return -1;
  }
}

static uint8_t info_commands [] = {
    CMD_GET_COMMANDS,
    CMD_GET_VERSION,
    CMD_GET_ID,
    CMD_GO,
    CMD_READ_MEMORY,
    CMD_WRITE_FLASH,
    CMD_ERASE_FLASH
};

void bl_get_info(void)
{
  buf[0] = CMD_GET_COMMANDS;
  copy_bytes (&buf[1], info_commands, sizeof(info_commands));

  bl_send_frame (buf, sizeof(info_commands)+1);
}

void bl_get_version(void)
{
  buf[0] = CMD_GET_VERSION;
  copy_bytes (&buf[1], signature, 4);
  buf[5] = PROTO_VERSION_MAJOR;
  buf[6] = PROTO_VERSION_MINOR;
  buf[7] = BOOTLOADER_VERSION_MAJOR;
  buf[8] = BOOTLOADER_VERSION_MINOR;

  bl_send_frame (buf, 9);
}

void bl_send_error (uint8_t response_code)
{
  buf[0] = response_code;

  bl_send_frame (buf, 1);
}

void bl_get_id(void)
{
  int count=0;

  buf[0] = CMD_GET_ID;
  count = hw_read_part_id (&buf[1]);
  count++;

  bl_send_frame (buf, count);
}

void bl_go(void)
{
  bl_send_frame(buf, 1);

  execute_user_code ();
}


void bl_write_flash()
{
  int i;
  unsigned int len;
  unsigned char crc;

  // get address
  if (get_address(1))
  {
    len = rx_count - 5;
    if (len > 0)
    {
      //write flash
      if (addr < (USER_FLASH_END - USER_FLASH_START + 1))
      {
        write_flash(USER_FLASH_START + addr, &buf[5], len);
      }

      buf[0] = CMD_WRITE_FLASH;
      bl_send_frame(buf, 1);
      return;
    }
  }

  buf[0] = RES_INVALID_REQUEST;
  bl_send_frame(buf, 1);
}

void bl_read_flash()
{
  int j;
  int len = 0;

  // get address
  if (get_address(1))
  {
    // get data len
    len = get_word (5);

    //todo: check len

    // send data
    for (j = 0; j < len; j++)
    {
      buf[j+1] = *(unsigned char *) (addr++);
    }

    bl_send_frame(buf, 1+len);
    return;
  }

  buf[0] = RES_INVALID_REQUEST;
  bl_send_frame(buf, 1);
}

void bl_erase_flash()
{
  int res;

  res = erase_user_flash();

  bl_send_frame(buf, 1);
}

int run_protocol(void)
{
//  bl_wait_for_sync();

  while (1)
  {
    if (bl_get_cmd() == 0)
    {
      switch (buf[0])
      {
      case CMD_GET_COMMANDS:
        bl_get_info();
        break;
      case CMD_GET_VERSION:
        bl_get_version();
        break;
      case CMD_GET_ID:
        bl_get_id();
        break;
      case CMD_READ_MEMORY:
        bl_read_flash();
        break;
      case CMD_WRITE_FLASH:
        bl_write_flash();
        break;
      case CMD_ERASE_FLASH:
        bl_erase_flash();
        break;
      case CMD_GO:
        bl_go();
        break;
      default:
        bl_send_error(RES_INVALID_COMMAND);
        break;
      }
    }
  }
  return 0;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------

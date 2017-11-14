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

#include "pal_flash_iap.h"
#include "pal_target.h"

#define COMPUTE_CHECKSUM

static bool verify_checksum (uint32_t *pmem)
{
  uint32_t checksum, i;
/*
 * The reserved Cortex-M3 exception vector location 7 (offset 0x001C
 * in the vector table) should contain the 2’s complement of the
 * checksum of table entries 0 through 6. This causes the checksum
 * of the first 8 table entries to be 0. This code checksums the
 * first 8 locations of the start of user flash. If the result is 0,
 * then the contents is deemed a 'valid' image.
 */
  checksum = 0;
  for (i = 0; i <= 7; i++)
  {
    checksum += *pmem;
    pmem++;
  }
  return checksum == 0;
}

/* Jump to user application.
 *
 * this seems to fix an issue with handoff after poweroff
 * found here http://knowledgebase.nxp.trimm.net/showthread.php?t=2869
 *
 * SP (stack pointer) register must be set correctly before jump to the
 *  user application: http://www.keil.com/forum/17342/
 *
 * Set PC to the address contained in the second word
 * of user flash
 */

static void jump_to_user_code (uint32_t base_addr)
{
  __asm("LDR SP, [%0]" : : "r"(base_addr));
  __asm("LDR PC, [%0, #4]" : : "r"(base_addr));
  // never returns
}


uint32_t write_flash(uint32_t dest, char * src, uint32_t no_of_bytes)
{
  if (flash_write (dest, no_of_bytes, src))
    return IAP_CMD_SUCCESS;
  else
    return IAP_GENERAL_ERROR;
}


void execute_user_code(void)
{
  uint32_t addr = (uint32_t)USER_FLASH_START;

  // TODO : need delay?
  // delay
  //! delay_loop(3000000);

  /* Change the Vector Table to the USER_FLASH_START
  in case the user application uses interrupts */
  SCB->VTOR = addr & 0x1FFFFF80;

  // TODO: switch to default/internal clock here?

  // reset pipeline, sync bus and memory access
  __asm (
       "dmb\n"
       "dsb\n"
       "isb\n"
      );

  jump_to_user_code(addr);
}

bool user_code_present(void)
{
#ifdef COMPUTE_CHECKSUM
  if (!verify_checksum ((uint32_t *)USER_FLASH_START) )
  {
    return false;
  }
#else
  iap_blank_check (USER_START_SECTOR, USER_START_SECTOR);
  if( result_table[0] == IAP_CMD_SUCCESS )
    return false;
#endif

  return true;
}


uint32_t erase_user_flash(void)
{
  if (flash_erase (USER_FLASH_START, USER_FLASH_SIZE))
    return IAP_CMD_SUCCESS;
  else
    return IAP_GENERAL_ERROR;
}


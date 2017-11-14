//-----------------------------------------------------------------------------
//TODO
//-----------------------------------------------------------------------------


#include "pal_flash_iap.h"
#include "pal_target.h"

#include "stm32f10x.h"

// If COMPUTE_CHECKSUM is defined, then code will check that checksum
// contained within binary image is valid.
#define COMPUTE_CHECKSUM

/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

//#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08008000)
//#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800C000)

#ifdef STM32F10X_XL
 #define BANK2_WRITE_START_ADDR   ((uint32_t)0x08088000)
 #define BANK2_WRITE_END_ADDR     ((uint32_t)0x0808C000)
#endif /* STM32F10X_XL */


#define FLASH_BUF_SIZE FLASH_PAGE_SIZE

static char flash_buf [FLASH_BUF_SIZE];
static unsigned byte_ctr = 0;
static unsigned saved_address = 0;


static uint32_t delay_loop(uint32_t count)
{
    volatile uint32_t j, del;
    for(j=0; j<count; ++j) {
        del=j; // volatiles, so the compiler will not optimize the loop
    }
    return del;
}

uint32_t write_flash(uint32_t dest, char * src, uint32_t no_of_bytes)
{
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

    unsigned i;

  if (byte_ctr==0)
    saved_address = dest;

  for(i = 0; i<no_of_bytes; i++)
  {
    flash_buf[byte_ctr++] = *(src+i);
  }

  // NB: this assumes that FLASH_BUF_SIZE is an integer multiple of no_of_bytes

  if(byte_ctr == FLASH_BUF_SIZE)
  {
    /* We have accumulated enough bytes to trigger a flash write */

    /* Reset byte counter */
    byte_ctr = 0;

    /* Unlock the Flash Bank1 Program Erase controller */
    FLASH_UnlockBank1();

    /* Clear All pending flags */
    FLASH_ClearFlag (FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    /* Erase the FLASH page */
    FLASHStatus = FLASH_ErasePage (saved_address);

    if (FLASHStatus == FLASH_COMPLETE)
    {
        uint32_t address = saved_address;

        /* Program Flash */
        for (i=0; i < FLASH_BUF_SIZE && (FLASHStatus == FLASH_COMPLETE); i+=4)
        {
            uint32_t Data = *(uint32_t *)&flash_buf [i];
            FLASHStatus = FLASH_ProgramWord (address, Data);
            address += 4;
        }

        // verify
        address = saved_address;
        for (i=0; i < FLASH_BUF_SIZE && (FLASHStatus == FLASH_COMPLETE); i+=4)
        {
            uint32_t Data = *(uint32_t *)&flash_buf [i];
            if((*(__IO uint32_t*) address) != Data)
            {
                return IAP_COMPARE_ERROR;
            }
            address += 4;
        }
        FLASH_LockBank1();
    }
  }

  switch (FLASHStatus)
  {
      case FLASH_BUSY:        return IAP_BUSY;
      case FLASH_ERROR_PG:    return IAP_GENERAL_ERROR;
      case FLASH_ERROR_WRP:   return IAP_GENERAL_ERROR;
      case FLASH_COMPLETE:    return IAP_CMD_SUCCESS;
      case FLASH_TIMEOUT:     return IAP_BUSY;
      default:
          return IAP_GENERAL_ERROR;
  }
}

void flash_finalise(void)
{
    if (byte_ctr > 0)
    {
        //TODO: write flash
    }
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
	asm("LDR SP, [%0]" : : "r"(base_addr));
	asm("LDR PC, [%0, #4]" : : "r"(base_addr));
	// never returns
}

void execute_user_code(void)
{
  // void (*user_code_entry)(void);
  // unsigned *p;  // used for loading address of reset handler from user flash

  uint32_t addr=(uint32_t)USER_FLASH_START;
  // delay
  delay_loop(3000000);

  /* Change the Vector Table to the USER_FLASH_START
  in case the user application uses interrupts */
  SCB->VTOR = addr & 0x1FFFFF80;

  //
	// switch to RC generator

  delay_loop(1000);

	// reset pipeline, sync bus and memory access
	__asm (
		   "dmb\n"
		   "dsb\n"
		   "isb\n"
		  );

  jump_to_user_code(addr);
}

static bool verify_checksum (uint32_t *pmem)
{
  uint32_t checksum, i;
/*
 * The reserved Cortex-M3 exception vector location 7 (offset 0x001C
 * in the vector table) should contain the 2's complement of the
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

bool user_code_present(void)
{
#ifdef COMPUTE_CHECKSUM
  if (!verify_checksum ((uint32_t *)USER_FLASH_START) )
  {
    return false;
  }
#else
  //iap_blank_check (USER_START_SECTOR, USER_START_SECTOR);
  //if( result_table[0] == IAP_CMD_SUCCESS )
    return false;
#endif

  return true;
}


uint32_t erase_sectors(uint32_t start, uint32_t end)
{

  return IAP_INVALID_COMMAND;
}


uint32_t erase_user_flash(void)
{
    uint32_t NbrOfPage = 0x00;
    uint32_t EraseCounter = 0x00;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

    /* Unlock the Flash Bank1 Program Erase controller */
    FLASH_UnlockBank1();

    /* Define the number of page to be erased */
    NbrOfPage = (USER_FLASH_END - USER_FLASH_START) / FLASH_PAGE_SIZE;

    /* Clear All pending flags */
    FLASH_ClearFlag (FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    /* Erase the FLASH pages */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
      FLASHStatus = FLASH_ErasePage (USER_FLASH_START + (FLASH_PAGE_SIZE * EraseCounter));
    }

    FLASH_LockBank1();

    switch (FLASHStatus)
    {
        case FLASH_BUSY:        return IAP_BUSY;
        case FLASH_ERROR_PG:    return IAP_GENERAL_ERROR;
        case FLASH_ERROR_WRP:   return IAP_GENERAL_ERROR;
        case FLASH_COMPLETE:    return IAP_CMD_SUCCESS;
        case FLASH_TIMEOUT:     return IAP_BUSY;
        default:
            return IAP_GENERAL_ERROR;
    }
}




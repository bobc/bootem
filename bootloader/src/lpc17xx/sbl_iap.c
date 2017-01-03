//-----------------------------------------------------------------------------
// Software that is described herein is for illustrative purposes only
// which provides customers with programming information regarding the
// products. This software is supplied "AS IS" without any warranties.
// NXP Semiconductors assumes no responsibility or liability for the
// use of the software, conveys no license or title under any patent,
// copyright, or mask work right to the product. NXP Semiconductors
// reserves the right to make changes in the software without
// notification. NXP Semiconductors also make no representation or
// warranty that such application will be suitable for the specified
// use without further testing or modification.
//-----------------------------------------------------------------------------

#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#include "sbl_iap.h"
#include "sbl_config.h"

//#include "config_pins.h"


// If COMPUTE_BINARY_CHECKSUM is defined, then code will check that checksum
// contained within binary image is valid.
//#define COMPUTE_BINARY_CHECKSUM

#define IAP_ADDRESS 0x1FFF1FF1

typedef enum
{
	PREPARE_SECTOR_FOR_WRITE= 50,
	COPY_RAM_TO_FLASH		= 51,
	ERASE_SECTOR			= 52,
	BLANK_CHECK_SECTOR		= 53,
	READ_PART_ID			= 54,
	READ_BOOT_VER			= 55,
	COMPARE                 = 56,
	REINVOKE_ISP			= 57,

	READ_UID                = 58,
	ERASE_PAGE              = 59,
	EEPROM_WRITE            = 61,
	EEPROM_READ             = 62
} IAP_Command_Code;


const unsigned sector_start_map[NUM_FLASH_SECTOR] = {
	SECTOR_0_START, SECTOR_1_START, SECTOR_2_START, SECTOR_3_START, SECTOR_4_START,
	SECTOR_5_START, SECTOR_6_START, SECTOR_7_START, SECTOR_8_START, SECTOR_9_START,
	SECTOR_10_START,SECTOR_11_START,SECTOR_12_START,SECTOR_13_START,SECTOR_14_START,
	SECTOR_15_START,SECTOR_16_START,SECTOR_17_START,SECTOR_18_START,SECTOR_19_START,
	SECTOR_20_START,SECTOR_21_START,SECTOR_22_START,SECTOR_23_START,SECTOR_24_START,
	SECTOR_25_START,SECTOR_26_START,SECTOR_27_START,SECTOR_28_START,SECTOR_29_START
};

const unsigned sector_end_map[NUM_FLASH_SECTOR] = {
	SECTOR_0_END,SECTOR_1_END,SECTOR_2_END,SECTOR_3_END,SECTOR_4_END,
	SECTOR_5_END,SECTOR_6_END,SECTOR_7_END,SECTOR_8_END,SECTOR_9_END,
	SECTOR_10_END,SECTOR_11_END,SECTOR_12_END,SECTOR_13_END,SECTOR_14_END,
	SECTOR_15_END,SECTOR_16_END,SECTOR_17_END,SECTOR_18_END,SECTOR_19_END,
	SECTOR_20_END,SECTOR_21_END,SECTOR_22_END,SECTOR_23_END,SECTOR_24_END,
	SECTOR_25_END,SECTOR_26_END,SECTOR_27_END,SECTOR_28_END,SECTOR_29_END
};

static char flash_buf[FLASH_BUF_SIZE];
static unsigned byte_ctr = 0;
static unsigned saved_address = 0;

static unsigned param_table[5];
static unsigned result_table[5];


//static void iap_entry(unsigned param_tab[],unsigned result_tab[]);
//static void compare_data(unsigned cclk, unsigned dst, void * flash_data_buf, unsigned count);
//static void erase_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
//static void find_erase_prepare_sector(unsigned cclk, unsigned dst);
//static void prepare_sector(unsigned start_sector,unsigned end_sector,unsigned cclk);
//static void write_data(unsigned cclk,unsigned dst,void * flash_data_buf, unsigned count);

static void iap_entry(unsigned param_tab[], unsigned result_tab[])
{
  void (*iap)(unsigned [], unsigned []);

  iap = (void (*)(unsigned [], unsigned []))IAP_ADDRESS;
  iap(param_tab,result_tab);
}

static void iap_compare_data(unsigned cclk, uint32_t dst, void * flash_data_buf, uint32_t count)
{
  __disable_irq();
  param_table[0] = COMPARE;
  param_table[1] = dst;
  param_table[2] = (unsigned)flash_data_buf;
  param_table[3] = count;
  param_table[4] = cclk;
  iap_entry(param_table,result_table);
  __enable_irq();
}

void iap_erase_sectors(uint32_t start_sector, uint32_t end_sector)
{
  param_table[0] = ERASE_SECTOR;
  param_table[1] = start_sector;
  param_table[2] = end_sector;
  param_table[3] = SystemCoreClock/1000;
  iap_entry(param_table,result_table);
}


static void iap_prepare_sector(uint32_t start_sector,uint32_t end_sector,uint32_t cclk)
{
  param_table[0] = PREPARE_SECTOR_FOR_WRITE;
  param_table[1] = start_sector;
  param_table[2] = end_sector;
  param_table[3] = cclk;
  iap_entry(param_table,result_table);
}

static void iap_blank_check(uint32_t start_sector,uint32_t end_sector)
{
  param_table[0] = BLANK_CHECK_SECTOR;
  param_table[1] = start_sector;
  param_table[2] = end_sector;
  iap_entry(param_table, result_table);
}

static void iap_write_data(uint32_t cclk, uint32_t dst, void * flash_data_buf, uint32_t count)
{
  __disable_irq();
  param_table[0] = COPY_RAM_TO_FLASH;
  param_table[1] = dst;
  param_table[2] = (unsigned)flash_data_buf;
  param_table[3] = count;
  param_table[4] = cclk;
  iap_entry(param_table,result_table);
  __enable_irq();
}

static void find_erase_prepare_sector(uint32_t cclk, uint32_t dst)
{
  unsigned i;

  __disable_irq();
  result_table[0] = IAP_CMD_SUCCESS;
  for(i = USER_START_SECTOR; i <= USER_END_SECTOR; i++)
  {
    if(dst < sector_end_map[i])
    {
      if(dst == sector_start_map[i])
      {
        iap_prepare_sector(i, i, cclk);
        iap_erase_sectors (i, i);
      }
      iap_prepare_sector(i , i, cclk);
      __enable_irq();
      return;
    }
  }
  __enable_irq();
}


static uint32_t delay_loop(uint32_t count)
{
    volatile uint32_t j, del;
    for(j=0; j<count; ++j) {
        del=j; // volatiles, so the compiler will not optimize the loop
    }
    return del;
}

uint32_t iap_read_part_id (void)
{
    param_table[0] = READ_PART_ID;
    iap_entry(param_table,result_table);
    return result_table[1];
}

uint32_t iap_read_unique_id (void)
{
    param_table[0] = READ_UID;
    iap_entry(param_table,result_table);
    return result_table[1];
}


unsigned write_flash(uint32_t dest, char * src, uint32_t no_of_bytes)
{
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

    find_erase_prepare_sector(SystemCoreClock/1000, saved_address);
    if(result_table[0] != IAP_CMD_SUCCESS)
    {
      // DBG("Error: prepare sectors\n");
      return result_table[0];
    }
    iap_write_data( SystemCoreClock/1000, saved_address, (void *)flash_buf, FLASH_BUF_SIZE);
    if(result_table[0] != IAP_CMD_SUCCESS)
    {
      // DBG("Error: writing data\n");
      return result_table[0];
    }

    iap_compare_data( SystemCoreClock/1000, saved_address, (void *)flash_buf, FLASH_BUF_SIZE);
    if(result_table[0] != IAP_CMD_SUCCESS)
    {
      // DBG("Error: verifying data\n");
      return result_table[0];
    }

  }
  return IAP_CMD_SUCCESS;
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
  //void (*user_code_entry)(void);
//  unsigned *p;  // used for loading address of reset handler from user flash

  uint32_t addr=(uint32_t)USER_FLASH_START;
  // delay
  delay_loop(3000000);

  /* Change the Vector Table to the USER_FLASH_START
  in case the user application uses interrupts */
  SCB->VTOR = addr & 0x1FFFFF80;

  //
	// switch to RC generator
	LPC_SC->PLL0CON = 0x1; // disconnect PLL0
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;
	while (LPC_SC->PLL0STAT&(1<<25));
	LPC_SC->PLL0CON = 0x0;    // power down
	LPC_SC->PLL0FEED = 0xAA;
	LPC_SC->PLL0FEED = 0x55;
	while (LPC_SC->PLL0STAT&(1<<24));
	LPC_SC->FLASHCFG &= 0x0fff;  // This is the default flash read/write setting for IRC
	LPC_SC->FLASHCFG |= 0x5000;
	LPC_SC->CCLKCFG = 0x0;     //  Select the IRC as clk
	LPC_SC->CLKSRCSEL = 0x00;
	LPC_SC->SCS = 0x00;		    // not using XTAL anymore
	delay_loop(1000);
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
  iap_blank_check (USER_START_SECTOR, USER_START_SECTOR);
  if( result_table[0] == IAP_CMD_SUCCESS )
    return false;

#ifdef COMPUTE_BINARY_CHECKSUM
	unsigned *pmem, checksum,i;
/*
 * The reserved Cortex-M3 exception vector location 7 (offset 0x001C
 * in the vector table) should contain the 2’s complement of the
 * checksum of table entries 0 through 6. This causes the checksum
 * of the first 8 table entries to be 0. This code checksums the
 * first 8 locations of the start of user flash. If the result is 0,
 * then the contents is deemed a 'valid' image.
 */
  checksum = 0;
  pmem = (unsigned *)USER_FLASH_START;
  for (i = 0; i <= 7; i++)
  {
    checksum += *pmem;
    pmem++;
  }
  if (checksum != 0)
  {
    return (FALSE);
  }
  else
#endif

  {
    return true;
  }
}


void erase_sectors(uint32_t start, uint32_t end)
{
  iap_prepare_sector(start, end, SystemCoreClock/1000);
  iap_erase_sectors (start, end);
}

int erase_user_flash(void)
{
  iap_prepare_sector(USER_START_SECTOR, USER_END_SECTOR, SystemCoreClock/1000);
  iap_erase_sectors (USER_START_SECTOR, USER_END_SECTOR);

  return result_table[0];
}


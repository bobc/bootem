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


#include "SSD_FTFx.h"


#include "flash.h"

// --------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Local defines
// --------------------------------------------------------------------------

/* size of array to copy__Launch_Command function to.*/
/* It should be at least equal to actual size of __Launch_Command func */
/* User can change this value based on RAM size availability and actual size of __Launch_Command function */
#define LAUNCH_CMD_SIZE         0x100

/* Size of function used for callback.  Change this depending on the size of your function */
#define CALLBACK_SIZE           0x80

//
#define FTFx_REG_BASE           0x40020000
#define P_FLASH_BASE            0x00000000


/* Data Flash block information */
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS
#define FLEXNVM_SECTOR_SIZE     FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE
#define FLEXNVM_BLOCK_SIZE      FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SIZE
#define FLEXNVM_BLOCK_NUM       FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_COUNT

/* Flex Ram block information */
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS
#define EERAM_SIZE              FSL_FEATURE_FLASH_FLEX_RAM_SIZE

/* Other defines */
#define DEBUGENABLE               0x00

/* Has flash cache control in MCM module */
#if (FSL_FEATURE_FLASH_HAS_MCM_FLASH_CACHE_CONTROLS == 1)
    #define CACHE_DISABLE             BW_MCM_PLACR_DFCS(MCM_BASE, 1);
/* Has flash cache control in FMC module */
#elif (FSL_FEATURE_FLASH_HAS_FMC_FLASH_CACHE_CONTROLS == 1)
    #if defined(FMC_PFB1CR) && defined(FMC_PFB1CR_B1SEBE_MASK)
        #define CACHE_DISABLE     FMC_PFB0CR &= ~(FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);\
                                  FMC_PFB1CR &= ~(FMC_PFB1CR_B1SEBE_MASK | FMC_PFB1CR_B1IPE_MASK | FMC_PFB1CR_B1DPE_MASK | FMC_PFB1CR_B1ICE_MASK | FMC_PFB1CR_B1DCE_MASK);
    #else
        #define CACHE_DISABLE     FMC_PFB0CR &= ~(FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);
    #endif

    #if defined(FMC_PFB1CR) && defined(FMC_PFB1CR_B1SEBE_MASK)
        #define CACHE_ENABLE     FMC_PFB0CR |= (FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);\
                                  FMC_PFB1CR |= (FMC_PFB1CR_B1SEBE_MASK | FMC_PFB1CR_B1IPE_MASK | FMC_PFB1CR_B1DPE_MASK | FMC_PFB1CR_B1ICE_MASK | FMC_PFB1CR_B1DCE_MASK);
    #else
        #define CACHE_ENABLE     FMC_PFB0CR |= (FMC_PFB0CR_B0SEBE_MASK | FMC_PFB0CR_B0IPE_MASK | FMC_PFB0CR_B0DPE_MASK | FMC_PFB0CR_B0ICE_MASK | FMC_PFB0CR_B0DCE_MASK);
    #endif

#else
/* No cache in the device */
    #define CACHE_DISABLE
#endif


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

FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /* FTFx control register base */
    P_FLASH_BASE,           /* base address of PFlash block */
    P_FLASH_SIZE,           /* size of PFlash block */
    FLEXNVM_BASE,           /* base address of DFlash block */
    0,                      /* size of DFlash block */
    EERAM_BASE,             /* base address of EERAM block */
    0,                      /* size of EEE block */
    DEBUGENABLE,            /* background debug mode enable bit */
    NULL_CALLBACK           /* pointer to callback function */
};

pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

/* array to copy __Launch_Command func to RAM */
uint16_t __ram_func[LAUNCH_CMD_SIZE/2];
uint16_t __ram_for_callback[CALLBACK_SIZE/2]; /* length of this array depends on total size of the functions need to be copied to RAM*/

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

bool flash_init(void)
{
  uint32_t ret;

  ret = FlashInit(&flashSSDConfig);
  if (FTFx_OK != ret)
  {
    return false;
  }
  else
  {
    // Set CallBack to callback function
//!    flashSSDConfig.CallBack = (PCALLBACK)RelocateFunction((uint32_t)__ram_for_callback , CALLBACK_SIZE , (uint32_t)callback);
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_func , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);

    return true;
  }
}

bool flash_finalize(void)
{
  CACHE_ENABLE
}

bool flash_erase (uint32_t offset, uint32_t size)
{
  uint32_t ret;
  uint32_t destination;
  uint32_t end;
  uint16_t number;
  uint32_t margin_read_level;    /* 0=normal, 1=user - margin read for reading 1's */

  CACHE_DISABLE

  //flashSSDConfig.PFlashSize

  destination = flashSSDConfig.PFlashBase + offset;
  size = ( (size+FTFx_PSECTOR_SIZE-1) / FTFx_PSECTOR_SIZE) * FTFx_PSECTOR_SIZE;
  end  = destination + size;

  while (destination + FTFx_PSECTOR_SIZE < end)
  {
      size = FTFx_PSECTOR_SIZE;
      ret = FlashEraseSector(&flashSSDConfig, destination, size, g_FlashLaunchCommand);
      if (FTFx_OK != ret)
      {
//          ErrorTrap(ret);
          return false;
      }

      /* Verify section for several sector of PFLASH */
      number = FTFx_PSECTOR_SIZE / FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT;
      for(margin_read_level = 0; margin_read_level < 2; margin_read_level++)
      {
          ret = FlashVerifySection(&flashSSDConfig, destination, number, margin_read_level, g_FlashLaunchCommand);
          if (FTFx_OK != ret)
          {
//              ErrorTrap(ret);
              return false;
          }
      }

      /* print message for user */
//      printf("\n\r\tDemo:  Successfully Erased Sector 0x%x -> 0x%x", (unsigned int)destination, (unsigned int)(destination+size));

      destination += size;
  }

  return true;
}


bool flash_write (uint32_t offset, uint32_t data_size, uint8_t *data)
{
  uint32_t ret;
  uint32_t destination;
  uint32_t end;
  uint32_t size;
  uint32_t margin_read_level;    /* 0=normal, 1=user - margin read for reading 1's */
  uint32_t FailAddr;


  destination = flashSSDConfig.PFlashBase + offset;
  size = ( (data_size+FTFx_PSECTOR_SIZE-1) / FTFx_PSECTOR_SIZE) * FTFx_PSECTOR_SIZE;
  end = flashSSDConfig.PFlashBase + size;

  /* message for user */
  //printf("\n\n\r---->Running FlashProgram() and FlashProgramCheck()...");

  {
      ret = FlashProgram(&flashSSDConfig, destination, data_size, data, g_FlashLaunchCommand);
      if (FTFx_OK != ret)
      {
          //ErrorTrap(ret);
        return false;
      }

      /* Program Check user margin levels*/
      for (margin_read_level = 1; margin_read_level < 0x2; margin_read_level++)
      {
          ret = FlashProgramCheck(&flashSSDConfig, destination, data_size, data, &FailAddr, margin_read_level, g_FlashLaunchCommand);
          if (FTFx_OK != ret)
          {
              //ErrorTrap(ret);
            return false;
          }
      }

      //printf("\n\r\tSuccessfully Programmed and Verified Location 0x%x -> 0x%x", (unsigned int)destination, (unsigned int)(destination + size));

      //destination += BUFFER_SIZE_BYTE;
  }

  return true;
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------


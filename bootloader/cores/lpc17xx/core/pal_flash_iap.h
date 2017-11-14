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

#ifndef  _PAL_FLASH_IAP_H
#define  _PAL_FLASH_IAP_H

#include <stdbool.h>
#include <stdint.h>

#define IAP_CMD_SUCCESS             0   /*!< Command is executed successfully */
#define IAP_INVALID_COMMAND         1   /*!< Invalid command */
#define IAP_SRC_ADDR_ERROR          2   /*!< Source address is not on word boundary */
#define IAP_DST_ADDR_ERROR          3   /*!< Destination address is not on a correct boundary */
#define IAP_SRC_ADDR_NOT_MAPPED     4   /*!< Source address is not mapped in the memory map */
#define IAP_DST_ADDR_NOT_MAPPED     5   /*!< Destination address is not mapped in the memory map */
#define IAP_COUNT_ERROR             6   /*!< Byte count is not multiple of 4 or is not a permitted value */
#define IAP_INVALID_SECTOR          7   /*!< Sector number is invalid or end sector number is greater than start sector number */
#define IAP_SECTOR_NOT_BLANK        8   /*!< Sector is not blank */
#define IAP_SECTOR_NOT_PREPARED     9   /*!< Command to prepare sector for write operation was not executed */
#define IAP_COMPARE_ERROR           10  /*!< Source and destination data not equal */
#define IAP_BUSY                    11  /*!< Flash programming hardware interface is busy */
#define IAP_PARAM_ERROR             12  /*!< Insufficient number of parameters or invalid parameter */
#define IAP_ADDR_ERROR              13  /*!< Address is not on word boundary */
#define IAP_ADDR_NOT_MAPPED         14  /*!< Address is not mapped in the memory map */
#define IAP_CMD_LOCKED              15  /*!< Command is locked */
#define IAP_INVALID_CODE            16  /*!< Unlock code is invalid */
#define IAP_INVALID_BAUD_RATE       17  /*!< Invalid baud rate setting */
#define IAP_INVALID_STOP_BIT        18  /*!< Invalid stop bit setting */
#define IAP_CRP_ENABLED             19  /*!< Code read protection enabled */


uint32_t iap_read_part_id (void);
uint32_t iap_read_unique_id (void);

uint32_t erase_sectors (uint32_t startSector, uint32_t endSector);
uint32_t erase_user_flash(void);
uint32_t write_flash(uint32_t dst, char * src, uint32_t no_of_bytes);

void execute_user_code(void);
bool user_code_present(void);

#endif /* _PAL_FLASH_IAP_H */

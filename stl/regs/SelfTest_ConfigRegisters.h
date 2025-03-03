/*******************************************************************************
* File Name: SelfTest_ConfigRegisters.h
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for Startup Configuration Register self tests.
*
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
/**
* \addtogroup group_regs
* \{
*
* This test describes and shows an example of how to check the startup configuration registers: <br>
* 1) Test digital clock configuration registers <br>
* 2) Test analog configuration registers (set to default values after startup) <br>
* 3) Test the GPIO configuration and HSIOM registers <br>
*
* \section group_regs_more_information More Information
*
// \verbatim
Two test modes are implemented in the functions:

   1) Store duplicates of startup configuration registers in flash memory after device startup. 
      Periodically, the configuration registers are compared with stored duplicates. Corrupted
      registers can be restored from flash after checking.
   2) Compare the calculated CRC with the CRC previously stored in flash if the CRC status semaphore
      is set. If the status semaphore is not set, the CRC must be calculated and stored in flash, and
      the status semaphore must be set.
\endverbatim
*
*
* \section group_regs_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_regs_macros Macros
* \defgroup group_regs_functions Functions
*/

#if !defined(SELFTEST_CONFIGREGISTERS_H)
    #define SELFTEST_CONFIGREGISTERS_H
#include "SelfTest_common.h"
#include <string.h>
/***************************************
* Initial Parameter Constants
***************************************/
/** \addtogroup group_regs_macros
* \{
*/

/* Supports two self test modes: */
/** Stores duplicates of registers to FLASH and compares duplicates with registers. Registers can be restored in this mode. */
#define CFG_REGS_TO_FLASH_MODE           (1u)
/** Calculates registers CRC and stores to FLASH; Recalculates CRC and compares with saved CRC. */
#define CFG_REGS_CRC_MODE                (0u)

/** Select which mode to use (CFG_REGS_TO_FLASH_MODE or CFG_REGS_CRC_MODE) */
#define STARTUP_CFG_REGS_MODE            CFG_REGS_TO_FLASH_MODE


#if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)

/** First byte to store before storing CRC. */
    #define CRC_STARTUP_SEMAPHORE        (0x5Au)

/** Number of bytes from end of FLASH */
    #define CRC_STARTUP_SEMAPHORE_SHIFT  (13u)

/** Number of bytes from end of FLASH */
    #define CRC_STARTUP_LO               (12u)


/* Define number for last row in FLASH */
#if (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M0P) 
/** Calculating offset address for last row of flash for storing the register data*/
    #define LAST_ROW_IN_FLASH_OFFSET        ((CY_FLASH_SIZE - CY_FLASH_SIZEOF_ROW))
/** Calculating the starting address of last row. This value may differ depending on device used. */
    #define LAST_ROW_IN_FLASH_ADDR          (CY_FLASH_BASE + (LAST_ROW_IN_FLASH_OFFSET))
#else
/** Base address of Code flash only for XMC */
    #define CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE      CY_FLASH_SM_SBM_BASE
/** Size of Code flash only for XMC */
    #define CONF_REG_FLASH_SMALL_SECTOR_SIZE           CY_FLASH_SM_SBM_SIZE 
/** Calculating offset address for last row of flash for storing the register data*/
    #define LAST_ROW_IN_FLASH_OFFSET        ((CONF_REG_FLASH_SMALL_SECTOR_SIZE - CY_FLASH_SIZEOF_ROW))
/** Calculating the starting address of last row. This value may differ depending on device used. */
    #define LAST_ROW_IN_FLASH_ADDR          (CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE + (LAST_ROW_IN_FLASH_OFFSET))
#endif
#else /* if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */


#if (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))
/** Base address of Code flash only for XMC */
    #define CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE      CY_FLASH_SM_SBM_BASE
/** Size of Code flash only for XMC */
    #define CONF_REG_FLASH_SMALL_SECTOR_SIZE           CY_FLASH_SM_SBM_SIZE 
/** Number of flash rows to save configuration registers */
    #define CONF_REG_NUMBER_OF_ROWS     0x02u
/** Row number to store config register */
    #define CONF_REG_FIRST_ROW          \
    ((CONF_REG_FLASH_SMALL_SECTOR_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
/** Starting address of row to store config register */
    #define CONF_REG_FIRST_ROW_ADDR     (CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))
#elif (CY_CPU_CORTEX_M0P || CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M33)
/** Number of flash rows to save configuration registers */
    #define CONF_REG_NUMBER_OF_ROWS     0x01u
/** Row number to store config register */
    #define CONF_REG_FIRST_ROW          \
    ((CY_FLASH_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
/** Starting address of row to store config register */
    #define CONF_REG_FIRST_ROW_ADDR     (CY_FLASH_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))
#endif
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */

/** \} group_regs_macros */


/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_regs_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTests_StartUp_ConfigReg
****************************************************************************//**
*
* This function call checks configuration registers by comparing the value
* stored in flash with current the current configuration registers value.
* If values are different function returns fail.
*
*
*
*
* \return
*  0 - Test Passed <br>
*  1 - Test failed
*
*
*******************************************************************************/
uint8_t SelfTests_StartUp_ConfigReg(void);

#if (defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4))

/*******************************************************************************
* Function Name: SelfTests_Init_StartUp_ConfigReg
****************************************************************************//**
*
* This function should be always called to initilize the AREF address depending on the device.
*
*
* \note
* Applicable only for CAT1A devices.
*
*
* \return
*  None
*
*
*******************************************************************************/
void SelfTests_Init_StartUp_ConfigReg(void);
#endif

/** \} group_regs_functions */


/**
* \addtogroup group_regs_functions
* \{
*/
/*******************************************************************************
* Function Name: SelfTests_Save_StartUp_ConfigReg
****************************************************************************//**
*
*  
*  This function stores configuration registers to FlashRowData array and writes this array to flash.
*
*
* \note
* To be used only if the STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE <br>
* This function should be called once after the initial PSoC power up and initialization before entering the main program.
*
* \return
*  0 - writing to flash is successful. <br>
*  >=1 - writing to flash is not successful. Refer to Flash Driver PDL documentation
*  for error codes.
*
*
*******************************************************************************/
#if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)
cy_en_flashdrv_status_t SelfTests_Save_StartUp_ConfigReg(void);
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */
/** \} group_regs_functions */

/** \} group_regs */

#endif /* SELFTEST_CONFIGREGISTERS */

/* [] END OF FILE */

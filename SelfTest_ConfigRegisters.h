/*******************************************************************************
* File Name: SelfTest_ConfigRegisters.h
* Version 2.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for Startup Configuration Register self tests for PSoC 4.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "SelfTest_common.h"


#if !defined(SELFTEST_CONFIGREGISTERS_H)
    #define SELFTEST_CONFIGREGISTERS_H


/***************************************
* Initial Parameter Constants
***************************************/

/* Supports two self test modes: */
/* CFG_REGS_TO_FLASH_MODE - stores duplicates of registers to FLASH and compares duplicates with
   registers. */
/*                          Registers can be restored in this mode. */
/* CFG_REGS_CRC_MODE - calculates registers CRC and stores to FLASH; Recalculates CRC and compares
   with saved CRC. */
#define STARTUP_CFG_REGS_MODE            CFG_REGS_TO_FLASH_MODE

/* CFG_REGS_TO_FLASH_MODE and CFG_REGS_CRC_MODE - described above */
#define CFG_REGS_TO_FLASH_MODE           (1u)
#define CFG_REGS_CRC_MODE                (0u)

#if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)

    #define CRC_STARTUP_SEMAPHORE        (0x5Au)

/* Number of bytes from end of FLASH */
    #define CRC_STARTUP_SEMAPHORE_SHIFT  (13u)

/* Number of bytes from end of FLASH */
    #define CRC_STARTUP_LO               (12u)


/* Define number for last row in FLASH */
    #define LAST_ROW_IN_FLASH_OFFSET        ((CY_FLASH_SIZE - CY_FLASH_SIZEOF_ROW))
    #define LAST_ROW_IN_FLASH_ADDR          (CY_FLASH_BASE + (LAST_ROW_IN_FLASH_OFFSET))
#else // if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)

/* Number of flash rows to save configuration registers */
    #define CONF_REG_NUMBER_OF_ROWS     0x01u
    #define CONF_REG_FIRST_ROW          \
    ((CY_FLASH_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
    #define CONF_REG_FIRST_ROW_ADDR     (CY_FLASH_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))

#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */


/***************************************
* Function Prototypes
***************************************/

uint8_t SelfTests_StartUp_ConfigReg(void);

#if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)
cy_en_flashdrv_status_t SelfTests_Save_StartUp_ConfigReg(void);
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */

#endif /* SELFTEST_CONFIGREGISTERS */


/* [] END OF FILE */

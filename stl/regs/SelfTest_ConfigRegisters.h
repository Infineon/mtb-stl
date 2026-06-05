/*******************************************************************************
* File Name: SelfTest_ConfigRegisters.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for the Startup Configuration Register self tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/
/**
 * \addtogroup group_regs
 * \{
 *
 * This test describes and shows an example of how to check the startup configuration registers:
 * <br>
 * 1) Test digital clock configuration registers <br>
 * 2) Test analog configuration registers (set to default values after startup) <br>
 * 3) Test the GPIO configuration and HSIOM registers <br>
 *
 * \section group_regs_more_information More Information
 *
 * Two test modes are implemented in the functions:
 *
 *      1) Store duplicates of startup configuration registers in Flash memory after the device startup.
 *         Periodically, the configuration registers are compared with the stored duplicates. Corrupted
 *         registers can be restored from Flash after checking.
 *      2) Compare the calculated CRC with the CRC previously stored in Flash if the CRC status
 *         semaphore is set. If the status semaphore is not set, the CRC must be calculated and
 *         stored in Flash, and the status semaphore must be set.
 *
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
/** Stores duplicates of registers to Flash and compares duplicates with registers. Registers can be
 *  restored in this mode. */
#define CFG_REGS_TO_FLASH_MODE           (1u)
/** Calculates the registers CRC and stores to Flash; recalculates CRC and compares with the saved CRC. */
#define CFG_REGS_CRC_MODE                (0u)

/** Select which mode to use (CFG_REGS_TO_FLASH_MODE or CFG_REGS_CRC_MODE) */
#define STARTUP_CFG_REGS_MODE            CFG_REGS_TO_FLASH_MODE


#if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)

/** The first byte to store before storing CRC. */
    #define CRC_STARTUP_SEMAPHORE        (0x5Au)

/** The number of bytes from the end of Flash. */
    #define CRC_STARTUP_SEMAPHORE_SHIFT  (13u)

/** The number of bytes from the end of Flash. */
    #define CRC_STARTUP_LO               (12u)


/* Define the number for the last row in Flash. */
#if defined(SELFTEST_PSOC4_FAMILY) || defined(SELFTEST_PSOC6_FAMILY)
/** Calculates the offset address for the last row of Flash for storing the register data.*/
    #define LAST_ROW_IN_FLASH_OFFSET        ((CY_FLASH_SIZE - CY_FLASH_SIZEOF_ROW))
/** Calculates the starting address of the last row. This value may differ depending on the device used. */
    #define LAST_ROW_IN_FLASH_ADDR          (CY_FLASH_BASE + (LAST_ROW_IN_FLASH_OFFSET))
#elif defined(SELFTEST_XMC7X_FAMILY) || defined(SELFTEST_PSC3_FAMILY) || defined(SELFTEST_XMC5X_FAMILY)
/** The base address of Code Flash for XMC7X/XMC5X/PSC3. */
    #define CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE      CY_FLASH_SM_SBM_BASE
/** The size of Code Flash for XMC7X/XMC5X/PSC3. */
    #define CONF_REG_FLASH_SMALL_SECTOR_SIZE           CY_FLASH_SM_SBM_SIZE
/** Calculates the offset address for the last row of Flash for storing the register data.*/
    #define LAST_ROW_IN_FLASH_OFFSET        \
    ((CONF_REG_FLASH_SMALL_SECTOR_SIZE - CY_FLASH_SIZEOF_ROW))
/** Calculates the starting address of the last row. This value may differ depending on the device used. */
    #define LAST_ROW_IN_FLASH_ADDR          \
    (CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE + (LAST_ROW_IN_FLASH_OFFSET))
#endif /* defined(SELFTEST_PSOC4_FAMILY) || defined(SELFTEST_PSOC6_FAMILY) */
#else /* if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */


#if defined(SELFTEST_XMC7X_FAMILY)
/** Base address of Code Flash only for XMC7X. */
    #define CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE      CY_FLASH_SM_SBM_BASE
/** The size of Code Flash only for XMC7X. */
    #define CONF_REG_FLASH_SMALL_SECTOR_SIZE           CY_FLASH_SM_SBM_SIZE
/** The number of Flash rows to save the configuration registers. */
    #define CONF_REG_NUMBER_OF_ROWS     0x02u
/** The row number to store the config register. */
    #define CONF_REG_FIRST_ROW          \
    ((CONF_REG_FLASH_SMALL_SECTOR_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
/** The start address of the row to store the config register. */
    #define CONF_REG_FIRST_ROW_ADDR     \
    (CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))
#elif defined(SELFTEST_PSOC4_FAMILY) || defined(SELFTEST_PSOC6_FAMILY) || defined(SELFTEST_PSC3_FAMILY)
/** The number of Flash rows to save the configuration registers. */
    #define CONF_REG_NUMBER_OF_ROWS     0x01u
/** The row number to store the config register. */
    #define CONF_REG_FIRST_ROW          \
    ((CY_FLASH_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
/** The start address of the row to store the config register. */
    #define CONF_REG_FIRST_ROW_ADDR     (CY_FLASH_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))
#elif defined(SELFTEST_XMC5X_FAMILY)
/** Base address of Code Flash for XMC5000. */
    #define CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE      CY_FLASH_SM_SBM_BASE
/** The size of Code Flash for XMC5000. */
    #define CONF_REG_FLASH_SMALL_SECTOR_SIZE           CY_FLASH_SM_SBM_SIZE
/** The number of Flash rows to save the configuration registers. */
    #define CONF_REG_NUMBER_OF_ROWS     0x02u
/** The row number to store the config register. */
    #define CONF_REG_FIRST_ROW          \
    ((CONF_REG_FLASH_SMALL_SECTOR_SIZE / CY_FLASH_SIZEOF_ROW) - CONF_REG_NUMBER_OF_ROWS)
/** The start address of the row to store the config register. */
    #define CONF_REG_FIRST_ROW_ADDR     \
    (CONF_REG_FLASH_SMALL_SECTOR_ADDR_BASE + (CONF_REG_FIRST_ROW * CY_FLASH_SIZEOF_ROW))
#endif /* defined(SELFTEST_XMC7X_FAMILY) */
#endif /* (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */

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
* This function call checks the configuration registers by comparing the value
* stored in Flash with the current configuration registers value.
* If the values are different, the function returns a fail.
*
* \return
*  0 - Test Passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTests_StartUp_ConfigReg(void);

#if defined(SELFTEST_PSOC6_FAMILY)

/*******************************************************************************
* Function Name: SelfTests_Init_StartUp_ConfigReg
****************************************************************************//**
*
* This function must be always called to initialize the AREF address depending on the device.
*
*
* \note
* Applicable only for PSOC6 devices
*
*
* \return
*  None
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
*  This function stores the configuration registers to the FlashRowData array and writes this array to
*  Flash.
*
*
* \note
* Used only if the STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE. <br>
* This function must be called once after the initial PSOC power up and initialization before
* entering the main program.
*
* \return
*  0 - Writing to Flash is successful. <br>
*  >=1 - Writing to Flash is not successful. Refer to the Flash Driver PDL documentation
*  for error codes.
*
*
*******************************************************************************/
#if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)
cy_en_flashdrv_status_t SelfTests_Save_StartUp_ConfigReg(void);
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */
/** \} group_regs_functions */

/** \} group_regs */

#endif /* SELFTEST_CONFIGREGISTERS_H */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_Flash.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for FLASH self tests.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_flash
* \{
*
* To complete a full diagnostic of the flash memory, a checksum of all used flash needs to be calculated.
* The current library uses a Fletcher's 64-bit checksum. The stl_Flash.h file defines the flash size that needs to be monitored.
*
* \section group_flash_more_information More Information
*
* The proposed checksum flash test reads each ROM or flash location and accumulates the values in a 64-bit variable to calculate
* a running checksum of the entire user flash memory. The actual 64-bit checksum of flash is stored in the last 8 bytes of flash
* itself. When the test reaches the end of flash minus 8 bytes (0x7FF8 on 32-KB devices), it stops. Custom linker files are used
* to place the checksum in the desired location. The calculated checksum value is then compared with the actual value stored in 
* the last 8 bytes of flash. A mismatch indicates flash failure. The checksum can also be stored in SFLASH, EEPROM, 
* or any other external flash. 
*
*
* \defgroup group_flash_macros Macros
* \defgroup group_flash_functions Functions
*/


#if !defined(SELFTEST_FLASH_H)
    #define SELFTEST_FLASH_H

#include "cy_pdl.h"
#include "SelfTest_common.h"
 
/** \cond INTERNAL */
#define FLASH_TEST_FLETCHER64      (0u)
#define FLASH_TEST_CRC32           (1u)
/** \endcond */

/** \addtogroup group_flash_macros
* \{
*/
/** Supports two self test modes: <br> 1) FLASH_TEST_CHECKSUM  - performs a Checksum calculation on flash  <br> 2) FLASH_TEST_CRC - Performs a CRC calculation on flash  */
#define FLASH_TEST_MODE            (FLASH_TEST_FLETCHER64)

/** \} group_flash_macros */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_flash_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_FlashCheckSum
****************************************************************************//**
*
*  This function checks for data corruption in flash memory using a checksum calculation.
*
*
* \param DoubleWordsToTest 
* Number of 32-bit Double Words of flash to be calculated per each function call. <br>
*
* \return
*  1 - Test failed <br>
*  2 - Test in progress <br>
*  3 - Test completed
*
*******************************************************************************/
uint8_t SelfTest_FlashCheckSum(uint32_t DoubleWordsToTest);


/*******************************************************************************
* Function Name: SelfTest_Flash_init
****************************************************************************//**
*
*  This function checks the checksum of the flash for the given range of flash with
* the expected/reference checksum passed along with this API.
*
*
* \param StartAddressOfFlash 
* Start address of the flash from where the checksum needs to be calculated.<br>
* \param EndAddressOfFlash 
* End address of the flash till where the checksum needs to be calculated. <br>
* \param flash_ExpectedCheckSum 
* Expected checksum. Must be stored outside the range of check. <br>
*
* \note
* This function needs to be called prior to \ref SelfTest_FlashCheckSum else the test will fail
*
*******************************************************************************/
void SelfTest_Flash_init(uint32_t StartAddressOfFlash,uint32_t EndAddressOfFlash, uint64_t flash_ExpectedCheckSum);
/** \} group_flash_functions */



/***************************************
* Initial Parameter Constants
***************************************/
/** \addtogroup group_flash_macros
* \{
*/
/** Initial value for Counter of Iteration in Fletcher32 Checksum algorithm */
#define ITER_COUNT_INIT_VALUE          (uint32_t)(1u)

/** The magic value 359 is the largest number of sums that can be performed without numeric overflow. 
   Given the possible initial starting value of sum1 = 0x1fffe. Any smaller value is also permissible; 256 may be convenient in many
   cases.  */
#define LARGEST_NUM_OF_SUMS            (uint32_t)(359u)

/** Init value for checksum calculation */
#define CHECKSUM_INIT_VALUE            (uint32_t)(0xFFFFFFFFu)

/** Set size of one block in Flash test */
#define FLASH_DOUBLE_WORDS_TO_TEST     (uint32_t)(512u)

/** No of bytes to store the checksum*/
#define FLASH_RESERVED_CHECKSUM_SIZE    (8u)

/** Define last Flash address which is used for checksum calculation. Last two bytes used for flash checksum storing, So need to subtract "8" */
#if (CY_CPU_CORTEX_M0P || CY_CPU_CORTEX_M4)
#define FLASH_END_ADDR                \
    (uint32_t)(CY_FLASH_BASE + CY_FLASH_SIZE - FLASH_RESERVED_CHECKSUM_SIZE)

/** \} group_flash_macros */

/** \cond INTERNAL */

#elif CY_CPU_CORTEX_M7
#if  defined(CY_DEVICE_SERIES_XMC7200)
#define CY_FLASH_BASE      0x10080000UL
#define CY_FLASH_SIZE      0x007B0000UL
#else
#define CY_FLASH_BASE      0x10080000UL
#define CY_FLASH_SIZE      0x00390000UL
#endif

#define FLASH_END_ADDR                \
    (uint32_t)(CY_FLASH_BASE + CY_FLASH_SIZE - FLASH_RESERVED_CHECKSUM_SIZE)
#endif

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
#define CY_FLASH_NSC_SIZE 0x00000100UL
#define FLASH_END_ADDR         \
    (uint32_t)(CY_FLASH_BASE + (CY_FLASH_SIZE/2UL) - (CY_FLASH_NSC_SIZE +FLASH_RESERVED_CHECKSUM_SIZE))
#endif

/***************************************
* External Variables
***************************************/
#if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
extern uint64_t flash_CheckSum;
#elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
extern uint32_t flash_CheckSum;
#endif


/** \endcond */

/** \} group_flash */

#endif /* SELFTEST_FLASH_H */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_Flash.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for Flash self tests.
*
*******************************************************************************
* (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
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
 * \addtogroup group_flash
 * \{
 *
 * To complete a full diagnostic of the Flash memory, a checksum of all used Flash needs to be
 * calculated. The current library uses a Fletcher's 64-bit checksum. The stl_Flash.h file defines
 * the Flash size that needs to be monitored.
 *
 * \section group_flash_more_information More Information
 *
 * The proposed checksum Flash test reads each ROM or Flash location and accumulates the values in a
 * 64-bit variable to calculate a running checksum of the entire user Flash memory. The actual
 * 64-bit checksum of Flash is stored in the last 8 bytes of Flash itself. When the test reaches the
 * end of Flash minus 8 bytes (0x7FF8 on 32-KB devices), it stops. Custom linker files are used to
 * place the checksum in the desired location. The calculated checksum value is then compared with
 * the actual value stored in the last 8 bytes of Flash. A mismatch indicates Flash failure. The
 * checksum can also be stored in SFLASH, EEPROM, or any other external Flash.
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
/** Supports two self test modes: <br> 1) FLASH_TEST_CHECKSUM  - performs a Checksum calculation on
 *  Flash  <br> 2) FLASH_TEST_CRC - Performs a CRC calculation on Flash  */
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
*  This function checks for data corruption in Flash memory using a checksum calculation.
*
*
* \param DoubleWordsToTest
* Number of 32-bit Double Words of Flash to be calculated per each function call. <br>
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
* This function checks the checksum of the Flash for the given range of Flash with
* the expected/reference checksum passed along with this API.
*
*
* \param StartAddressOfFlash
* Start address of the Flash from where the checksum needs to be calculated.<br>
* \param EndAddressOfFlash
* End address of the Flash till where the checksum needs to be calculated. <br>
* \param flash_ExpectedCheckSum
* Expected checksum. Must be stored outside the range of check. <br>
*
* \note
* This function needs to be called prior to \ref SelfTest_FlashCheckSum else the test will fail
*
*******************************************************************************/
void SelfTest_Flash_init(uint32_t StartAddressOfFlash, uint32_t EndAddressOfFlash,
                         uint64_t flash_ExpectedCheckSum);
/** \} group_flash_functions */



/***************************************
* Initial Parameter Constants
***************************************/
/** \addtogroup group_flash_macros
 * \{
 */
/** Initial value for Counter of Iteration in Fletcher32 Checksum algorithm */
#define ITER_COUNT_INIT_VALUE          (uint32_t)(1u)

/** The magic value 359 is the largest number of sums that can be performed without numeric
 * overflow. Given the possible initial starting value of sum1 = 0x1fffe. Any smaller value is also
 * permissible; 256 may be convenient in many cases.  */
#define LARGEST_NUM_OF_SUMS            (uint32_t)(359u)

/** Init value for checksum calculation */
#define CHECKSUM_INIT_VALUE            (uint32_t)(0xFFFFFFFFu)

/** Set size of one block in Flash test */
#define FLASH_DOUBLE_WORDS_TO_TEST     (uint32_t)(512u)

/** No of bytes to store the checksum*/
#define FLASH_RESERVED_CHECKSUM_SIZE    (8u)

/** Define last Flash address which is used for checksum calculation. Last two bytes used for Flash
 *  checksum storing, So need to subtract "8" */
#if (defined (CY_IP_M0S8CPUSSV3) || defined(CY_IP_M4CPUSS) || defined (CY_DOXYGEN))
#define FLASH_END_ADDR                \
    (uint32_t)(CY_FLASH_BASE + CY_FLASH_SIZE - FLASH_RESERVED_CHECKSUM_SIZE)

/** \} group_flash_macros */

/** \cond INTERNAL */

#elif defined (CY_IP_M7CPUSS)
#if  defined(CY_DEVICE_SERIES_XMC7200)
#define CY_FLASH_BASE      0x10080000UL
#define CY_FLASH_SIZE      0x007B0000UL
#else
#define CY_FLASH_BASE      0x10080000UL
#define CY_FLASH_SIZE      0x00390000UL
#endif

#define FLASH_END_ADDR                \
    (uint32_t)(CY_FLASH_BASE + CY_FLASH_SIZE - FLASH_RESERVED_CHECKSUM_SIZE)
#endif /* if (defined (CY_IP_M0S8CPUSSV3) || defined(CY_IP_M4CPUSS) || defined (CY_DOXYGEN)) */

#if defined (CY_IP_M33SYSCPUSS)
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

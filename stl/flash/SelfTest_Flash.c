/*******************************************************************************
* File Name: SelfTest_Flash.c
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for FLASH self tests.
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
#include "cy_pdl.h"

#include "SelfTest_Flash.h"
#include "SelfTest_ErrorInjection.h"

#if (FLASH_TEST_MODE == FLASH_TEST_CRC32)
#include "SelfTest_CRC_calc.h"
#endif

/***************************************
* Initial Parameter Constants
***************************************/


#if CY_CPU_CORTEX_M0P
/* CPU Subsystem Configuration register */
/* is used to map Vector table to FLASH*/
/* CPUSS Vector Table */
#define REG_CPUSS_VTOR             (SCB->VTOR)

/* System Request Register */
/* is used to map Reset Vector to FLASH */
#define REG_CPUSS_SYSREQ_REG       (CPUSS_SYSREQ)

/* Map reset vector to point to flash */
#define REG_MEM_MAP_VECTOR           ((uint32_t)1u << 27u)

#endif

/* Check whether the init function was called or not*/
static bool init_fxn_called = false;

/* Pointer to currently calculating flash addr*/
static uint32_t* flash_Pointer_FourBytes;

/* Flash start addr*/
static uint32_t flash_start_address;

/* Flash end addr*/
static uint32_t flash_end_address;

#if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
/* Variable that stores the running checksum of the FLASH */
uint64_t flash_CheckSum = CHECKSUM_INIT_VALUE;
static uint64_t sumA = CHECKSUM_INIT_VALUE;
static uint32_t IterationCounter = ITER_COUNT_INIT_VALUE;
/* Expected checksum*/
static uint64_t expected_flash_shecksum = 0ULL;

/***************************************
* Local Function Prototypes
***************************************/
static void SelfTest_Fletcher64_CheckSum_Formula(uint32_t endAdress);

#elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
/* Variable that store the running CRC checksum of the FLASH */
uint32_t flash_CheckSum = CRC32_INIT_VALUE;
/* Expected checksum*/
static uint32_t expected_flash_shecksum = 0ULL;
#endif /* FLASH_TEST_MODE == FLASH_TEST_CRC */



/*******************************************************************************
 * Function Name: SelfTest_Flash_init()
 *******************************************************************************
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
* This function needs to be called prior to \ref SelfTest_FlashCheckSum else the test will fail.
*
 ******************************************************************************/

void SelfTest_Flash_init(uint32_t StartAddressOfFlash,uint32_t EndAddressOfFlash, uint64_t flash_ExpectedCheckSum)
{
    flash_start_address = StartAddressOfFlash;
    /* Set pointer to access Flash memory to start address */
    flash_Pointer_FourBytes = (uint32_t*)flash_start_address;
    flash_end_address = EndAddressOfFlash;
    #if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
        expected_flash_shecksum = flash_ExpectedCheckSum;
    #elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
        expected_flash_shecksum = (uint32_t)flash_ExpectedCheckSum;
    #endif
    init_fxn_called = true;
}



/*******************************************************************************
 * Function Name: SelfTest_FlashCheckSum()
 *******************************************************************************
 *
 * Summary:
 *  This function checks for data corruption in flash memory using a checksum
 *  calculation.
 *
 * Parameters:
 *  uint32 DoubleWordsToTest - number of 32-bit Double Words of flash to be
 *  calculated per each function call.
 *
 * Return:
 *  Result of test:   "1" - fail test; "2" - Test in progress;
 *   "3" - Test completed OK;
 *
 ******************************************************************************/

uint8_t SelfTest_FlashCheckSum(uint32_t DoubleWordsToTest)
{
    if (init_fxn_called == false)
    {
        return ERROR_STATUS;
    }

#if CY_CPU_CORTEX_M0P
    /* Variable to save CPUSS_CONFIG or CM0P_VTOR and CPUSS_SYSREQ registers */
    uint32_t regCPUSS_VECTORTABLE;

    uint32_t regCPUSS_SYSREQ;
#endif
    /* End address of current tested block */
    uint32_t endAdressOfTest;

    /* Function result */
    uint8_t ret = PASS_STILL_TESTING_STATUS;

#if CY_CPU_CORTEX_M0P
    /* Enable global interrupts */
    __enable_irq();
#endif

    /* Check if we are in the last permitted byte of Flash */
    if ((uint32_t)flash_Pointer_FourBytes == flash_end_address)
    {
        /* Set pointer to access Flash memory to start address */
        flash_Pointer_FourBytes = (uint32_t*)flash_start_address;


        #if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
        /* Set init values for Iteration Counter in Fletcher64 Checksum algorithm */
        IterationCounter = ITER_COUNT_INIT_VALUE;

        /* Set init values for checksum calculation */

        flash_CheckSum = CHECKSUM_INIT_VALUE;
        sumA = CHECKSUM_INIT_VALUE;
        #elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
        flash_CheckSum = CRC32_INIT_VALUE;
        #endif
    }

    /* Calculate end address of current tested block */
    endAdressOfTest =
        (uint32_t)((uint32_t)flash_Pointer_FourBytes + (DoubleWordsToTest * sizeof(uint32_t)));

    /* Check if Flash byte address is in Flash range */
    if (endAdressOfTest > flash_end_address)
    {
        endAdressOfTest = flash_end_address;
    }

#if CY_CPU_CORTEX_M0P
    /* Disable global interrupts */
    __disable_irq();

    /* Save CPU registers */
    regCPUSS_VECTORTABLE = REG_CPUSS_VTOR;

    /* Map Vector table to FLASH*/
    REG_CPUSS_VTOR = 0u;

    regCPUSS_SYSREQ = CPUSS_SYSREQ;

    /* Map Reset vector to FLASH */
    REG_CPUSS_SYSREQ_REG = REG_MEM_MAP_VECTOR;
#endif

    #if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
    /* Calculating checksum for all bytes in Flash block */
    SelfTest_Fletcher64_CheckSum_Formula(endAdressOfTest);
    #elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
    flash_CheckSum = SelfTests_CRC32_ACC(flash_CheckSum, (uint32_t)flash_Pointer_FourBytes,
                                         endAdressOfTest - (uint32_t)flash_Pointer_FourBytes);
    flash_Pointer_FourBytes = (uint32_t*)endAdressOfTest;
    #endif

#if CY_CPU_CORTEX_M0P   
    /* Restore CPU registers */
    REG_CPUSS_VTOR = regCPUSS_VECTORTABLE;

    CPUSS_SYSREQ = regCPUSS_SYSREQ;

    /* Enable global interrupts */
    __enable_irq();
#endif

    /* Check if we are in the last permitted byte of Flash */
    if ((uint32_t)flash_Pointer_FourBytes == flash_end_address)
    {
        #if (ERROR_IN_FLASH == 1)

        /* Compare the stored check sum and calculated check sum */
        if (flash_CheckSum == (expected_flash_shecksum + 1))
        #else

        /* Compare the stored check sum and calculated check sum */
        if (flash_CheckSum == expected_flash_shecksum)
        #endif /* End ERROR_IN_FLASH */
        {
            /* Test passed */
            ret = PASS_COMPLETE_STATUS;
        }
        else
        {
            /* Flash checksum error */
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


/*******************************************************************************
 * Function Name: SelfTest_Fletcher64_CheckSum_Formula()
 *******************************************************************************
 *
 * Summary:
 *  This is function for Checksum calculation. In current example used optimized
 *  Fletcher-64 algorithm. But Checksum algorithm can be replaced by any in this
 *  function.
 *
 * Theory:
 *     Fletcher Checksum
 *     *    Use two running one's complement checksums
 *      -    For fair comparison, each running sum is half width
 *      -    E.g., 16-bit Fletcher Checksum is two 8-bit running sums
 *      -    Initialize: A = CHECKSUM_INIT_VALUE; B = CHECKSUM_INIT_VALUE;
 *      -    For each byte in data word: A = A + Byte i; B = B + A;
 *      -    Result is A concatenated with B (16-bit result)
 *     *    Significant improvement comes from the running sum B
 *      -    B = Byte N-1 + 2*Byte N-2 + 3*Byte N-3 + ...
 *      -    Makes checksum order-dependent (switched byte order detected)
 *      -    Gives HD=3 until the B value rolls over
 *
 * Also, theory and examples: http://en.wikipedia.org/wiki/Fletcher's_checksum
 *
 * Parameters:
 *  uint32_t endAdress - number of last byte for block checksum calculation.
 *
 * Global Variables Used:
 *  uint32_t *flash_Pointer_FourBytes - pointer to current byte of Flash.
 *  sumA - Fletcher64 A variable.
 *  flash_CheckSum - Fletcher64 B variable.
 *  IterationCounter - Byte N counter for Fletcher64.
 *
 * Return:
 *  No
 *
 ******************************************************************************/
#if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
static void SelfTest_Fletcher64_CheckSum_Formula(uint32_t endAdress)
{
    uint32_t doubleWords = ((endAdress - (uint32_t)flash_Pointer_FourBytes) / sizeof(uint32_t));
    uint32_t tlen;

    /* Calculate Fletcher64 Checksum for Flash Block with size doubleWords. */
    /* Maximum defined size is FLASH_DOUBLE_WORDS_TO_TEST, but can be less in case when last block
       of Flash */
    while (doubleWords != 0u)
    {
        /* Calculate length of Checksum block according value LARGEST_NUM_OF_SUMS */
        tlen = (doubleWords > (LARGEST_NUM_OF_SUMS)) ? (LARGEST_NUM_OF_SUMS) : doubleWords;
        doubleWords -= tlen;

        /* Calculate Fletcher64 Checksum for one block with size LARGEST_NUM_OF_SUMS or less */
        do
        {
            /* Calculate sum of A = A + Byte i */
            sumA = sumA + *flash_Pointer_FourBytes;

            /* B = B + A */
            flash_CheckSum = flash_CheckSum + sumA;

            /* Increase Fletcher64 Iteration Counter */
            /* and pointer to Flash current byte (+4 bytes because DoubleWord) */
            IterationCounter++;
            flash_Pointer_FourBytes++;
            --tlen;
        } while (tlen != 0u);

        /* First reduction step to 32 bits */
        sumA = (sumA & 0xFFFFFFFFu) + (sumA >> 32);
        flash_CheckSum = (flash_CheckSum & 0xFFFFFFFFu) + (flash_CheckSum >> 32);
    }

    /* If end of Flash summarize Checksum */
    if ((uint32_t)flash_Pointer_FourBytes == flash_end_address)
    {
        /* Second reduction step to 32 bits */
        sumA = (sumA & 0xFFFFFFFFu) + (sumA >> 32);
        flash_CheckSum = (flash_CheckSum & 0xFFFFFFFFu) + (flash_CheckSum >> 32);

        /* Summarize Checksum and store to 64 bits */
        flash_CheckSum = (flash_CheckSum << 32) | sumA;
    }
}


#endif /* if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64) */

/* [] END OF FILE */

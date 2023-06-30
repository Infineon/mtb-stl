/*******************************************************************************
* File Name: SelfTest_Flash.h
* Version 2.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for FLASH self tests for PSoC4.
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


#if !defined(SELFTEST_FLASH_H)
    #define SELFTEST_FLASH_H

#include "cy_pdl.h"

/* Supports two self test modes: */
/* FLASH_TEST_CHECKSUM  - performs a Checksum calculation on flash
 *
 * FLASH_TEST_CRC - Performs a CRC calculation on flash
 * */
#define FLASH_TEST_MODE            (FLASH_TEST_FLETCHER64)

#define FLASH_TEST_FLETCHER64    (0u)
#define FLASH_TEST_CRC32           (1u)

/***************************************
* Function Prototypes
***************************************/
uint8_t SelfTest_FlashCheckSum(uint32_t DoubleWordsToTest);


/***************************************
* Initial Parameter Constants
***************************************/

/* Initial value for Counter of Iteration in Fletcher32 Checksum algorithm */
#define ITER_COUNT_INIT_VALUE          (uint32_t)(1u)

/* The magic value 359 is the largest number of sums that can be performed without numeric
   overflow. Given the possible initial starting value of sum1 = 0x1fffe. */
/* Any smaller value is also permissible; 256 may be convenient in many
   cases. */
#define LARGEST_NUM_OF_SUMS            (uint32_t)(359u)

/* Init value for checksum calculation */
#define CHECKSUM_INIT_VALUE            (uint32_t)(0xFFFFFFFFu)

/* Set size of one block in Flash test */
#define FLASH_DOUBLE_WORDS_TO_TEST     (uint32_t)(512u)

#define FLASH_RESERVED_CHECKSUM_SIZE    (8u)

/* Define last address Flash which used for checksum calculation */
/* Last two bytes used for flash checksum storing, and need to subtract "8" */
#define PSOC_FLASH_END_ADDR                \
    (uint32_t)(CY_FLASH_BASE + CY_FLASH_SIZE - FLASH_RESERVED_CHECKSUM_SIZE)


/***************************************
* External Variables
***************************************/
#if (FLASH_TEST_MODE == FLASH_TEST_FLETCHER64)
extern uint64_t flash_CheckSum;
#elif (FLASH_TEST_MODE == FLASH_TEST_CRC32)
extern uint32_t flash_CheckSum;
#endif

#endif /* SELFTEST_FLASH_H */

/* [] END OF FILE */

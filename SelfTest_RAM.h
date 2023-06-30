/*******************************************************************************
* File Name: SelfTest_RAM.h
* Version 2.0
*
* Description:
*  This file provides constants and parameter values used for SRAM
*  self tests for PSoC 4
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


#if !defined(SELFTEST_RAM_H)
    #define SELFTEST_RAM_H

/***************************************
* Function Prototypes
***************************************/

extern void March_Test_Stack_Init(uint8_t shift);
extern uint8_t March_Test_Stack(void);

extern void March_Test_Init(uint8_t shift);
extern uint8_t March_Test_SRAM(void);

void SelfTests_Init_March_SRAM_Test(uint8_t shift);
uint8_t SelfTests_SRAM_March(void);

void SelfTests_Init_March_Stack_Test(uint8_t shift);
uint8_t SelfTests_Stack_March(void);

/***************************************
* External Variables
***************************************/
#if defined(__ICCARM__)
/* used by IAR SRAM march Assembly code */
/* variable to set start address for test Stack block */
extern volatile uint32_t test_Stack_Addr;
/* variable to set start address for test RAM block */
extern volatile uint32_t test_SRAM_Addr;
#endif
/***************************************
* Initial Parameter Constants
***************************************/
//#define CHECKERBOARD_PATTERN_55        (0x55u)
//#define CHECKERBOARD_PATTERN_AA        (0xAAu)

/* This is a duplicate define of TEST_BLOCK_SIZE defined in asm files */
/* SelfTest_SRAM_March.s */
#define TEST_BLOCK_SIZE                (0x0400u)

/* Number of blocks on which SRAM is divided */
#define NUMBERS_OF_TEST_CYCLES         (uint16)(CY_SRAM_SIZE  / TEST_BLOCK_SIZE)

#endif /* SELFTEST_RAM_H */

/* [] END OF FILE */

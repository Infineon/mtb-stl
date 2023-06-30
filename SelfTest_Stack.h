/*******************************************************************************
* File Name: SelfTest_Stack.h
* Version 2.0
*
* Description:
*  This file provides constants and parameter values used for Stack
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


#if !defined(SELFTEST_STACK_H)
    #define SELFTEST_STACK_H

/***************************************
* Function Prototypes
***************************************/

void SelfTests_Init_Stack_Test(void);
uint8_t SelfTests_Stack_Check(void);


/***************************************
* Initial Parameter Constants
***************************************/

/* Stack test parameters */
#define STACK_TEST_PATTERN        0x55AAu

/* Block size to be tested. Should be EVEN*/
#define STACK_TEST_BLOCK_SIZE     0x08u

/* PSoC memory parameters */
#define PSOC_SRAM_SIZE            CY_SRAM_SIZE
#define PSOC_SRAM_BASE            CY_SRAM_BASE

/* PSoC stack parameters */
#define PSOC_STACK_BASE           (CY_SRAM_BASE + CY_SRAM_SIZE)
#define PSOC_STACK_SIZE           (0x00000400u) // This value is directly entered. Find other
                                                // solution.
#define PSOC_STACK_END            (uint32_t)(PSOC_STACK_BASE - PSOC_STACK_SIZE)

#endif /* SELFTEST_STACK_H */

/* END OF FILE */

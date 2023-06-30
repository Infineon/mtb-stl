/*******************************************************************************
* File Name: SelfTest_CPU.h
* Version 2.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for CPU register self tests for PSoC 4
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


#if !defined(SELFTEST_CPU_H)
#define SELFTEST_CPU_H

#include <cycfg.h>

/***************************************
* Function Prototypes
***************************************/

uint8_t SelfTest_PC(void);
uint8_t SelfTest_CPU_Registers(void);

/***************************************
* Initial Parameter Constants
***************************************/
#define CHECKERBOARD_PATTERN_55     0x55u
#define CHECKERBOARD_PATTERN_AA     0xAAu

#define SELF_TEST_A_1               0x5Au
#define SELF_TEST_B_1               0xBF51u
#define SELF_TEST_C_1               0xCA82D3F8u

#define SELF_TEST_A_2               0xF5u
#define SELF_TEST_B_2               0x5CF1u
#define SELF_TEST_C_2               0x72F6C4B5u

#endif /* End SELFTEST_CPU_H */

/* [] END OF FILE */

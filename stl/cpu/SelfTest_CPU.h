/*******************************************************************************
* File Name: SelfTest_CPU.h
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for CPU register self tests for CAT2(PSoC4), CAT1A, CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
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
* \addtogroup group_cpu
* \{
*
* CPU Module mainly carries out 3 test: <br>
* 1) CPU register Test <br>
* 2) Program Counter Test <br>
* 3) Program Flow Test <br>
*
* \section group_cpu_more_information More Information
*
* 1) CPU register Test:
* The CPU registers test detects stuck-at faults in the CPU registers by using the checkerboard test.
* This test ensures that the bits in the registers are not stuck at value '0' or '1'. It is a nondestructive test that performs the following major tasks: <br>
// \verbatim
  1)The contents of the CPU registers to be tested are saved on the stack before executing the routine. 
  2)The registers are tested by successively writing the binary sequences 01010101 followed by 10101010 into the registers, and then 
    reading the values from these registers for verification. 
  3)The test returns an error code if the returned values do not match. 
\endverbatim
* 2) Program Counter Test: To test this registers, a checkerboard test is commonly used; the addresses 0x5555 and 0xAAAA must be allocated 
* for this test. 0x5555 and 0xAAAA represent the checkerboard bit patterns. This functions that are in flash memory at different addresses are called. <br>

* 3) Program Flow Test: For every critical execution code block, unique numbers are added 
*    to or subtracted from complementary counters before block execution and immediately after execution. These procedures allow to see if the 
*    code block is correctly called from the main program flow and to check if the block is correctly executed. As long as there are always the 
*    same number of exit and entry points, the counter pair will always be complementary after each tested block.
* \section group_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_cpu_macros Macros
* \defgroup group_cpu_functions Functions
*/

#include "SelfTest_common.h"


#if !defined(SELFTEST_CPU_H)
#define SELFTEST_CPU_H

#include <cycfg.h>

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_cpu_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_PC
****************************************************************************//**
*
*  This function performs the test of the PC register. Function calls
*  SelfTest_PC5555() and SelfTest_PCAAAA() which are placed at addresses in
*  flash that most closely match the 0x5555... pattern and 0xAAAA... pattern.
*  These functions must return unique values to SelfTestA, SelfTestB and
*  SelfTestC variables if PC is working correctly.
*
*
* \return
* 0 - test pass <br> 
* 1 - test fail.
*
*******************************************************************************/
uint8_t SelfTest_PC(void);

/*******************************************************************************
* Function Name: SelfTest_CPU_Registers
****************************************************************************//**
*
*  This function performs checkerboard test for all CPU registers except the PC
*  register.
*
*
* \return
* 0 - test pass <br> 
* 1 - test fail.
*
*******************************************************************************/
uint8_t SelfTest_CPU_Registers(void);

/*******************************************************************************
* Function Name: SelfTest_PROGRAM_FLOW
****************************************************************************//**
*
*  @brief This function tests the program flow.
*
*  For every critical execution code block, this function adds to or subtracts from
*  complementary counters before the block execution and immediately after execution.
*  This procedure allows verification of whether the code block is correctly called
*  from the main program flow and to check if the block is correctly executed. 
*  Provided there are the same number of exit and entry points, the counter pair 
*  will always be complementary after each tested block.
*
*
* \return
* 0 - test pass <br> 
* 1 - test fail.
*
*******************************************************************************/
uint8_t SelfTest_PROGRAM_FLOW(void);


/** \} group_cpu_functions */




/***************************************
* Initial Parameter Constants
***************************************/
/** \addtogroup group_cpu_macros
* \{
*/
/** Unique value returned by SelfTest_PC5555 function if PC is working correctly. */
#define CHECKERBOARD_PATTERN_55     0x55u
/** Unique value returned by SelfTest_PCAAAA function if PC is working correctly. */
#define CHECKERBOARD_PATTERN_AA     0xAAu
/** Unique value written to a variable CPU_SelfTestA by SelfTest_PC5555 function if PC is working correctly. */
#define SELF_TEST_A_1               0x5Au
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PC5555 function if PC is working correctly. */
#define SELF_TEST_B_1               0xBF51u
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PC5555 function if PC is working correctly. */
#define SELF_TEST_C_1               0xCA82D3F8u
/** Unique value written to a variable CPU_SelfTestA by SelfTest_PCAAAA function if PC is working correctly. */
#define SELF_TEST_A_2               0xF5u
/** Unique value written to a variable CPU_SelfTestB by SelfTest_PCAAAA function if PC is working correctly. */
#define SELF_TEST_B_2               0x5CF1u
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PCAAAA function if PC is working correctly. */
#define SELF_TEST_C_2               0x72F6C4B5u

/** \} group_cpu_macros */

/** \} group_cpu */

#endif /* End SELFTEST_CPU_H */

/* [] END OF FILE */
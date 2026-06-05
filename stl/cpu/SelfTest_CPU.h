/*******************************************************************************
* File Name: SelfTest_CPU.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for CPU register self tests.
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
 * \addtogroup group_cpu
 * \{
 *
 * CPU Module does 3 tests: <br>
 * 1) CPU register Test <br>
 * 2) Program Counter Test <br>
 * 3) Program Flow Test <br>
 *
 * \section group_cpu_more_information More Information
 *
 * 1) CPU register Test:
 * The CPU registers test detects stuck-at faults in the CPU registers by using the checkerboard
 * test. This test ensures that the bits in the registers are not stuck at value '0' or '1'. It is
 * a nondestructive test that performs the following major tasks: <br>
 *
 *      1) The contents of the CPU registers to be tested are saved on the stack before executing
 *         the routine.
 *      2) The registers are tested by successively writing the binary sequences 01010101 followed
 *         by 10101010 into the registers and then reading the values from these registers for
 *         verification.
 *      3) The test returns an error code if the returned values do not match.
 *
 * 2) Program Counter Test: To test these registers, the checkerboard test is commonly used;
 * addresses 0x5555 and 0xAAAA must be allocated for this test. 0x5555 and 0xAAAA represent the
 * checkerboard bit patterns. Functions that are in Flash memory at different addresses are
 * called. <br>
 *
 * 3) Program Flow Test: For every critical execution code block, unique numbers are added
 * to or subtracted from complementary counters before the block execution and immediately after
 * execution. This procedures allows verification of whether the code block is correctly called from the main
 * program flow and check if the block is correctly executed. Provided there are always the
 * same numbers of exit and entry points, the counter pair will always be complementary after each
 * tested block.
 *
 *
 * \defgroup group_cpu_macros Macros
 * \defgroup group_cpu_functions Functions
 */

#if !defined(SELFTEST_CPU_H)
#define SELFTEST_CPU_H
#include "SelfTest_common.h"
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
*  This function performs the test of the PC register. The function calls
*  SelfTest_PC5555() and SelfTest_PCAAAA(), which are placed at addresses in
*  Flash that match the 0x5555... and 0xAAAA... pattern most closely.
*  These functions must return unique values to SelfTestA, SelfTestB, and
*  SelfTestC variables if PC is working correctly.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_PC(void);

/*******************************************************************************
* Function Name: SelfTest_CPU_Registers
****************************************************************************//**
*
*  This function performs the checkerboard test for all CPU registers except the PC
*  register.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
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
*  Provided there are the same numbers of exit and entry points, the counter pair
*  will always be complementary after each tested block.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
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
/** Unique value written to a variable CPU_SelfTestA by SelfTest_PC5555 function if PC is working
 *  correctly. */
#define SELF_TEST_A_1               0x5Au
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PC5555 function if PC is working
 *  correctly. */
#define SELF_TEST_B_1               0xBF51u
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PC5555 function if PC is working
 *  correctly. */
#define SELF_TEST_C_1               0xCA82D3F8u
/** Unique value written to a variable CPU_SelfTestA by SelfTest_PCAAAA function if PC is working
 *  correctly. */
#define SELF_TEST_A_2               0xF5u
/** Unique value written to a variable CPU_SelfTestB by SelfTest_PCAAAA function if PC is working
 *  correctly. */
#define SELF_TEST_B_2               0x5CF1u
/** Unique value written to a variable CPU_SelfTestC by SelfTest_PCAAAA function if PC is working
 *  correctly. */
#define SELF_TEST_C_2               0x72F6C4B5u
/** Section where the function SelfTest_PC5555 must be stored at addresses in Flash that most
 *  closely match
 *  the 0x5555... pattern. This section needs to be defined in linker script.*/
#define SELF_TEST_SECTION_PC5555    "PC5555"
/** The section to  store the function SelfTest_PCAAAA at addresses in Flash that
 *  match the 0xAAAA... pattern most closely. This section must be defined in the linker script.*/
#define SELF_TEST_SECTION_PCAAAA    "PCAAAA"
/** \} group_cpu_macros */

/** \} group_cpu */

#endif /* End SELFTEST_CPU_H */

/* [] END OF FILE */

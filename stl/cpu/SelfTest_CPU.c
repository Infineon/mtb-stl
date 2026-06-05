/*******************************************************************************
* File Name: SelfTest_CPU.c
*
* Description:
*  This file provides the source code for the CPU register self tests.
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

#include "cy_pdl.h"
#include "SelfTest_CPU.h"
#include "SelfTest_CPU_Regs.h"
#include "SelfTest_ErrorInjection.h"

static volatile uint8_t CPU_SelfTestA;
static volatile uint16_t CPU_SelfTestB;
static volatile uint32_t CPU_SelfTestC;
static uint16_t counter1;
static uint16_t counter2;
static bool Program_flow_test;
/* The definition of the section for the SelfTest_PC5555() and SelfTest_PCAAAA() functions (GCC Compiler).
 *   Custom linkers files are required to placed at correct location
 */

#if defined(__GNUC__)
static uint8_t SelfTest_PC5555(void) __attribute__((noinline, section(SELF_TEST_SECTION_PC5555)));
static uint8_t SelfTest_PCAAAA(void) __attribute__((noinline, section(SELF_TEST_SECTION_PCAAAA)));
#elif defined(__ICCARM__)
/* Place the functions in the required location in flash using the linker file. (IAR compilers) */
#pragma default_function_attributes  = @ "PC5555"
static uint8_t SelfTest_PC5555(void);
#pragma default_function_attributes =
#pragma default_function_attributes  = @ "PCAAAA"
static uint8_t SelfTest_PCAAAA(void);
#pragma default_function_attributes =
#elif defined(__ARMCC_VERSION__)
/* Place the functions in the required location in flash using the linker file. (ARM compilers) */
static uint8_t SelfTest_PC5555(void) __attribute__((section(".PC5555")));
static uint8_t SelfTest_PCAAAA(void) __attribute__((section(".PCAAAA")));
#endif /* if defined(__GNUC__) */

/*******************************************************************************
 * Function Name: SelfTest_CPU_RAM
 *******************************************************************************
 *
 * Summary:
 *  This function performs the checkerboard test for all CPU registers except the PC
 *  register.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_CPU_Registers(void)
{
    uint8_t ret;

    /* Enable global interrupts */
    __disable_irq();

    #if defined(__ARMCC_VERSION)
    ret = SelfTest_CPU_Regs_ARM();
    #elif defined(__GNUC__)
    ret = SelfTest_CPU_Regs_GCC();
    #elif defined(__ICCARM__)
    ret = SelfTest_CPU_Regs_IAR();
    #else
    ret = ERROR_STATUS;
    #endif

    /* Enable global interrupts */
    __enable_irq();

    return ret;
}


/*******************************************************************************
 * Function Name: SelfTest_PC
 *******************************************************************************
 *
 * Summary:
 *  This function performs the test of the PC register. The function calls
 *  SelfTest_PC5555() and SelfTest_PCAAAA(), which are placed at addresses in
 *  flash that match the 0x5555... and 0xAAAA... patterns most closely.
 *  These functions must return unique values to SelfTestA, SelfTestB, and
 *  SelfTestC variables if the PC is working correctly.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_PC(void)
{
    uint8_t ret = ERROR_STATUS;
    uint8_t returned_value;

    /* Clear global variables for test */
    CPU_SelfTestA = 0x00u;
    CPU_SelfTestB = 0x0000u;
    CPU_SelfTestC = 0x00000000u;
    Program_flow_test = false;

    returned_value = SelfTest_PC5555();

    /* Check if SelfTest_PC5555() returned and wrote correct values to global variables*/
    if (CPU_SelfTestA == SELF_TEST_A_1)
    {
        if (CPU_SelfTestB == SELF_TEST_B_1)
        {
            if (CPU_SelfTestC == SELF_TEST_C_1)
            {
                if (returned_value == CHECKERBOARD_PATTERN_55)
                {
                    /* Clear global variables for next test */
                    CPU_SelfTestA = 0x00u;
                    CPU_SelfTestB = 0x0000u;
                    CPU_SelfTestC = 0x00000000u;

                    returned_value = SelfTest_PCAAAA();

                    /* Check if SelfTest_PCAAAA() returned and wrote correct values to global
                       variables*/
                    if (CPU_SelfTestA == SELF_TEST_A_2)
                    {
                        if (CPU_SelfTestB == SELF_TEST_B_2)
                        {
                            if (CPU_SelfTestC == SELF_TEST_C_2)
                            {
                                if (returned_value == CHECKERBOARD_PATTERN_AA)
                                {
                                    ret = OK_STATUS;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}


/*******************************************************************************
 * Function Name: SelfTest_PROGRAM_FLOW
 *******************************************************************************
 *
 * Summary:
 *  This function performs the test of the Program Flow. For every critical
 *  execution code block, unique numbers are added to or subtracted from
 *  complementary counters before the block execution and immediately after execution.
 *  These procedures allow to see if the code block is correctly called from the main
 *  program flow and check if the block is correctly executed. As long as there is
 *  always the same number of exit and entry points, the counter pair will always be
 *  complementary after each tested block.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_PROGRAM_FLOW(void)
{
    uint8_t ret = ERROR_STATUS;

    /* Clear global variables for test */
    CPU_SelfTestA = 0x00u;
    CPU_SelfTestB = 0x0000u;
    CPU_SelfTestC = 0x00000000u;
    counter1 = 0x0000;
    counter2 = 0xFFFF;

    Program_flow_test = true;

    counter1 += (uint16_t)0x0010;
    (void)SelfTest_PC5555();
    counter2 -= (uint16_t)0x0010;
    counter1 += (uint16_t)0x0030;
    (void)SelfTest_PCAAAA();

    #if ERROR_IN_PROGRAM_FLOW
    /* Return error value */
    counter2 -= 0x0060;
    #else
    /* Return OK value */
    counter2 -= (uint16_t)0x0030;
    #endif /* End ERROR_IN_PROGRAM_FLOW */

    if ((uint16_t)(counter1 ^ counter2) == (uint16_t)0xFFFF)
    {
        ret = OK_STATUS;
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_PCAAAA
********************************************************************************
*
* Summary:
*  This function performs the test for the PC register. The function is placed at the
*  largest value in flash, which resembles pattern 0xAAAA... This function
*  must return value 0xAA and write a unique value to SelfTestA, SelfTestB, and
*  SelfTestC variables if the PC is working correctly. Also, this function executes
*  a cycle loop for additional check if the PC is working.
*
* Parameters:
*  None.
*
* Return:
*  SelfTestA:  write unique value to variable for this function SELF_TEST_A_2.
*  SelfTestB:  write unique value to variable for this function SELF_TEST_B_2.
*  SelfTestC:  write to unique value to variable for this function
*  SELF_TEST_C_2. Return unique value for this function
*  CHECKERBOARD_PATTERN_AA.
*
*******************************************************************************/
static uint8_t SelfTest_PCAAAA(void)
{
    if (Program_flow_test)
    {
        counter1 += (uint16_t)0x0040;
    }
    CPU_SelfTestA = SELF_TEST_A_2;

    CPU_SelfTestB = SELF_TEST_B_2;

    CPU_SelfTestC = SELF_TEST_C_2;

    if (Program_flow_test)
    {
        counter2 -= (uint16_t)0x0040;
    }

    #if ERROR_IN_PROGRAM_COUNTER
    /* Return error value */
    return (CHECKERBOARD_PATTERN_AA + 1u);
    #else
    /* Return OK value */
    return CHECKERBOARD_PATTERN_AA;
    #endif /* End ERROR_IN_PROGRAM_COUNTER */
}


/*******************************************************************************
 * Function Name: SelfTest_PC5555
 *******************************************************************************
 *
 * Summary:
 *  This function performs the test for the PC register. The function is placed at the
 *  largest value in flash, which resembles pattern 0x5555... This function
 *  must return value 0x55 and  write a unique value to SelfTestA, SelfTestB, and
 *  SelfTestC variables if the PC is working correctly. Also this function executes
 *  a cycle loop for additional check if the PC is working.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  SelfTestA:  write unique value to variable for this function SELF_TEST_A_1.
 *  SelfTestB:  write unique value to variable for this function SELF_TEST_B_1.
 *  SelfTestC:  write unique value to variable for this function SELF_TEST_C_1.
 *  Return unique value for this function CHECKERBOARD_PATTERN_55.
 *
 ******************************************************************************/
static uint8_t SelfTest_PC5555(void)
{
    if (Program_flow_test)
    {
        counter1 += (uint16_t)0x0015;
    }
    CPU_SelfTestA = SELF_TEST_A_1;

    CPU_SelfTestB = SELF_TEST_B_1;

    CPU_SelfTestC = SELF_TEST_C_1;

    if (Program_flow_test)
    {
        counter2 -= (uint16_t)0x0015;
    }

    #if ERROR_IN_PROGRAM_COUNTER
    /* Return error value */
    return (CHECKERBOARD_PATTERN_55 + 1u);
    #else
    /* Return OK value */
    return CHECKERBOARD_PATTERN_55;
    #endif
}


/* [] END OF FILE */

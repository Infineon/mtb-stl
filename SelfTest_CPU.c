/*******************************************************************************
* File Name: SelfTest_CPU.c
* Version 2.0
*
* Description: This file provides the source code for the CPU register self tests
* for PSoC 4
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

#include "cy_pdl.h"
#include "SelfTest_CPU.h"
#include "SelfTest_CPU_Regs.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

static volatile uint8_t CPU_SelfTestA;
static volatile uint16_t CPU_SelfTestB;
static volatile uint32_t CPU_SelfTestC;

/* Definition of section for SelfTest_PC5555() and SelfTest_PCAAAA() functions (GCC Compiler,)
 *   custom linkers files are required to place at correct location
 */

#if defined(__GNUC__)
static uint8_t SelfTest_PC5555(void) __attribute__((noinline, section("PC5555")));
static uint8_t SelfTest_PCAAAA(void) __attribute__((noinline, section("PCAAAA")));
#elif defined(__ICCARM__)
/* Place the functions in required location in flash using linker file. (IAR compilers) */
#pragma default_function_attributes  = @ "PC5555"
static uint8_t SelfTest_PC5555(void);
#pragma default_function_attributes =
#pragma default_function_attributes  = @ "PCAAAA"
static uint8_t SelfTest_PCAAAA(void);
#pragma default_function_attributes =
#endif // if defined(__GNUC__)

/*******************************************************************************
 * Function Name: SelfTest_CPU_RAM
 *******************************************************************************
 *
 * Summary:
 *  This function performs checkerboard test for all CPU registers except the PC
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

    #if defined(__GNUC__)
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
 *  This function performs the test of the PC register. Function calls
 *  SelfTest_PC5555() and SelfTest_PCAAAA() which are placed at addresses in
 *  flash that most closely match the 0x5555... pattern and 0xAAAA... pattern.
 *  These functions must return unique values to SelfTestA, SelfTestB and
 *  SelfTestC variables if PC is working correctly.
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
* Function Name: SelfTest_PCAAAA
********************************************************************************
*
* Summary:
*  This function performs a test for the PC register. Function is placed at the
*  largest value in flash that resembles the pattern 0xAAAA... This function
*  must return value 0xAA and write unique value to SelfTestA, SelfTestB and
*  SelfTestC variables if PC is working correctly. Also this function executes
*  a cycle loop for additional checking if the PC is working.
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
    CPU_SelfTestA = SELF_TEST_A_2;

    CPU_SelfTestB = SELF_TEST_B_2;

    CPU_SelfTestC = SELF_TEST_C_2;

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
 *  This function performs a test for the PC register. Function is placed at the
 *  largest value in flash that resembles the pattern 0x5555... This function
 *  must return value 0x55 and  write unique value to SelfTestA, SelfTestB and
 *  SelfTestC variables if PC is working correctly. Also this function executes
 *  a cycle loop for additional checking if the PC is working.
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
    CPU_SelfTestA = SELF_TEST_A_1;

    CPU_SelfTestB = SELF_TEST_B_1;

    CPU_SelfTestC = SELF_TEST_C_1;

    #if ERROR_IN_PROGRAM_COUNTER
    /* Return error value */
    return (CHECKERBOARD_PATTERN_55 + 1u);
    #else
    /* Return OK value */
    return CHECKERBOARD_PATTERN_55;
    #endif
}


/* [] END OF FILE */

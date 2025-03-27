/*******************************************************************************
* File Name: SelfTest_Stack.h
*
* Description:
*  This file provides constants and parameter values used for Stack
*  self tests.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_stack
* \{
*
This Module checks for the stack overflow or underflow condition. The stack is a section of RAM used by the CPU to store information temporarily.
The purpose of the stack overflow/Underflow test is to ensure that the stack does not overlap with the program or data memory during program execution. 
This can occur, for example, if recursive functions are used.
This library fills a block of memory above and below the stack with a fixed pattern and periodically tested the pattern for corruption. 
*
*
* \defgroup group_stack_macros Macros
* \defgroup group_stack_functions Functions
*/

#if !defined(SELFTEST_STACK_H)
    #define SELFTEST_STACK_H

#include "SelfTest_common.h"

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_stack_functions
* \{
*/

/*******************************************************************************
 * Function Name: SelfTests_Init_Stack_Range
 *****************************************************************************//**
 *
 * This function initializes the upper and lower 
 * stack area with 0xAA and 0x55 pattern.
 *
 * \param stack_address
 * The pointer to the stack
 * \param stack_length
 * The length of the stack
 * \param stack_pattern_blk_size
 * No of bytes to be used to fill the pattern. Must be 2^n where n=1 to n=8. 
 * (Recommended value for n = 3)
 *
 *
 *******************************************************************************/

void SelfTests_Init_Stack_Range(uint16_t* stack_address, uint16_t stack_length, uint8_t stack_pattern_blk_size);
/*******************************************************************************
 * Function Name: SelfTests_Stack_Check_Range
 *****************************************************************************//**
 *
 * This function performs stack self test. It checks upper and lower
 * stack area for 0xAA and 0x55 pattern.
 *
 * \param stack_address
 * The pointer to the stack
 * \param stack_length
 * The length of the stack
 *
 * \return
 *  0 - Test passed <br>
 *  1 - Test failed
 *
 **********************************************************************************/
uint8_t SelfTests_Stack_Check_Range(uint16_t* stack_address, uint16_t stack_length);

/** \} group_stack_functions */


/** \addtogroup group_stack_macros
* \{
*/

/***************************************
* Initial Parameter Constants
***************************************/

/** Stack test pattern */
#define STACK_TEST_PATTERN        0x55AAu

/** \} group_stack_macros */


/** \} group_stack */

#endif /* SELFTEST_STACK_H */

/* END OF FILE */

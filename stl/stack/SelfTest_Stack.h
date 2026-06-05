/*******************************************************************************
* File Name: SelfTest_Stack.h
*
* Description:
*  This file provides constants and parameter values used for the Stack
*  self tests.
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
 * \addtogroup group_stack
 * \{
 *
 * This Module checks for the stack overflow or underflow condition. The stack is a section of RAM
 * used by the CPU to store information temporarily. The purpose of the stack overflow/Underflow
 * test is to ensure that the stack does not overlap with the program or data memory during program
 * execution. This can occur, for example, if recursive functions are used. This library fills a
 * block of memory above and below the stack with a fixed pattern and periodically tests the
 * pattern for corruption.
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
* stack areas with the 0xAA and 0x55 patterns.
*
* \param stack_address
* The pointer to the stack.
* \param stack_length
* The length of the stack.
* \param stack_pattern_blk_size
* The number of bytes to fill the pattern. Must be 2^n where n=1 to n=8.
* (Recommended value for n = 3).
*
* \note
* Ensure sufficient stack space is available before calling this function.
* The stack must not be overflowed or near overflow. Guard zones provide
* backup protection but should not be the primary defense mechanism.
* Proper stack sizing and monitoring are essential for safe operation.
*
*******************************************************************************/

void SelfTests_Init_Stack_Range(uint16_t* stack_address, uint16_t stack_length,
                                uint8_t stack_pattern_blk_size);
/*******************************************************************************
 * Function Name: SelfTests_Stack_Check_Range
 *****************************************************************************//**
 *
 * This function performs the stack self test. It checks the upper and lower
 * stack areas for the 0xAA and 0x55 patterns.
 *
 * \param stack_address
 * The pointer to the stack.
 * \param stack_length
 * The length of the stack.
 *
 * \return
 *  Result of test (bitmask): <br>
 *  0x00 (0) - Pass: both guard zones intact <br>
 *  0x01 (1) - Fail: stack overflow detected (bottom guard corrupted) <br>
 *  0x02 (2) - Fail: stack underflow detected (top guard corrupted) <br>
 *  0x03 (3) - Fail: both overflow and underflow detected
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

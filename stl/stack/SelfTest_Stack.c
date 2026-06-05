/* *****************************************************************************
* File Name: SelfTest_Stack.c
*
* Description:
*  This file provides the source code to the API for the runtime Stack self tests.
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
#include "SelfTest_Stack.h"
#include "SelfTest_ErrorInjection.h"


static uint8_t cy_stack_pattern_block_size = 0;

/*******************************************************************************
 * Function Name: SelfTests_Init_Stack_Range
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the upper and lower stack areas with the 0xAA and
 *  0x55 pattern.
 *
 * Parameters:
 * \param stack_address
 * The pointer to the stack.
 * \param stack_length
 * The length of the stack.
 * \param stack_pattern_blk_size
 * The size of the pattern block (must be less than half of stack_length).
 *
 * Return:
 *  None.
 *
 * Note:
 * Ensure sufficient stack space is available before calling this function.
 * The stack must not be overflowed or near overflow. Guard zones provide
 * backup protection but should not be the primary defense mechanism.
 * Proper stack sizing and monitoring are essential for safe operation.
 *
 **********************************************************************************/

void SelfTests_Init_Stack_Range(uint16_t* stack_address, uint16_t stack_length,
                                uint8_t stack_pattern_blk_size)
{
    uint8_t i;

    /* Validate that guard zones don't overlap */
    CY_ASSERT(((uint16_t)stack_pattern_blk_size * 2U) <= stack_length);

    cy_stack_pattern_block_size = stack_pattern_blk_size;
    /* The pointer to the last word in the stack */
    uint16_t* stack = (stack_address - (stack_length/sizeof(uint16_t)));

    /* Fill the test stack block with a predefined pattern */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        #if (ERROR_IN_STACK_OVERFLOW)
        *stack = STACK_TEST_PATTERN + 1u;
        stack++;
        #else
        *stack = STACK_TEST_PATTERN;
        stack++;
        #endif /* #if (ERROR_IN_STACK_OVERFLOW) */
    }

    /* The pointer to the first word in the stack */
    stack = (stack_address - (cy_stack_pattern_block_size / sizeof(uint16_t)));
    /* Fill the test stack block with a predefined pattern */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        #if (ERROR_IN_STACK_UNDERFLOW)
        *stack = STACK_TEST_PATTERN + 1u;
        stack++;
        #else

        *stack = STACK_TEST_PATTERN;
        stack++;
        #endif /* #if (ERROR_IN_STACK_UNDERFLOW) */
    }
}


/*******************************************************************************
 * Function Name: SelfTests_Stack_Check_Range
 ********************************************************************************
 *
 * Summary:
 *  This function performs the stack self test. It checks the upper and lower
 *  stack areas for the 0xAA and 0x55 patterns.
 *
 * Parameters:
 * \param stack_address
 * The pointer to the stack.
 * \param stack_length
 * The length of the stack.
 *
 * Return:
 *  Result of test (bitmask):
 *    0x00 (0) - Pass: both guard zones intact
 *    0x01 (1) - Fail: stack overflow detected (bottom guard corrupted)
 *    0x02 (2) - Fail: stack underflow detected (top guard corrupted)
 *    0x03 (3) - Fail: both overflow and underflow detected
 *
 **********************************************************************************/

uint8_t SelfTests_Stack_Check_Range(uint16_t* stack_address, uint16_t stack_length)
{
    uint8_t i;
    uint8_t ret = OK_STATUS;

    /* The pointer to the last word in the stack. */
    uint16_t* stack = (stack_address - (stack_length/sizeof(uint16_t)));

    /* Check test stack block for pattern and return error if no pattern found */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        if (*stack != STACK_TEST_PATTERN)
        {
            ret |= (uint8_t)(1U << 0);
            break;
        }
        stack++;
    }

    stack = (stack_address - (cy_stack_pattern_block_size / sizeof(uint16_t)));

    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        if (*stack != STACK_TEST_PATTERN)
        {
            ret |= (uint8_t)(1U << 1);
            break;
        }
        stack++;
    }
    return ret;
}


/* [] END OF FILE */

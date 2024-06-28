/* *****************************************************************************
* File Name: SelfTest_Stack.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to the API for runtime Stack self tests for
*  CAT2(PSoC4), CAT1A, CAT1C devices.
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

#include "cy_pdl.h"
#include "SelfTest_Stack.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"


static uint8_t cy_stack_pattern_block_size = 0;
/*******************************************************************************
 * Function Name: SelfTests_Init_Stack_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the upper stack area with 0xAA and 0x55 pattern.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_Stack_Test(uint8_t stack_pattern_blk_size)
{
    SelfTests_Init_Stack_Range((uint16_t*)DEVICE_STACK_BASE, DEVICE_STACK_SIZE, stack_pattern_blk_size);
}


/*******************************************************************************
 * Function Name: SelfTests_Stack_Check
 ********************************************************************************
 *
 * Summary:
 *  This function performs stack self test. It checks upper stack area for 0xAA
 *  and 0x55 pattern.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 **********************************************************************************/
uint8_t SelfTests_Stack_Check(void)
{
   uint8_t ret = OK_STATUS;
   ret = SelfTests_Stack_Check_Range((uint16_t*)DEVICE_STACK_BASE, DEVICE_STACK_SIZE);
   return ret; 
}

/*******************************************************************************
 * Function Name: SelfTests_Init_Stack_Range
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the upper stack area with 0xAA and 0x55 pattern.
 *
 * Parameters:
 * \param stack_address
 * The pointer to the stack.
 * \param stack_address
 * The length of the stack.
 *
 * Return:
 *  None.
 *
 **********************************************************************************/

void SelfTests_Init_Stack_Range(uint16_t* stack_address, uint16_t stack_length, uint8_t stack_pattern_blk_size)
{
    uint8_t i;
    cy_stack_pattern_block_size = stack_pattern_blk_size;
    /* Pointer to the last word in the stack*/
   uint16_t* stack = (stack_address - (stack_length/sizeof(uint16_t)));
   
    /* Fill test stack block with predefined pattern */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        #if (ERROR_IN_STACK_OVERFLOW)
            *stack = STACK_TEST_PATTERN + 1u;
             stack++;
        #else
            *stack = STACK_TEST_PATTERN;
             stack++;
        #endif /* End (ERROR_IN_STACK) */
        
    }
    
    /* Pointer to the first word in the stack*/
    stack = (stack_address - (cy_stack_pattern_block_size / sizeof(uint16_t))); 
    /* Fill test stack block with predefined pattern */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        #if (ERROR_IN_STACK_UNDERFLOW)
            *stack = STACK_TEST_PATTERN + 1u;
             stack++;
        #else
            
            *stack = STACK_TEST_PATTERN;
             stack++; 
        #endif /* End (ERROR_IN_STACK) */
    }
}


/*******************************************************************************
 * Function Name: SelfTests_Stack_Check_Range
 ********************************************************************************
 *
 * Summary:
 *  This function performs stack self test. It checks upper stack area for 0xAA
 *  and 0x55 pattern.
 *
 * Parameters:
 * \param stack_address
 * The pointer to the stack.
 * \param stack_address
 * The length of the stack.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 **********************************************************************************/

uint8_t SelfTests_Stack_Check_Range(uint16_t* stack_address, uint16_t stack_length)
{
    uint8_t i;
    uint8_t ret = OK_STATUS;

    /* Pointer to the last word in the stack */
   uint16_t* stack = (stack_address - (stack_length/sizeof(uint16_t)));

    /* Check test stack block for pattern and return error if no pattern found */
    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        if (*stack != STACK_TEST_PATTERN)
        {
            stack++;
            ret |= (1 << 0);
            break;
        }
    }

    stack = (stack_address - (cy_stack_pattern_block_size / sizeof(uint16_t)));

    for (i = 0u; i < (cy_stack_pattern_block_size / sizeof(uint16_t)); i++)
    {
        if (*stack != STACK_TEST_PATTERN)
        {
            stack++;
            ret |= (1 << 1);
            break;
        }
    }
    return ret;
}
/* [] END OF FILE */



/*******************************************************************************
* File Name: SelfTest_Stack.h
* Version 1.0.0
*
* Description:
*  This file provides constants and parameter values used for Stack
*  self tests for CAT2(PSoC4), CAT1A, CAT1C devices.
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
* \addtogroup group_stack
* \{
*
This Module checks for the stack overflow or underflow condition. The stack is a section of RAM used by the CPU to store information temporarily.
The purpose of the stack overflow/Underflow test is to ensure that the stack does not overlap with the program or data memory during program execution. 
This can occur, for example, if recursive functions are used.
This library fills a block of memory above and below the stack with a fixed pattern and periodically tested the pattern for corruption. 
*
*
*
*
* \section group_stack_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_stack_macros Macros
* \defgroup group_stack_functions Functions
*/

#include "SelfTest_common.h"


#if !defined(SELFTEST_STACK_H)
    #define SELFTEST_STACK_H

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_stack_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTests_Init_Stack_Test
****************************************************************************//**
*
* This function initializes the upper and Lower stack area with 0xAA and 0x55 pattern.
*
*
*
* \return
*  None.
*
*******************************************************************************/
void SelfTests_Init_Stack_Test(void);

/*******************************************************************************
* Function Name: SelfTests_Stack_Check
****************************************************************************//**
*
*  This function performs stack self test. It checks upper and Lower stack area for 0xAA
*  and 0x55 pattern.
*
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed.
*
*******************************************************************************/
uint8_t SelfTests_Stack_Check(void);

/** \} group_stack_functions */


/** \addtogroup group_stack_macros
* \{
*/

/***************************************
* Initial Parameter Constants
***************************************/

/** Stack test pattern */
#define STACK_TEST_PATTERN        0x55AAu

/* Block size to be tested. Should be EVEN*/

// Below 2 lines can be used to make the code take the block size from linker script.
// extern uint32_t __STACK_BLOCK_SIZE;
// #define STACK_TEST_BLOCK_SIZE     ((uint32_t)&__STACK_BLOCK_SIZE)

/** Block size to be tested. Should be EVEN. Pattern is Filled in this block size */
#define STACK_TEST_BLOCK_SIZE     (8u)   //Block size for pattern fixed to 8


#if CY_CPU_CORTEX_M0P
    // //accessing the linker script symbol.
    // extern uint32_t __STACK_SIZE;
    // #define PSOC_STACK_SIZE           ((uint32_t)&__STACK_SIZE)
	
/** Base address of SRAM. May vary with the device used. */
    #define DEVICE_SRAM_BASE           (CY_SRAM_BASE)
/** Size of SRAM. May vary with the device used. */
    #define DEVICE_SRAM_SIZE           (CY_SRAM_SIZE)
/** Size of Stack. May vary with the device used. */
    #define DEVICE_STACK_SIZE          (0x00000400)    

#elif CY_CPU_CORTEX_M4
    // //accessing the linker script symbol.
    // extern uint32_t __STACK_SIZE;
    // #define PSOC_STACK_SIZE    ((uint32_t)&__STACK_SIZE)
    #define DEVICE_SRAM_BASE           (0x08002000)
    
    #if defined(CY_DEVICE_PSOC6A512K)
        #define DEVICE_SRAM_SIZE       (0x3D800)
    #endif

    #if defined(CY_DEVICE_PSOC6A2M)
        #define DEVICE_SRAM_SIZE       (0xFD800)
    #endif

    #define DEVICE_STACK_SIZE          (0x1000)

#elif CY_CPU_CORTEX_M7

    #define DEVICE_SRAM_BASE       (0x28004000)
    #define DEVICE_SRAM_SIZE       (0x000FC000)
    #define DEVICE_STACK_SIZE        (STACK_SIZE)

#endif


/** Start of Stack address excluding the block size to store pattern */
#define DEVICE_STACK_BASE           (DEVICE_SRAM_BASE + DEVICE_SRAM_SIZE - STACK_TEST_BLOCK_SIZE)

/** End of Stack address excluding the block size to store pattern */
#define DEVICE_STACK_END            (uint32_t)(DEVICE_STACK_BASE - DEVICE_STACK_SIZE + STACK_TEST_BLOCK_SIZE)


/** \} group_stack_macros */


/** \} group_stack */

#endif /* SELFTEST_STACK_H */

/* END OF FILE */

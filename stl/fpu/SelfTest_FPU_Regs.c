/*******************************************************************************
* File Name: SelfTest_FPU_Regs.c
*
* Description:
*  This file provides the source code for the FPU register self tests.
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

#include "cy_pdl.h"
#include "SelfTest_FPU_Regs.h"

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))

/*******************************************************************************
 * Function Name: SelfTest_FPU_REG
 *******************************************************************************
 *
 * Summary:
 *  This function performs checkerboard test for all FPU registers.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_FPU_Registers(void)
{
    uint8_t ret;

    /* Enable global interrupts */
    __disable_irq();
    #if defined(__GNUC__)
        ret = SelfTest_FPU_Regs_GCC();
    #elif defined(__ICCARM__)
        ret = SelfTest_FPU_Regs_IAR();
    #elif defined (__ARMCC_VERSION)
        ret = SelfTest_FPU_Regs_ARM();
    #else
    ret = ERROR_STATUS;
    #endif
    /* Enable global interrupts */
    __enable_irq();

    return ret;
}

#endif

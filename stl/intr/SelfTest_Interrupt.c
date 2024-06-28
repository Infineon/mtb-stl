/******************************************************************************
 * File Name:  SelfTest_Interrupt.c
 * Version 1.0.0
 *
 * Description:
 *  This file provides the source code for the interrupt self test for CAT2(PSoC4),
 *  CAT1A, CAT1C devices.
 *  The interrupt handler implementation uses the base address and counter
 *  number definitions from the Device Configurator generated code.
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
#include "cybsp.h"
#include "SelfTest_Interrupt.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

/* This variable is used in isr_1 interrupt handler */
static volatile uint16_t selfTest_interrupt_counter = 0;
static TCPWM_Type* base1;
static uint32_t cntNum1;
/******************************************************************************
* Function Name: SelfTest_Interrupt_ISR_TIMER
*******************************************************************************
*
* Summary:
*  Handle Interrupt Service Routine. Source - Timer.
*
******************************************************************************/
void SelfTest_Interrupt_ISR_TIMER(void)
{
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, CY_TCPWM_INT_ON_TC);

    /* Increment the counter to indicate ISR is triggered */
    selfTest_interrupt_counter++;
}


/*******************************************************************************
 * Function Name: SelfTest_Interrupt
 ********************************************************************************
 *
 * Summary:
 *  This function performs the interrupt execution self test. This function checks interrupt
 *  isr from Timer.
 *
 * Parameters:
 * TCPWM_Type *base - The pointer to a TCPWM instance.
 * uint32_t cntNum - The Counter instance number in the selected TCPWM.
 *
 * Return:
 *  0 - pass test.
 *  1 - fail test.
 *
 **********************************************************************************/
uint8_t SelfTest_Interrupt(TCPWM_Type* base, uint32_t cntNum)
{
    base1 = base;
    cntNum1 = cntNum;
    uint8_t ret = OK_STATUS;

    /* Stop and reset timer */
    #if CY_CPU_CORTEX_M0P
        Cy_TCPWM_TriggerStopOrKill(base, 1<<cntNum);
    #else
        Cy_TCPWM_TriggerStopOrKill_Single(base, cntNum);
    #endif
    /* Reset interrupt counter */
    selfTest_interrupt_counter = 0u;

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_INTERRUPT_HANDLING == 1)

    Cy_TCPWM_SetInterruptMask(base, cntNum, CY_TCPWM_INT_NONE);

    #endif /* End ERROR_IN_INTERRUPT_HANDLING */

    #if CY_CPU_CORTEX_M0P
        Cy_TCPWM_TriggerStart(base, 1<<cntNum);
    #else
        Cy_TCPWM_TriggerStart_Single(base, cntNum);
    #endif

    /* wait 1000uS */
    Cy_SysLib_DelayUs(INTERRUPT_TEST_TIME);

    /* Stop and reset timer */
    #if CY_CPU_CORTEX_M0P
        Cy_TCPWM_TriggerStopOrKill(base, 1<<cntNum);        
    #else
        Cy_TCPWM_TriggerStopOrKill_Single(base, cntNum);
    #endif
    /* If less than NUMBER_OF_TIMER_TICKS_LO ticks or greater than NUMBER_OF_TIMER_TICKS_HI - error
       in test */
    if ((selfTest_interrupt_counter < NUMBER_OF_TIMER_TICKS_LO) ||
        (selfTest_interrupt_counter > NUMBER_OF_TIMER_TICKS_HI))
    {
        ret = ERROR_STATUS;
    }

    return ret;
}

/* [] END OF FILE */

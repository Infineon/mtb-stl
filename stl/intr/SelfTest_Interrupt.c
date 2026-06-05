/******************************************************************************
 * File Name:  SelfTest_Interrupt.c
 *
 * Description:
 *  This file provides the source code for the interrupt self test.
 *  The interrupt handler implementation uses the base address and counter
 *  number definitions from the Device Configurator generated code.
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
#include "SelfTest_Interrupt.h"
#include "SelfTest_ErrorInjection.h"

/* This variable is used in the isr_1 interrupt handler */
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

    /* Stop and reset the timer */
    #if defined(CY_IP_M0S8TCPWM)
    uint32_t shiftedValue = (uint32_t)1U << cntNum;
    Cy_TCPWM_TriggerStopOrKill(base, shiftedValue);
    #else
    Cy_TCPWM_TriggerStopOrKill_Single(base, cntNum);
    #endif
    /* Delay for performing Cy_TCPWM_TriggerStopOrKill_Single. According to the documentation,
       Cy_TCPWM_TriggerStopOrKill_Single does not execute instantly. */
    Cy_SysLib_DelayUs(1u);

    /* Reset interrupt counter */
    selfTest_interrupt_counter = 0u;

    /* Check if an intentional error should be made for testing */
    #if (ERROR_IN_INTERRUPT_HANDLING == 1)

    Cy_TCPWM_SetInterruptMask(base, cntNum, CY_TCPWM_INT_NONE);

    #endif /* End ERROR_IN_INTERRUPT_HANDLING */

    #if defined(CY_IP_M0S8TCPWM)
    shiftedValue = (uint32_t)1U << cntNum;
    Cy_TCPWM_TriggerStart(base, shiftedValue);
    #else
    Cy_TCPWM_TriggerStart_Single(base, cntNum);
    #endif

    /* wait 1000uS */
    Cy_SysLib_DelayUs(INTERRUPT_TEST_TIME);

    /* Stop and reset the timer */
    #if defined(CY_IP_M0S8TCPWM)
    shiftedValue = (uint32_t)1U << cntNum;
    Cy_TCPWM_TriggerStopOrKill(base, shiftedValue);
    #else
    Cy_TCPWM_TriggerStopOrKill_Single(base, cntNum);
    #endif
    /* If less than NUMBER_OF_TIMER_TICKS_LO ticks or greater than NUMBER_OF_TIMER_TICKS_HI - error
       in the test */
    const uint32_t selfTest_interrupt_counter_val = selfTest_interrupt_counter;

    if ((selfTest_interrupt_counter_val < NUMBER_OF_TIMER_TICKS_LO) ||
        (selfTest_interrupt_counter_val > NUMBER_OF_TIMER_TICKS_HI))
    {
        ret = ERROR_STATUS;
    }

    return ret;
}


/* [] END OF FILE */

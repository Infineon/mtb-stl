/*******************************************************************************
* File Name: SelfTest_Clock.c
*
* Description: This file provides the source code for the APIs to perform
* clock source testing according to the Class B library.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
#include "SelfTest_Clock.h"
#include "SelfTest_ErrorInjection.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
static volatile uint16_t clock_test_isr_count;
static volatile uint32_t wdt_counter_1;
static TCPWM_Type* base1;
static uint32_t cntNum1;

/******************************************************************************
* Function Name: SelfTest_Clock_ISR_TIMER
*******************************************************************************
*
* Summary:
*  Handle Interrupt Service Routine. Source - Timer.
*
******************************************************************************/
void SelfTest_Clock_ISR_TIMER(void)
{
    /* Read WDT timer final value */
    wdt_counter_1 = Cy_WDT_GetCount();

    #if defined(CY_IP_MXTCPWM)
    Cy_TCPWM_Counter_Disable(base1, cntNum1);
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(base1, cntNum1);
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, interrupts);
    #else

    Cy_TCPWM_ClearInterrupt(base1, cntNum1, CY_TCPWM_INT_ON_TC);

    #endif
    /* Set the ISR count */
    clock_test_isr_count++;
}


/*****************************************************************************
* Function Name: SelfTests_Clock
******************************************************************************
*
* Summary:
*  Performs Clock test and verifies the measured clock frequency to be in the accuracy range
*
* Parameters:
*  TCPWM_Type * base - The pointer to a TCPWM instance.
*  cntNum - The Counter instance number in the selected TCPWM.
*
* Return:
*  1 - Test failed
*  2 - Still testing
*  3 - Test completed
*  4 - Incorrect Usage
*
* Theory:
*  For measuring the tested clock, the 16-bit WDT timer is used. It counts the number
*  of LFCLK (32 kHz) clock edges per 1000 uS period using a HW TCPWM.
*  After simple calculations, it is used to verify the clock accuracy.
*
*****************************************************************************/
uint8_t SelfTest_Clock(TCPWM_Type* base, uint32_t cntNum)
{
    base1 = base;
    cntNum1 = cntNum;
    static volatile uint32_t counter_0 = 0uL;
    volatile uint32_t counter_1;
    uint8_t ret;
    static uint8_t test_in_progress = 0;
    #if defined (CY_IP_MXS40SRSS)
    uint32_t CLOCK_TEST_TIME_TIMER_PERIOD =
        ((Cy_SysClk_ClkPeriGetFrequency() / 1000000U) * (CLOCK_TEST_TIME));
    #endif
    #if (defined(CY_IP_MXS40SSRSS))
    uint32_t CLOCK_TEST_TIME_TIMER_PERIOD =
        ((uint32_t)(((uint64_t)Cy_SysClk_ClkHfGetFrequency(3) / 1000000U) * CLOCK_TEST_TIME));
    #endif
    if (test_in_progress == 0u)
    {
        test_in_progress = 1u;
        clock_test_isr_count = 0u;

        #if (ERROR_IN_CLOCK == 1)
        #if defined(CY_IP_MXTCPWM)
        Cy_TCPWM_Counter_Enable(base, cntNum);
        #endif
        /* wait 250uS */
        Cy_TCPWM_Counter_SetPeriod(base, cntNum, CLOCK_TEST_TIME_TIMER_PERIOD/4);
        #else
        #if defined(CY_IP_MXTCPWM)
        Cy_TCPWM_Counter_Enable(base, cntNum);
        #endif
        /* Wait 1000uS */
        Cy_TCPWM_Counter_SetPeriod(base, cntNum, CLOCK_TEST_TIME_TIMER_PERIOD);
        #endif /* End ERROR_IN_CLOCK */

        /* Read WDT timer initial value */
        counter_0 = Cy_WDT_GetCount();

        /* Refresh and start the counter */
        #if defined(CY_IP_MXTCPWM)
        Cy_TCPWM_TriggerReloadOrIndex_Single(base, cntNum);
        #else
        Cy_TCPWM_TriggerReloadOrIndex(base, 1uL << cntNum);
        #endif

        ret = PASS_STILL_TESTING_STATUS;
    }
    else if (clock_test_isr_count == 0u)
    {
        /* Still waiting for the timer interrupt */
        ret = PASS_STILL_TESTING_STATUS;
    }
    else if (clock_test_isr_count == 1u)
    {
        /* ISR triggered once and is ready to perform calculations */

        /* Copy global ISR counter value to the local variable to avoid a value change*/
        counter_1 = wdt_counter_1;

        if (counter_0 < counter_1)
        {
            counter_0 = counter_1 - counter_0;
        }
        else
        {
            counter_0 = (counter_1 + 0xFFFFu) - counter_0;
        }

        const uint32_t counter_0_val = counter_0;
        /* If less than CLOCK_TICKS_LO ticks or more than CLOCK_TICKS_HI - error in test */
        if ((counter_0_val < CLOCK_TICKS_LO) || (counter_0_val > CLOCK_TICKS_HI))
        {
            ret = ERROR_STATUS;
        }
        else
        {
            ret = PASS_COMPLETE_STATUS;
        }

        test_in_progress = 0;
    }
    else
    {
        /* clock_test_isr_count > 1 */
        ret = ERROR_INCORRECT_USAGE_STATUS;
        test_in_progress = 0;
    }


    return ret;
}


/* [] END OF FILE */

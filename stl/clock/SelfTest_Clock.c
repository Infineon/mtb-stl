/*******************************************************************************
* File Name: SelfTest_Clock.c
* Version 1.0.0
*
* Description: This file provides the source code for the APIs to perform
* clock source testing according to the Class B library.
*
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

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))

    Cy_TCPWM_Counter_Disable(base1, cntNum1);
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(base1, cntNum1);
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, interrupts);
#else

    Cy_TCPWM_ClearInterrupt(base1, cntNum1, CY_TCPWM_INT_ON_TC);

#endif
    /* set the isr count */
    clock_test_isr_count++;
}


/*****************************************************************************
* Function Name: SelfTests_Clock
******************************************************************************
*
* Summary:
*  Performs Clock test and verify measured clock frequency to be in accuracy range
*
* Parameters:
*  TCPWM_Type * base - the pointer to a TCPWM instance
*  cntNum - The Counter instance number in the selected TCPWM
*
* Return:
*  1 - Test failed
*  2 - Still testing
*  3 - Test completed
*  4 - Incorrect Usage
*
* Theory:
*  For measuring the tested clock the 16-bit WDT timer is used. It counts the number
*  of LFCLK (32 kHz) clock edges per 1000 uS period using a HW TCPWM.
*  After simple calculations it is used to verify clock accuracy.
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
    #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)))
        uint32_t CLOCK_TEST_TIME_TIMER_PERIOD = ((Cy_SysClk_ClkPeriGetFrequency() / 1000000U) * (CLOCK_TEST_TIME));
    #endif
    #if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
        uint32_t CLOCK_TEST_TIME_TIMER_PERIOD = ((uint32_t)(((uint64_t)Cy_SysClk_ClkHfGetFrequency(3) / 1000000U) * CLOCK_TEST_TIME));
    #endif
    if (test_in_progress == 0u)
    {
        test_in_progress = 1u;
        clock_test_isr_count = 0u;

        #if (ERROR_IN_CLOCK == 1)
        #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
            Cy_TCPWM_Counter_Enable(base, cntNum);
        #endif
            /* wait 250uS */
            Cy_TCPWM_Counter_SetPeriod(base, cntNum, CLOCK_TEST_TIME_TIMER_PERIOD/4);
        #else
        #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
            Cy_TCPWM_Counter_Enable(base, cntNum);
        #endif
        /* wait 1000uS */
            Cy_TCPWM_Counter_SetPeriod(base, cntNum, CLOCK_TEST_TIME_TIMER_PERIOD);
        #endif /* End ERROR_IN_CLOCK */

        /* Read WDT timer initial value */
        counter_0 = Cy_WDT_GetCount();

        /* refresh and start counter */
        #if (CY_CPU_CORTEX_M0P)
            Cy_TCPWM_TriggerReloadOrIndex(base, 1uL << cntNum);
        #else
            Cy_TCPWM_TriggerReloadOrIndex_Single(base, cntNum);
        #endif

        ret = PASS_STILL_TESTING_STATUS;
    }
    else if (clock_test_isr_count == 0u)
    {
        /* Still waiting for timer interrupt */
        ret = PASS_STILL_TESTING_STATUS;
    }
    else if (clock_test_isr_count == 1u)
    {
        /* ISR triggered once and is ready to perform calculations */

        /* copy global isr counter value to local variable to avoid value change*/
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

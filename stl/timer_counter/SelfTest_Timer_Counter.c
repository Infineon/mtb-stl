/*******************************************************************************
* File Name: SelfTest_Timer_Counter.c
*
* Description: 
*  This file provides the source code for the APIs to perform
*  Timer counter source testing according to the Class B library.
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
#include "SelfTest_Timer_Counter.h"
#include "SelfTest_ErrorInjection.h"

#if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM))

/*******************************************************************************
* Global Variables
*******************************************************************************/
static volatile uint8_t int_occur = 0U;
static TCPWM_Type* base1;
static uint32_t cntNum1;


/******************************************************************************
* Function Name: SelfTest_TIMER_COUNTER_ISR
*******************************************************************************
*
* Summary:
*  Handle Interrupt Service Routine.
*
******************************************************************************/
void SelfTest_TIMER_COUNTER_ISR(void)
{
    Cy_TCPWM_Counter_Disable(base1, cntNum1);
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(base1, cntNum1);
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, interrupts);
    int_occur++;
}


/******************************************************************************
* Function Name: SelfTest_Timer_Counter_init
*******************************************************************************
*
* Summary:
* Initialize the counter and Timer interrupt for the Self test.
*
* \param base
* The pointer to a TCPWM instance.
*
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
* \param config
* The pointer to configuration structure. See \ref cy_stc_tcpwm_counter_config_t.
*
* \param intsrc
* Interrupt source
******************************************************************************/
void SelfTest_Timer_Counter_init(TCPWM_Type* base, uint32_t cntNum, cy_stc_tcpwm_counter_config_t const *config, IRQn_Type intsrc)
{
    base1 = base;
    cntNum1 = cntNum;

    #if defined (CY_IP_M7CPUSS)
    const cy_stc_sysint_t intrCfg =
    {
        /* Bit 0-15 of intrSrc is used to store system interrupt value and bit 16-31 to store CPU IRQ value */
        .intrSrc = (((uint32_t)NvicMux3_IRQn << 16U) | ((uint32_t)intsrc)),
        .intrPriority = 3UL
    };
    #else
    const cy_stc_sysint_t intrCfg =
    {
        .intrSrc = intsrc,
        .intrPriority = 3UL
    };
    #endif

    if(CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(base, cntNum, config))
    {
        CY_ASSERT(0U);
    }

    if(CY_SYSINT_SUCCESS != Cy_SysInt_Init(&intrCfg, SelfTest_TIMER_COUNTER_ISR))
    {
        CY_ASSERT(0U);
    }

    /* Enable Interrupt */
    #if defined (CY_IP_M7CPUSS)
    NVIC_EnableIRQ((IRQn_Type) NvicMux3_IRQn);
    #else
    NVIC_EnableIRQ(intrCfg.intrSrc);
    #endif

    Cy_TCPWM_Counter_Enable(base, cntNum);

    Cy_TCPWM_SetInterruptMask(base, cntNum, CY_TCPWM_INT_ON_CC);
}


/*****************************************************************************
* Function Name: SelfTest_Counter_Timer
******************************************************************************
*
* Summary:
*  Performs Timer Counter test by checking that the counter is incrementing.
*
*
* Return:
*  0 - Test Passed
*  1 - Test failed
*
*
*****************************************************************************/
uint8_t SelfTest_Counter_Timer(void)
{
    uint8_t ret = ERROR_STATUS;
    uint16_t delay_cnt = 0U;
    uint32_t counterVal;

    int_occur = 0U;

    Cy_TCPWM_Counter_SetCompare0(base1, cntNum1, TIMER_COUNTER_TEST_COMPARE);
    Cy_TCPWM_Counter_SetPeriod(base1, cntNum1, TIMER_COUNTER_TEST_PERIOD);
    Cy_TCPWM_Counter_SetCounter(base1, cntNum1, 0U);
    Cy_TCPWM_Counter_Enable(base1, cntNum1);

    #if (ERROR_IN_TIMER_COUNTER == 0U)
        #if defined(CY_IP_M0S8TCPWM)
        Cy_TCPWM_TriggerReloadOrIndex(base1, 1UL << cntNum1);
        #else
        Cy_TCPWM_TriggerReloadOrIndex_Single(base1, cntNum1);
        #endif
    #endif

    do
    {
        counterVal = Cy_TCPWM_Counter_GetCounter(base1, cntNum1);
        Cy_SysLib_DelayUs(1);
        delay_cnt++;
    } while ((int_occur == 0U) && (TIMER_COUNTER_TIMEOUT > delay_cnt));

    if(TIMER_COUNTER_TIMEOUT > delay_cnt)
    {
        if((TIMER_COUNTER_COUNT_LOW < counterVal) && (TIMER_COUNTER_COUNT_HIGH > counterVal))
        {
            ret = OK_STATUS;
        }
    }
    return ret;
}

#endif /* (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM)) */

/* [] END OF FILE */

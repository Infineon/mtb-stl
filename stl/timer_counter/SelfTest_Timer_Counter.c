/*******************************************************************************
* File Name: SelfTest_Timer_Counter.c
*
* Description:
*  This file provides the source code for the APIs to perform
*  the Timer counter self tests according to the Class B library.
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
*  Handles Interrupt Service Routine.
*
******************************************************************************/
static void SelfTest_TIMER_COUNTER_ISR(void)
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
* Initializes the counter and Timer interrupt for the self test.
*
* \param base
* The pointer to a TCPWM instance.
*
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
* \param config
* The pointer to the configuration structure. See \ref cy_stc_tcpwm_counter_config_t.
*
* \param intsrc
* Interrupt source
******************************************************************************/
void SelfTest_Timer_Counter_init(TCPWM_Type* base, uint32_t cntNum,
                                 cy_stc_tcpwm_counter_config_t const* config, IRQn_Type intsrc)
{
    base1 = base;
    cntNum1 = cntNum;

    #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
    const cy_stc_sysint_t intrCfg =
    {
        /* Bit 0-15 of intrSrc is used to store the system interrupt value and bit 16-31 to store CPU.
         * IRQ value */
        .intrSrc      = (((uint32_t)NvicMux3_IRQn << 16U) | ((uint32_t)intsrc)),
        .intrPriority = 3UL
    };
    #else
    const cy_stc_sysint_t intrCfg =
    {
        .intrSrc      = intsrc,
        .intrPriority = 3UL
    };
    #endif /* if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING)) */

    if (CY_TCPWM_SUCCESS != Cy_TCPWM_Counter_Init(base, cntNum, config))
    {
        CY_ASSERT(0U);
    }

    if (CY_SYSINT_SUCCESS != Cy_SysInt_Init(&intrCfg, SelfTest_TIMER_COUNTER_ISR))
    {
        CY_ASSERT(0U);
    }

    /* Enable Interrupt */
    #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
    NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);
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
*  Performs the Timer Counter test. Checks if the counter is incrementing.
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

    if (TIMER_COUNTER_TIMEOUT > delay_cnt)
    {
        if ((TIMER_COUNTER_COUNT_LOW < counterVal) && (TIMER_COUNTER_COUNT_HIGH > counterVal))
        {
            ret = OK_STATUS;
        }
    }
    return ret;
}


#endif /* (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM)) */

/* [] END OF FILE */

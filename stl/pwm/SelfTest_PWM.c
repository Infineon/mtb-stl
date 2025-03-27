/*******************************************************************************
* File Name: SelfTest_PWM.c
*
* Description:
*  This file provides the source code for the PWM self tests.
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
#include "SelfTest_PWM.h"
#include "SelfTest_ErrorInjection.h"


#if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM))

static TCPWM_Type *base1;
static uint32_t cntNum1;
static volatile uint16_t pwm_test_isr_count;
static uint32_t h_cnt, l_cnt;
static cy_stc_tcpwm_pwm_config_t const *config1;


static void TIMER_ISR(void)
{
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(base1, cntNum1);

    if (0UL != (CY_TCPWM_INT_ON_TC & interrupts))
    {
        if(pwm_test_isr_count >= 4U)
        {
            Cy_TCPWM_PWM_Disable(base1, cntNum1);
        }
        pwm_test_isr_count++;
    }
    
    /* Clear the interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, interrupts);
}


uint8_t SelfTest_PWM_init(TCPWM_Type *base, uint32_t cntNum,
             cy_stc_tcpwm_pwm_config_t const *config, IRQn_Type  intr_src)
{
    uint8_t ret = OK_STATUS;

#if defined (CY_IP_M7CPUSS)
    const cy_stc_sysint_t sTIMER_IRQ_cfg =
    {
        /* Bit 0-15 of intrSrc is used to store system interrupt value and bit 16-31 to store CPU IRQ value */
        .intrSrc = (((uint32_t)NvicMux3_IRQn << 16U) | ((uint32_t)intr_src)),
        .intrPriority = 2UL
    };
#else
    const cy_stc_sysint_t sTIMER_IRQ_cfg =
    {
        .intrSrc = intr_src,
        .intrPriority = 2UL
    };
#endif

    /* Configure the TCPWM for PWM operation. */
    if(CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(base, cntNum, config))
    {
        ret = (uint8_t)-1;
    }

    if(OK_STATUS == ret)
    {
        /* Set the interrupt line for TIMER_HW */
        if ((CY_SYSINT_SUCCESS != Cy_SysInt_Init(&sTIMER_IRQ_cfg, &TIMER_ISR)))
        {
            ret =  (uint8_t)-1;
        }
    }

    if (OK_STATUS == ret)
    {
#if defined (CY_IP_M7CPUSS)
        /* Enable system Interrupt */
        NVIC_EnableIRQ((IRQn_Type) NvicMux3_IRQn);
#else
        /* Enable Interrupt */
        NVIC_EnableIRQ(sTIMER_IRQ_cfg.intrSrc);
#endif

        base1 = base;
        cntNum1 = cntNum;
        config1  = config;
    }

    return ret;
}


uint8_t SelfTest_PWM(GPIO_PRT_Type *pinbase, uint32_t pinNum)
{
#if defined (CY_IP_MXTCPWM) && (CY_IP_MXTCPWM_VERSION >= 2U)
    (void)pinbase;
    (void)pinNum;
#endif
    uint8_t ret = ERROR_STATUS;
    uint8_t clk_divide;

    h_cnt = 0U;
    l_cnt = 0U;
    pwm_test_isr_count = 0;

    switch (config1->clockPrescaler)
    {
    case CY_TCPWM_PWM_PRESCALER_DIVBY_1:
        clk_divide = 1;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_2:
        clk_divide = 2;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_4:
        clk_divide = 4;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_8:
        clk_divide = 8;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_16:
        clk_divide = 16;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_32:
        clk_divide = 32;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_64:
        clk_divide = 64;
        break;
    case CY_TCPWM_PWM_PRESCALER_DIVBY_128:
        clk_divide = 128;
        break;
    default:
        clk_divide = 1;
        break;
    }

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
    uint32_t PWM_PERIOD = (((Cy_SysClk_ClkHfGetFrequency(3U) / 1000000U) / clk_divide) * (PWM_TIME));

#elif (defined(CY_CPU_CORTEX_M0P) && (CY_CPU_CORTEX_M0P))
    uint32_t PWM_PERIOD = (((Cy_SysClk_ClkHfGetFrequency() / 1000000U) / clk_divide) * (PWM_TIME));

#else
    uint32_t PWM_PERIOD = (((Cy_SysClk_ClkPeriGetFrequency() / 1000000U) / clk_divide) * (PWM_TIME));
#endif

    Cy_TCPWM_PWM_SetPeriod0(base1, cntNum1, (PWM_PERIOD - 1U));

#if (ERROR_IN_PWM == 1u)
    /* Set 50% duty cycle instead of 33% */
    Cy_TCPWM_PWM_SetCompare0(base1, cntNum1, (PWM_PERIOD / 2U));
#else
    Cy_TCPWM_PWM_SetCompare0(base1, cntNum1, ((PWM_PERIOD / 3U) - 1U));
#endif

    Cy_TCPWM_PWM_Enable(base1, cntNum1);

#if defined(CY_IP_M0S8TCPWM)
    Cy_TCPWM_TriggerReloadOrIndex(base1, 1UL << cntNum1);
#else
    Cy_TCPWM_TriggerReloadOrIndex_Single(base1, cntNum1);
#endif

    do
    {
#if defined (CY_IP_MXTCPWM) && (CY_IP_MXTCPWM_VERSION >= 2U)
        if (Cy_TCPWM_PWM_LineOutStatus(base1, cntNum1, CY_TCPWM_PWM_LINE_PWM) != 0UL)
        {
            h_cnt++;
        }
        else
        {
            l_cnt++;
        }
#else
        Cy_SysLib_DelayUs(1U);

        if (0U != Cy_GPIO_Read(pinbase, pinNum))
        {
            h_cnt++;
        }
        else
        {
            l_cnt++;
        }
#endif
    } while (pwm_test_isr_count < 5U);

    if (h_cnt != 0U) 
    {
        float off_on_ratio = (float)l_cnt/(float)h_cnt;
        if ((off_on_ratio > (float)1.875) && (off_on_ratio < (float)2.125))
        {
            ret = OK_STATUS;
        }
    }

    return ret;
}

#endif

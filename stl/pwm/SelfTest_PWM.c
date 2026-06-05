/*******************************************************************************
* File Name: SelfTest_PWM.c
*
* Description:
*  This file provides the source code for the PWM self tests.
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
#include "SelfTest_PWM.h"
#include "SelfTest_ErrorInjection.h"


#if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM))

static TCPWM_Type* base1;
static uint32_t cntNum1;
static volatile uint16_t pwm_test_isr_count;
static uint32_t h_cnt, l_cnt;
static cy_stc_tcpwm_pwm_config_t const* config1;


/******************************************************************************
* Function Name: TIMER_ISR
***************************************************************************//**
*
*  Handle the timer Interrupt Service Routine.
*
******************************************************************************/
static void TIMER_ISR(void)
{
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(base1, cntNum1);

    if (0UL != (CY_TCPWM_INT_ON_TC & interrupts))
    {
        if (pwm_test_isr_count >= 4U)
        {
            Cy_TCPWM_PWM_Disable(base1, cntNum1);
        }
        pwm_test_isr_count++;
    }

    /* Clear the interrupt */
    Cy_TCPWM_ClearInterrupt(base1, cntNum1, interrupts);
}


/******************************************************************************
* Function Name: SelfTest_PWM_init
***************************************************************************//**
*
* Initialize Timer interrupt for the Self test.
*
* \param base
* The pointer to a TCPWM instance.
*
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
* \param config
* The pointer to the configuration structure.
*
* \param intr_src
* Interrupt source
*
* \return
*  0 - Initialization successful <br>
*  -1 (255) - Initialization failed
******************************************************************************/
uint8_t SelfTest_PWM_init(TCPWM_Type* base, uint32_t cntNum,
                          cy_stc_tcpwm_pwm_config_t const* config, IRQn_Type  intr_src)
{
    uint8_t ret = OK_STATUS;

    #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
    const cy_stc_sysint_t sTIMER_IRQ_cfg =
    {
        /* Bit 0-15 of intrSrc is used to store the system interrupt value and bit 16-31 to store the CPU
           IRQ value */
        .intrSrc      = (((uint32_t)NvicMux3_IRQn << 16U) | ((uint32_t)intr_src)),
        .intrPriority = 2UL
    };
    #else
    const cy_stc_sysint_t sTIMER_IRQ_cfg =
    {
        .intrSrc      = intr_src,
        .intrPriority = 2UL
    };
    #endif /* if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING)) */

    /* Configure the TCPWM for PWM operation. */
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(base, cntNum, config))
    {
        ret = (uint8_t)-1;
    }

    if (OK_STATUS == ret)
    {
        /* Set the interrupt line for TIMER_HW */
        if ((CY_SYSINT_SUCCESS != Cy_SysInt_Init(&sTIMER_IRQ_cfg, &TIMER_ISR)))
        {
            ret =  (uint8_t)-1;
        }
    }

    if (OK_STATUS == ret)
    {
        #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
        /* Enable system Interrupt */
        NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);
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


/*******************************************************************************
* Function Name: SelfTest_PWM
****************************************************************************//**
*
* This function performs the PWM test and verifies that the duty cycle set is actually achieved
* in the output.
*
* \param pinbase
* The pointer to the GPIO port instance to which the PWM pin is connected.
*
* \param pinNum
* The GPIO pin number to which the PWM pin is connected.
*
*
* \note
* The parameter passed for CAT1C devices will be ignored
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_PWM(GPIO_PRT_Type* pinbase, uint32_t pinNum)
{
    #if defined (CY_IP_MXTCPWM) && (CY_IP_MXTCPWM_VERSION >= 2U)
    (void)pinbase;
    (void)pinNum;
    #endif
    uint8_t ret = ERROR_STATUS;
    uint8_t clk_divide;
    uint32_t timeout;

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

    #if (defined(CY_IP_MXS40SSRSS))
    uint32_t PWM_PERIOD =
        (((Cy_SysClk_ClkHfGetFrequency(3U) / 1000000U) / clk_divide) * (PWM_TIME));

    #elif defined (CY_IP_MXS40SRSS)
    uint32_t PWM_PERIOD =
        (((Cy_SysClk_ClkPeriGetFrequency() / 1000000U) / clk_divide) * (PWM_TIME));
    #else
    uint32_t PWM_PERIOD = (((Cy_SysClk_ClkHfGetFrequency() / 1000000U) / clk_divide) * (PWM_TIME));
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

    timeout = 0uL;

    do
    {
        Cy_SysLib_DelayUs(1U);
        timeout++;

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
        if (0U != Cy_GPIO_Read(pinbase, pinNum))
        {
            h_cnt++;
        }
        else
        {
            l_cnt++;
        }
        #endif /* if defined (CY_IP_MXTCPWM) && (CY_IP_MXTCPWM_VERSION >= 2U) */
    } while ((pwm_test_isr_count < 5U) && (PWM_TIME_TIMEOUT > timeout));

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


#endif /* if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM)) */

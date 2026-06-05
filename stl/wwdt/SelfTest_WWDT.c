/*******************************************************************************
* File Name: SelfTest_WWDT.c
*
* Description:
*  This file provides the source code for the windowed watchdog timer
*  Class B self tests.
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
#include "SelfTest_WWDT.h"
#include "SelfTest_ErrorInjection.h"

#if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2))

void WdtInterruptHandler(void);


static volatile uint32_t wwdt_count;
static volatile uint32_t wwdt_intr_occured;

/* WDT interrupt configuration structure */
static const cy_stc_sysint_t WDT_IRQ_cfg =
{
    .intrSrc      = (((uint32_t)NvicMux3_IRQn << 16) | ((uint32_t)WDT_IRQ_INTC_NUMBER)),
    .intrPriority = 3
};


/*******************************************************************************
* Function Name: SelfTest_Windowed_WDT
****************************************************************************//**
*
* This function checks the WDT in window mode. It: <br>
* 1) Services the WDT before the LOWER limit which causes the device to reset. <br>
* 2) Check the occurance of interrupt at WARN LIMIT.
*
*
* \note
* Only applicable for CAT1C devices
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_Windowed_WDT(void)
{
    wwdt_intr_occured = 0U;
    /* Initialize WDT */
    /* Check the reason for the device restart */
    if (CY_SYSLIB_RESET_HWWDT == Cy_SysLib_GetResetReason())
    {
        Cy_SysLib_ClearResetReason();
        wwdt_intr_occured = 1U;
    }
    else
    {
        Cy_SysLib_ClearResetReason();
    }

    /* Unlock WDT and set actions and limits */
    Cy_WDT_Unlock();
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);
    Cy_WDT_SetLowerLimit(WDT_LOWER_LIMIT);
    Cy_WDT_SetLowerAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_RESET);
    Cy_WDT_SetUpperLimit(WDT_UPPER_LIMIT);
    Cy_WDT_SetUpperAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_NONE);
    Cy_WDT_SetWarnLimit(WDT_WARN_LIMIT);
    Cy_WDT_SetWarnAction(CY_WDT_WARN_ACTION_INT);

    /* Clear match event interrupt, if any */
    Cy_WDT_ClearInterrupt();
    /* Enable interrupt */
    (void)Cy_SysInt_Init(&WDT_IRQ_cfg, WdtInterruptHandler);
    NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);
    Cy_WDT_UnmaskInterrupt();

    Cy_WDT_Enable();

    Cy_SysLib_Delay(3000);

    if (1U == wwdt_intr_occured)
    {
        do
        {
            wwdt_count = Cy_WDT_GetCount();
        } while (wwdt_count < WDT_UPPER_LIMIT);

        if (1U == wwdt_intr_occured)
        {
            Cy_WDT_Disable();
            Cy_WDT_Lock();
            return OK_STATUS;
        }
    }
    else
    {
        do
        {
            wwdt_count = Cy_WDT_GetCount();
            #if (!ERROR_IN_WWDT_LOWER_LIMIT)
            if (wwdt_count >= 3000U)
            {
                Cy_WDT_SetService();
            }
            #endif
        } while (wwdt_count <= WDT_LOWER_LIMIT);
    }

    Cy_WDT_Disable();
    Cy_WDT_Lock();
    return ERROR_STATUS;
}


/*******************************************************************************
* Function Name: Wdt_IsWdtInterrutpSet
****************************************************************************//**
*
* Checks if the Watchdog Timer (WDT) interrupt is set.
*
* \return
*  0 - WDT interrupt is not set <br>
*  Non-zero - WDT interrupt is set
*
*******************************************************************************/
static uint32_t Wdt_IsWdtInterrutpSet(void)
{
    return (SRSS_WDT_INTR & WDT_INTR_WDT_Msk);
}


/*******************************************************************************
* Function Name: WdtInterruptHandler
****************************************************************************//**
*
* Interrupt Service Routine (ISR) for the Watchdog Timer (WDT).
*
*******************************************************************************/
void WdtInterruptHandler(void)
{
    /* Check if the interrupt is from WDT */
    if (Wdt_IsWdtInterrutpSet() != 0UL)
    {
        #if ERROR_IN_WWDT_INTR
        wwdt_intr_occured+=2;
        #else
        wwdt_intr_occured++;
        #endif
        /* Clear WDT Interrupt */
        Cy_WDT_ClearInterrupt();
    }
}


#endif /* if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2)) */

/* [] END OF FILE */

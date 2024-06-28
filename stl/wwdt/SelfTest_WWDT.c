/*******************************************************************************
* File Name: SelfTest_WWDT.c
* Version 1.0.0
*
* Description:
*  This file provides the source code for the windowed watchdog timer
*  Class B support tests for CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  XMC7200D-E272K8384
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
#include "SelfTest_WWDT.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

#if CY_CPU_CORTEX_M7

void WdtInterruptHandler(void);


static volatile uint32_t wwdt_count;
static volatile uint32_t wwdt_intr_occured;
static volatile bool wwdt_reset_occured;

/* WDT interrupt configuration structure */
const cy_stc_sysint_t WDT_IRQ_cfg = {
    .intrSrc = ((NvicMux3_IRQn << 16) | WDT_IRQ_INTC_NUMBER),
    .intrPriority = 3
};

uint8_t SelfTest_Windowed_WDT(void)
{
    wwdt_intr_occured = 0;
    wwdt_intr_occured = false;
    /* Initialize WDT */
    /* Check the reason for device restart */
    if(CY_SYSLIB_RESET_HWWDT == Cy_SysLib_GetResetReason())
    {
        Cy_SysLib_ClearResetReason();
        wwdt_intr_occured = true;
    }
    else
    {
        Cy_SysLib_ClearResetReason();
    }

    /* Step 1- Unlock WDT */
    Cy_WDT_Unlock();
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);
    Cy_WDT_SetLowerLimit(WDT_LOWER_LIMIT);
    Cy_WDT_SetLowerAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_RESET);
    Cy_WDT_SetUpperLimit(WDT_UPPER_LIMIT);
    Cy_WDT_SetUpperAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_NONE);
    Cy_WDT_SetWarnLimit(WDT_WARN_LIMIT);
    Cy_WDT_SetWarnAction(CY_WDT_WARN_ACTION_INT);

    /* Step 4- Clear match event interrupt, if any */
    Cy_WDT_ClearInterrupt();
    /* Step 5 - Enable interrupt */
    Cy_SysInt_Init(&WDT_IRQ_cfg, WdtInterruptHandler);
    NVIC_EnableIRQ((IRQn_Type) NvicMux3_IRQn);
    Cy_WDT_UnmaskInterrupt();

    /* Step 6- Enable WDT */
    Cy_WDT_Enable();

    Cy_SysLib_Delay(3000);

    if(true == wwdt_intr_occured)
    {
        do
        {
            wwdt_count = Cy_WDT_GetCount();
        } while (wwdt_count < WDT_UPPER_LIMIT);

        if (1 == wwdt_intr_occured)
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
            if (wwdt_count >= 3000)
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


uint32_t Wdt_IsWdtInterrutpSet(void)
{
#if defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2)
    return (SRSS_WDT_INTR & WDT_INTR_WDT_Msk);
#else
    return (SRSS_SRSS_INTR & SRSS_SRSS_INTR_WDT_MATCH_Msk);
#endif
}

void WdtInterruptHandler(void)
{
    /* Check if the interrupt is from WDT */
    if(Wdt_IsWdtInterrutpSet())
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

#endif

/* [] END OF FILE */

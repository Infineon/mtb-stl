/*******************************************************************************
* File Name: SelfTest_WWDT.c
*
* Description:
*  This file provides the source code for the windowed watchdog timer
*  Class B self tests.
*
*******************************************************************************
* (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
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


static volatile uint32_t stlWwdt_count;
static volatile uint32_t stlWwdt_intrOccurred;

/* WDT interrupt configuration structure */
static const cy_stc_sysint_t stlWwdt_irqConfig =
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
* Only applicable for XMC7000 and XMC5000 devices
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_Windowed_WDT(void)
{
    uint8_t result = ERROR_STATUS;
    cy_en_sysint_status_t sysintStatus;

    stlWwdt_intrOccurred = 0U;
    /* Initialize WDT */
    /* Check the reason for the device restart */
    if (CY_SYSLIB_RESET_HWWDT == Cy_SysLib_GetResetReason())
    {
        Cy_SysLib_ClearResetReason();
        stlWwdt_intrOccurred = 1U;
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
    sysintStatus = Cy_SysInt_Init(&stlWwdt_irqConfig, WdtInterruptHandler);
    if (CY_SYSINT_SUCCESS == sysintStatus)
    {
        NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);
        Cy_WDT_UnmaskInterrupt();
    }

    if (CY_SYSINT_SUCCESS == sysintStatus)
    {
        Cy_WDT_Enable();

        Cy_SysLib_Delay(3000);

        if (1U == stlWwdt_intrOccurred)
        {
            do
            {
                stlWwdt_count = Cy_WDT_GetCount();
            } while (stlWwdt_count < WDT_UPPER_LIMIT);

            if (1U == stlWwdt_intrOccurred)
            {
                result = OK_STATUS;
            }
        }
        else
        {
            do
            {
                stlWwdt_count = Cy_WDT_GetCount();
                #if (!ERROR_IN_WWDT_LOWER_LIMIT)
                if (stlWwdt_count >= 3000U)
                {
                    Cy_WDT_SetService();
                }
                #endif
            } while (stlWwdt_count <= WDT_LOWER_LIMIT);
        }
    }
    Cy_WDT_Disable();
    Cy_WDT_Lock();
    return result;
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
        stlWwdt_intrOccurred+=2;
        #else
        stlWwdt_intrOccurred++;
        #endif
        /* Clear WDT Interrupt */
        Cy_WDT_ClearInterrupt();
    }
}


#endif /* if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2)) */

#if defined(CY_IP_MXS40SSRSS)

/* Lower limit (ticks). Service before this threshold triggers SWWDTx system reset (~0.610 s at 32.8 kHz nominal). */
#define STL_MCWDT_LOWER_LIMIT           (20000u)
/* Match limit (ticks). Counter reaching this value fires the interrupt (~1.372 s at 32.8 kHz nominal). */
#define STL_MCWDT_MATCH_LIMIT           (45000u)
/* Counter threshold above which Phase 1 services the MCWDT early to trigger the windowed reset. */
#define STL_MCWDT_SERVICE_THRESHOLD     (1000u)
/* Extra LFCLK ticks polled above MATCH_LIMIT in Phase 2 to allow the match ISR to execute.
 * The MCWDT counter and interrupt both pass through a 2-LFCLK CDC synchronizer to the CPU
 * clock domain. 100 ticks (~3 ms at 32.8 kHz nominal) is well above the worst-case
 * 2-cycle synchronizer latency and negligible relative to the ~1372 ms test duration. */
#define STL_MCWDT_MATCH_SETTLE          (100u)
/* Maximum consecutive CPU-loop iterations without observing an LFCLK tick change before
 * declaring a counter stall fault. Resets to 0 on every observed tick. This guards against
 * a frozen counter regardless of CPU speed; the loop exits within microseconds if the
 * MCWDT clock is absent. A large value avoids false trips on a heavily loaded CPU. */
#define STL_MCWDT_STALL_LIMIT           (500000u)

static void McwdtInterruptHandler(void);

static volatile uint32_t stlWwdt_mcwdtIntrOccurred;
static MCWDT_STRUCT_Type* stlWwdt_mcwdtBase;
static cy_en_mcwdtctr_t   stlWwdt_mcwdtCounter;


/*******************************************************************************
* Function Name: SelfTest_Windowed_MCWDT
*******************************************************************************/
uint8_t SelfTest_Windowed_MCWDT(MCWDT_STRUCT_Type* base, cy_en_mcwdtctr_t counter)
{
    uint32_t count;
    uint32_t  ctrMask = (1UL << (uint32_t)counter);
    bool      mcwdtResetOccurred;
    uint8_t   result = ERROR_STATUS;

    /* Derive the SWWDTx reset-cause bit and peripheral IRQ from the MCWDT block pointer.
     * CY_SYSLIB_RESET_SWWDTx maps to the block number, not the counter within it.
     * Both remain at their invalid sentinel values for NULL or unrecognised base,
     * which is used as a validity gate below. */
    uint32_t  swwdtResetMask = 0UL;
    IRQn_Type irqIntcNumber  = srss_interrupt_mcwdt_0_IRQn;
    if (base == MCWDT_STRUCT0)
    {
        swwdtResetMask = CY_SYSLIB_RESET_SWWDT0;
    }
    #if defined(MCWDT_STRUCT1)
    else if (base == MCWDT_STRUCT1)
    {
        swwdtResetMask = CY_SYSLIB_RESET_SWWDT1;
        irqIntcNumber  = srss_interrupt_mcwdt_1_IRQn;
    }
    #endif
    #if defined(MCWDT_STRUCT2)
    else if (base == MCWDT_STRUCT2)
    {
        swwdtResetMask = CY_SYSLIB_RESET_SWWDT2;
        irqIntcNumber  = srss_interrupt_mcwdt_2_IRQn;
    }
    #endif
    #if defined(MCWDT_STRUCT3)
    else if (base == MCWDT_STRUCT3)
    {
        swwdtResetMask = CY_SYSLIB_RESET_SWWDT3;
        irqIntcNumber  = srss_interrupt_mcwdt_3_IRQn;
    }
    #endif
    else
    {
        /* NULL or unknown base — swwdtResetMask stays 0, rejected below */
    }

    /* Only CTR0/CTR1 support windowed mode; base must be a known MCWDT block */
    if (((counter == CY_MCWDT_COUNTER0) || (counter == CY_MCWDT_COUNTER1)) &&
        (swwdtResetMask != 0UL))
    {
        cy_stc_sysint_t mcwdtIrqCfg =
        {
            .intrSrc      = irqIntcNumber,
            .intrPriority = 3U
        };

        stlWwdt_mcwdtBase           = base;
        stlWwdt_mcwdtCounter        = counter;
        stlWwdt_mcwdtIntrOccurred   = 0U;

        /* Determine test phase from reset reason.
         * Clear only when the expected SWWDTx reset is detected; preserving other
         * reset-cause bits that may be needed by other tests.
         * Note: Cy_SysLib_ClearResetReason() clears all bits — the PDL provides no
         * selective-clear API, so it is called only when this test owns the reset. */
        mcwdtResetOccurred = (0UL != (Cy_SysLib_GetResetReason() & swwdtResetMask));
        if (mcwdtResetOccurred)
        {
            Cy_SysLib_ClearResetReason();
        }

        /* Configure selected MCWDT counter */
        Cy_MCWDT_Unlock(base);
        cy_stc_mcwdt_config_t cfg = { 0 };
        if (counter == CY_MCWDT_COUNTER0)
        {
            cfg.c0Match          = STL_MCWDT_MATCH_LIMIT;
            cfg.c0Mode           = (uint8_t)CY_MCWDT_MODE_INT;
            cfg.c0LowerLimit     = STL_MCWDT_LOWER_LIMIT;
            cfg.c0LowerLimitMode = (uint16_t)CY_MCWDT_LOWER_LIMIT_MODE_RESET;
            cfg.c1LowerLimitMode = (uint16_t)CY_MCWDT_LOWER_LIMIT_MODE_NOTHING;
        }
        else /* CY_MCWDT_COUNTER1 */
        {
            cfg.c1Match          = STL_MCWDT_MATCH_LIMIT;
            cfg.c1Mode           = (uint8_t)CY_MCWDT_MODE_INT;
            cfg.c1LowerLimit     = STL_MCWDT_LOWER_LIMIT;
            cfg.c1LowerLimitMode = (uint16_t)CY_MCWDT_LOWER_LIMIT_MODE_RESET;
            cfg.c0LowerLimitMode = (uint16_t)CY_MCWDT_LOWER_LIMIT_MODE_NOTHING;
        }

        if (CY_MCWDT_SUCCESS == Cy_MCWDT_Init(base, &cfg))
        {
            /* Install ISR and enable interrupt */
            if (CY_SYSINT_SUCCESS == Cy_SysInt_Init(&mcwdtIrqCfg, McwdtInterruptHandler))
            {
                Cy_MCWDT_SetInterruptMask(base, ctrMask);
                NVIC_EnableIRQ(irqIntcNumber);

                /* Enable selected counter */
                Cy_MCWDT_Unlock(base);
                Cy_MCWDT_Enable(base, ctrMask, 0U);
                Cy_MCWDT_Lock(base);

                if (mcwdtResetOccurred)
                {
                    /* Phase 2: poll until the counter has advanced STL_MCWDT_MATCH_SETTLE ticks
                    * past MATCH_LIMIT, giving the match ISR at least one LFCLK period to execute.
                    * Guard on observed tick changes exits the loop if the counter is frozen. */
                    uint32_t prevCount  = Cy_MCWDT_GetCount(base, counter);
                    uint32_t stallCount = 0U;
                    do
                    {
                        count = Cy_MCWDT_GetCount(base, counter);
                        if (count != prevCount)
                        {
                            stallCount = 0U;
                            prevCount  = count;
                        }
                        else
                        {
                            stallCount++;
                        }
                        if (stallCount >= (uint32_t)STL_MCWDT_STALL_LIMIT)
                        {
                            break;
                        }
                    } while (count < ((uint32_t)STL_MCWDT_MATCH_LIMIT + (uint32_t)STL_MCWDT_MATCH_SETTLE));

                    if (1U == stlWwdt_mcwdtIntrOccurred)
                    {
                        result = OK_STATUS;
                    }
                }
                else
                {
                    /* Phase 1: service early to trigger SWWDTx reset.
                     * Guard on observed tick changes exits the loop if the counter is frozen. */
                    uint32_t prevCount  = Cy_MCWDT_GetCount(base, counter);
                    uint32_t stallCount = 0U;
                    do
                    {
                        count = Cy_MCWDT_GetCount(base, counter);
                        if (count != prevCount)
                        {
                            stallCount = 0U;
                            prevCount  = count;
                        }
                        else
                        {
                            stallCount++;
                        }
                        if (stallCount >= (uint32_t)STL_MCWDT_STALL_LIMIT)
                        {
                            break;
                        }
                        #if (!ERROR_IN_WWDT_LOWER_LIMIT)
                        if (count >= (uint32_t)STL_MCWDT_SERVICE_THRESHOLD)
                        {
                            /* Must unlock MCWDT_CTL before ResetCounters — lock protects CTL */
                            Cy_MCWDT_Unlock(base);
                            Cy_MCWDT_ResetCounters(base, ctrMask, 0U);
                            Cy_MCWDT_Lock(base);
                            /* System resets here via SWWDTx */
                            prevCount  = 0U; /* counter restarted from 0 after reset */
                            stallCount = 0U; /* reset stall counter after hardware reset */
                        }
                        #endif /* if (!ERROR_IN_WWDT_LOWER_LIMIT) */
                    } while (count <= (uint32_t)STL_MCWDT_LOWER_LIMIT);
                }

                Cy_MCWDT_Unlock(base);
                Cy_MCWDT_Disable(base, ctrMask, 0U);
            }
            else
            {
                Cy_MCWDT_Lock(base);
            }
        }
        else
        {
            Cy_MCWDT_Lock(base);
        }
    } /* end if counter == CTR0 || CTR1 */

    return result;
}


/*******************************************************************************
* Function Name: McwdtInterruptHandler
****************************************************************************//**
*
* Interrupt Service Routine (ISR) for the MCWDT CTR0 match event.
*
*******************************************************************************/
static void McwdtInterruptHandler(void)
{
    if (0UL != Cy_MCWDT_GetInterruptStatusMasked(stlWwdt_mcwdtBase))
    {
        uint32_t ctrMask = (1UL << (uint32_t)stlWwdt_mcwdtCounter);
        #if ERROR_IN_WWDT_INTR
        stlWwdt_mcwdtIntrOccurred += 2U;
        #else
        stlWwdt_mcwdtIntrOccurred++;
        #endif
        Cy_MCWDT_ClearInterrupt(stlWwdt_mcwdtBase, ctrMask);
    }
}


#endif /* defined(CY_IP_MXS40SSRSS) */

/* [] END OF FILE */

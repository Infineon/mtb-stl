/*******************************************************************************
* File Name: SelfTest_Clock.c
*
* Description: This file provides the source code for the APIs to perform
* clock source testing according to the Class B library.
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
#include "SelfTest_Clock.h"
#include "SelfTest_ErrorInjection.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
static volatile uint16_t stlClock_testIsrCount;
static volatile uint32_t stlClock_wdtCounter1;
static TCPWM_Type* stlClock_base1;
static uint32_t stlClock_cntNum1;



#if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL))

/* Shared state consumed by SelfTest_Clock_CSV and SelfTest_Clock_CSV_ISR. */
static volatile cy_en_SysFault_source_t stlCsv_observedSrc;
static FAULT_STRUCT_Type* stlCsv_activeFault;
static IRQn_Type stlCsv_activeNvicMux;

static cy_stc_SysFault_t fault_cfg =
{
    .ResetEnable   = false,
    .OutputEnable  = false,
    .TriggerEnable = false,
};
#endif // if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL))

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
    stlClock_wdtCounter1 = Cy_WDT_GetCount();

    #if defined(CY_IP_MXTCPWM)
    Cy_TCPWM_Counter_Disable(stlClock_base1, stlClock_cntNum1);
    uint32_t interrupts = Cy_TCPWM_GetInterruptStatusMasked(stlClock_base1, stlClock_cntNum1);
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(stlClock_base1, stlClock_cntNum1, interrupts);
    #else

    Cy_TCPWM_ClearInterrupt(stlClock_base1, stlClock_cntNum1, CY_TCPWM_INT_ON_TC);

    #endif
    /* Set the ISR count */
    stlClock_testIsrCount++;
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
    stlClock_base1 = base;
    stlClock_cntNum1 = cntNum;
    static volatile uint32_t counter_0 = 0uL;
    volatile uint32_t counter_1;
    uint8_t ret;
    static uint8_t test_in_progress = 0;
    #if defined (CY_IP_MXS40SRSS)
    uint32_t CLOCK_TEST_TIME_TIMER_PERIOD =
        ((Cy_SysClk_ClkPeriGetFrequency() / 1000000U) * (CLOCK_TEST_TIME));
    #endif
    #if (defined(CY_IP_MXS40SSRSS) || defined(CY_IP_MXS22SRSS))
    uint32_t CLOCK_TEST_TIME_TIMER_PERIOD =
        ((uint32_t)(((uint64_t)Cy_SysClk_ClkHfGetFrequency(STL_CLOCK_SOURCE_HFCLOCK) / 1000000U) * CLOCK_TEST_TIME));
    #endif
    if (test_in_progress == 0u)
    {
        test_in_progress = 1u;
        stlClock_testIsrCount = 0u;

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
    else if (stlClock_testIsrCount == 0u)
    {
        /* Still waiting for the timer interrupt */
        ret = PASS_STILL_TESTING_STATUS;
    }
    else if (stlClock_testIsrCount == 1u)
    {
        /* ISR triggered once and is ready to perform calculations */

        /* Copy global ISR counter value to the local variable to avoid a value change*/
        counter_1 = stlClock_wdtCounter1;

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
        /* stlClock_testIsrCount > 1 */
        ret = ERROR_INCORRECT_USAGE_STATUS;
        test_in_progress = 0;
    }


    return ret;
}


#if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL))
/*******************************************************************************
* Function Name: SelfTest_Clock_CSV_ISR
********************************************************************************
*
* Summary:
*  ISR handler for SelfTest_Clock_CSV. The application routes the
*  fault-struct interrupt to this handler with Cy_SysInt_Init and enables
*  the mux line before calling SelfTest_Clock_CSV.
*
*  The NVIC mux line is disabled by this function so the ISR doesn't re-enter
*  while the impossible CSV window is still armed. SelfTest_Clock_CSV clears the
*  interrupt mask and any residual pending edge during teardown.
*
*******************************************************************************/
void SelfTest_Clock_CSV_ISR(void)
{
    /* Record the source */
    stlCsv_observedSrc = Cy_SysFault_GetErrorSource(stlCsv_activeFault);

    Cy_SysFault_ClearStatus(stlCsv_activeFault);
    Cy_SysFault_ClearInterrupt(stlCsv_activeFault);

    NVIC_DisableIRQ(stlCsv_activeNvicMux);
}


/*******************************************************************************
* Function Name: SelfTest_Clock_CSV
********************************************************************************
*
* Summary:
*  Runs the CSV functional self-test for the selected CLK_HF instance:
*  triggers induced CSV fault by setting impossible supervision window,
*  waits for the induced fault to propagate and register by
*  \ref SelfTest_Clock_CSV_ISR.
*
* \param entry
*  Pointer to \ref stl_clock_csv_entry_t describing the CLK_HF under
*  test, the app-provided fault structure, and the NVIC mux the
*  fault-struct interrupt is routed to.
*
* \return
*  \ref OK_STATUS        - CSV self test pass
*  \ref ERROR_STATUS     - Induced CSV fault was not observed within
*                          \ref CSV_TEST_TIMEOUT_US poll iterations, or
*                          \ref SelfTest_Clock_CSV_ISR recorded a fault source
*                          other than \c CY_SYSFAULT_SRSS_CSV.
*  \ref ERROR_BAD_PARAM  - \p entry is NULL, \c clkHf is 0, \c faultStruct is
*                          NULL, the \c refClk frequency is 0 (e.g. ECO/EXT not
*                          configured), or \c Cy_SysClk_ClkHfCsvManualConfigure()
*                          rejected the configuration.
*
* \note
*  Before calling this function the application MUST:
*  -# Enable global interrupts (e.g. \c __enable_irq()). The self-test checks
*     induced CSV fault using \ref SelfTest_Clock_CSV_ISR interrupt handler.
*  -# Route the fault-struct interrupt (e.g. \c cpuss_interrupts_fault_N_IRQn)
*     to \ref SelfTest_Clock_CSV_ISR using \c Cy_SysInt_Init with
*     \c NVIC_ClearPendingIRQ and \c NVIC_EnableIRQ on \ref stl_clock_csv_entry_t::nvicMux.
*  -# Provide a valid \ref stl_clock_csv_entry_t::faultStruct.
*
* \note
*  Side effects on entry:
*   - The function reinitializes the provided fault structure, sets the
*     \c CY_SYSFAULT_SRSS_CSV mask, and clears STATUS/INTR. Any prior
*     application configuration of that fault structure is overwritten.
*   - The CSV comparator on \ref stl_clock_csv_entry_t::clkHf is disabled and
*     temporarily reprogrammed with an impossible supervision window
*     to force an immediate fault. Any previously programmed operational CSV
*     window for specified CLK_HF is overwritten.
* \note
*  Side effects on exit:
*   - CSV on \ref stl_clock_csv_entry_t::clkHf is left disabled — the app
*     must call \c Cy_SysClk_ClkHfCsvConfigure() and \c Cy_SysClk_ClkHfCsvEnable()
*     with its expected accuracy window.
*   - The fault-struct interrupt mask is cleared. On \ref OK_STATUS the NVIC
*     mux is also disabled (by \ref SelfTest_Clock_CSV_ISR), otherwise it is
*     left as the app configured it. The app must setup its own interrupt
*     handler to handle operational CSV faults.
*   - After the app re-arms its expected accuracy window, enabling CSV can
*     latch \c STATUS.VALID while the CSV state machine settles. Clear STATUS
*     (loop until \c Cy_SysFault_GetErrorSource() returns \c CY_SYSFAULT_NO_FAULT)
*     and clear INTR before enabling the fault-struct interrupt mask and the
*     NVIC mux to avoid false faults.
* \note
*  This function is intended to be called ONCE at startup, before operational
*  CSV supervision is armed.
* \note
*  This function is blocking (polls up to \ref CSV_TEST_TIMEOUT_US microseconds)
*  and is not reentrant.
*
*******************************************************************************/
uint8_t SelfTest_Clock_CSV(const stl_clock_csv_entry_t* entry)
{
    uint8_t  ret = ERROR_BAD_PARAM;

    if ((entry != NULL) &&
        (entry->clkHf != 0u) &&
        (entry->faultStruct != NULL) &&
        (Cy_SysClk_ClkHfCsvGetRefFrequency(entry->refClk) != 0u))
    {
        ret = ERROR_STATUS;
        stlCsv_observedSrc   = (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;
        stlCsv_activeFault   = entry->faultStruct;
        stlCsv_activeNvicMux = entry->nvicMux;

        (void)Cy_SysFault_Init(entry->faultStruct, &fault_cfg);
        Cy_SysFault_SetMaskByIdx(entry->faultStruct, CY_SYSFAULT_SRSS_CSV);
        Cy_SysFault_ClearStatus(entry->faultStruct);
        Cy_SysFault_ClearInterrupt(entry->faultStruct);
        Cy_SysFault_SetInterruptMask(entry->faultStruct);

        cy_stc_clkhf_csv_hf_config_t hfCfg  =
        {
            .clkHf  = entry->clkHf,
            .refClk = entry->refClk
        };

        /* Impossible window */
        cy_stc_clkhf_csv_manual_config_t badCfg =
        {
            .startTime  = 3u,
            .lowerLimit = 199u,
            .upperLimit = 201u,
            .period     = 1u
        };

        Cy_SysClk_ClkHfCsvDisable(entry->clkHf);
        Cy_SysClk_ClkHfCsvAction(entry->clkHf, CY_SYSCLK_CLKHF_CSV_ACTION_FAULT);
        cy_en_sysclk_status_t csvCfgStatus = Cy_SysClk_ClkHfCsvManualConfigure(&hfCfg, &badCfg);

        if (csvCfgStatus == CY_SYSCLK_SUCCESS)
        {
            #if (ERROR_IN_CSV == 0u)
            Cy_SysClk_ClkHfCsvEnable(entry->clkHf);
            #endif

            /* Wait for SelfTest_Clock_CSV_ISR to record a source, then verify it
             * is expected CSV fault that was armed before. */
            uint32_t elapsed = 0u;
            cy_en_SysFault_source_t observed = (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;
            while ((observed == (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT) &&
                   (elapsed < CSV_TEST_TIMEOUT_US))
            {
                observed = stlCsv_observedSrc;
                Cy_SysLib_DelayUs(1u);
                elapsed++;
            }

            if (observed == (cy_en_SysFault_source_t)CY_SYSFAULT_SRSS_CSV)
            {
                ret = OK_STATUS;
            }
        }
        else if (csvCfgStatus == CY_SYSCLK_BAD_PARAM)
        {
            ret = ERROR_BAD_PARAM;
        }
        else
        {
            /* Other error returned by Cy_SysClk_ClkHfCsvManualConfigure(). */
            ret = ERROR_STATUS;
        }

        /* Teardown. Disable CSV, then loop-clear STATUS until the fault-struct
         * reads no-fault. */
        Cy_SysClk_ClkHfCsvDisable(entry->clkHf);
        Cy_SysFault_ClearInterruptMask(entry->faultStruct);
        for (uint32_t i = 0u; i < 16u; i++)
        {
            Cy_SysFault_ClearStatus(entry->faultStruct);
            if (Cy_SysFault_GetErrorSource(entry->faultStruct) ==
                (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT)
            {
                break;
            }
        }
        Cy_SysFault_ClearInterrupt(entry->faultStruct);
        NVIC_ClearPendingIRQ(entry->nvicMux);

        stlCsv_activeFault   = NULL;
        stlCsv_activeNvicMux = (IRQn_Type)0;
    }

    return ret;
}


#endif /* (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL) */

/* [] END OF FILE */

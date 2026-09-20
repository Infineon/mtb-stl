/*******************************************************************************
* File Name: SelfTest_EPU.c
*
*
* Description:
*  This file provides the source code for the EPU (Event Processing Unit)
*  self tests.
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
#include "SelfTest_EPU.h"
#include "SelfTest_ErrorInjection.h"

/* EPU is a mandatory sub-block of the MXS40PPSS IP and cannot be disabled
 * by HW configuration; no finer-grained EPU feature macro is provided by
 * the device DSL. The same CY_IP_MXS40PPSS guard is used by the PDL EPU
 * driver (cy_ppca_epu.c), which guarantees that the PPCA_EPU_EPU instance
 * and all Cy_PPCA_EPU_* APIs used below are available whenever this guard
 * is true. */
#if defined(CY_IP_MXS40PPSS)

/* COMBO index used for self-test routing */
#define EPU_TEST_COMBO_INDEX            (0u)

/* Settling delay (microseconds) for the SW event to propagate through the
 * EPU datapath: PU (asynchronous bypass) -> COMBO (combinational) -> IRQ
 * source mux -> IRQ INTR latch (synchronous to the PPCA peripheral clock).
 * The intrinsic latency is a few PPCA clock cycles; 100 us is a safety
 * margin that covers any supported PPCA clock and the granularity of
 * Cy_SysLib_DelayUs(). */
#define EPU_TEST_TIMEOUT_US             (100u)

/* PU_T1 -> COMBO input mux split:
 *   - PUT1_OUT[0..15]  routed via bankSel  (cy_en_ppca_epu_bank_sel_t)
 *   - PUT1_OUT[16..31] routed via puSrcIn  (cy_en_ppca_epu_pu_src_in_t)
 * The two ranges must remain contiguous; the runtime check below in
 * TestSinglePuT1() verifies this. */
#define EPU_PU_T1_BANKSEL_MAX_INDEX     (15u)
#define EPU_PU_T1_PUSRC_BASE_INDEX      (16u)


/*******************************************************************************
* Function Name: TestSinglePuT1
*********************************************************************************
* Summary:
*  Tests a single PU_T1 unit by generating a SW event through the
*  PU (bypass mode) → COMBO → IRQ path and verifying interrupt status.
*
* Parameters:
*  index - PU_T1 index (0..31)
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestSinglePuT1(uint8_t index)
{
    /* The bankSel and puSrcIn index ranges must stay contiguous; otherwise
     * the routing branch below picks the wrong COMBO input field. */
    CY_ASSERT(EPU_PU_T1_PUSRC_BASE_INDEX == (EPU_PU_T1_BANKSEL_MAX_INDEX + 1u));

    uint8_t ret = ERROR_STATUS;

    /* Save original configurations */
    uint32_t savedPuCnfg = PPCA_EPU_EPU_PU_T1_CNFG(PPCA_EPU_EPU, (uint32_t)index);
    uint32_t savedComboCnfg = PPCA_EPU_EPU_COMBO_CNFG(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX);
    uint32_t savedIrqSrcsel = PPCA_EPU_EPU_IRQ_INTR_SRCSEL(PPCA_EPU_EPU_IRQ0);

    /* Configure PU_T1 in asynchronous bypass mode */
    Cy_PPCA_EPU_PU_T1_Enable(PPCA_EPU_EPU, (uint32_t)index, CY_ENABLE_ASYNC_BYPASS);

    /* Enable SW event generation and start with event = 0 */
    Cy_PPCA_EPU_PU_T1_Enable_SWEventGeneration(PPCA_EPU_EPU, (uint32_t)index, true);
    Cy_PPCA_EPU_PU_T1_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);

    /* Configure COMBO[0] to route this PU_T1 output. */
    cy_stc_ppca_epu_combo_config_t comboConfig =
    {
        .bankSel      = CY_COMBO_PUT1_OUT0_PUT1_OUT1,
        .put1InEnable = CY_COMBO_DISABLE_INPUT_SIGNAL_A_AND_B,
        .puSrcIn      = CY_COMBO_PUT1_OUT16_PUT2_OUT0,
        .srcInEnable  = CY_COMBO_DISABLE_INPUT_SIGNAL_A_AND_B,
        .inQual       = CY_COMBO_PUT1_OUT28_QUAL,
        .enableQual   = false,
        .enablePolInv = false,
    };

    if (index <= EPU_PU_T1_BANKSEL_MAX_INDEX)
    {
        /* PU_T1[0..15]: use PUT1_OUT[0..15] bank selection */
        uint32_t bankIndex       = (uint32_t)index / 2u;
        comboConfig.bankSel      = (cy_en_ppca_epu_bank_sel_t)bankIndex;
        comboConfig.put1InEnable = (((uint32_t)index % 2u) == 0u) ?
                                   CY_COMBO_ENABLE_INPUT_SIGNAL_A : CY_COMBO_ENABLE_INPUT_SIGNAL_B;
    }
    else
    {
        /* PU_T1[16..31]: use PUT1_OUT[16..31] via PU_SRC path */
        uint32_t srcIndex       = (uint32_t)index - EPU_PU_T1_PUSRC_BASE_INDEX;
        comboConfig.puSrcIn     = (cy_en_ppca_epu_pu_src_in_t)srcIndex;
        comboConfig.srcInEnable = CY_COMBO_ENABLE_INPUT_SIGNAL_A;
    }

    Cy_PPCA_EPU_Combo_Configure(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX, &comboConfig);

    /* Configure EPU IRQ[0] to use COMBO[0] output as interrupt source */
    Cy_PPCA_EPU_InterruptSourceSelect(PPCA_EPU_EPU_IRQ0, false, (uint8_t)EPU_TEST_COMBO_INDEX);

    /* Clear any pending interrupt */
    Cy_PPCA_EPU_ClearInterrupt(PPCA_EPU_EPU_IRQ0);

    /* Generate SW event (transition 0 -> 1) */
    #if ERROR_IN_EPU
    /* Error-injection: drive the SW event LOW so no rising edge reaches the
     * COMBO/IRQ datapath, causing the test to report ERROR_STATUS. */
    Cy_PPCA_EPU_PU_T1_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);
    #else
    Cy_PPCA_EPU_PU_T1_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, true);
    #endif

    /* Wait for interrupt to propagate through hardware */
    Cy_SysLib_DelayUs(EPU_TEST_TIMEOUT_US);

    /* Check interrupt status */
    uint32_t intrStatus = Cy_PPCA_EPU_GetInterruptStatus(PPCA_EPU_EPU_IRQ0);

    if (intrStatus != 0u)
    {
        ret = OK_STATUS;
    }

    /* Cleanup: clear SW event and interrupt */
    Cy_PPCA_EPU_PU_T1_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);
    Cy_PPCA_EPU_ClearInterrupt(PPCA_EPU_EPU_IRQ0);

    /* Restore original configurations */
    PPCA_EPU_EPU_PU_T1_CNFG(PPCA_EPU_EPU, (uint32_t)index) = savedPuCnfg;
    PPCA_EPU_EPU_COMBO_CNFG(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX) = savedComboCnfg;
    PPCA_EPU_EPU_IRQ_INTR_SRCSEL(PPCA_EPU_EPU_IRQ0) = savedIrqSrcsel;

    return ret;
}


/*******************************************************************************
* Function Name: TestSinglePuT2
*********************************************************************************
* Summary:
*  Tests a single PU_T2 unit by generating a SW event through the
*  PU (bypass mode) → COMBO → IRQ path and verifying interrupt status.
*
* Parameters:
*  index - PU_T2 index (0..15)
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestSinglePuT2(uint8_t index)
{
    uint8_t ret = ERROR_STATUS;

    /* Save original configurations */
    uint32_t savedPuCnfg = PPCA_EPU_EPU_PU_T2_CNFG(PPCA_EPU_EPU, (uint32_t)index);
    uint32_t savedComboCnfg = PPCA_EPU_EPU_COMBO_CNFG(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX);
    uint32_t savedIrqSrcsel = PPCA_EPU_EPU_IRQ_INTR_SRCSEL(PPCA_EPU_EPU_IRQ0);

    /* Configure PU_T2 in asynchronous bypass mode */
    Cy_PPCA_EPU_PU_T2_Enable(PPCA_EPU_EPU, (uint32_t)index, CY_ENABLE_ASYNC_BYPASS);

    /* Enable SW event generation and start with event = 0 */
    Cy_PPCA_EPU_PU_T2_Enable_SWEventGeneration(PPCA_EPU_EPU, (uint32_t)index, true);
    Cy_PPCA_EPU_PU_T2_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);

    /* Configure COMBO[0] to route this PU_T2 output. */
    cy_stc_ppca_epu_combo_config_t comboConfig =
    {
        .bankSel      = CY_COMBO_PUT1_OUT0_PUT1_OUT1,
        .put1InEnable = CY_COMBO_DISABLE_INPUT_SIGNAL_A_AND_B,
        .puSrcIn      = CY_COMBO_PUT1_OUT16_PUT2_OUT0,
        .srcInEnable  = CY_COMBO_DISABLE_INPUT_SIGNAL_A_AND_B,
        .inQual       = CY_COMBO_PUT1_OUT28_QUAL,
        .enableQual   = false,
        .enablePolInv = false,
    };

    /* PU_T2[0..15]: use PUT2_OUT[0..15] via PU_SRC path (SIGNAL_B) */
    comboConfig.puSrcIn     = (cy_en_ppca_epu_pu_src_in_t)((uint32_t)index);
    comboConfig.srcInEnable = CY_COMBO_ENABLE_INPUT_SIGNAL_B;

    Cy_PPCA_EPU_Combo_Configure(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX, &comboConfig);

    /* Configure EPU IRQ[0] to use COMBO[0] output as interrupt source */
    Cy_PPCA_EPU_InterruptSourceSelect(PPCA_EPU_EPU_IRQ0, false, (uint8_t)EPU_TEST_COMBO_INDEX);

    /* Clear any pending interrupt */
    Cy_PPCA_EPU_ClearInterrupt(PPCA_EPU_EPU_IRQ0);

    /* Generate SW event (transition 0 -> 1) */
    #if ERROR_IN_EPU
    /* Error-injection: drive the SW event LOW so no rising edge reaches the
     * COMBO/IRQ datapath, causing the test to report ERROR_STATUS. */
    Cy_PPCA_EPU_PU_T2_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);
    #else
    Cy_PPCA_EPU_PU_T2_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, true);
    #endif

    /* Wait for interrupt to propagate through hardware */
    Cy_SysLib_DelayUs(EPU_TEST_TIMEOUT_US);

    /* Check interrupt status */
    uint32_t intrStatus = Cy_PPCA_EPU_GetInterruptStatus(PPCA_EPU_EPU_IRQ0);

    if (intrStatus != 0u)
    {
        ret = OK_STATUS;
    }

    /* Cleanup: clear SW event and interrupt */
    Cy_PPCA_EPU_PU_T2_Generate_SW_Event(PPCA_EPU_EPU, (uint32_t)index, false);
    Cy_PPCA_EPU_ClearInterrupt(PPCA_EPU_EPU_IRQ0);

    /* Restore original configurations */
    PPCA_EPU_EPU_PU_T2_CNFG(PPCA_EPU_EPU, (uint32_t)index) = savedPuCnfg;
    PPCA_EPU_EPU_COMBO_CNFG(PPCA_EPU_EPU, EPU_TEST_COMBO_INDEX) = savedComboCnfg;
    PPCA_EPU_EPU_IRQ_INTR_SRCSEL(PPCA_EPU_EPU_IRQ0) = savedIrqSrcsel;

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_EPU_PU
****************************************************************************//**
*
* Performs a self-test of the EPU processing unit signal path by generating
* a software event on each PU in the specified range and verifying that
* the resulting trigger arrives as an interrupt via the COMBO and IRQ path.
*
* \param type
* PU sub-type to test: EPU_PU_TYPE_T1 (0) or EPU_PU_TYPE_T2 (1).
*
* \param puInitial
* First PU index to test (inclusive).
*
* \param puEnd
* Last PU index to test (inclusive).
*
* \return
*  \ref OK_STATUS                    (0) - Test passed <br>
*  \ref ERROR_STATUS                 (1) - HW self-test failed (the SW event did
*                                     not propagate through the PU -> COMBO
*                                     -> IRQ datapath for at least one PU in
*                                     the requested range) <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS (4) - Invalid input parameters (unknown
*                                     \p type, \p puEnd out of range for
*                                     the selected sub-type, or
*                                     \p puInitial > \p puEnd). No HW
*                                     access is performed in this case.
*
*******************************************************************************/
uint8_t SelfTest_EPU_PU(uint8_t type, uint8_t puInitial, uint8_t puEnd)
{
    uint8_t ret = ERROR_INCORRECT_USAGE_STATUS;
    bool paramsOk = false;

    /* Cache the auto-derived per-type maximum index in a local before the
     * range checks below. */
    const uint8_t maxT1 = EPU_PU_T1_MAX_INDEX;
    const uint8_t maxT2 = EPU_PU_T2_MAX_INDEX;

    /* Parameter validation: type must be a known PU sub-type, the requested
     * range must lie within the supported PU instance count, and puInitial
     * must not exceed puEnd. */
    if ((type == EPU_PU_TYPE_T1) && (puEnd <= maxT1) && (puInitial <= puEnd))
    {
        paramsOk = true;
    }
    else if ((type == EPU_PU_TYPE_T2) && (puEnd <= maxT2) && (puInitial <= puEnd))
    {
        paramsOk = true;
    }
    else
    {
        /* Invalid parameters: no HW access is performed. */
    }

    if (paramsOk)
    {
        /* Precondition: the application is responsible for enabling the PPCA_EPU
         * IP block (Cy_PPCA_EPU_Enable) before invoking this self-test. The
         * library deliberately does not toggle PPCA_EPU.CTRL to avoid affecting
         * application-level power/clock policy. */

        /* Test each PU in the specified range. Start from OK_STATUS and let
         * any failing sub-test latch ERROR_STATUS through the early break. */
        ret = OK_STATUS;
        for (uint8_t i = puInitial; i <= puEnd; i++)
        {
            ret = (type == EPU_PU_TYPE_T1) ? TestSinglePuT1(i)
                                           : TestSinglePuT2(i);

            if (ret != OK_STATUS)
            {
                break;
            }
        }
    }

    return ret;
}


#endif /* defined(CY_IP_MXS40PPSS) */

/* [] END OF FILE */

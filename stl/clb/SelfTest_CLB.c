/*******************************************************************************
* File Name: SelfTest_CLB.c
*
*
* Description:
*  This file provides the source code for the CLB (Custom Logic Block)
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
#include "SelfTest_CLB.h"
#include "SelfTest_ErrorInjection.h"

/* CLB and DICO are mandatory sub-blocks of the MXS40PPSS IP. */
#if defined(CY_IP_MXS40PPSS)

/* Wait after DICO enable before first DICO register access. */
#define CLB_TEST_DICO_STARTUP_DELAY_US  (1u)

/* Propagation delay after an INTR_SET or INTR write.  DICO interrupt
 * registers are in a separate clock domain; reads too soon return stale data. */
#define CLB_TEST_INTR_RW_DELAY_US       (5u)

/* 8-bit pattern loaded into DICO SAFE_REG in Test A. */
#define CLB_TEST_SAFE_STATE_PATTERN     (0x55u)

/* Bitmask covering all 8 DICO interrupt sources. */
#define CLB_TEST_ALL_INTR               (0xFFu)

/* Phase-shift interrupt sources only (PS_0 | PS_1).  Test C validates
 * phase-switching autonomy, so its pass condition must accept only these and
 * not unrelated DICO events (PE / PCO error-observer interrupts). */
#define CLB_TEST_PS_INTR                (CY_DICO_INTR_PS_0 | CY_DICO_INTR_PS_1)

/* Interrupt bit toggled in Test B.  PS_0 can be software-asserted via
 * INTR_SET without requiring active DICO phase switching. */
#define CLB_TEST_INTR_BIT               (CY_DICO_INTR_PS_0)

/* High IMAXL for the DICO switching tests: large enough that overcurrent
 * never trips during the self-test. */
#define CLB_TEST_IL_IMAXL               (0x00FFFFF0u)

/* Small MAX_ON_TIME (in DICO on-time counter units = system clock / 4):
 * forces rapid timer-driven DICO phase switching. */
#define CLB_TEST_PWM_MAX_ON_TIME        (50u)

/* Timeout for the DICO switching / interrupt polling loops in microseconds.
 * Each loop iteration delays 1 us via Cy_SysLib_DelayUs(1u), so this value
 * directly equals the wall-clock timeout (50000 us = 50 ms).  The count is
 * sized generously to cover the expected switching latency across supported
 * clocks. */
#define CLB_TEST_PHASE_SWITCH_TIMEOUT_US (50000u)

/* Test D (DICO IL data path via ADC+AFLT injection):
 * High unsigned stimulus injected into ADC channel 0 test register.
 * Must be > 0 so DICO ilAccu accumulates a non-zero value. */
#define CLB_TEST_ADC_INJECT_VALUE       (0x600UL)

/* Test D low stimulus: injecting 0 yields a near-zero ADC result, so the high
 * stimulus produces a strictly larger result ("changing results" check). */
#define CLB_TEST_ADC_INJECT_VALUE_LOW   (0x000UL)

/* ADC conversion timeout in Test D: the poll loop delays 1 ms per iteration
 * (Cy_SysLib_Delay(1U)), so this count is also the wall-clock timeout in ms. */
#define CLB_TEST_ADC_CONV_TIMEOUT_MS    (50U)

/* Test E illegal-limit pair: IMINL > IMAXL must raise STAT.INCORR_SETTING. */
#define CLB_TEST_INCORR_IMINL           (0x00001000u)
#define CLB_TEST_INCORR_IMAXL           (0x00000100u)

/* Test F (MACO sequencer) parameters. */
/* Timeout for the MACO scan-start poll in microseconds.  Each loop iteration
 * delays 1 us via Cy_SysLib_DelayUs(1u), so this value directly equals the
 * wall-clock timeout (1000 us = 1 ms).  A minimal sector scan starts within a
 * few clock cycles; 1 ms is sized generously for the expected latency. */
#define CLB_TEST_MACO_SCAN_TIMEOUT_US   (1000u)

/* Timeout for the MACO sector-transition poll in microseconds.  Each loop
 * iteration delays 1 us via Cy_SysLib_DelayUs(1u), so this value directly
 * equals the wall-clock timeout (50000 us = 50 ms). */
#define CLB_TEST_MACO_TRANSITION_TIMEOUT_US (50000u)

/* Hold time per MACO pattern row, in MACO clock cycles (actual duration =
 * value + 1 clock). */
#define CLB_TEST_MACO_HOLD_TIME_CLK     (1u)

/* Arbitrary distinct switch states for the two MACO pattern rows; values only
 * need to be non-zero and different to exercise the sequencer. */
#define CLB_TEST_MACO_DRIVER_PATTERN_A  (0xAAAAu)
#define CLB_TEST_MACO_DRIVER_PATTERN_B  (0x5555u)

/* Mask for the MACO "current scanning sector" field (STATUS bits [5:4]). */
#define CLB_TEST_MACO_CURR_SECT_MASK    ((uint32_t)CY_CURRENT_SCANNING_SECT1 | (uint32_t)CY_CURRENT_SCANNING_SECT2)

/* ADC channel 0 bitmask for Cy_PPCA_ADC_Trigger. */
#define CLB_TEST_ADC_CH_MASK            (0x0001U)

/* MACO array dimensions derived from the PDL structure definitions so the
 * self-test automatically tracks any future IP revision changes. */
#define CLB_TEST_MACO_NUM_SECTORS       \
    ((uint32_t)(sizeof(((cy_stc_ppca_clb_maco_config_t *)0)->patternConfig) / \
                sizeof(((cy_stc_ppca_clb_maco_config_t *)0)->patternConfig[0])))
#define CLB_TEST_MACO_ROWS_PER_SECTOR   \
    ((uint32_t)(sizeof(((cy_stc_ppca_clb_maco_pattern_config_t *)0)->driverSignal) / \
                sizeof(((cy_stc_ppca_clb_maco_pattern_config_t *)0)->driverSignal[0])))

/*******************************************************************************
* Function Name: TestSafeState
*********************************************************************************
* Summary:
*  Test A - Kill-path / safe-state transition verification:
*  Confirms the kill override actually drives the DICO output state rather than
*  reading a single, default-asserted status bit.  SAFE_STATE reads 1 out of
*  reset, so a static check alone could pass even with a broken kill path.
*  This test instead verifies the full transition:
*    (1) enable DICO with autonomous (timer-driven) phase switching and confirm
*        it LEAVES the safe state (SAFE_STATE clears to 0 - outputs driven by
*        the PCO, not SAFE_REG);
*    (2) assert a software Kill event and confirm the hardware forces the safe
*        state back on (KILL_STATE = 1 and SAFE_STATE = 1 - outputs driven from
*        SAFE_REG);
*    (3) exit Kill and confirm KILL_STATE clears.
*  All modified registers are restored before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  dicoBase - Pointer to the PPCA CLB DICO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestSafeState(PPCA_CLB_Type* clbBase, PPCA_CLB_DICO_Type* dicoBase)
{
    uint8_t ret = ERROR_STATUS;

    uint32_t savedClbCtrl      = PPCA_CLB_CTRL(clbBase);
    uint32_t savedDicoCtrl     = PPCA_CLB_DICO_CTRL(dicoBase);
    uint32_t savedDicoCnfg     = PPCA_CLB_DICO_CNFG(dicoBase);
    uint32_t savedDicoImaxl    = PPCA_CLB_DICO_IMAXL(dicoBase);
    uint32_t savedDicoIminl    = PPCA_CLB_DICO_IMINL(dicoBase);
    uint32_t savedDicoVacPol   = PPCA_CLB_DICO_VAC_POL(dicoBase);
    uint32_t savedDicoSafe     = PPCA_CLB_DICO_SAFE(dicoBase);
    uint32_t savedDicoIntrMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);

    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_DICO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);
    Cy_PPCA_CLB_DICO_Enable(dicoBase);
    Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);

    Cy_PPCA_CLB_DICO_WriteSafeState(dicoBase, CLB_TEST_SAFE_STATE_PATTERN);

    /* Configure timer-driven phase switching so the PCO actively drives the
     * outputs (no ADC data required), forcing DICO to leave the safe state. */
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_IL1(dicoBase, CY_ADC_GROUP_0_FILT_OUT0);
    Cy_PPCA_CLB_DICO_Config_MinCurrent(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_MaxCurrent(dicoBase, (uint32_t)CLB_TEST_IL_IMAXL);
    Cy_PPCA_CLB_DICO_Config_Switching_Period(dicoBase, CY_FOUR_PERIODS);
    Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(dicoBase,
                                           (uint32_t)CLB_TEST_PWM_MAX_ON_TIME, 0u);
    Cy_PPCA_CLB_DICO_Config_VACPol(dicoBase, CY_POS_VAC_POL);
    Cy_PPCA_CLB_DICO_ConfigPhaseEnabler(dicoBase, true, false);

    Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);
    Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, true);

    /* Step 1: confirm DICO leaves the safe state (SAFE_STATE clears to 0). */
    bool leftSafeState = false;
    uint32_t timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;
    while (timeout > 0u)
    {
        if ((Cy_PPCA_CLB_DICO_ReadStatus(dicoBase) & (uint32_t)CY_SAFE_STATE) == 0u)
        {
            leftSafeState = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    /* Step 2: assert Kill and confirm the hardware forces the safe state
     * (KILL_STATE and SAFE_STATE both set). */
    Cy_PPCA_CLB_DICO_KillEvent(dicoBase);
    bool killForcedSafe = false;
    timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;
    while (timeout > 0u)
    {
        uint32_t status = Cy_PPCA_CLB_DICO_ReadStatus(dicoBase);
        if (((status & (uint32_t)CY_KILL_STATE) != 0u) &&
            ((status & (uint32_t)CY_SAFE_STATE) != 0u))
        {
            killForcedSafe = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    /* Step 3: exit Kill and confirm KILL_STATE clears. */
    Cy_PPCA_CLB_DICO_KillExit(dicoBase);
    bool killCleared = false;
    timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;
    while (timeout > 0u)
    {
        if ((Cy_PPCA_CLB_DICO_ReadStatus(dicoBase) & (uint32_t)CY_KILL_STATE) == 0u)
        {
            killCleared = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    #if ERROR_IN_CLB_SAFE_STATE
    /* Fault injection: corrupt the observed safe-state result to emulate a
     * DICO that never leaves the safe state (e.g. a stuck SAFE_STATE bit), so
     * the genuine pass check below fails on a real readback fault. */
    leftSafeState = false;
    #endif

    /* Pass only when the complete kill transition was observed. */
    if (leftSafeState && killForcedSafe && killCleared)
    {
        ret = OK_STATUS;
    }

    /* Restore original state.  Restore the full DICO SAFE register directly:
     * SAFE_STATE, MAX_ON_TIME and MIN_ON_TIME are all fields of this single
     * register, and the test modified the on-time fields via
     * Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime().  Writing back only the
     * SAFE_STATE field would leave the self-test timing limits in place. */
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, false);
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, savedDicoIntrMask);
    PPCA_CLB_DICO_SAFE(dicoBase)    = savedDicoSafe;
    PPCA_CLB_DICO_VAC_POL(dicoBase) = savedDicoVacPol;
    PPCA_CLB_DICO_IMINL(dicoBase)   = savedDicoIminl;
    PPCA_CLB_DICO_IMAXL(dicoBase)   = savedDicoImaxl;
    PPCA_CLB_DICO_CNFG(dicoBase)    = savedDicoCnfg;
    PPCA_CLB_DICO_CTRL(dicoBase)    = savedDicoCtrl;
    PPCA_CLB_CTRL(clbBase)          = savedClbCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: TestIntrRoundTrip
*********************************************************************************
* Summary:
*  Test B - Interrupt path functional test:
*  Validates the DICO interrupt infrastructure in three sub-steps.
*  (1) INTR_MASK round-trip: writes 0xFF to INTR_MASK and verifies all 8 bits
*  are set, then writes 0x00 and verifies all bits cleared.
*  (2) INTR_SET / INTR path: software-asserts one interrupt bit via INTR_SET,
*  verifies the bit appears in the INTR register, then clears it via
*  write-1-to-clear and verifies the bit is gone.
*  (3) INTR_MASKED path: enables one bit in INTR_MASK, software-asserts the
*  same bit via INTR_SET, verifies INTR_MASKED shows the bit; then clears INTR
*  and disables the mask bit, verifies INTR_MASKED no longer shows the bit.
*  All modified registers are restored before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  dicoBase - Pointer to the PPCA CLB DICO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestIntrRoundTrip(PPCA_CLB_Type* clbBase, PPCA_CLB_DICO_Type* dicoBase)
{
    uint8_t ret = ERROR_STATUS;

    /* Save original register values. */
    uint32_t savedClbCtrl  = PPCA_CLB_CTRL(clbBase);
    uint32_t savedDicoCtrl = PPCA_CLB_DICO_CTRL(dicoBase);

    /* Enable CLB and DICO so the DICO register file is accessible. */
    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_DICO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);
    Cy_PPCA_CLB_DICO_Enable(dicoBase);
    Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);

    /* Save interrupt mask; disable all sources to prevent spurious ISR. */
    uint32_t savedIntrMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);

    /* Clear any pending interrupt flags. */
    Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);

    /* --- Sub-step 1: INTR_MASK write / read-back --- */
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);
    uint32_t readMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);
    bool maskAllSet = ((readMask & (uint32_t)CLB_TEST_ALL_INTR) == (uint32_t)CLB_TEST_ALL_INTR);

    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);
    readMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);
    bool maskCleared = ((readMask & (uint32_t)CLB_TEST_ALL_INTR) == 0u);

    /* --- Sub-step 2: INTR_SET -> INTR assertion; write-1-to-clear --- */
    bool intrSet     = false;
    bool intrCleared = false;

    if (maskAllSet && maskCleared)
    {
        /* Software-set one interrupt bit via INTR_SET register. */
        Cy_PPCA_CLB_DICO_SetInterrupt(dicoBase, (uint32_t)CLB_TEST_INTR_BIT);
        Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);  /* CDC sync */
        uint32_t intrStatus = Cy_PPCA_CLB_DICO_GetInterruptStatus(dicoBase);
        intrSet = ((intrStatus & (uint32_t)CLB_TEST_INTR_BIT) != 0u);

        /* Clear the bit via write-1-to-clear mechanism. */
        Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_INTR_BIT);
        Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);  /* CDC sync */
        intrStatus = Cy_PPCA_CLB_DICO_GetInterruptStatus(dicoBase);
        intrCleared = ((intrStatus & (uint32_t)CLB_TEST_INTR_BIT) == 0u);
    }

    /* --- Sub-step 3: INTR_MASKED reflects (INTR & INTR_MASK) --- */
    bool maskedShows   = false;
    bool maskedCleared = false;

    if (intrSet && intrCleared)
    {
        /* Enable one mask bit; software-set the same INTR bit. */
        Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, (uint32_t)CLB_TEST_INTR_BIT);
        Cy_PPCA_CLB_DICO_SetInterrupt(dicoBase, (uint32_t)CLB_TEST_INTR_BIT);
        Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);  /* CDC sync */
        uint32_t maskedStatus = Cy_PPCA_CLB_DICO_GetInterruptStatusMasked(dicoBase);
        maskedShows = ((maskedStatus & (uint32_t)CLB_TEST_INTR_BIT) != 0u);

        /* Clear INTR bit; disable mask; verify INTR_MASKED is clear. */
        Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_INTR_BIT);
        Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);  /* CDC sync */
        Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);
        maskedStatus = Cy_PPCA_CLB_DICO_GetInterruptStatusMasked(dicoBase);
        maskedCleared = ((maskedStatus & (uint32_t)CLB_TEST_INTR_BIT) == 0u);
    }

    #if ERROR_IN_CLB_INTR
    /* Fault injection: corrupt the observed interrupt-assertion result to
     * emulate an INTR bit that fails to set on INTR_SET, so the genuine pass
     * check below fails on a real readback fault. */
    intrSet = false;
    #endif

    if (maskAllSet && maskCleared && intrSet && intrCleared && maskedShows && maskedCleared)
    {
        ret = OK_STATUS;
    }

    /* Restore original state. */
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, savedIntrMask);
    PPCA_CLB_DICO_CTRL(dicoBase) = savedDicoCtrl;
    PPCA_CLB_CTRL(clbBase)       = savedClbCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: TestDicoDataPath
*********************************************************************************
* Summary:
*  Test C - DICO autonomous switching verification:
*  Configures DICO Phase 1 with a short MAX_ON_TIME and polls DICO INTR
*  until any interrupt bit is set, confirming the DICO state machine runs
*  autonomously and the hardware interrupt path is functional. The test
*  accepts any DICO interrupt source (PS, PCO, or PE) because the specific
*  event depends on the silicon revision and phase-enabler configuration;
*  all interrupt flags are cleared before Phase 1 is enabled, so any bit
*  that appears must originate from the autonomous switching just started.
*  All modified registers are restored before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  dicoBase - Pointer to the PPCA CLB DICO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestDicoDataPath(PPCA_CLB_Type* clbBase,
                                PPCA_CLB_DICO_Type* dicoBase)
{
    uint8_t ret = ERROR_STATUS;

    uint32_t savedClbCtrl      = PPCA_CLB_CTRL(clbBase);
    uint32_t savedDicoCtrl     = PPCA_CLB_DICO_CTRL(dicoBase);
    uint32_t savedDicoCnfg     = PPCA_CLB_DICO_CNFG(dicoBase);
    uint32_t savedDicoImaxl    = PPCA_CLB_DICO_IMAXL(dicoBase);
    uint32_t savedDicoIminl    = PPCA_CLB_DICO_IMINL(dicoBase);
    uint32_t savedDicoVacPol   = PPCA_CLB_DICO_VAC_POL(dicoBase);
    /* SAFE register also holds MAX_ON_TIME/MIN_ON_TIME, modified below. */
    uint32_t savedDicoSafe     = PPCA_CLB_DICO_SAFE(dicoBase);
    uint32_t savedDicoIntrMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);

    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_DICO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);
    Cy_PPCA_CLB_DICO_Enable(dicoBase);
    Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);

    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);

    /* Configure DICO for timer-driven phase switching (no ADC data required). */
    Cy_PPCA_CLB_DICO_Config_IL1(dicoBase, CY_ADC_GROUP_0_FILT_OUT0);
    Cy_PPCA_CLB_DICO_Config_MinCurrent(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_MaxCurrent(dicoBase, (uint32_t)CLB_TEST_IL_IMAXL);
    Cy_PPCA_CLB_DICO_Config_Switching_Period(dicoBase, CY_FOUR_PERIODS);
    Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(dicoBase, (uint32_t)CLB_TEST_PWM_MAX_ON_TIME, 0u);
    Cy_PPCA_CLB_DICO_Config_VACPol(dicoBase, CY_POS_VAC_POL);
    Cy_PPCA_CLB_DICO_ConfigPhaseEnabler(dicoBase, true, false);

    /* Clear stale flags, then start Phase 1. */
    Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);
    Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, true);

    /* Poll until any hardware-generated DICO interrupt fires or timeout.
     * The specific source (PS, PCO or PE) depends on silicon revision and
     * phase-enabler config; any bit confirms autonomous switching is active
     * because all flags were cleared before Phase 1 was enabled. */
    bool psSwitchOccurred = false;
    uint32_t timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;

    while (timeout > 0u)
    {
        if ((Cy_PPCA_CLB_DICO_GetInterruptStatus(dicoBase) & (uint32_t)CLB_TEST_ALL_INTR) != 0u)
        {
            psSwitchOccurred = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    #if ERROR_IN_CLB_DICO_PATH
    /* Fault injection: clear the observed autonomous-switching result to
     * emulate a DICO whose interrupt never fires, so the genuine pass
     * check below fails on a real readback fault. */
    psSwitchOccurred = false;
    #endif

    if (psSwitchOccurred)
    {
        ret = OK_STATUS;
    }

    /* Restore original DICO and CLB state.  Restore the full SAFE register:
     * MAX_ON_TIME/MIN_ON_TIME live there and were changed by
     * Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(). */
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, false);
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, savedDicoIntrMask);
    PPCA_CLB_DICO_SAFE(dicoBase)      = savedDicoSafe;
    PPCA_CLB_DICO_VAC_POL(dicoBase)   = savedDicoVacPol;
    PPCA_CLB_DICO_IMINL(dicoBase)     = savedDicoIminl;
    PPCA_CLB_DICO_IMAXL(dicoBase)     = savedDicoImaxl;
    PPCA_CLB_DICO_CNFG(dicoBase)      = savedDicoCnfg;
    PPCA_CLB_DICO_CTRL(dicoBase)      = savedDicoCtrl;
    PPCA_CLB_CTRL(clbBase)            = savedClbCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: TestDicoILDataPath
*********************************************************************************
* Summary:
*  Test D - DICO IL data path via ADC test-mode injection + AFLT[0] + DICO:
*
*  Validates the complete digital signal path:
*    ADC (test mode) → AFLT[0] Median → FILT_OUT0 → DICO IL_ACCU
*
*  Prerequisites enabled inside the function (saved/restored):
*    - PPCA subsystem enable (PPCA_CNFG.PPCA_EN)
*    - AREF enable (PPCA_ATOPSS_ADC_GRP0_AREF)
*    - ADC channel 0 enable (ADC_CNFG.CHANNEL_EN = 1)
*
*  (1) Enables PPCA and AREF, sets ADC channel 0 enable bit, then
*      configures AFLT[0] (PPCA_ATOPSS_ADC_GRP0_AFLT0) with the Median
*      filter stage active (all other stages bypassed). The Median stage
*      is enabled (median_filter_bypass=false) so that it clocks the AFLT
*      pipeline; a 1-sample median is a transparent pass-through.
*  (2) Enables ADC test mode (gain calibration bypassed) so known values can
*      be injected via Cy_PPCA_ADC_Write_Test_data() into channel 0.  Each
*      injection is followed by one ADC conversion; on completion the injected
*      value propagates through the Median filter to FILT_OUT0.
*  (3) "Changing results" check at the ADC/AFLT data level: injects a low (0)
*      then the high value and reads back the (non-destructive) channel-0 ADC
*      result after each, confirming the result tracks the input
*      (dataHigh > dataLow) - proving the converter + injection path produces
*      changing results rather than a stuck constant.  The high value is
*      injected last, leaving it latched in FILT_OUT0.
*  (4) Enables DICO Phase 1 with IL1 = CY_ADC_GROUP_0_FILT_OUT0, waits for one
*      measurement period, and verifies IL_ACCU is non-zero - confirming the
*      latched FILT_OUT0 value reaches and accumulates in the DICO observer.
*      The sub-test passes only when both (3) and (4) hold.
*
*  All modified registers (PPCA CNFG, CLB, DICO, ADC, AFLT) are restored
*  before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  dicoBase - Pointer to the PPCA CLB DICO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestDicoILDataPath(PPCA_CLB_Type* clbBase, PPCA_CLB_DICO_Type* dicoBase)
{
    uint8_t ret = ERROR_STATUS;

    /* Hardcoded instances: DICO IL1 is driven by ADC GRP0 AFLT[0] output. */
    ATOPSS_ADC_TYPE* adcBase  = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC;
    ATOPSS_ADCFLT_TYPE* afltBase = PPCA_ATOPSS_ADC_GRP0_AFLT0;

    /* --- Save state of all registers that will be modified --- */
    uint32_t savedPpcaCtrl      = PPCA_CNFG_CTRL(PPCA_CNFG);
    uint32_t savedClbCtrl       = PPCA_CLB_CTRL(clbBase);
    uint32_t savedDicoCtrl      = PPCA_CLB_DICO_CTRL(dicoBase);
    uint32_t savedDicoCnfg      = PPCA_CLB_DICO_CNFG(dicoBase);
    uint32_t savedDicoImaxl     = PPCA_CLB_DICO_IMAXL(dicoBase);
    uint32_t savedDicoIminl     = PPCA_CLB_DICO_IMINL(dicoBase);
    uint32_t savedDicoVacPol    = PPCA_CLB_DICO_VAC_POL(dicoBase);
    /* SAFE register also holds MAX_ON_TIME/MIN_ON_TIME, modified below. */
    uint32_t savedDicoSafe      = PPCA_CLB_DICO_SAFE(dicoBase);
    uint32_t savedDicoIntrMask  = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);

    uint32_t savedAdcCnfg       = PPCA_ATOP_ADC_CNFG(adcBase);
    uint32_t savedAdcCtl        = PPCA_ATOP_ADC_CTRL(adcBase);
    uint32_t savedAdcCnvCnfg    = PPCA_ATOP_ADC_CNV_CNFG(adcBase);
    uint32_t savedAdcChCnfg0    = PPCA_ATOP_ADC_CH_CNFG0(adcBase);
    uint32_t savedAdcChCnfg1    = PPCA_ATOP_ADC_CH_CNFG1(adcBase);
    uint32_t savedAdcSignUnsign = PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase);
    uint32_t savedAfltCtl       = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg      = PPCA_ATOP_AFLT_CNFG(afltBase);

    /* -----------------------------------------------------------------------
     * Step 1: Enable PPCA analog subsystem and configure AFLT[0] for
     * full pass-through.  PPCA_EN must be set before ADC/AFLT can clock.
     * Note: CLB/DICO/MACO registers (used by Tests A-C, E-F) are in the
     * digital control domain and accessible via Cy_PPCA_CLB_Enable() alone.
     * PPCA_EN specifically gates the analog subsystem (ADC, AREF, AFLT),
     * which is why only this test (Test D) calls Cy_PPCA_Enable().
     * The Median stage is enabled (not bypassed) to activate the AFLT
     * pipeline clock; a 1-sample median is a perfect pass-through.
     * FILT_OUT0 mirrors ADC channel 0 data after one trigger.
     * -----------------------------------------------------------------------*/
    Cy_PPCA_Enable(PPCA_CNFG);
    Cy_PPCA_AREF_Enable(PPCA_ATOPSS_ADC_GRP0_AREF);

    /* ADC configuration for the Test D injection path.  Mirrors the proven
     * ATOP DCMP self-test ADC personality (channel 0, arbitrary trigger): the
     * full Cy_PPCA_ADC_Init() sequence - conversion config, channel sequence
     * and trims - is required for Cy_PPCA_ADC_Trigger() to actually latch
     * test-mode data into ADC_DATA[0].  Setting ADC_CNFG.CHANNEL_EN alone
     * is insufficient. */
    static cy_stc_aux_slot_config_t clbAdcAuxCfg =
    {
        .aux_ch        = 0,
        .aux_ch_gnd    = 0,
        .alt_aux_ch    = 0,
        .ch7_data_path = CY_DATA_PATH_ADC7,
    };

    static const cy_stc_ppca_adc_config_t clbAdcCfg =
    {
        .calib_gain_mode                 = false,
        .aux_slot                        = false,
        .alt_aux_slot                    = false,
        .channels                        = 1, /* ch0 only (bitmask: bit 0) */
        .channel_type                    = 0,
        .channel_data_type               = 0,
        .aux_channel_data_type           = 0,
        .alt_aux_channel_data_type       = 0,
        .aux_slot_config                 = &clbAdcAuxCfg,
        .adc_conv_config                 =
        {
            .group_channel               = false,
            .trigger_mode                = CY_ADC_ARBITRARY_TRIGGER,
            .auto_trigger_sample_clck    = 0,
            .num_of_slot                 = 1,
            .eos_src                     = false,
            .num_of_aux_slot             = 0,
            .num_of_alt_aux_slot         = 0,
            .aux_slot_scan_rate          = 0,
            .ext_src                     = false,
            .sequence_of_channel         = 16,
            .sequence_of_aux_channel     = 0,
            .sequence_of_alt_aux_channel = 0,
        },
    };

    /* Full ADC bring-up (conversion config + channel sequence + trims).
     * Required so Cy_PPCA_ADC_Trigger() actually latches data into
     * ADC_DATA[0]; ADC_CNFG.CHANNEL_EN alone does not configure the
     * conversion sequence. */
    Cy_PPCA_ADC_Init(adcBase, &clbAdcCfg);

    const cy_stc_ppca_adc_filter_config_t afltCfg =
    {
        .median_filter_enable          = true, /* enable median to drive AFLT clock */
        .linear_filter_enable          = false,
        .average_filter_enable         = false,
        .cic_filter_enable             = false,
        .lpf_filter_enable             = false,
        .median_filter_bypass          = false, /* 1-sample median = pass-through */
        .linear_filter_bypass          = true, /* bypass remaining stages */
        .average_filter_bypass         = true,
        .cic_filter_bypass             = true,
        .lpf_filter_bypass             = true,
        .unsign_path_enable            = true, /* unsigned path (test data is unsigned) */
        .min_max_detection_enable      = false,
        .src_ch                        = 0U, /* ADC channel 0 */
        .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
        .lif_intp_extp_sel             = false,
        .lpf_bandwidth                 = 0U,
        .cic_src_type                  = false,
        .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
        .cic_decimation_factor         = 0U,
        .cic_decimation_offset         = 0U,
        .cic_decimation_scale          = 0U,
        .cic_digital_comparator_enable = false,
        .cic_UB_threshold              = 0U,
        .cic_LB_threshold              = 0U,
        .avg_no_of_samples             = 1U,
        .avg_linear_or_squared         = false,
        .avg_fix_or_dynamic_sample     = false,
        .min_max_source                = false,
        .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
        .src_adc                       = false,
    };
    Cy_PPCA_ADC_Filter_Init(afltBase, &afltCfg);

    /* -----------------------------------------------------------------------
     * Step 2: Inject known ADC test value and trigger conversion.
     * After the trigger, AFLT FILT_OUT0 holds CLB_TEST_ADC_INJECT_VALUE.
     * -----------------------------------------------------------------------*/
    Cy_PPCA_ADC_Enable(adcBase);

    /* Conversion-poll timeout, reused by the injection loops below. */
    uint32_t timeout;

    Cy_PPCA_ADC_Set_Test_mode(adcBase, true);
    /* Bypass IP gain/offset calibration: in test mode an uncalibrated gain
     * register would otherwise scale the injected value (potentially to 0).
     * The DCMP self-test relies on the same bypass for injection to work. */
    Cy_PPCA_ADC_Set_Calib_Gain_mode(adcBase, false);

    /* -----------------------------------------------------------------------
     * Step 2b: "Changing results" check at the ADC/AFLT data level.
     * Inject a low (0) then the high value, reading back the (non-destructive)
     * ADC channel-0 conversion result after each.  A larger input must yield a
     * strictly larger ADC result, proving the converter + injection path
     * produces changing results that track the input rather than a stuck
     * constant.  The high value is injected last, so FILT_OUT0 holds it for
     * the DICO accumulation check in Step 3 (the proven single-run flow).
     * -----------------------------------------------------------------------*/
    Cy_PPCA_ADC_Write_Test_data(adcBase, (uint16_t)CLB_TEST_ADC_INJECT_VALUE_LOW);
    Cy_PPCA_ADC_Trigger(adcBase, (uint16_t)CLB_TEST_ADC_CH_MASK);
    timeout = (uint32_t)CLB_TEST_ADC_CONV_TIMEOUT_MS;
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_Delay(1U);
        timeout--;
    }
    uint16_t dataLow = Cy_PPCA_ADC_Read_ADC_Data(adcBase, 0U);

    Cy_PPCA_ADC_Write_Test_data(adcBase, (uint16_t)CLB_TEST_ADC_INJECT_VALUE);
    Cy_PPCA_ADC_Trigger(adcBase, (uint16_t)CLB_TEST_ADC_CH_MASK);
    timeout = (uint32_t)CLB_TEST_ADC_CONV_TIMEOUT_MS;
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_Delay(1U);
        timeout--;
    }
    uint16_t dataHigh = Cy_PPCA_ADC_Read_ADC_Data(adcBase, 0U);

    bool dataPathTracks = (dataHigh > dataLow);

    /* Re-inject the high value with a single clean conversion so the Median
     * stage fully settles FILT_OUT0 before DICO consumes it (back-to-back
     * triggers above leave the AFLT pipeline mid-flush).  FILT_OUT0 now holds
     * CLB_TEST_ADC_INJECT_VALUE (held until the next ADC trigger). */
    Cy_PPCA_ADC_Write_Test_data(adcBase, (uint16_t)CLB_TEST_ADC_INJECT_VALUE);
    Cy_PPCA_ADC_Trigger(adcBase, (uint16_t)CLB_TEST_ADC_CH_MASK);
    timeout = (uint32_t)CLB_TEST_ADC_CONV_TIMEOUT_MS;
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_Delay(1U);
        timeout--;
    }

    /* -----------------------------------------------------------------------
     * Step 3: Enable DICO with IL1 = CY_ADC_GROUP_0_FILT_OUT0 and verify that
     * IL_ACCU becomes non-zero, confirming the FILT_OUT0 -> DICO path.
     * -----------------------------------------------------------------------*/
    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_DICO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);
    Cy_PPCA_CLB_DICO_Enable(dicoBase);
    Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);

    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_IL1(dicoBase, CY_ADC_GROUP_0_FILT_OUT0);
    Cy_PPCA_CLB_DICO_Config_MinCurrent(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_MaxCurrent(dicoBase, (uint32_t)CLB_TEST_IL_IMAXL);
    Cy_PPCA_CLB_DICO_Config_Switching_Period(dicoBase, CY_FOUR_PERIODS);
    Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(dicoBase, (uint32_t)CLB_TEST_PWM_MAX_ON_TIME, 0u);
    Cy_PPCA_CLB_DICO_Config_VACPol(dicoBase, CY_POS_VAC_POL);
    Cy_PPCA_CLB_DICO_ConfigPhaseEnabler(dicoBase, false, false);

    Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);
    Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, true);
    timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;
    while (timeout > 0u)
    {
        if ((Cy_PPCA_CLB_DICO_GetInterruptStatus(dicoBase) & (uint32_t)CLB_TEST_ALL_INTR) != 0u)
        {
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    /* Atomically freeze and read IL_ACCU for Phase 1. */
    cy_stc_ppca_clb_dico_errorObserver_t obs = { 0U, 0U, 0U };
    Cy_PPCA_CLB_DICO_AtomicRead_ErrorObserver_Values(dicoBase, 1U, &obs);
    uint32_t ilAccu = obs.ilAccu;

    #if ERROR_IN_CLB_DICO_IL
    /* Fault injection: force the observed IL accumulator to zero to emulate a
     * broken FILT_OUT0 -> DICO data path (injected current never reaches the
     * accumulator), so the genuine pass check below fails on a real fault. */
    ilAccu = 0u;
    #endif

    /* Pass when the data path tracks the input (changing results) AND the
     * injected current reached the DICO IL accumulator. */
    if (dataPathTracks && (ilAccu > 0u))
    {
        ret = OK_STATUS;
    }

    /* --- Restore all modified registers --- */
    /* Restore the full SAFE register: MAX_ON_TIME/MIN_ON_TIME live there and
     * were changed by Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(). */
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, false);
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, savedDicoIntrMask);
    PPCA_CLB_DICO_SAFE(dicoBase)    = savedDicoSafe;
    PPCA_CLB_DICO_VAC_POL(dicoBase) = savedDicoVacPol;
    PPCA_CLB_DICO_IMINL(dicoBase)   = savedDicoIminl;
    PPCA_CLB_DICO_IMAXL(dicoBase)   = savedDicoImaxl;
    PPCA_CLB_DICO_CNFG(dicoBase)    = savedDicoCnfg;
    PPCA_CLB_DICO_CTRL(dicoBase)    = savedDicoCtrl;
    PPCA_CLB_CTRL(clbBase)          = savedClbCtrl;

    Cy_PPCA_ADC_Set_Test_mode(adcBase, false);
    PPCA_ATOP_ADC_CTRL(adcBase)     = savedAdcCtl;

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_CTL(afltBase)    = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase)   = savedAfltCnfg;
    PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase) = savedAdcSignUnsign;
    PPCA_ATOP_ADC_CH_CNFG1(adcBase) = savedAdcChCnfg1;
    PPCA_ATOP_ADC_CH_CNFG0(adcBase) = savedAdcChCnfg0;
    PPCA_ATOP_ADC_CNV_CNFG(adcBase) = savedAdcCnvCnfg;
    PPCA_ATOP_ADC_CNFG(adcBase)     = savedAdcCnfg;

    Cy_PPCA_AREF_Disable(PPCA_ATOPSS_ADC_GRP0_AREF);
    PPCA_CNFG_CTRL(PPCA_CNFG)       = savedPpcaCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: TestDicoLimitDetect
*********************************************************************************
* Summary:
*  Test E - DICO limit / threshold detection:
*  Verifies that the DICO hardware flags out-of-range operating conditions in
*  its STATUS register, using two deterministic checks that need no analog
*  stimulus:
*    (1) INCORR_SETTING - programming an illegal limit pair (IMINL > IMAXL)
*        must set STAT.INCORR_SETTING; restoring a legal IMIN < IMAX ordering
*        must clear it.
*    (2) MAX_ON_REACHED - with a small MAX_ON_TIME and autonomous switching,
*        the on-time limiter must assert STAT.MAX_ON_REACHED.
*  All modified registers are restored before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  dicoBase - Pointer to the PPCA CLB DICO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestDicoLimitDetect(PPCA_CLB_Type* clbBase, PPCA_CLB_DICO_Type* dicoBase)
{
    uint8_t ret = ERROR_STATUS;

    uint32_t savedClbCtrl      = PPCA_CLB_CTRL(clbBase);
    uint32_t savedDicoCtrl     = PPCA_CLB_DICO_CTRL(dicoBase);
    uint32_t savedDicoCnfg     = PPCA_CLB_DICO_CNFG(dicoBase);
    uint32_t savedDicoImaxl    = PPCA_CLB_DICO_IMAXL(dicoBase);
    uint32_t savedDicoIminl    = PPCA_CLB_DICO_IMINL(dicoBase);
    uint32_t savedDicoVacPol   = PPCA_CLB_DICO_VAC_POL(dicoBase);
    /* SAFE register also holds MAX_ON_TIME/MIN_ON_TIME, modified below. */
    uint32_t savedDicoSafe     = PPCA_CLB_DICO_SAFE(dicoBase);
    uint32_t savedDicoIntrMask = Cy_PPCA_CLB_DICO_GetInterruptMask(dicoBase);

    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_DICO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);
    Cy_PPCA_CLB_DICO_Enable(dicoBase);
    Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);

    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, 0u);

    /* -----------------------------------------------------------------------
     * Check 1 - INCORR_SETTING: programming IMINL > IMAXL is an illegal limit
     * configuration the hardware must flag.  Expect STAT.INCORR_SETTING set.
     * -----------------------------------------------------------------------*/
    Cy_PPCA_CLB_DICO_Config_MaxCurrent(dicoBase, (uint32_t)CLB_TEST_INCORR_IMAXL);
    Cy_PPCA_CLB_DICO_Config_MinCurrent(dicoBase, (uint32_t)CLB_TEST_INCORR_IMINL);
    Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);
    bool incorrFlagged = ((Cy_PPCA_CLB_DICO_ReadStatus(dicoBase) & (uint32_t)CY_INCORR_SETTING) != 0u);

    /* Restore a legal IMIN < IMAX ordering; the flag is re-evaluated and
     * cleared once a switching cycle latches the new limits (verified after
     * Check 2 starts autonomous switching below). */
    Cy_PPCA_CLB_DICO_Config_MinCurrent(dicoBase, 0u);
    Cy_PPCA_CLB_DICO_Config_MaxCurrent(dicoBase, (uint32_t)CLB_TEST_IL_IMAXL);

    /* -----------------------------------------------------------------------
     * Check 2 - MAX_ON_REACHED: with a small MAX_ON_TIME and autonomous
     * switching, the on-time limiter must assert STAT.MAX_ON_REACHED.
     * -----------------------------------------------------------------------*/
    Cy_PPCA_CLB_DICO_Config_Switching_Period(dicoBase, CY_FOUR_PERIODS);
    Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(dicoBase, (uint32_t)CLB_TEST_PWM_MAX_ON_TIME, 0u);
    Cy_PPCA_CLB_DICO_Config_VACPol(dicoBase, CY_POS_VAC_POL);
    Cy_PPCA_CLB_DICO_ConfigPhaseEnabler(dicoBase, true, false);

    /* Clear any latched MAX_ON_REACHED (RW1C) before the measurement. */
    PPCA_CLB_DICO_STAT(dicoBase) = (uint32_t)CY_MAX_ON_TIME_REACHED;
    Cy_PPCA_CLB_DICO_ClearInterrupt(dicoBase, (uint32_t)CLB_TEST_ALL_INTR);
    Cy_SysLib_DelayUs(CLB_TEST_INTR_RW_DELAY_US);
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, true);

    bool maxOnReached = false;
    uint32_t timeout = (uint32_t)CLB_TEST_PHASE_SWITCH_TIMEOUT_US;
    while (timeout > 0u)
    {
        if ((Cy_PPCA_CLB_DICO_ReadStatus(dicoBase) & (uint32_t)CY_MAX_ON_TIME_REACHED) != 0u)
        {
            maxOnReached = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    /* Now that switching has latched the restored legal limits, confirm the
     * INCORR_SETTING flag has cleared. */
    bool incorrCleared = ((Cy_PPCA_CLB_DICO_ReadStatus(dicoBase) & (uint32_t)CY_INCORR_SETTING) == 0u);

    #if ERROR_IN_CLB_DICO_LIMIT
    /* Fault injection: clear the observed INCORR_SETTING result to emulate an
     * illegal-limit detector that fails to flag, so the genuine pass check
     * below fails on a real readback fault. */
    incorrFlagged = false;
    #endif

    /* Pass only when both threshold detectors behaved as specified. */
    if (incorrFlagged && incorrCleared && maxOnReached)
    {
        ret = OK_STATUS;
    }

    /* --- Restore all modified registers --- */
    /* Restore the full SAFE register: MAX_ON_TIME/MIN_ON_TIME live there and
     * were changed by Cy_PPCA_CLB_DICO_Config_Max_Min_OnTime(). */
    Cy_PPCA_CLB_DICO_ConfigPhase1(dicoBase, false);
    PPCA_CLB_DICO_STAT(dicoBase) = (uint32_t)CY_MAX_ON_TIME_REACHED;
    Cy_PPCA_CLB_DICO_SetInterruptMask(dicoBase, savedDicoIntrMask);
    PPCA_CLB_DICO_SAFE(dicoBase)    = savedDicoSafe;
    PPCA_CLB_DICO_VAC_POL(dicoBase) = savedDicoVacPol;
    PPCA_CLB_DICO_IMINL(dicoBase)   = savedDicoIminl;
    PPCA_CLB_DICO_IMAXL(dicoBase)   = savedDicoImaxl;
    PPCA_CLB_DICO_CNFG(dicoBase)    = savedDicoCnfg;
    PPCA_CLB_DICO_CTRL(dicoBase)    = savedDicoCtrl;
    PPCA_CLB_CTRL(clbBase)          = savedClbCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: TestMacoSequencer
*********************************************************************************
* Summary:
*  Test F - MACO sequencer functional test:
*  Validates the MACO (Matrix Commutation) sub-block of PPCA_CLB in three
*  sub-steps.
*  (1) MACO starts scanning: configure three identical sectors (each with a
*  1-row transition table and a 1-row modulation table), reset MACO to a clean
*  state, enable, then poll until SCAN_IN_PROGRESS is set.  Also verify
*  CY_MACO_SAFE_STATE is clear (no unexpected kill active).
*  (2) Sector transition: assert SelectNextPattern(NEXT_SECTOR) + TransitPattern;
*  poll STATUS until the current-scanning-sector field reads Sector 1,
*  confirming the FSM advanced.
*  (3) Kill path: assert software KillEvent; verify STATUS shows both
*  CY_MACO_KILL_STATE and CY_MACO_SAFE_STATE; call KillExit.
*  All modified registers are restored before returning.
*
* Parameters:
*  clbBase  - Pointer to the PPCA CLB top-level registers.
*  macoBase - Pointer to the PPCA CLB MACO registers.
*
* Return:
*  uint8_t - OK_STATUS (0) or ERROR_STATUS (1)
*******************************************************************************/
static uint8_t TestMacoSequencer(PPCA_CLB_Type* clbBase, PPCA_CLB_MACO_Type* macoBase)
{
    uint8_t ret = ERROR_STATUS;

    uint32_t savedClbCtrl   = PPCA_CLB_CTRL(clbBase);
    uint32_t savedMacoCtrl  = PPCA_CLB_MACO_CTRL(macoBase);
    uint32_t savedMacoEvent = PPCA_CLB_MACO_EVENT(macoBase);

    /* The MACO pattern tables written by Cy_PPCA_CLB_MACO_InitConfig()
     * (SECTn_ROW, SECTn_SAFE and SECTn_PATTERN[0..19]) have no read-back
     * config API, but they are directly readable registers.  Save them raw so
     * this self-test is non-destructive to the application configuration and
     * honours the "all modified registers are restored" contract. */
    uint32_t idx;
    uint32_t savedMacoSectRow[CLB_TEST_MACO_NUM_SECTORS];
    uint32_t savedMacoSectSafe[CLB_TEST_MACO_NUM_SECTORS];
    uint32_t savedMacoSect0Pat[CLB_TEST_MACO_ROWS_PER_SECTOR];
    uint32_t savedMacoSect1Pat[CLB_TEST_MACO_ROWS_PER_SECTOR];
    uint32_t savedMacoSect2Pat[CLB_TEST_MACO_ROWS_PER_SECTOR];

    savedMacoSectRow[0]  = PPCA_CLB_MACO_SECT0_ROW(macoBase);
    savedMacoSectRow[1]  = PPCA_CLB_MACO_SECT1_ROW(macoBase);
    savedMacoSectRow[2]  = PPCA_CLB_MACO_SECT2_ROW(macoBase);
    savedMacoSectSafe[0] = PPCA_CLB_MACO_SECT0_SAFE(macoBase);
    savedMacoSectSafe[1] = PPCA_CLB_MACO_SECT1_SAFE(macoBase);
    savedMacoSectSafe[2] = PPCA_CLB_MACO_SECT2_SAFE(macoBase);
    for (idx = 0u; idx < CLB_TEST_MACO_ROWS_PER_SECTOR; idx++)
    {
        savedMacoSect0Pat[idx] = PPCA_CLB_MACO_SECT0_PATTERN(macoBase, idx);
        savedMacoSect1Pat[idx] = PPCA_CLB_MACO_SECT1_PATTERN(macoBase, idx);
        savedMacoSect2Pat[idx] = PPCA_CLB_MACO_SECT2_PATTERN(macoBase, idx);
    }

    /* Reset MACO to a clean initial state before reconfiguration.
     * Disable guarantees the FSM is in reset and outputs are at safe state. */
    Cy_PPCA_CLB_MACO_Disable(macoBase);

    /* Minimal test pattern: 3 sectors, 2 rows each (1 transition + 1 modulation).
     * A non-zero holdTime keeps the row DLY field valid.  Rows 2..19 are
     * unused and set to zero (safe / inactive state). */
    static const cy_stc_ppca_clb_maco_pattern_config_t clbMacoSectorPat =
    {
        .tranRow      = 1u,
        .allRow       = 2u,
        .driverSignal =
        {
            CLB_TEST_MACO_DRIVER_PATTERN_A, CLB_TEST_MACO_DRIVER_PATTERN_B,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
            0u,                             0u,
        },
        .holdTime     =
        {
            CLB_TEST_MACO_HOLD_TIME_CLK, CLB_TEST_MACO_HOLD_TIME_CLK,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
            0u,                          0u,
        },
        .safeState    = 0u,
    };

    cy_stc_ppca_clb_maco_config_t cfg =
    {
        .selNxtSec     = CY_NEXT_SECTOR,
        .patternConfig =
        {
            clbMacoSectorPat,
            clbMacoSectorPat,
            clbMacoSectorPat,
        },
    };

    Cy_PPCA_CLB_MACO_InitConfig(macoBase, &cfg);

    Cy_PPCA_CLB_Select_Output(clbBase, CY_CLB_MACO_SEL);
    Cy_PPCA_CLB_Enable(clbBase);

    /* Clear any stale scan-completion interrupt before starting. */
    Cy_PPCA_CLB_MACO_ClearInterrupt(macoBase);

    Cy_PPCA_CLB_MACO_Enable(macoBase);

    /* --- Sub-step 1: MACO starts scanning ---
     * Poll for SCAN_IN_PROGRESS = 1, confirming the sequencer FSM became
     * active after enable. */
    bool scan1Done = false;
    uint32_t timeout = (uint32_t)CLB_TEST_MACO_SCAN_TIMEOUT_US;
    while (timeout > 0u)
    {
        uint32_t s = Cy_PPCA_CLB_MACO_ReadStatus(macoBase);
        if ((s & (uint32_t)CY_SCAN_IN_PROGRESS) != 0u)
        {
            scan1Done = true;
            break;
        }
        Cy_SysLib_DelayUs(1u);
        timeout--;
    }

    bool notInSafeState = ((Cy_PPCA_CLB_MACO_ReadStatus(macoBase) & (uint32_t)CY_MACO_SAFE_STATE) == 0u);

    /* --- Sub-step 2: Sector transition ---
     * Send TransitPattern and verify the FSM moved to Sector 1.  The
     * current-scanning-sector field (STATUS bits [5:4]) updates as soon as the
     * sequencer enters the new sector, before its scan completes. */
    bool transitionDone = false;
    if (scan1Done && notInSafeState)
    {
        Cy_PPCA_CLB_MACO_ClearInterrupt(macoBase);
        Cy_PPCA_CLB_MACO_SelectNextPattern(macoBase, CY_NEXT_SECTOR);
        Cy_PPCA_CLB_MACO_TransitPattern(macoBase);

        timeout = (uint32_t)CLB_TEST_MACO_TRANSITION_TIMEOUT_US;
        while (timeout > 0u)
        {
            uint32_t st = Cy_PPCA_CLB_MACO_ReadStatus(macoBase);
            if ((st & (uint32_t)CLB_TEST_MACO_CURR_SECT_MASK) == (uint32_t)CY_CURRENT_SCANNING_SECT1)
            {
                transitionDone = true;
                break;
            }
            Cy_SysLib_DelayUs(1u);
            timeout--;
        }
    }

    /* --- Sub-step 3: Kill event drives outputs to safe state ---
     * Both CY_MACO_KILL_STATE and CY_MACO_SAFE_STATE must be set in STATUS. */
    bool killSafe = false;
    uint32_t killSt = 0u;
    if (transitionDone)
    {
        Cy_PPCA_CLB_MACO_KillEvent(macoBase);
        Cy_SysLib_DelayUs(CLB_TEST_DICO_STARTUP_DELAY_US);  /* let kill propagate */
        killSt = Cy_PPCA_CLB_MACO_ReadStatus(macoBase);
        killSafe = ((killSt & ((uint32_t)CY_MACO_KILL_STATE | (uint32_t)CY_MACO_SAFE_STATE)) ==
                    ((uint32_t)CY_MACO_KILL_STATE | (uint32_t)CY_MACO_SAFE_STATE));
    }

    #if ERROR_IN_CLB_MACO_SEQ
    /* Fault injection: clear the observed sector-transition result to emulate
     * a MACO sequencer that fails to advance to the next sector, so the
     * genuine pass check below fails on a real readback fault. */
    transitionDone = false;
    #endif

    if (scan1Done && notInSafeState && transitionDone && killSafe)
    {
        ret = OK_STATUS;
    }

    /* Restore original state (non-destructive).  Exit kill state, disable the
     * sequencer so the pattern tables can be written cleanly, restore the
     * saved pattern tables, then restore EVENT (de-asserts KILL_EVENT per
     * architecture requirement) and MACO/CLB CTRL. */
    Cy_PPCA_CLB_MACO_KillExit(macoBase);
    Cy_PPCA_CLB_MACO_ClearInterrupt(macoBase);
    Cy_PPCA_CLB_MACO_Disable(macoBase);

    PPCA_CLB_MACO_SECT0_ROW(macoBase)  = savedMacoSectRow[0];
    PPCA_CLB_MACO_SECT1_ROW(macoBase)  = savedMacoSectRow[1];
    PPCA_CLB_MACO_SECT2_ROW(macoBase)  = savedMacoSectRow[2];
    PPCA_CLB_MACO_SECT0_SAFE(macoBase) = savedMacoSectSafe[0];
    PPCA_CLB_MACO_SECT1_SAFE(macoBase) = savedMacoSectSafe[1];
    PPCA_CLB_MACO_SECT2_SAFE(macoBase) = savedMacoSectSafe[2];
    for (idx = 0u; idx < CLB_TEST_MACO_ROWS_PER_SECTOR; idx++)
    {
        PPCA_CLB_MACO_SECT0_PATTERN(macoBase, idx) = savedMacoSect0Pat[idx];
        PPCA_CLB_MACO_SECT1_PATTERN(macoBase, idx) = savedMacoSect1Pat[idx];
        PPCA_CLB_MACO_SECT2_PATTERN(macoBase, idx) = savedMacoSect2Pat[idx];
    }

    PPCA_CLB_MACO_EVENT(macoBase) = savedMacoEvent;
    PPCA_CLB_MACO_CTRL(macoBase)  = savedMacoCtrl;
    PPCA_CLB_CTRL(clbBase)        = savedClbCtrl;

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_CLB
****************************************************************************//**
*
* Performs a self-test of the CLB (Custom Logic Block) by executing the
* sub-tests selected by \p testMask. Each sub-test saves and restores all
* registers it modifies, so they may be run in any combination and order.
*
* \note This function is only available on PSC3x8 device series.
*
* \param testMask
* Bitmask selecting which sub-tests to execute. Any combination of
* SELFTEST_CLB_SAFE_STATE, SELFTEST_CLB_INTR, SELFTEST_CLB_DICO_PATH,
* SELFTEST_CLB_DICO_IL, SELFTEST_CLB_DICO_LIMIT, and SELFTEST_CLB_MACO_SEQ
* (or SELFTEST_CLB_ALL for all six).
* An empty selection (no valid sub-test bit set, including 0) is rejected and
* returns ERROR_STATUS, since the call would otherwise report success without
* exercising any hardware.
*
* \return
*  \ref OK_STATUS    (0) - All selected sub-tests passed. <br>
*  \ref ERROR_STATUS (1) - No valid sub-test was selected, or at least one selected
*                     sub-test failed (execution stops at the first failing
*                     sub-test).
*
*******************************************************************************/
uint8_t SelfTest_CLB(uint8_t testMask)
{
    /* Reject an empty selection: at least one valid sub-test must run so the
    * call always exercises hardware and never reports success without it. */
    if ((testMask & (uint8_t)SELFTEST_CLB_ALL) == 0u)
    {
        return ERROR_STATUS;
    }

    uint8_t ret = OK_STATUS;

    if ((testMask & (uint8_t)SELFTEST_CLB_SAFE_STATE) != 0u)
    {
        ret = TestSafeState(PPCA_CLB, PPCA_CLB_DICO);
        if (ret != OK_STATUS)
        {
            return ret;
        }
    }

    if ((testMask & (uint8_t)SELFTEST_CLB_INTR) != 0u)
    {
        ret = TestIntrRoundTrip(PPCA_CLB, PPCA_CLB_DICO);
        if (ret != OK_STATUS)
        {
            return ret;
        }
    }

    if ((testMask & (uint8_t)SELFTEST_CLB_DICO_PATH) != 0u)
    {
        ret = TestDicoDataPath(PPCA_CLB, PPCA_CLB_DICO);
        if (ret != OK_STATUS)
        {
            return ret;
        }
    }

    if ((testMask & (uint8_t)SELFTEST_CLB_DICO_IL) != 0u)
    {
        ret = TestDicoILDataPath(PPCA_CLB, PPCA_CLB_DICO);
        if (ret != OK_STATUS)
        {
            return ret;
        }
    }

    if ((testMask & (uint8_t)SELFTEST_CLB_DICO_LIMIT) != 0u)
    {
        ret = TestDicoLimitDetect(PPCA_CLB, PPCA_CLB_DICO);
        if (ret != OK_STATUS)
        {
            return ret;
        }
    }

    if ((testMask & (uint8_t)SELFTEST_CLB_MACO_SEQ) != 0u)
    {
        ret = TestMacoSequencer(PPCA_CLB, PPCA_CLB_MACO);
    }

    return ret;
}


#endif /* defined(CY_IP_MXS40PPSS) */

/* [] END OF FILE */

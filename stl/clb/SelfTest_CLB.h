/*******************************************************************************
* File Name: SelfTest_CLB.h
*
* Description:
*  This file provides the constants and parameter values for the CLB
*  (Custom Logic Block) self tests.
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
/**
 * \defgroup group_clb CLB (CLB STL module)
 * \{
 *
 * The CLB (Custom Logic Block) self-test validates the DICO (Digital Current
 * Observer) sub-block and the MACO (Matrix Commutation) sub-block of the
 * PPCA subsystem. Six independent sub-tests are provided and can be selected
 * via a bitmask:
 *
 * \section group_clb_test_a Test A - DICO kill-path / safe-state transition
 *
 * Confirms the kill override actually drives the DICO output state rather than
 * reading the default-asserted SAFE_STATE bit (which reads 1 out of reset).
 * The test procedure:
 *
 *      1) Save all CLB and DICO registers that are modified.
 *      2) Enable CLB (select DICO output) and DICO; load a known SAFE_REG
 *         pattern; start timer-driven phase switching so the PCO actively
 *         drives the outputs (no ADC data required).
 *      3) Confirm DICO leaves the safe state (SAFE_STATE clears to 0).
 *      4) Assert a software Kill event; confirm the hardware forces the safe
 *         state back on (KILL_STATE and SAFE_STATE both set).
 *      5) Exit Kill; confirm KILL_STATE clears.
 *      6) Restore all saved registers.
 *
 * \section group_clb_test_b Test B - DICO interrupt path functional test
 *
 * The test procedure:
 *
 *      1) Enable CLB and DICO; disable all DICO interrupt sources (INTR_MASK = 0)
 *         to prevent spurious ISR delivery during the test.
 *      2) Clear any pending interrupt flags in INTR.
 *      3) INTR_MASK round-trip: write 0xFF to INTR_MASK and verify all 8 bits are
 *         set; write 0x00 and verify all 8 bits cleared.
 *      4) INTR_SET path: software-assert one interrupt bit via the INTR_SET
 *         register; read INTR and verify the bit is set; write the same bit back to
 *         INTR (write-1-to-clear); read INTR and verify the bit is cleared.
 *      5) INTR_MASKED path: enable one bit in INTR_MASK; software-assert the same
 *         bit via INTR_SET; read INTR_MASKED and verify the bit is present (= INTR
 *         AND INTR_MASK); clear the INTR bit and disable the mask bit; read
 *         INTR_MASKED and verify the bit is absent.
 *      6) Restore the original DICO control and interrupt-mask registers; restore
 *         CLB control.
 *
 * \section group_clb_test_c Test C - DICO autonomous switching verification
 *
 * The test procedure:
 *
 *      1) Save original CLB and DICO register values.
 *      2) Enable CLB and DICO; disable all DICO interrupt sources.
 *      3) Configure DICO Phase 1: IMINL = 0, IMAXL = large, numPeriod = 4,
 *         small pwmMaxOnTime (forces rapid timer-driven switching).
 *      4) Clear any pending DICO interrupt flags.
 *      5) Enable Phase 1 - DICO state machine begins autonomous switching.
 *      6) Poll DICO INTR until any interrupt bit is set (hardware-generated,
 *         timer-driven) or a timeout expires.  Any DICO interrupt confirms
 *         the control logic and interrupt path are functional; the specific
 *         source (PS, PCO, or PE) depends on silicon revision and
 *         phase-enabler configuration and is not material to the test.
 *      7) Restore all saved registers.
 *
 * \note ADC data is not required.  DICO switches autonomously on the internal
 *       pwmMaxOnTime timer.  No external EPU or power converter is needed.
 *
 *
 * \section group_clb_test_d Test D - DICO IL data path (ADC test-mode + AFLT[0] + DICO)
 *
 * Validates the signal chain:
 *   ADC (test mode) -> AFLT[0] Median -> FILT_OUT0 -> DICO IL_ACCU
 *
 * The test procedure:
 *
 *      1) Enable PPCA and AREF; bring up ADC channel 0 with a full
 *         Cy_PPCA_ADC_Init() and configure AFLT[0] (PPCA_ATOPSS_ADC_GRP0_AFLT0)
 *         with the Median stage active (a 1-sample median is a transparent
 *         pass-through that drives the AFLT pipeline clock so FILT_OUT0 tracks
 *         ADC channel 0).  Save all modified registers.
 *      2) Enable ADC test mode with calibration gain bypassed so known values
 *         can be injected via Cy_PPCA_ADC_Write_Test_data().
 *      3) Changing-results check: inject a low (0) then a high value, reading
 *         back the channel-0 ADC result after each, and confirm the result
 *         tracks the input (high > low) - proving the path produces changing
 *         results rather than a stuck constant.  The high value is injected
 *         last and remains latched in FILT_OUT0.
 *      4) Enable DICO Phase 1 with IL1 = CY_ADC_GROUP_0_FILT_OUT0 and confirm
 *         IL_ACCU becomes non-zero, proving the latched FILT_OUT0 value reaches
 *         and accumulates in the DICO observer.  The sub-test passes only when
 *         both (3) and (4) hold.
 *      5) Restore all saved registers.
 *
 * \note No external EPU or power converter is needed.
 *
 *
 * \section group_clb_test_e Test E - DICO limit / threshold detection
 *
 * The test procedure:
 *
 *      1) INCORR_SETTING: program an illegal limit pair (IMINL > IMAXL) and
 *         confirm STAT.INCORR_SETTING is set; restore a legal IMIN < IMAX
 *         ordering and confirm it clears once a switching cycle latches the
 *         new limits.
 *      2) MAX_ON_REACHED: with a small MAX_ON_TIME and autonomous switching,
 *         confirm the on-time limiter asserts STAT.MAX_ON_REACHED.
 *      3) Restore all saved registers.
 *
 * \note No analog stimulus or external hardware is needed.
 *
 *
 * \section group_clb_test_f Test F - MACO sequencer functional test
 *
 * The test procedure:
 *
 *      1) Save original CLB and MACO control register values; reset MACO to a
 *         clean state and initialize three identical minimal sectors in
 *         manual-transition mode.
 *      2) Enable CLB (select MACO output) and MACO; confirm the sequencer
 *         starts scanning (CY_SCAN_IN_PROGRESS) and is not in safe state.
 *      3) Issue SelectNextPattern(NEXT_SECTOR) + TransitPattern; confirm the
 *         FSM advances to Sector 1 (current-scanning field = Sector 1).
 *      4) Assert software KillEvent; verify STATUS shows both CY_MACO_KILL_STATE
 *         and CY_MACO_SAFE_STATE; call KillExit to restore normal operation.
 *      5) Restore original CLB and MACO control register values.
 *
 * \note No external hardware is required.  MACO is configured with minimal
 *       sector patterns that complete within a few clock cycles.
 *
 *
 * \defgroup group_clb_macros Macros
 * \defgroup group_clb_functions Functions
 */

#if !defined(SELFTEST_CLB_H)
    #define SELFTEST_CLB_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN))

/***************************************
* Macros
***************************************/

/**
 * \addtogroup group_clb_macros
 * \{
 */

/**
 * \defgroup group_clb_test_mask Test selection bitmask values
 * Pass one or more of these ORed together as the \p testMask argument of
 * \ref SelfTest_CLB.
 * \{
 */

/** Run Test A: DICO kill-path / safe-state transition. */
#define SELFTEST_CLB_SAFE_STATE     (0x01U)

/** Run Test B: DICO software interrupt set / clear round-trip. */
#define SELFTEST_CLB_INTR           (0x02U)

/** Run Test C: DICO phase-switching autonomy (timer-driven PS interrupt). */
#define SELFTEST_CLB_DICO_PATH      (0x04U)

/** Run Test D: DICO IL data path via ADC test-mode injection + AFLT[0] + DICO IL_ACCU. */
#define SELFTEST_CLB_DICO_IL        (0x08U)

/** Run Test E: DICO limit/threshold detection (INCORR_SETTING + MAX_ON_REACHED). */
#define SELFTEST_CLB_DICO_LIMIT     (0x10U)

/** Run Test F: MACO sequencer functional test (start, sector transition, kill path). */
#define SELFTEST_CLB_MACO_SEQ       (0x20U)

/** Run all six sub-tests (A + B + C + D + E + F). */
#define SELFTEST_CLB_ALL            (0x3FU)

/** \} group_clb_test_mask */

/** \} group_clb_macros */


/***************************************
* Function Prototypes
***************************************/

/**
 * \addtogroup group_clb_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_CLB
****************************************************************************//**
*
* Performs a self-test of the CLB (Custom Logic Block) DICO and MACO
* sub-blocks by executing the sub-tests selected by \p testMask. Each sub-test
* saves and restores all registers it modifies, so they may be run in any
* combination and in any order.
*
* \note This function is only available on PSC3x8 device series.
*
* \param testMask
* Bitmask selecting which sub-tests to execute. Any combination of
* \ref SELFTEST_CLB_SAFE_STATE, \ref SELFTEST_CLB_INTR,
* \ref SELFTEST_CLB_DICO_PATH, \ref SELFTEST_CLB_DICO_IL,
* \ref SELFTEST_CLB_DICO_LIMIT, and \ref SELFTEST_CLB_MACO_SEQ
* (or \ref SELFTEST_CLB_ALL for all six).
* An empty selection (no valid sub-test bit set, including 0) is rejected and
* returns ERROR_STATUS, since the call would otherwise report success without
* exercising any hardware.
*
* \return
*  \ref OK_STATUS    (0) - All selected sub-tests passed. <br>
*  \ref ERROR_STATUS (1) - No valid sub-test was selected, or at least one selected
*                     sub-test failed. Execution stops at the first failing
*                     sub-test. <br>
*
*******************************************************************************/
uint8_t SelfTest_CLB(uint8_t testMask);

/** \} group_clb_functions */

#endif /* (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)) */

#endif /* SELFTEST_CLB_H */

/** \} group_clb */
/* [] END OF FILE */

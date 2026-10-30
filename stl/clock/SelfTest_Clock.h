/*******************************************************************************
* File Name: SelfTest_Clock.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the clock self tests according to Class B library.
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
 * \defgroup group_clock Clock (Clock STL module)
 * \{
 *
 * The clock test implements independent time-slot monitoring and verifies the reliability
 * of the internal main oscillator (IMO) system clock, specifically, that the system clock
 * is neither too fast nor too slow within the tolerance of the internal low-speed oscillator (ILO).
 *
 *
 * \section group_clock_more_information More Information
 *
 * The ILO clock is accurate to ± 60 percent. If accuracy greater than 60 percent is required, the
 * ILO may be trimmed to be more accurate using a precision system level signal or production test.
 * If ILO trimming is required, it is trimmed using the CLK_ILO_TRIM register. <br>
 * If the WCO is available, it should be used for this test, since it is much more accurate. If the
 * other oscillators (ECO, EXT_CLK, IMO) are safety critical, customers can use this test as a
 * guideline.
 *
 *
 * \defgroup group_clock_macros Macros
 * \defgroup group_clock_functions Functions
 */

#if !defined(SELFTEST_CLOCK_H)
    #define SELFTEST_CLOCK_H
#include "cybsp.h"
#include "SelfTest_common.h"

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_clock_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_Clock
****************************************************************************//**
*
* Performs Clock test and verifies if the measured clock frequency is in the accuracy range.
*
*
* \param base
* The pointer to a TCPWM instance <br>
* \param cntNum
* The Counter instance number in the selected TCPWM
*
* \note
* Use a TCPWM counter dedicated to the clock self-test. The test programs the
* selected counter period/counter value and depends on \ref SelfTest_Clock_ISR_TIMER
* being installed for the counter terminal-count interrupt.
*
* \return
*  \ref ERROR_STATUS (1) - Test failed <br>
*  \ref PASS_STILL_TESTING_STATUS (2) - Still testing <br>
*  \ref PASS_COMPLETE_STATUS (3) - Test completed <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS (4) - Incorrect usage <br>
*
*******************************************************************************/
uint8_t SelfTest_Clock(TCPWM_Type* base, uint32_t cntNum);

/*******************************************************************************
* Function Name: SelfTest_Clock_ISR_TIMER
****************************************************************************//**
*
* Handle Interrupt Service Routine. Source - Timer.
*
*******************************************************************************/
void SelfTest_Clock_ISR_TIMER(void);
/** \} group_clock_functions */

/***************************************
* Initial Parameter Constants
***************************************/

/** \addtogroup group_clock_macros
 * \{
 */
#if (!defined(STL_CLOCK_SOURCE_HFCLOCK)) && ((defined(CY_IP_MXS40SSRSS)) || (defined(CY_IP_MXS22SRSS)) || \
    (defined(CY_DOXYGEN)))
/** High-frequency clock source selection used by the Clock self-test timer on PSOC Control C3 devices. */
#define STL_CLOCK_SOURCE_HFCLOCK (3u)
#endif \
    /* (!defined(STL_CLOCK_SOURCE_HFCLOCK)) && ((defined(CY_IP_MXS40SSRSS)) || (defined(CY_IP_MXS22SRSS)) ||
       (defined(CY_DOXYGEN))) */

#if defined(SELFTEST_PSOC4_FAMILY) || defined(CY_DOXYGEN)
/** Lower possible clock count for WDT depending on the accuracy of oscillator.
 *
 * - PSOC 4 devices: `20u`, calculated for PSOC 4100S Max with ILO = 40 kHz:
 *   `40 * (1 - 50%) = 20`
 * - PSOC 61 Programmable Line, PSOC 62 Performance Line, PSOC Control C3,
 *   and XMC7000 devices: `31u`, calculated with WCO = 32 kHz:
 *   `32 * (1 - 0.015%) = 31`
 * - XMC5000 devices: `30u`, calculated with WCO = 32.77 kHz:
 *   `32.77 * (1 - 7%) = 30`
 */
#define CLOCK_TICKS_LO                      (20u)

/** Higher possible clock count for WDT depending on the accuracy of oscillator.
 *
 * - PSOC 4 devices: `80u`, calculated for PSOC 4100S Max with ILO = 40 kHz:
 *   `40 * (1 + 100%) = 80`
 * - PSOC 61 Programmable Line, PSOC 62 Performance Line, PSOC Control C3,
 *   and XMC7000 devices: `33u`, calculated with WCO = 32 kHz:
 *   `32 * (1 + 0.015%) = 33`
 * - XMC5000 devices: `36u`, calculated with WCO = 32.77 kHz:
 *   `32.77 * (1 + 7%) = 36`
 */
#define CLOCK_TICKS_HI                      (80u)

/** Clock self-test interval in microseconds. */
#define CLOCK_TEST_TIME                     (1000u)

/** Number of timer/source-clock cycles corresponding to CLOCK_TEST_TIME.
 *
 * This macro is used on PSOC 4 devices. PSOC 61 Programmable Line, PSOC 62
 * Performance Line, PSOC Control C3, XMC7000, and XMC5000 devices calculate
 * the equivalent period at runtime from the peripheral or high-frequency clock
 * and its divider.
 */
#define CLOCK_TEST_TIME_TIMER_PERIOD        \
    (((CY_CFG_SYSCLK_IMO_FREQ_HZ/(1000000uL)) * (CLOCK_TEST_TIME)) / CLOCK_TEST_TIMER_CLK_DIV)

/** \cond INTERNAL */

/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)
/** \endcond */

#elif (defined(SELFTEST_PSOC6_FAMILY) || defined(SELFTEST_XMC7X_FAMILY) || \
    defined(SELFTEST_PSC3_FAMILY))

/** Lower possible clock count for PSOC 61 Programmable Line, PSOC 62
 * Performance Line, PSOC Control C3, and XMC7000 devices.
 *
 * For PSOC 6 and XMC, WCO = 32 kHz.
 * Lower possible clock count = 32 * (1 - 0.015%) = 31.
 */
#define CLOCK_TICKS_LO                      (31u)

/** Higher possible clock count for PSOC 61 Programmable Line, PSOC 62
 * Performance Line, PSOC Control C3, and XMC7000 devices.
 *
 * Higher possible clock count = 32 * (1 + 0.015%) = 33.
 */
#define CLOCK_TICKS_HI                      (33u)

/** Clock self-test interval in microseconds. */
#define CLOCK_TEST_TIME                     (1000u)

/** \cond INTERNAL */
/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)
/** \endcond */

#elif defined(SELFTEST_XMC5X_FAMILY)

/** Lower possible clock count for XMC5000 devices.
 *
 * For XMC5000 WCO = 32.77 kHz.
 * Lower possible clock count = 32.77 * (1 - 7%) = 30.
 */
#define CLOCK_TICKS_LO                      (30u)

/** Higher possible clock count for XMC5000 devices.
 *
 * Higher possible clock count = 32.77 * (1 + 7%) = 36.
 */
#define CLOCK_TICKS_HI                      (36u)

/** Clock self-test interval in microseconds. */
#define CLOCK_TEST_TIME                     (1000u)

#endif /* defined(SELFTEST_PSOC4_FAMILY) */
/** \} group_clock_macros */
/** \} group_clock */

#if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL)) || \
    defined(CY_DOXYGEN)

/**
 * \addtogroup group_clock_macros
 * \{
 */

/** \defgroup group_clock_csv CSV Clock Supervisor
 * \{
 * Functional self-test for the CLK_HF CSV (Clock Supervisor) comparators.
 *
 * The CSV hardware monitors a target CLK_HF against a reference clock and raises
 * a SRSS fault (CY_SYSFAULT_SRSS_CSV) when the target frequency falls outside a
 * configured supervision window.
 *
 * The self-test verifies end-to-end CSV functionality by temporarily programming
 * an impossible supervision window (period=1 monitored cycle, lower=199,
 * upper=201 reference cycles). Any CLK_HF frequency other than about
 * refFreq/200 violates it: a running clock arrives before the lower limit and a
 * stopped clock misses the upper limit, forcing an immediate
 * CY_SYSFAULT_SRSS_CSV fault. The test then polls for the fault to reach
 * SelfTest_Clock_CSV_ISR through the fault structure for up to
 * CSV_TEST_TIMEOUT_US 1-us poll iterations.
 *
 * \par Usage
 * -# Route the fault-structure interrupt to SelfTest_Clock_CSV_ISR via
 *    Cy_SysInt_Init and enable the NVIC mux line. The fault structure itself
 *    is configured by the test.
 * -# Fill an \ref stl_clock_csv_entry_t with the target CLK_HF index, the
 *    CSV reference clock (the test writes it to SRSS_CSV_REF_SEL, which is
 *    shared by all CLK_HF CSVs), the fault-structure pointer, and the
 *    NvicMuxN_IRQn the fault interrupt is routed to.
 * -# Call \ref SelfTest_Clock_CSV.  On success (\ref OK_STATUS) the CSV
 *    comparator is left disabled; the application must re-arm CSV and restore
 *    its own fault handler before enabling operational supervision.
 *
 * \}
 */

/** \addtogroup group_clock_csv
 * \{
 */

/**
 * Timeout for the SelfTest_Clock_CSV.
 */
#define CSV_TEST_TIMEOUT_US   (100u)

/** \} group_clock_macros */

/**
 * CSV self-test entry */
typedef struct
{
    uint32_t clkHf;                     /**< CLK_HF index
                                         * (1..N; CLK_HF0 unsupported since it requires RESET fault action). */
    cy_en_clkhf_csv_ref_clk_t refClk;   /**< CSV reference clock. Written by the test to SRSS_CSV_REF_SEL,
                                         * which is shared by all CLK_HF CSVs. */
    FAULT_STRUCT_Type* faultStruct;     /**< App's fault structure receiving CY_SYSFAULT_SRSS_CSV. */
    IRQn_Type nvicMux;                  /**< NvicMuxN_IRQn the app routed the fault-struct interrupt to. */
} stl_clock_csv_entry_t;

/**
 * \addtogroup group_clock_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_Clock_CSV_ISR
****************************************************************************//**
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
void SelfTest_Clock_CSV_ISR(void);

/*******************************************************************************
* Function Name: SelfTest_Clock_CSV
****************************************************************************//**
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
uint8_t SelfTest_Clock_CSV(const stl_clock_csv_entry_t* entry);

/** \} group_clock_functions */
/** \} group_clock_csv */

#endif /* (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 3)) && defined(SRSS_CSV_HF_CSV_REF_CTL) */

#endif /* End SELFTEST_CLOCK_H */

/* [] END OF FILE */

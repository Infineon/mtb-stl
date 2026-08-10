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
#if (!defined(STL_CLOCK_SOURCE_HFCLOCK)) && ((defined(CY_IP_MXS40SSRSS)) || (defined(CY_DOXYGEN)))
/** High-frequency clock source selection used by the Clock self-test timer on PSOC Control C3 devices. */
#define STL_CLOCK_SOURCE_HFCLOCK (3u)
#endif /* (!defined(STL_CLOCK_SOURCE_HFCLOCK)) && ((defined(CY_IP_MXS40SSRSS)) || (defined(CY_DOXYGEN))) */

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

#elif (defined(SELFTEST_PSOC6_FAMILY) || defined(SELFTEST_XMC7X_FAMILY) || defined(SELFTEST_PSC3_FAMILY))

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

#endif /* End SELFTEST_CLOCK_H */

/* [] END OF FILE */

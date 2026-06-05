/*******************************************************************************
* File Name: SelfTest_Clock.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the clock self tests according to Class B library.
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
/**
 * \addtogroup group_clock
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
* \return
*  1 - Test failed <br>
*  2 - Still testing <br>
*  3 - Test completed <br>
*  4 - Incorrect Usage
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
#if defined(SELFTEST_PSOC4_FAMILY)

/** \addtogroup group_clock_macros
 * \{
 */
/** Lower possible clock count for WDT depending on the accuracy of oscillator */
/* For PSoC 4100S Max, ILO = 40kHz */
/* Lower possible clock count = 40 * (1 - 50%) = 20 */
#define CLOCK_TICKS_LO                      (20u)

/** Higher possible clock count for WDT depending on the accuracy of oscillator */
/* Higher possible clock count = 40 * (1 + 100%) = 80 */
#define CLOCK_TICKS_HI                      (80u)

/** Time to test in uS */
#define CLOCK_TEST_TIME                     (1000u)

/** Number of IMO clock cycles equivalent to CLOCK_TEST_TIME (Only for CAT2 devices). */
/** For CAT1A, CAT1B(PSoC C3) and CAT1C, this value is calculated during the runtime depending on the
    Pheripheral clock and its divider. */
#define CLOCK_TEST_TIME_TIMER_PERIOD        \
    (((CY_CFG_SYSCLK_IMO_FREQ_HZ/(1000000uL)) * (CLOCK_TEST_TIME)) / CLOCK_TEST_TIMER_CLK_DIV)

/** \} group_clock_macros */

/** \cond INTERNAL */

/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)

#elif (defined(SELFTEST_PSOC6_FAMILY) || defined(SELFTEST_XMC7X_FAMILY) || \
    defined(SELFTEST_PSC3_FAMILY))

/* For PSoC6 and XMC  WCO = 32kHz */
/* Lower possible clock count = 32 * (1 - 0.015%) = 31 */
#define CLOCK_TICKS_LO                      (31u)

/* Higher possible clock count = 32 * (1 + 0.015%) = 33 */
#define CLOCK_TICKS_HI                      (33u)

#define CLOCK_TEST_TIME                     (1000u)

/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)

#elif defined(SELFTEST_XMC5X_FAMILY)

/* For XMC5X WCO = 32.77kHz */
/* Lower possible clock count = 32.77 * (1 - 7%) = 30 */
#define CLOCK_TICKS_LO                      (30u)

/* Higher possible clock count = 32.77 * (1 + 7%) = 36 */
#define CLOCK_TICKS_HI                      (36u)

#define CLOCK_TEST_TIME                     (1000u)
/** \endcond */

#endif /* defined(SELFTEST_PSOC4_FAMILY) */
/** \} group_clock */

#endif /* End SELFTEST_CLOCK_H */

/* [] END OF FILE */

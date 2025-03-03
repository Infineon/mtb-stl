/*******************************************************************************
* File Name: SelfTest_Clock.h
* Version 1.0.0
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the clock self tests according to Class B library.
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
* The ILO clock is accurate to ± 60 percent. If accuracy greater than 60 percent is required, the ILO may 
* be trimmed to be more accurate using a precision system level signal or production test. If ILO trimming
* is required, it is trimmed using the CLK_ILO_TRIM register. <br> 
* If the WCO is available, it should be used for this test, since it is much more accurate. If the 
* other oscillators (ECO, EXT_CLK, IMO) are safety critical, customers can use this test as a guideline.
*
*
* \section group_clock_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
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
* Performs Clock test and verify measured clock frequency to be in accuracy range
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



#if CY_CPU_CORTEX_M0P


/***************************************
* Initial Parameter Constants
***************************************/

/** \addtogroup group_clock_macros
* \{
*/
/** Lower possible clock count for WDT depending on the accuracy of Oscillator */
/* For PSoC 4100S Max, ILO = 40kHz */
/* lower possible clock count = 40 * (1 - 50%) = 16 */
#define CLOCK_TICKS_LO                      (20u)

/** Higher possible clock count for WDT depending on the accuracy of Oscillator */
/* Higher possible clock count = 40 * (1 + 100%) = 80 */
#define CLOCK_TICKS_HI                      (80u)

/** Time to test in uS */
#define CLOCK_TEST_TIME                     (1000u)

/** Number of IMO clock cycles equivalent to CLOCK_TEST_TIME (Only for CAT2 devices). */
/** For CAT1A, CAT1B(PSoC C3) and CAT1C this value is calculated during runtime depending on the Pheripheral clock and it's divider. */
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

#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7 || CY_CPU_CORTEX_M33)

/***************************************
* Initial Parameter Constants
***************************************/


/* For PSoC6 and XMC  WCO = 32kHz */
/* lower possible clock count = 32 * (1 - 0.015%) = 16 */
#define CLOCK_TICKS_LO                      (31u)

/* Higher possible clock count = 32 * (1 + 0.015%) = 80 */
#define CLOCK_TICKS_HI                      (33u)

#define CLOCK_TEST_TIME                     (1000u)

/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)

/** \endcond */

#endif



/** \} group_clock */

#endif /* End SELFTEST_CLOCK_H */


/* [] END OF FILE */

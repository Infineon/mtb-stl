/*******************************************************************************
* File Name: SelfTest_Clock.h
* Version 2.0
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the clock self tests according to Class B library for PSoC 4.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "SelfTest_common.h"


#if !defined(SELFTEST_CLOCK_H)
    #define SELFTEST_CLOCK_H
#include "cybsp.h"

/***************************************
* Function Prototypes
***************************************/
uint8_t SelfTest_Clock(TCPWM_Type* base, uint32_t cntNum);
void SelfTest_Clock_ISR_TIMER(void);

/***************************************
* Initial Parameter Constants
***************************************/

/* For PSoC 4100S Max, ILO = 40kHz */
/* lower possible clock count = 40 * (1 - 50%) = 16 */
#define CLOCK_TICKS_LO                      (20u)

/* Higher possible clock count = 40 * (1 + 100%) = 80 */
#define CLOCK_TICKS_HI                      (80u)

/* Time to test in uS*/
#define CLOCK_TEST_TIME                     (1000u)

/* How much the source clock to the timer is divided */
#define CLOCK_TEST_TIMER_CLK_DIV            (1u)

/* Number of IMO clock cycles equivalent to CLOCK_TEST_TIME */
#define CLOCK_TEST_TIME_TIMER_PERIOD        \
    (((CY_CFG_SYSCLK_IMO_FREQ_HZ/(1000000uL)) * (CLOCK_TEST_TIME)) / CLOCK_TEST_TIMER_CLK_DIV)

/* WDT Counter 0 */
#define WDT_COUNTER0                        (0u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS_CLK_TEST                (0U)

#endif /* End SELFTEST_CLOCK_H */


/* [] END OF FILE */

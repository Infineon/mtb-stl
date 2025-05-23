/*******************************************************************************
* File Name: SelfTest_Timer_Counter.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the clock self tests according to Class B library.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_timer_counter
* \{
*
* This test uses the timer function to ensure that the counter is 
* incrementing correctly.
* It involves initializing the counter and its corresponding
* interrupt service routine (ISR).
* The counter is configured to increment based on specified period and
* compare values.
* The ISR verifies the occurrence of the compare count interrupt,
* indicating that the timer has reached its configured compare value.
*
*
* \defgroup group_timer_counter_macros Macros
* \defgroup group_timer_counter_functions Functions
*/


#if !defined(SELFTEST_TIMER_COUNTER_H)
    #define SELFTEST_TIMER_COUNTER_H
#include "cybsp.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM) || defined(CY_DOXYGEN))
/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_timer_counter_functions
* \{
*/
/******************************************************************************
* Function Name: SelfTest_Timer_Counter_init
***************************************************************************//**
*
* Initialize the counter and initialize the timer interrupt for the Self test.
*
* \param base
* The pointer to a TCPWM instance
*
* \param cntNum
* The Counter instance number in the selected TCPWM
*
* \param config
* The pointer to configuration structure
*
* \param intsrc
* Interrupt source
*
*
******************************************************************************/
void SelfTest_Timer_Counter_init(TCPWM_Type* base, uint32_t cntNum, cy_stc_tcpwm_counter_config_t const *config, IRQn_Type intsrc);

/*****************************************************************************
* Function Name: SelfTest_Counter_Timer
**************************************************************************//**
*
*  Performs Timer Counter test by checking that the counter is incrementing.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*
*****************************************************************************/
uint8_t SelfTest_Counter_Timer(void);

/** \} group_timer_counter_functions */


/***************************************
* Initial Parameter Constants
***************************************/

/** \addtogroup group_timer_counter_macros
* \{
*/
/** TCPWM Period value */
#define TIMER_COUNTER_TEST_PERIOD                      (65535U)

/** TCPWM Compare value */
#define TIMER_COUNTER_TEST_COMPARE                     (50000U)

/** Lower possible clock cycles count*/
#define TIMER_COUNTER_COUNT_LOW                        (48800U)

/** Higher possible clock cycles count*/
#define TIMER_COUNTER_COUNT_HIGH                       (51200U)

/** Higher possible clock cycles count*/
#define TIMER_COUNTER_TIMEOUT                          (600U)

/** \} group_timer_counter_macros */

/** \} group_timer_counter */
#endif /* (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_TIMER_COUNTER_H) */

/* [] END OF FILE */
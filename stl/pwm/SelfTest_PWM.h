/*******************************************************************************
* File Name: SelfTest_PWM.h
* Version 1.0.0
*
* Description:
* Description: This file provides the constants and parameter values for the PWM 
* self tests.
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
* \addtogroup group_pwm
* \{
*
* This module performs PWM test and verify that the duty cycle set is actually
* achieved in output. 
*
* \section group_pwm_more_information More Information
*
* A 16 bit PWM is configured to 33% duty cycle with a 1 millisecond period and 
* the output is continuously polled in the loop for 5 millisecond. The test is
*  successful if the off/on ratio is between 1.875(1 7/8) to 2.125(2 1/8). A 
* range is used because the CPU polling loop is asynchronous to the PWM timing.
*
* \section group_pwm_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_pwm_macros Macros
* \defgroup group_pwm_functions Functions
*/

#if !defined(SELFTEST_PWM_H)
    #define SELFTEST_PWM_H
#include "SelfTest_common.h"

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
/** \addtogroup group_pwm_macros
* \{
*/
/** Time to test in uS */
#define PWM_TIME (1000u)
/** \} group_pwm_macros */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_pwm_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_PWM
****************************************************************************//**
*
* Performs PWM test and verify that the duty cycle set is actually achieved 
* in output. 
*
* \param pinbase
* The pointer to a GPIO port instance to which the PWM pin is connected to.
*
* \param pinNum
* The GPIO pin number to which the PWM pin is connected to.
*
*
* \note
* The parameter passed for CAT1C devices will be ignored.
*
* \return
*  0 - Test Passed <br>
*  1 - Test failed 
*
*******************************************************************************/
uint8_t SelfTest_PWM(GPIO_PRT_Type *pinbase, uint32_t pinNum);

/******************************************************************************
* Function Name: SelfTest_PWM_init
***************************************************************************//**
*
* Initialize Timer interrupt for the Self test.
*
* \param base
* The pointer to a TCPWM instance.
*
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
* \param config
* The pointer to configuration structure.
*
* \param intr_src
* Interrupt source 
******************************************************************************/
uint8_t SelfTest_PWM_init(TCPWM_Type *base, uint32_t cntNum,
             cy_stc_tcpwm_pwm_config_t const *config, IRQn_Type  intr_src);

/** \} group_pwm_functions */

/** \} group_pwm */

#endif
#endif
/*******************************************************************************
* File Name: SelfTest_PWM_Gatekill.h
* Version 1.0.0
*
* Description:
* Description: This file provides the constants and parameter values for the PWM 
* Gatekill self tests.
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
* \addtogroup group_pwm_gatekill
* \{
*
* The "Gate Kill" function is used in motor controllers and multi-level power 
* converters. When an over-voltage or over-current state is detected, the Gate Kill
* shuts down the output drivers in less than 50 nanoseconds.
*
* \section group_pwm_gatekill_more_information More Information
*
// \verbatim
The test is carried out as mentioned below.
 1) The Low power comparator/ SAR ADC Range Voilation Intr output is routed to Kill
    signal of TCPWM indicating over-voltage/over-current condition if voltage on +ve
    terminal is > -ve.
 2) If over-voltage or over-current condition it will Kill PWM output.
 3) The TCPWM base and CntNum is passed to check whether the counter is stopped or not. 
 4) If counter is not incrementing/decrementing the PWM output is inactive.
\endverbatim
*
* \section group_pwm_gatekill_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_pwm_gatekill_macros Macros
* \defgroup group_pwm_gatekill_functions Functions
*/

#if !defined(SELFTEST_PWM_GATEKILL_H)
    #define SELFTEST_PWM_GATEKILL_H
    
#include "SelfTest_common.h"

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_pwm_gatekill_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_PWM_GateKill
****************************************************************************//**
*
* The TCPWM base and CntNum is passed to check whether the counter is stopped or
* not. If counter is not incrementing/decrementing the PWM output is inactive. 
*
*
* \param base 
* The pointer to a TCPWM instance <br>
* \param cntNum 
* The Counter instance number in the selected TCPWM
*
*
* \note
* Applicable to CAT1A and CAT1C devices.
*
*
* \return
*  0 - Test Passed <br>
*  1 - Test failed 
*
*******************************************************************************/
uint8_t SelfTest_PWM_GateKill(TCPWM_Type *base, uint32_t cntNum);

/** \} group_pwm_gatekill_functions */

/** \} group_pwm_gatekill */

#endif
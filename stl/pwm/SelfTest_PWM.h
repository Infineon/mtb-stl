/*******************************************************************************
* File Name: SelfTest_PWM.h
*
* Description:
*  This file provides the constants and parameter values for the PWM
*  self tests.
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
 * \addtogroup group_pwm
 * \{
 *
 * This module performs PWM test and verifies that the duty cycle set is actually
 * achieved in the output.
 *
 * \section group_pwm_more_information More Information
 *
 * A 16-bit PWM is configured to a 33% duty cycle with a 1ms period and
 * the output is continuously polled in the loop for 5ms. The test is
 *  successful if the off/on ratio is between 1.875(1 7/8) to 2.125(2 1/8). A
 * range is used because the CPU polling loop is asynchronous to the PWM timing.
 *
 *
 * \defgroup group_pwm_macros Macros
 * \defgroup group_pwm_functions Functions
 */

#if !defined(SELFTEST_PWM_H)
    #define SELFTEST_PWM_H
#include "SelfTest_common.h"

#if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM) || CY_DOXYGEN)

/** \addtogroup group_pwm_macros
 * \{
 */
/** Time to test in uS */
#define PWM_TIME            (1000u)
/** Test timeout in uS */
#define PWM_TIME_TIMEOUT    (10000u)
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
* This function performs the PWM test and verifies that the duty cycle set is actually achieved
* in the output.
*
* \param pinbase
* The pointer to the GPIO port instance to which the PWM pin is connected.
*
* \param pinNum
* The GPIO pin number to which the PWM pin is connected.
*
*
* \note
* The pin parameters passed for CAT1C and XMC5000 devices will be ignored.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_PWM(GPIO_PRT_Type* pinbase, uint32_t pinNum);

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
* The pointer to the configuration structure.
*
* \param intr_src
* Interrupt source
*
* \return
*  0 - Initialization successful <br>
*  -1 (255) - Initialization failed
******************************************************************************/
uint8_t SelfTest_PWM_init(TCPWM_Type* base, uint32_t cntNum,
                          cy_stc_tcpwm_pwm_config_t const* config, IRQn_Type  intr_src);

/** \} group_pwm_functions */

/** \} group_pwm */

#endif /* if (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM) || CY_DOXYGEN) */
#endif /* if !defined(SELFTEST_PWM_H) */

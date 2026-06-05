/*******************************************************************************
* File Name: SelfTest_PWM_Gatekill.h
*
* Description:
*  This file provides the constants and parameter values for the PWM
*  Gatekill self tests.
*
*******************************************************************************
* (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
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
 * \addtogroup group_pwm_gatekill
 * \{
 *
 * The Gate Kill function is used in motor controllers and multi-level power
 * converters. When an over-voltage or over-current state is detected, the Gate Kill
 * shuts down the output drivers in less than 50 nanoseconds.
 *
 * \section group_pwm_gatekill_more_information More Information
 *
 * The test procedure:
 *
 *      1) The Low power comparator/ SAR ADC Range Voilation Intr output is routed to Kill
 *      signal of TCPWM indicating over-voltage/over-current condition if the voltage on +ve
 *      terminal is > -ve.
 *      2) Under over-voltage or over-current condition it will Kill the PWM output.
 *      3) The TCPWM base and CntNum is passed to check whether the counter is stopped or not.
 *      4) If the counter is not incrementing/decrementing, the PWM output is inactive.
 *
 *
 * \defgroup group_pwm_gatekill_functions Functions
 */

#if !defined(SELFTEST_PWM_GATEKILL_H)
    #define SELFTEST_PWM_GATEKILL_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_M0S8TCPWM) || defined(CY_IP_MXTCPWM) || defined(CY_DOXYGEN))
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
* not. If the counter is not incrementing/decrementing, the PWM output is inactive.
*
*
* \param base
* The pointer to a TCPWM instance. <br>
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_PWM_GateKill(TCPWM_Type* base, uint32_t cntNum);

/** \} group_pwm_gatekill_functions */

#endif /* (defined(CY_IP_M0S8TCPWM) || defined(CY_IP_MXTCPWM) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_PWM_GATEKILL_H) */

/** \} group_pwm_gatekill */
/* [] END OF FILE */

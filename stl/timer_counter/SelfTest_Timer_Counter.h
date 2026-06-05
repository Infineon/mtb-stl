/*******************************************************************************
* File Name: SelfTest_Timer_Counter.h
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
void SelfTest_Timer_Counter_init(TCPWM_Type* base, uint32_t cntNum,
                                 cy_stc_tcpwm_counter_config_t const* config, IRQn_Type intsrc);

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
#define TIMER_COUNTER_TIMEOUT                          (2500U)

/** \} group_timer_counter_macros */

/** \} group_timer_counter */
#endif /* (defined(CY_IP_MXTCPWM) || defined(CY_IP_M0S8TCPWM) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_TIMER_COUNTER_H) */

/* [] END OF FILE */

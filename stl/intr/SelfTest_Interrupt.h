/******************************************************************************
 * File Name:  SelfTest_Interrupt.h
 *
 * Description:
 *  This file provides function prototypes, constants, and parameter values
 *  used for the interrupt self test.
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
 * \defgroup group_intr INTR (INTR STL module)
 * \{
 *
 * To meet the Class B requirement, check the interrupt for “incorrect frequency”. This test is a
 * highly system dependent and therefore the STL can only contribute the wrap up
 * handle, which checks that a number of specific interrupts occurred at least and at most a
 * predefined number of times.
 *
 *
 *
 * \section group_intr_more_information More Information
 *
 * It is assumed that Interrupt_Test (interrupt test function) is called periodically, e.g.
 * triggered by a timer or constantly in the main loop. Each specific interrupt handler
 * to be supervised must decrement a dedicated global variable (freq), InterruptTest(), compares
 * that variable to predefined upper and lower bounds, returns an error, if the limits are exceeded.
 *
 *
 * \defgroup group_intr_macros Macros
 * \defgroup group_intr_functions Functions
 */

#if !defined(SELFTEST_INTERRUPT_H)
    #define SELFTEST_INTERRUPT_H

#include "cy_pdl.h"
#include "SelfTest_common.h"


/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_intr_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_Interrupt
****************************************************************************//**
*
*  This function performs the interrupt execution self test. This function checks the interrupt
*  isr from Timer.
*
*
* \param base
* The pointer to a TCPWM instance <br>
* \param cntNum
* The Counter instance number in the selected TCPWM
*
* \note
* Use a TCPWM counter and interrupt source dedicated to this self-test. The
* application must route the selected counter interrupt to
* \ref SelfTest_Interrupt_ISR_TIMER, and the counter interrupt state is consumed
* by the test.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTest_Interrupt(TCPWM_Type* base, uint32_t cntNum);

/*******************************************************************************
* Function Name: SelfTest_Interrupt_ISR_TIMER
****************************************************************************//**
*
* Handle Interrupt Service Routine. Source - Timer.
*
*******************************************************************************/
void SelfTest_Interrupt_ISR_TIMER(void);

/** \} group_intr_functions */

/***************************************
* Initial Parameter Constants
***************************************/

/** \addtogroup group_intr_macros
 * \{
 */
#if (defined (CY_IP_M0S8CPUSSV3) || defined (CY_DOXYGEN))
/** Lower possible interrupt count.
 *
 * - PSOC 4 devices: `9u`
 * - PSOC 61 Programmable Line, PSOC 62 Performance Line, PSOC Control C3,
 *   XMC7000, and XMC5000 devices: `22u` for configurations
 *   where the timer input clock and period produce about 22 to 27 terminal-
 *   count interrupts in 1000 us; `22u` is the low end of that fixed pass
 *   window
 */
#define NUMBER_OF_TIMER_TICKS_LO          (9u)

/** Higher possible interrupt count.
 *
 * - PSOC 4 devices: `15u`
 * - PSOC 61 Programmable Line, PSOC 62 Performance Line, PSOC Control C3,
 *   XMC7000, and XMC5000 devices: `27u` for configurations
 *   where the timer input clock and period produce about 22 to 27 terminal-
 *   count interrupts in 1000 us; `27u` is the high end of that fixed pass
 *   window
 */
#define NUMBER_OF_TIMER_TICKS_HI          (15u)

#elif (defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) || defined (CY_IP_M33SYSCPUSS))
/** Lower possible interrupt count for PSOC 61 Programmable Line, PSOC 62 Performance Line,
*  PSOC Control C3, XMC7000, and XMC5000 devices when the timer input clock and
*  period are configured to produce about 22 to 27 terminal-count interrupts in 1000 us. */
#define NUMBER_OF_TIMER_TICKS_LO          (22u)

/** Higher possible interrupt count for PSOC 61 Programmable Line, PSOC 62 Performance Line,
 *  PSOC Control C3, XMC7000, and XMC5000 devices when the timer input clock and
 *  period are configured to produce about 22 to 27 terminal-count interrupts in 1000 us. */
#define NUMBER_OF_TIMER_TICKS_HI          (27u)
#endif /* if (defined (CY_IP_M0S8CPUSSV3) || defined (CY_DOXYGEN)) */


/** Interrupt test period */
#define INTERRUPT_TEST_TIME               (1000u)

/** \} group_intr_macros */

/** \} group_intr */

#endif /* SELFTEST_INTERRUPT_H */

/* [] END OF FILE */

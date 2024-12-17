/******************************************************************************
 * File Name:  SelfTest_Interrupt.h
 * Version 1.0.0
 *
 * Description:
 *  This file provides function prototypes, constants, and parameter values
 *  used for interrupt self test.
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
* \addtogroup group_intr
* \{
*
* To meet Class B requirement, interrupt must be checked for “incorrect frequency”. This test is a task which is highly system 
* dependent and therefore the STL can only contribute the wrap up handle, which checks that a number of specific interrupts 
* occurred at least and at most a predefined number of times. 
*
*
*
* \section group_intr_more_information More Information
*
* It is assumed that Interrupt_Test (interrupt test function) is called periodically, e.g. triggered by a timer or constantly in the 
* main loop. Each specific interrupt handler which is to be supervised, must decrement a dedicated global variable (freq), 
* InterruptTest() compares that variable to predefined upper and lower bounds, returns an error, if the limits are exceeded.
*
*
* \section group_intr_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
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
*  This function performs the interrupt execution self test. This function checks interrupt
*  isr from Timer.
*
*
* \param base 
* The pointer to a TCPWM instance <br>
* \param cntNum 
* The Counter instance number in the selected TCPWM
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
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
#if CY_CPU_CORTEX_M0P
/** Lower possible interrupt count. This value may differ depending on the device used (CAT1A, CAT1B(PSoC C3), CAT1C, or CAT2). */
#define NUMBER_OF_TIMER_TICKS_LO          (9u)

/** Higher possible interrupt count. This value may differ depending on the device used (CAT1A, CAT1B(PSoC C3), CAT1C, or CAT2).  */
#define NUMBER_OF_TIMER_TICKS_HI          (15u)

#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7 || CY_CPU_CORTEX_M33)
/* If the input clock is of 25MHz*/
/* lower possible int count */
#define NUMBER_OF_TIMER_TICKS_LO          (22u)

/* Higher possible int count */
#define NUMBER_OF_TIMER_TICKS_HI          (27u)
#endif


/** Interrupt test period */
#define INTERRUPT_TEST_TIME               (1000u)

/** \} group_intr_macros */

/** \} group_intr */

#endif /* SELFTEST_INTERRUPT_H */

/* [] END OF FILE */

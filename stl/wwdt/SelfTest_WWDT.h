/*******************************************************************************
* File Name: SelfTest_WWDT.h
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for watchdog timer self tests for CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  XMC7200D-E272K8384
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
* \addtogroup group_wwdt
* \{
*
* Window-selectable WDTs allow the watchdog timeout period to be adjusted,
* providing more flexibility to meet different processor timing requirements. 
* The windowed WDT provides a way to demand that the ClearWDT instruction be executed, for example, only in the last quarter of the watchdog timeout period.
* Essentially, this enables better code flow monitoring to catch firmware bugs. For example, an application bug that results in the ClearWDT instruction 
* repeatedly executing close to the beginning of the code flow could be interpreted as a normal operation in the non-windowed WDT mode.
* The caveat to the windowed WDT mode is that the ClearWDT instruction must be called within a prescribed window, which limits the tolerance
* of the clock source that drives the WDT.
*
*
*
* \section group_wwdt_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_wwdt_macros Macros
* \defgroup group_wwdt_functions Functions
*/
#include "SelfTest_common.h"

#if !defined(SELFTEST_WWDT_H)
    #define SELFTEST_WWDT_H

#include "cy_pdl.h"


#if CY_CPU_CORTEX_M7

/** \addtogroup group_wwdt_macros
* \{
*/
/** LOWER period before which if WDT is served, will reset the system or carries out no action */
#define WDT_LOWER_LIMIT                     (50000u)
/** Warning period. If WDT reaches this value then an interrupt will get generated.  */
#define WDT_WARN_LIMIT                      (80000u)
/** Upper period. If WDT reaches this value the system will get reset or carries out no action */
#define WDT_UPPER_LIMIT                     (100000U)

/** \} group_wwdt_macros */

/** \cond INTERNAL */
#if defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2)
#define WDT_IRQ_INTC_NUMBER                 srss_interrupt_IRQn
#else
#define WDT_IRQ_INTC_NUMBER                 srss_interrupt_wdt_IRQn
#endif
/** \endcond */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_wwdt_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_Windowed_WDT
****************************************************************************//**
*
* This function checks the WDT in window mode. It: <br>
* 1) Services the WDT before the LOWER limit which causes the device to reset. <br>
* 2) Check the occurance of interrupt at WARN LIMIT.
*
*
* \note
* Only applicable for CAT1C devices.
* \return
*  0 - Test passed <br>
*  1 - Test failed 
*
*******************************************************************************/
uint8_t SelfTest_Windowed_WDT(void);

#endif

/** \} group_wwdt_functions */

/** \} group_wwdt */

#endif /* End SELFTEST_WWDT_H */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_WWDT.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the windowed watchdog timer self tests.
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
 * \addtogroup group_wwdt
 * \{
 *
 * Window-selectable WDTs allow adjusting the watchdog timeout period
 * to provide more flexibility for the different processor timing requirements.
 * The windowed WDT provides a way to demand that the ClearWDT instruction be executed, for example,
 * only in the last quarter of the watchdog timeout period. Essentially, this enables better code
 * flow monitoring to catch firmware bugs. For example, an application bug that results in the
 * ClearWDT instruction repeatedly executing close to the beginning of the code flow could be
 * interpreted as a normal operation in the non-windowed WDT mode. The caveat to the windowed WDT
 * mode is that the ClearWDT instruction must be called within a prescribed window, which limits the
 * tolerance of the clock source, which drives the WDT.
 *
 *
 * \defgroup group_wwdt_macros Macros
 * \defgroup group_wwdt_functions Functions
 */

#if !defined(SELFTEST_WWDT_H)
    #define SELFTEST_WWDT_H

#include "cy_pdl.h"
#include "SelfTest_common.h"


#if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2)) || defined (CY_DOXYGEN))

/** \addtogroup group_wwdt_macros
 * \{
 */
/** The LOWER period before which if WDT is served it will reset the system or does nothing. */
#define WDT_LOWER_LIMIT                     (50000u)
/** The warning period. If WDT reaches this value, an interrupt will be generated.  */
#define WDT_WARN_LIMIT                      (80000u)
/** The upper period. If WDT reaches this value, the system will reset or does nothing. */
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
* This function checks the WDT in Window mode. It: <br>
* 1) Services the WDT before the LOWER limit, which causes the device to reset. <br>
* 2) Checks the occurance of interrupt at WARN LIMIT.
*
*
* \note
* Applicable only for CAT1C devices.
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_Windowed_WDT(void);

#endif /* if ((defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2)) || defined (CY_DOXYGEN)) */

/** \} group_wwdt_functions */

/** \} group_wwdt */

#endif /* End SELFTEST_WWDT_H */

/* [] END OF FILE */

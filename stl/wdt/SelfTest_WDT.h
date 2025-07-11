/*******************************************************************************
* File Name: SelfTest_WDT.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the watchdog timer self tests.
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
 * \addtogroup group_wdt
 * \{
 *
 * This function implements the watchdog functional test. The function starts the WDT and runs an
 * infinite loop. If the WDT works, it generates a reset. After the reset, the function analyzes the
 * reset source. If the watchdog is the source of the reset, the function returns; otherwise, the
 * executes infinite loop.
 *
 *
 * \defgroup group_wdt_macros Macros
 * \defgroup group_wdt_functions Functions
 */

#if !defined(SELFTEST_WDT_H)
    #define SELFTEST_WDT_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

/** \addtogroup group_wdt_macros
 * \{
 */


#if (defined(CY_IP_S8SRSSLT) || defined (CY_DOXYGEN))

/***************************************
* Initial Parameter Constants
***************************************/

/** To set the WDT period. <br>
   For CAT1A, and CAT2, WDT does a reset when this Limit is hit for the 3rd time. <br>
   For CAT1C, the Upper Limit is set to this value and will do a reset it WDT is not served
   before Upper Limit. <br>
   Note : This value differs for CAT1A, CAT1B(PSoC C3), CAT1C, and CAT2 devices. */
#define WDT_PERIOD                     (900u)

/** Sets the desired number of ignore bits. To make WDT counter (32/16 - IGNORE_BITS) bits up
 *  counter. */
#define IGNORE_BITS                    (3U)

/** WDT guard interval <br>
   Note : This value differs for CAT1A, CAT1B(PSoC C3), CAT1C, and CAT2 devices. */
#define WDT_DATA_TIME                  (3000u)

/** The waiting time in milliseconds, for the proper start-up of ILO. */
#define ILO_START_UP_TIME              (2U)


#elif (defined (CY_IP_MXS40SRSS) || defined (CY_IP_MXS40SSRSS))

#define WDT_PERIOD                     (32000u)

/* Set the desired number of ignore bits */
#define IGNORE_BITS                    (2U)

#if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2))
/* WDT guard interval */
#define WDT_DATA_TIME                  (4u)

#elif (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2))
/* WDT guard interval */
#define WDT_DATA_TIME                  (2u)
#elif defined (CY_IP_MXS40SSRSS)
/* WDT guard interval */
#define WDT_DATA_TIME                  (8u)
#define MATCH_BITS                     (14U)
#endif /* if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2)) */

#endif /* if (defined(CY_IP_S8SRSSLT) || defined (CY_DOXYGEN)) */

/** \} group_wdt_macros */


/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_wdt_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_WDT
****************************************************************************//**
*
* This function performs the Watchdog test. The function runs WDT and waits for a defined
* period of time. If the PSOC was not reset, the function returns the ERROR status.
* If the PSOC was reset, bit 1 in CyResetStatus must be set to 1. If
* this bit was set, the function returns the OK status.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_WDT(void);

/** \} group_wdt_functions */

/** \} group_wdt */

#endif /* End SELFTEST_WDT_H */

/* [] END OF FILE */

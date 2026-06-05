/*******************************************************************************
* File Name: SelfTest_WDT.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the watchdog timer self tests.
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

/*******************************************************************************
* File Name: SelfTest_Config.h
* Version 1.0.0
*
* Description:
*  This file provides defines to force artificial errors and to
*  provoke detection of defects by self tests for CAT2(PSoC4), CAT1A,
*  CAT1B(PSoC C3), CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#if !defined(SELFTEST_CONFIG_H)
#define SELFTEST_CONFIG_H

#include "cycfg.h"

#include "cy_pdl.h"

#if !defined(SELFTEST_CONFIG_DEBUG)
    #define SELFTEST_CONFIG_DEBUG  0UL
#endif


#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
#if !defined(CYBSP_CLOCK_TEST_TIMER_HW)
    #define CYBSP_CLOCK_TEST_TIMER_HW  TCPWM0
#endif
#else
#if !defined(CYBSP_CLOCK_TEST_TIMER_HW)
    #define CYBSP_CLOCK_TEST_TIMER_HW  TCPWM
#endif
#endif

#if (defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
#if !defined(CYBSP_CLOCK_TEST_TIMER_NUM)
    #define CYBSP_CLOCK_TEST_TIMER_NUM 0UL
#endif
#elif (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))
#if !defined(CYBSP_CLOCK_TEST_TIMER_NUM)
    #define CYBSP_CLOCK_TEST_TIMER_NUM 513UL
#endif
#else
#if !defined(CYBSP_CLOCK_TEST_TIMER_NUM)
    #define CYBSP_CLOCK_TEST_TIMER_NUM 1UL
#endif
#endif


#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
#if !defined(CYBSP_TIMER_HW)
    #define CYBSP_TIMER_HW TCPWM0
#endif
#else
#if !defined(CYBSP_TIMER_HW)
    #define CYBSP_TIMER_HW TCPWM
#endif
#endif


#if (defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
#if !defined(CYBSP_TIMER_NUM)
    #define CYBSP_TIMER_NUM 1UL 
#endif
#elif (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))
#if !defined(CYBSP_TIMER_NUM)
    #define CYBSP_TIMER_NUM 512UL
#endif
#else
#if !defined(CYBSP_TIMER_NUM)
    #define CYBSP_TIMER_NUM 0UL
#endif
#endif

#endif /* SELFTEST_CONFIG_H */

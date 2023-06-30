/*******************************************************************************
* File Name: SelfTest_Config.h
* Version 0.1
*
* Description:
*  This file provides defines to force artificial errors and to
*  provoke detection of defects by self tests for PSoC4.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
*
*******************************************************************************
* Copyright 2023, Cypress Semiconductor Corporation (an Infineon company) or
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

#if !defined(CYBSP_CLOCK_TEST_TIMER_HW)
    #define CYBSP_CLOCK_TEST_TIMER_HW  TCPWM
#endif

#if !defined(CYBSP_CLOCK_TEST_TIMER_NUM)
    #define CYBSP_CLOCK_TEST_TIMER_NUM 1UL
#endif

#if !defined(CYBSP_TIMER_HW)
    #define CYBSP_TIMER_HW TCPWM
#endif

#if !defined(CYBSP_TIMER_NUM)
    #define CYBSP_TIMER_NUM 0UL
#endif

#endif /* SELFTEST_CONFIG_H */

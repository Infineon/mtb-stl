/*******************************************************************************
* File Name: SelfTest_PWM_GateKill.c
* Version 1.0.0
*
* Description: This file provides the source code for the PWM Gatekill self tests
* for CAT1A, CAT1C devices.
*
*
* Hardware Dependency:
*  XMC7200D-E272K8384
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

#include "cy_pdl.h"
#include "SelfTest_PWM_GateKill.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

uint8_t SelfTest_PWM_GateKill(TCPWM_Type *base, uint32_t cntNum)
{

    uint32_t pwm_count;
    uint32_t ref_pwm_count;

    ref_pwm_count = Cy_TCPWM_Counter_GetCounter(base, cntNum);
    
    Cy_SysLib_Delay(10);

    pwm_count = Cy_TCPWM_Counter_GetCounter(base, cntNum);

#if ERROR_IN_PWM_GATEKILL
    pwm_count += 10;
#endif
    if(pwm_count == ref_pwm_count)
    {
        return OK_STATUS;
    }
    else
    {
        return ERROR_STATUS;
    }
}
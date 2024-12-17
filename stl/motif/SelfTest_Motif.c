/*******************************************************************************
* File Name: SelfTest_Motif.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to the API for Motif self tests.
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

#include "cy_pdl.h"
#include "SelfTest_Motif.h"

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))

#define MOTIF0 TCPWM0_MOTIF_GRP1_MOTIF0

#ifndef SELTTEST_MOTIF_H
#define SELTTEST_MOTIF_H
#include "SelfTest_ErrorInjection.h"

/* Inputs values */
int input0[6] = {1,0,0,0,1,1};
int input1[6] = {0,0,1,1,1,0};
int input2[6] = {1,1,1,0,0,0};


/*******************************************************************************
* Function Name: SelfTest_Motif_Init
****************************************************************************//**
*
*  This function perform self test on Motif module.
*  The MOTIF unit is a flexible and powerful component for motor control systems
*  that use Rotary Encoders, Hall Sensors as feedback loop.
*  This API initializes the MOTIF module in Hall sensor mode and
*  input PWMs which simulates HALL sensor output
*
*******************************************************************************/

void SelfTest_Motif_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base ,cy_stc_tcpwm_motif_hall_sensor_config_t const *config, stl_motif_tcpwm_config_t *input_config)
{
    /* Init and start PWM for phase A*/
    Cy_TCPWM_PWM_Init(input_config->Hall_0_base, input_config->Hall_0_Num , input_config->Hall_0_in_config);
    Cy_TCPWM_PWM_Enable(input_config->Hall_0_base, input_config->Hall_0_Num);
    Cy_TCPWM_TriggerStart_Single(input_config->Hall_0_base, input_config->Hall_0_Num);

    /* Init and start PWM for phase B*/
    Cy_TCPWM_PWM_Init(input_config->Hall_1_base, input_config->Hall_1_Num, input_config->Hall_1_in_config);
    Cy_TCPWM_PWM_Enable(input_config->Hall_1_base, input_config->Hall_1_Num);
    Cy_TCPWM_TriggerStart_Single(input_config->Hall_1_base, input_config->Hall_1_Num);

    /* Init and start PWM for phase C*/
    Cy_TCPWM_PWM_Init(input_config->Hall_2_base, input_config->Hall_2_Num , input_config->Hall_2_in_config);
    Cy_TCPWM_PWM_Enable(input_config->Hall_2_base, input_config->Hall_2_Num);
    Cy_TCPWM_TriggerStart_Single(input_config->Hall_2_base, input_config->Hall_2_Num);

    /* MOTIF module initialization in Hall sensor Mode */
    Cy_TCPWM_MOTIF_Hall_Sensor_Init(base, config);

}


/*******************************************************************************
* Function Name: SelfTest_Motif_Start
****************************************************************************//**
*
*  This function starts the Motif module and Modulation output from motif
*  is started with existing values. It reports the status of the interrupts
*  and clears the triggered interrupts.
*  It compares the defined Multi-Channel Mode Pattern with output modulation value, returns
*  the result OK_STATUS if values match or returns ERROR_STATUS
*  if the values do not match.
*
*******************************************************************************/

uint8_t SelfTest_Motif_Start(TCPWM_MOTIF_GRP_MOTIF_Type *base, stl_motif_tcpwm_config_t *input_config)
{
	uint32_t res_arr[6] = {0x2A33332A,0,0xCC8A8ACC,0x8ACC8ACC,0,0x332A332A};
	uint8_t result = OK_STATUS;
	uint32_t cnt1 = 0;
    Cy_TCPWM_MOTIF_Start(base);

    for(int i=1; i<=6; i++)
    {

    	uint32_t interrupt_status = Cy_TCPWM_MOTIF_GetInterruptStatus(base);
    	Cy_TCPWM_MOTIF_ClearInterrupt(base,interrupt_status);

        /* Get the counter value. This is started when motif is started and motif output is ticking. */
        cnt1 = Cy_TCPWM_MOTIF_HALL_Get_Output_Modulation_Value(base);
        if (cnt1 != res_arr[i-1])
        {
        	result = ERROR_STATUS;
        }
        CyDelay(100U);
		#if (ERROR_IN_MOTIF)
        	Cy_TCPWM_PWM_Configure_LineSelect(input_config->Hall_0_base, input_config->Hall_1_Num, (cy_en_line_select_config_t)input0[i], (cy_en_line_select_config_t)input0[i]);
		#else
			Cy_TCPWM_PWM_Configure_LineSelect(input_config->Hall_0_base, input_config->Hall_0_Num, (cy_en_line_select_config_t)input0[i], (cy_en_line_select_config_t)input0[i]);
		#endif
		Cy_TCPWM_PWM_Configure_LineSelect(input_config->Hall_1_base, input_config->Hall_1_Num, (cy_en_line_select_config_t)input1[i], (cy_en_line_select_config_t)input1[i]);
		Cy_TCPWM_PWM_Configure_LineSelect(input_config->Hall_2_base, input_config->Hall_2_Num, (cy_en_line_select_config_t)input2[i], (cy_en_line_select_config_t)input2[i]);

    }
	return result;
}

#endif /* SELFTEST_MOTIF_H */
#endif
/* [] END OF FILE */


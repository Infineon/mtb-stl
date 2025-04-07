/*******************************************************************************
* File Name: SelfTest_Motif.c
*
* Description:
*  This file provides the source code to the API for Motif self tests.
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

#include "cy_pdl.h"
#include "SelfTest_Motif.h"

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))

#ifndef SELTTEST_MOTIF_H
#define SELTTEST_MOTIF_H
#include "SelfTest_ErrorInjection.h"

/*******************************************************************************
* Function Name: SelfTest_MotifCfgSigGen
****************************************************************************//**
*
*  This function configure the TCPWM to generate the Encoder pulses (Phase-A,
*  Phase-B and Index). Generated pulses are internally connected to MOTIF
*  module.
*
* \param hPtr
* Pointer to the motif self test configuration handler.
*
*******************************************************************************/
static inline void SelfTest_MotifCfgSigGen(stl_motif_cfg_handle_t * hPtr)
{
    for(uint8_t i = 0u; i < EMU_SIG_NUM; i++)
    {
        /*Configure TCPWM to generate encoder input pulses*/
        Cy_TCPWM_PWM_Init(hPtr->sgen_base[i], hPtr->sgen[i].idx, hPtr->sgen[i].cfg);
        Cy_TCPWM_PWM_Enable(hPtr->sgen_base[i], hPtr->sgen[i].idx);
    }

    /* Start test signals.*/
    Cy_TCPWM_TriggerStart_Single(
            hPtr->sgen_base[(EMU_SIG_NUM-1u)],
            hPtr->sgen[(EMU_SIG_NUM-1u)].idx);
}

/*******************************************************************************
* Function Name: SelfTest_Motif_Init
*******************************************************************************/
void SelfTest_Motif_Init(stl_motif_cfg_handle_t * hPtr)
{
    /* Configure emulated signals generation*/
    SelfTest_MotifCfgSigGen(hPtr);

    /* Initialize the TCPWM to capture the count between two quadrature clock */
    Cy_TCPWM_Counter_Init(
            hPtr->qclk_base,
            hPtr->qclk.idx,
            hPtr->qclk.cfg);

    Cy_TCPWM_Counter_Enable(hPtr->qclk_base, hPtr->qclk.idx);

    /* Initialize and enable the MOTIF module in quadrature decoder mode */
    Cy_TCPWM_MOTIF_Quaddec_Init(
            hPtr->motif_base,
            hPtr->motif_config);

    /* Enable the MOTIF */
    Cy_TCPWM_MOTIF_Enable(hPtr->motif_base);
}

/*******************************************************************************
* Function Name: SelfTest_Motif_Start
*******************************************************************************/
uint8_t SelfTest_Motif_Start(stl_motif_cfg_handle_t *hPtr)
{
    uint8_t result = OK_STATUS;

    uint32_t cap_time = 0u;

    /* Start MOTIF */
    Cy_TCPWM_MOTIF_Start(hPtr->motif_base);

    /*Delay to generate the falling edge of Q-CLK ticks*/
    Cy_SysLib_Delay(hPtr->delay);

    /*Capture the tcpwm count between two q-clock*/
    cap_time = Cy_TCPWM_Counter_GetCapture(
            hPtr->qclk_base,
            hPtr->qclk.idx);

    if((cap_time < (hPtr->ref_count - hPtr->margin_count)) ||
            (cap_time > (hPtr->ref_count + hPtr->margin_count)))
    {
        result = ERROR_STATUS;
    }

    return result;
}

#endif /* SELFTEST_MOTIF_H */
#endif
/* [] END OF FILE */


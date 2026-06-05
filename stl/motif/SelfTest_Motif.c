/*******************************************************************************
* File Name: SelfTest_Motif.c
*
* Description:
*  This file provides the source code to the API for the Motif self tests.
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

#include "cy_pdl.h"
#include "SelfTest_Motif.h"

#if defined (CY_IP_MXS40TCPWM)

#ifndef SELTTEST_MOTIF_H
#define SELTTEST_MOTIF_H

/*******************************************************************************
* Function Name: SelfTest_MotifCfgSigGen
****************************************************************************//**
*
*  This function configures the TCPWM to generate Encoder pulses (Phase-A,
*  Phase-B and Index). Generated pulses are internally connected to MOTIF
*  module.
*
* \param hPtr
* The pointer to the motif self test configuration handler.
*
*******************************************************************************/
static inline void SelfTest_MotifCfgSigGen(stl_motif_cfg_handle_t* hPtr)
{
    for (uint8_t i = 0u; i < EMU_SIG_NUM; i++)
    {
        /*Configure TCPWM to generate encoder input pulses*/
        (void)Cy_TCPWM_PWM_Init(hPtr->sgen_base[i], hPtr->sgen[i].idx, hPtr->sgen[i].cfg);
        Cy_TCPWM_PWM_Enable(hPtr->sgen_base[i], hPtr->sgen[i].idx);
    }

    /* Start test signals.*/
    Cy_TCPWM_TriggerStart_Single(
        hPtr->sgen_base[(EMU_SIG_NUM-1u)],
        hPtr->sgen[(EMU_SIG_NUM-1u)].idx);
}


/*******************************************************************************
* Function Name: SelfTest_Motif_Init
****************************************************************************//**
*
*  This function initializes the MOTIF self test configuration. Initialization includes
*  - Generate the emulated signals for Phase-A, Phase-B and Index using TCPWM.
*  These signals are input to the MOTIF module.
*  - Configure the TCPWM counter to capture the Q-CLK resolution.
*  - Configure the MOTIF module.
* \param hPtr
* Pointer to the motif self test configuration handler.
*
*******************************************************************************/
void SelfTest_Motif_Init(stl_motif_cfg_handle_t* hPtr)
{
    /* Configure emulated signals generation*/
    SelfTest_MotifCfgSigGen(hPtr);

    /* Initialize the TCPWM to capture the count between two quadrature clocks */
    (void)Cy_TCPWM_Counter_Init(
        hPtr->qclk_base,
        hPtr->qclk.idx,
        hPtr->qclk.cfg);

    Cy_TCPWM_Counter_Enable(hPtr->qclk_base, hPtr->qclk.idx);

    /* Initialize and enable the MOTIF module in quadrature decoder mode */
    (void)Cy_TCPWM_MOTIF_Quaddec_Init(
        hPtr->motif_base,
        hPtr->motif_config);

    /* Enable MOTIF */
    Cy_TCPWM_MOTIF_Enable(hPtr->motif_base);
}


/*******************************************************************************
* Function Name: SelfTest_Motif_Start
****************************************************************************//**
*
*  This function starts the Motif and capture the count between two quadrature
*  clock edges.
*  Captured value compares with the reference value and returns
*  the result.
*
* \param hPtr
* Pointer to the motif self test configuration handler.
*
* \return
*  "0" "OK_STATUS" - Test passed <br>
*  "1" "ERROR_STATUS" - Test failed
*
*******************************************************************************/
uint8_t SelfTest_Motif_Start(stl_motif_cfg_handle_t* hPtr)
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

    if ((cap_time < (hPtr->ref_count - hPtr->margin_count)) ||
        (cap_time > (hPtr->ref_count + hPtr->margin_count)))
    {
        result = ERROR_STATUS;
    }

    return result;
}


#endif /* SELFTEST_MOTIF_H */
#endif /* if defined (CY_IP_MXS40TCPWM) */
/* [] END OF FILE */

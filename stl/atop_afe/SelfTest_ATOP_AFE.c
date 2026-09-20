/*******************************************************************************
* File Name: SelfTest_ATOP_AFE.c
*
* Description:
*  This file provides the source code for the ATOP AFE (Analog Front End)
*  programmable-gain verification self-test according to Class B library.
*
*******************************************************************************
* (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
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
#include "SelfTest_ATOP_AFE.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

/*******************************************************************************
* Test parameters
*******************************************************************************/

/* ADC busy-poll timeout. Each loop iteration delays 1 us via
 * Cy_SysLib_DelayUs(1U), so this value is the timeout in microseconds and is
 * independent of the CPU clock frequency. 100 ms is a defensive upper bound;
 * a single differential conversion completes within microseconds. */
#define ATOP_AFE_BUSY_POLL_TIMEOUT_US   (100000U)

/* Settling time (microseconds) for the analog front end after the AREF analog
 * control is asserted, before a conversion is triggered. */
#define ATOP_AFE_AREF_SETTLE_US         (10000U)

/* Time (microseconds) allowed for one conversion to complete after it is
 * triggered, before the busy status is polled. */
#define ATOP_AFE_CONV_SETTLE_US         (1000U)

/* Differential channel 0 (the AFE channel) as a single-bit mask; the AFE is
 * always on channel 0 of its group. */
#define ATOP_AFE_CH0_MASK               (0x0001U)


/*******************************************************************************
* Static configuration
*******************************************************************************/

/* AUX slot placeholder. The AFE test uses no AUX slot (channel 7 stays on the
 * ADC data path), but Cy_PPCA_ADC_Init dereferences aux_slot_config, so a valid
 * object must be provided. */
static cy_stc_aux_slot_config_t stlAtopAfe_auxSlot =
{
    .aux_ch             = 0U,
    .aux_ch_gnd         = 0U,
    .alt_aux_ch         = 0U,
    .alt_aux_ch_gnd     = 0U,
    .aux_gnd_group2     = (cy_en_adc_vssa_t)0U,
    .alt_aux_gnd_group2 = (cy_en_adc_vssa_t)0U,
    .ch7_data_path      = CY_DATA_PATH_ADC7,
    .gnd_group          = false,
};

/* ADC config: differential channel 0, signed, one manual-triggered slot. This
 * is invariant across the three reads, so it is kept const (in flash); only the
 * AFE gain changes per read. */
static const cy_stc_ppca_adc_config_t stlAtopAfe_adcCfg =
{
    .calib_gain_mode                 = false,
    .aux_slot                        = false,
    .alt_aux_slot                    = false,
    .channels                        = ATOP_AFE_CH0_MASK,
    .channel_type                    = ATOP_AFE_CH0_MASK,
    .channel_data_type               = ATOP_AFE_CH0_MASK,
    .aux_channel_data_type           = 0U,
    .alt_aux_channel_data_type       = 0U,
    .aux_slot_config                 = &stlAtopAfe_auxSlot,
    .adc_conv_config                 =
    {
        .group_channel               = false,
        .trigger_mode                = CY_ADC_MANUAL_TRIGGER,
        .auto_trigger_sample_clck    = 63U,
        .num_of_slot                 = 1U,
        .eos_src                     = false,
        .num_of_aux_slot             = 0U,
        .num_of_alt_aux_slot         = 0U,
        .aux_slot_scan_rate          = 0U,
        .ext_src                     = false,
        .sequence_of_channel         = 0U,
        .sequence_of_aux_channel     = 0U,
        .sequence_of_alt_aux_channel = 0U,
    },
};


/*******************************************************************************
* Internal helpers
*******************************************************************************/

/* Per-group base pointers for the ATOP ADC slice-0 instances. */
static ATOPSS_ADC_TYPE* SelfTest_AtopAfeGetAdcBase(uint8_t adcGroup)
{
    ATOPSS_ADC_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_AFE_GROUP_0:
            base = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC;
            break;

        case ATOP_AFE_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_SLICE0_ADC;
            break;

        case ATOP_AFE_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_SLICE0_ADC;
            break;

        case ATOP_AFE_GROUP_3:
            base = PPCA_ATOPSS_ADC_GRP3_SLICE0_ADC;
            break;

        default:
            base = NULL;
            break;
    }

    return base;
}


/* Per-group base pointers for the ATOP AFE slice-0 instances. */
static ATOPSS_AFE_TYPE* SelfTest_AtopAfeGetAfeBase(uint8_t adcGroup)
{
    ATOPSS_AFE_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_AFE_GROUP_0:
            base = PPCA_ATOPSS_ADC_GRP0_SLICE0_AFE;
            break;

        case ATOP_AFE_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_SLICE0_AFE;
            break;

        case ATOP_AFE_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_SLICE0_AFE;
            break;

        case ATOP_AFE_GROUP_3:
            base = PPCA_ATOPSS_ADC_GRP3_SLICE0_AFE;
            break;

        default:
            base = NULL;
            break;
    }

    return base;
}


/* Absolute value of a 32-bit signed reading. */
static int32_t SelfTest_AtopAfeAbs(int32_t value)
{
    return (value < 0) ? -value : value;
}


/* True when |measured - expected| is within the percentage tolerance of
 * |expected| plus the fixed noise floor. */
static bool SelfTest_AtopAfeWithinTolerance(int32_t measured, int32_t expected,
                                            uint16_t tolerancePercent)
{
    int32_t band = ((SelfTest_AtopAfeAbs(expected) * (int32_t)tolerancePercent) / 100)
                   + ATOP_AFE_CODE_NOISE_FLOOR;

    return (SelfTest_AtopAfeAbs(measured - expected) <= band);
}


/* Convert the AFE differential channel 0 once and return the sign-extended
 * 12-bit differential code in *code. When afeEnable is true the AFE is applied
 * at the requested gain; when false the AFE block is left disabled so the raw
 * (un-amplified) differential is read. AREF is a single global block (in ADC
 * group 0) shared by all groups, so it is re-asserted here regardless of the
 * group under test. */
static uint8_t SelfTest_AtopAfeConvert(ATOPSS_ADC_TYPE* adcBase, ATOPSS_AFE_TYPE* afeBase,
                                       bool afeEnable, cy_en_afe_gain_t gain, int32_t* code)
{
    /* Only the AFE gain varies between the three reads; the ADC differential
     * channel 0 setup is the invariant stlAtopAfe_adcCfg kept in flash. */
    cy_stc_ppca_afe_config_t afeCfg =
    {
        .afe_gain      = gain,
        .afe_pwr       = CY_AFE_PWR_HFLG,
        .afe_clk_ratio = CY_AFE_CLK_RATIO_ATOPCLK_BY_4,
    };

    if (afeEnable)
    {
        Cy_PPCA_AFE_Init(afeBase, &afeCfg);
        Cy_PPCA_AFE_Enable(afeBase);
    }

    Cy_PPCA_ADC_Init(adcBase, &stlAtopAfe_adcCfg);
    Cy_PPCA_ADC_Enable(adcBase);

    /* Enabling the ADC returns the shared AREF analog control to its
     * power-on-reset (disabled) state, so re-assert it and let the reference
     * settle before converting. */
    Cy_PPCA_AREF_SetAnalogCtrl(PPCA_ATOPSS_ADC_GRP0_AREF, CY_AREF_FORCE_POR_1);
    Cy_SysLib_DelayUs(ATOP_AFE_AREF_SETTLE_US);

    uint8_t status = OK_STATUS;
    Cy_PPCA_ADC_Trigger(adcBase, ATOP_AFE_CH0_MASK);
    Cy_SysLib_DelayUs(ATOP_AFE_CONV_SETTLE_US);

    uint32_t timeout = ATOP_AFE_BUSY_POLL_TIMEOUT_US;
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_DelayUs(1U);
        timeout--;
    }

    if (0U == timeout)
    {
        status = MTB_STL_ERROR_TIMEOUT;
    }
    else
    {
        *code = (int32_t)Cy_PPCA_ADC_Read_SignExtended_ADC_Data(adcBase, 0U);
    }

    Cy_PPCA_ADC_Disable(adcBase);
    if (afeEnable)
    {
        Cy_PPCA_AFE_Disable(afeBase);
    }

    return status;
}


/* Evaluate the three differential readings against the expected AFE gains.
 * rawCode is the AFE-off differential; lowCode / highCode are the 8.25x and
 * 16.5x readings of the same input. */
static uint8_t SelfTest_AtopAfeEvaluate(const stl_atop_afe_config_t* config,
                                        int32_t rawCode, int32_t lowCode, int32_t highCode)
{
    uint8_t status = OK_STATUS;

    /* Stimulus must be present and inside the caller's expected window. */
    if ((rawCode < config->rawMin) || (rawCode > config->rawMax))
    {
        status = ERROR_STATUS;
    }
    /* Neither amplified reading may be saturated (input driven out of range or
     * the amplifier railed on its own offset). */
    else if ((SelfTest_AtopAfeAbs(lowCode) >= ATOP_AFE_SATURATION_CODE) ||
             (SelfTest_AtopAfeAbs(highCode) >= ATOP_AFE_SATURATION_CODE))
    {
        status = ERROR_STATUS;
    }
    else
    {
        /* Expected amplified codes derived from the measured raw code, so the
         * check follows the actual applied stimulus (source / temperature
         * drift moves raw and the expectations together). */
        int32_t expectedLow  = (rawCode * ATOP_AFE_GAIN_LOW_NUM) / ATOP_AFE_GAIN_LOW_DEN;
        int32_t expectedHigh = (rawCode * ATOP_AFE_GAIN_HIGH_NUM) / ATOP_AFE_GAIN_HIGH_DEN;

        bool lowOk   = SelfTest_AtopAfeWithinTolerance(lowCode, expectedLow,
                                                       config->gainTolerancePercent);
        bool highOk  = SelfTest_AtopAfeWithinTolerance(highCode, expectedHigh,
                                                       config->gainTolerancePercent);
        /* Gain ratio check (independent of the exact stimulus amplitude): the
         * 16.5x reading must be about twice the 8.25x reading. A stuck-gain
         * fault (ratio ~1.0) is rejected here. */
        bool ratioOk = SelfTest_AtopAfeWithinTolerance(highCode,
                                                       lowCode * ATOP_AFE_GAIN_RATIO,
                                                       config->gainTolerancePercent);

        if ((!lowOk) || (!highOk) || (!ratioOk))
        {
            status = ERROR_STATUS;
        }
    }

    return status;
}


/*******************************************************************************
* Function Name: SelfTest_ATOP_AFE
*******************************************************************************/
uint8_t SelfTest_ATOP_AFE(const stl_atop_afe_config_t* config)
{
    /* Validate the configuration up front; any unsupported field is a bad
     * parameter reported before any HW access. */
    if (NULL == config)
    {
        return ERROR_BAD_PARAM;
    }
    if ((config->adcGroup >= ATOP_AFE_GROUP_COUNT) ||
        (config->gainTolerancePercent == 0U) || (config->gainTolerancePercent > 100U) ||
        (config->rawMin >= config->rawMax))
    {
        return ERROR_BAD_PARAM;
    }

    ATOPSS_ADC_TYPE* adcBase = SelfTest_AtopAfeGetAdcBase(config->adcGroup);
    ATOPSS_AFE_TYPE* afeBase = SelfTest_AtopAfeGetAfeBase(config->adcGroup);

    if ((NULL == adcBase) || (NULL == afeBase))
    {
        return ERROR_BAD_PARAM;
    }

    /* Snapshot every ADC / AFE configuration register the test mutates so the
     * caller's converter setup can be restored bit-for-bit on return. */
    uint32_t savedAdcCtrl       = PPCA_ATOP_ADC_CTRL(adcBase);
    uint32_t savedAdcCnfg       = PPCA_ATOP_ADC_CNFG(adcBase);
    uint32_t savedAdcAuxCnfg    = PPCA_ATOP_ADC_AUX_CNFG(adcBase);
    uint32_t savedAdcCnvCnfg    = PPCA_ATOP_ADC_CNV_CNFG(adcBase);
    uint32_t savedAdcChCnfg0    = PPCA_ATOP_ADC_CH_CNFG0(adcBase);
    uint32_t savedAdcChCnfg1    = PPCA_ATOP_ADC_CH_CNFG1(adcBase);
    uint32_t savedAdcChCnfg2    = PPCA_ATOP_ADC_CH_CNFG2(adcBase);
    uint32_t savedAdcChCnfg3    = PPCA_ATOP_ADC_CH_CNFG3(adcBase);
    uint32_t savedAdcAuxCh0     = PPCA_ATOP_ADC_AUX_CH_CNFG0(adcBase);
    uint32_t savedAdcAuxCh1     = PPCA_ATOP_ADC_AUX_CH_CNFG1(adcBase);
    uint32_t savedAdcAltAuxCh   = PPCA_ATOP_ADC_ALT_AUX_CH_CNFG(adcBase);
    uint32_t savedAdcSign       = PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase);
    uint32_t savedAdcAuxSign    = PPCA_ATOP_ADC_AUX_SIGN_UNSIGN_CNFG(adcBase);
    uint32_t savedAdcAltAuxSign = PPCA_ATOP_ADC_AUX_ALT_SIGN_UNSIGN_CNFG(adcBase);
    uint32_t savedAfeCtl        = PPCA_ATOP_AFE_CTL(afeBase);
    uint32_t savedAfeCnfg       = PPCA_ATOP_AFE_CNFG(afeBase);

    /* Read the same differential three times: AFE off (raw), then 8.25x, then
     * 16.5x. Stop at the first conversion error. */
    int32_t rawCode  = 0;
    int32_t lowCode  = 0;
    int32_t highCode = 0;

    uint8_t status = SelfTest_AtopAfeConvert(adcBase, afeBase, false, CY_AFE_GAIN_0, &rawCode);
    if (OK_STATUS == status)
    {
        status = SelfTest_AtopAfeConvert(adcBase, afeBase, true, CY_AFE_GAIN_3, &lowCode);
    }
    if (OK_STATUS == status)
    {
        status = SelfTest_AtopAfeConvert(adcBase, afeBase, true, CY_AFE_GAIN_6, &highCode);
    }

    if (OK_STATUS == status)
    {
        #if (ERROR_IN_ATOP_AFE)
        /* Fault injection: corrupt the 16.5x reading beyond the tolerance so a
         * fault-free run is forced outside the acceptance window and reported
         * as a failure. */
        highCode = highCode + (highCode * (int32_t)config->gainTolerancePercent) / 100
                   + (2 * ATOP_AFE_CODE_NOISE_FLOOR) + 1;
        #endif

        status = SelfTest_AtopAfeEvaluate(config, rawCode, lowCode, highCode);
    }

    /* Restore the caller's ADC / AFE configuration. */
    PPCA_ATOP_ADC_AUX_ALT_SIGN_UNSIGN_CNFG(adcBase) = savedAdcAltAuxSign;
    PPCA_ATOP_ADC_AUX_SIGN_UNSIGN_CNFG(adcBase)     = savedAdcAuxSign;
    PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase)         = savedAdcSign;
    PPCA_ATOP_ADC_ALT_AUX_CH_CNFG(adcBase)          = savedAdcAltAuxCh;
    PPCA_ATOP_ADC_AUX_CH_CNFG1(adcBase)             = savedAdcAuxCh1;
    PPCA_ATOP_ADC_AUX_CH_CNFG0(adcBase)             = savedAdcAuxCh0;
    PPCA_ATOP_ADC_CH_CNFG3(adcBase)                 = savedAdcChCnfg3;
    PPCA_ATOP_ADC_CH_CNFG2(adcBase)                 = savedAdcChCnfg2;
    PPCA_ATOP_ADC_CH_CNFG1(adcBase)                 = savedAdcChCnfg1;
    PPCA_ATOP_ADC_CH_CNFG0(adcBase)                 = savedAdcChCnfg0;
    PPCA_ATOP_ADC_CNV_CNFG(adcBase)                 = savedAdcCnvCnfg;
    PPCA_ATOP_ADC_AUX_CNFG(adcBase)                 = savedAdcAuxCnfg;
    PPCA_ATOP_ADC_CNFG(adcBase)                     = savedAdcCnfg;
    PPCA_ATOP_ADC_CTRL(adcBase)                     = savedAdcCtrl;
    PPCA_ATOP_AFE_CNFG(afeBase)                     = savedAfeCnfg;
    PPCA_ATOP_AFE_CTL(afeBase)                      = savedAfeCtl;

    return status;
}


#endif /* defined(CY_IP_MXS40PPSS) */

/* [] END OF FILE */

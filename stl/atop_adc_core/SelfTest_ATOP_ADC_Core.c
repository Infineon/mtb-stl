/*******************************************************************************
* File Name: SelfTest_ATOP_ADC_Core.c
*
*
* Description:
*  This file provides the source code for the ATOP ADC Core (PPSS Analog
*  TOP Subsystem 12-bit SAR ADC) self tests.
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
#include "SelfTest_ATOP_ADC_Core.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

/*******************************************************************************
* Test reference values
*******************************************************************************/

/* ADC busy-poll timeout. Each loop iteration delays 1 us via
 * Cy_SysLib_DelayUs(1U), so this value is the timeout in microseconds and is
 * independent of the CPU clock frequency. 100 ms is a defensive upper bound;
 * an AUX conversion normally completes within microseconds. */
#define ATOP_ADC_CORE_BUSY_POLL_TIMEOUT_US      (100000U)

/* Channel 7 is multiplexed with the AUX inputs. It must be enabled in the
 * ADC channel mask and its data path routed to the AUX datapath in order to
 * convert an AUX slot. Derived from the AUX channel index so the two stay
 * in sync. */
#define ATOP_ADC_CORE_CH7_MASK                  ((uint16_t)((uint16_t)1U << ATOP_ADC_CORE_AUX_CHANNEL))

/* Settling time (microseconds) for the internal bandgap reference after the
 * AREF analog control is asserted. The AUX VREF reading is not valid until
 * the reference has settled. */
#define ATOP_ADC_CORE_AREF_SETTLE_US            (10000U)

/* Time (microseconds) allowed for one AUX scan to complete after the
 * conversion is triggered, before the busy status is polled. */
#define ATOP_ADC_CORE_CONV_SETTLE_US            (1000U)


/*******************************************************************************
* Internal helpers
*******************************************************************************/

/* Per-group base pointers for the ATOP ADC and AFE slice-0 instances.
 * The PPSS exposes one ADC + one AFE per group; the slice-0 base address
 * is the canonical handle accepted by the cy_ppca_adc PDL APIs. */
static ATOPSS_ADC_TYPE* SelfTest_AtopAdcGetBase(uint8_t adcGroup)
{
    ATOPSS_ADC_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_ADC_CORE_GROUP_0:
            base = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC;
            break;

        case ATOP_ADC_CORE_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_SLICE0_ADC;
            break;

        case ATOP_ADC_CORE_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_SLICE0_ADC;
            break;

        case ATOP_ADC_CORE_GROUP_3:
            base = PPCA_ATOPSS_ADC_GRP3_SLICE0_ADC;
            break;

        default:
            base = NULL;
            break;
    }

    return base;
}


static ATOPSS_AFE_TYPE* SelfTest_AtopAdcGetAfeBase(uint8_t adcGroup)
{
    ATOPSS_AFE_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_ADC_CORE_GROUP_0:
            base = PPCA_ATOPSS_ADC_GRP0_SLICE0_AFE;
            break;

        case ATOP_ADC_CORE_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_SLICE0_AFE;
            break;

        case ATOP_ADC_CORE_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_SLICE0_AFE;
            break;

        case ATOP_ADC_CORE_GROUP_3:
            base = PPCA_ATOPSS_ADC_GRP3_SLICE0_AFE;
            break;

        default:
            base = NULL;
            break;
    }

    return base;
}


/* Number of SAR single-ended channels available on an ADC group. Groups 0 and
 * 3 expose 8 channels (0..7); groups 1 and 2 expose 4 (0..3). */
static uint8_t SelfTest_AtopAdcChannelCount(uint8_t adcGroup)
{
    return ((ATOP_ADC_CORE_GROUP_0 == adcGroup) ||
            (ATOP_ADC_CORE_GROUP_3 == adcGroup))
           ? ATOP_ADC_CORE_CHANNEL_COUNT : ATOP_ADC_CORE_CHANNEL_COUNT_GRP12;
}


/*******************************************************************************
* Function Name: SelfTest_ATOP_ADC_Core
*******************************************************************************/
uint8_t SelfTest_ATOP_ADC_Core(const stl_atop_adc_core_config_t* config)
{
    /* Validate the configuration up front; any unsupported field is a bad
     * parameter reported before any HW access. */
    if (NULL == config)
    {
        return ERROR_BAD_PARAM;
    }
    if ((config->expectedCode > ATOP_ADC_CORE_MAX_CODE) || (0U == config->toleranceCode))
    {
        return ERROR_BAD_PARAM;
    }

    /* Select the datapath from the reference source and validate the group /
     * channel against what that source supports. Internal references live on
     * the GROUP0 AUX channel 7; an external (user-provided) reference is read
     * from a regular channel of any group. */
    bool    external       = false;
    uint8_t auxChannel     = (uint8_t)CY_AUX_CHANNEL_VREF;
    bool    validRefSource = true;
    switch (config->refSource)
    {
        case CY_STL_ATOP_ADC_REF_VREF_1V2:
            if ((ATOP_ADC_CORE_GROUP_0 != config->adcGroup) ||
                (ATOP_ADC_CORE_AUX_CHANNEL != config->channel))
            {
                return ERROR_BAD_PARAM;
            }
            external   = false;
            auxChannel = (uint8_t)CY_AUX_CHANNEL_VREF;
            break;

        case CY_STL_ATOP_ADC_REF_EXTERNAL:
            /* External reference: any group 0..3, on a regular channel that
             * exists on that group (groups 0/3 have 8 channels, groups 1/2
             * have 4). */
            if ((config->adcGroup >= ATOP_ADC_CORE_GROUP_COUNT) ||
                (config->channel >= SelfTest_AtopAdcChannelCount(config->adcGroup)))
            {
                return ERROR_BAD_PARAM;
            }
            external = true;
            break;

        default:
            validRefSource = false;
            break;
    }

    if (!validRefSource)
    {
        return ERROR_BAD_PARAM;
    }

    uint8_t adcGroup = config->adcGroup;

    ATOPSS_ADC_TYPE* adcBase = SelfTest_AtopAdcGetBase(adcGroup);
    ATOPSS_AFE_TYPE* afeBase = SelfTest_AtopAdcGetAfeBase(adcGroup);

    if ((NULL == adcBase) || (NULL == afeBase))
    {
        return ERROR_BAD_PARAM;
    }

    /* Snapshot every ADC / AFE configuration register the test mutates so
     * the caller's converter setup can be restored bit-for-bit on return. */
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

    /* Slot configuration. For an internal reference the AUX MUX is routed to
     * the selected source (auxChannel) and channel 7 is put on the AUX data
     * path. For an external reference channel 7 is a plain regular ADC channel
     * (CY_DATA_PATH_ADC7); the fields below are otherwise unused. */
    cy_stc_aux_slot_config_t auxSlot =
    {
        .aux_ch             = auxChannel,
        .aux_ch_gnd         = 0U,
        .alt_aux_ch         = 0U,
        .alt_aux_ch_gnd     = 0U,
        .aux_gnd_group2     = (cy_en_adc_vssa_t)0U,
        .alt_aux_gnd_group2 = (cy_en_adc_vssa_t)0U,
        .ch7_data_path      = external ? CY_DATA_PATH_ADC7 : CY_DATA_PATH_AUX,
        .gnd_group          = false,
    };

    /* Single conversion using a manual software trigger: one AUX slot for an
     * internal reference, or the selected regular channel for an external
     * reference. The sample-clock divider is set conservatively. */
    cy_stc_adc_conversion_config_t convCfg =
    {
        .group_channel               = false,
        .trigger_mode                = CY_ADC_MANUAL_TRIGGER,
        .auto_trigger_sample_clck    = 63U,
        .num_of_slot                 = 0U,
        .eos_src                     = false,
        .num_of_aux_slot             = external ? 0U : 1U,
        .num_of_alt_aux_slot         = 0U,
        .aux_slot_scan_rate          = 31U,
        .ext_src                     = false,
        .sequence_of_channel         = external ? (uint32_t)config->channel : 0U,
        .sequence_of_aux_channel     = external ? 0U : (uint32_t)auxChannel,
        .sequence_of_alt_aux_channel = 0U,
    };

    /* Internal reference: AUX-only conversion (regular channels disabled,
     * AUX slot path enabled). External reference: enable only the selected
     * regular channel, AUX slot disabled. */
    cy_stc_ppca_adc_config_t adcCfg =
    {
        .calib_gain_mode           = false,
        .aux_slot                  = external ? false : true,
        .alt_aux_slot              = false,
        .channels                  = external ? (uint16_t)((uint16_t)1U << config->channel) : ATOP_ADC_CORE_CH7_MASK,
        .channel_type              = 0U,
        .channel_data_type         = 0U,
        .aux_channel_data_type     = 0U,
        .alt_aux_channel_data_type = 0U,
        .aux_slot_config           = &auxSlot,
        .adc_conv_config           = convCfg,
    };

    /* AFE in pass-through (gain off), low-frequency-low-gain power class,
     * with the conservative ATOP_CLK / 8 ratio. The AFE is part of the
     * front-end datapath and must be enabled for the SAR to convert AUX
     * inputs even when no programmable gain is requested. */
    cy_stc_ppca_afe_config_t afeCfg =
    {
        .afe_gain      = CY_AFE_GAIN_0,
        .afe_pwr       = CY_AFE_PWR_HFLG,
        .afe_clk_ratio = CY_AFE_CLK_RATIO_ATOPCLK_BY_4,
    };

    /* Apply the test configuration. AREF is assumed to be already enabled
     * by the application (typical PPSS startup). If it is not, the
     * conversion will fail to complete and the busy-poll loop below will
     * time out, which is reported as ERROR_STATUS. */
    Cy_PPCA_AFE_Init(afeBase, &afeCfg);
    Cy_PPCA_ADC_Init(adcBase, &adcCfg);

    Cy_PPCA_AFE_Enable(afeBase);
    Cy_PPCA_ADC_Enable(adcBase);

    /* Enabling the ADC resets the AREF analog control, returning the internal
     * bandgap to its disabled power-on-reset state. Re-assert it so the analog
     * front end / VREF is driven during the conversion, then allow it to
     * settle. The AREF block is present only on group 0 (needed by the
     * internal VREF reference; harmless for an external group-0 channel). */
    if (ATOP_ADC_CORE_GROUP_0 == adcGroup)
    {
        Cy_PPCA_AREF_SetAnalogCtrl(PPCA_ATOPSS_ADC_GRP0_AREF, CY_AREF_FORCE_POR_1);
        Cy_SysLib_DelayUs(ATOP_ADC_CORE_AREF_SETTLE_US);
    }

    /* Trigger a single conversion: the AUX slot (internal reference) or the
     * selected regular channel (external reference). */
    uint8_t status = OK_STATUS;
    Cy_PPCA_ADC_Trigger(adcBase, external ? (uint16_t)((uint16_t)1U << config->channel) : 1U);

    /* Allow at least one full conversion scan to complete before sampling the
     * data register. */
    Cy_SysLib_DelayUs(ATOP_ADC_CORE_CONV_SETTLE_US);

    uint32_t timeout = ATOP_ADC_CORE_BUSY_POLL_TIMEOUT_US;
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_DelayUs(1U);
        timeout--;
    }

    if (0U == timeout)
    {
        status = ERROR_STATUS;
    }
    else
    {
        uint16_t code     = external
                            ? Cy_PPCA_ADC_Read_ADC_Data(adcBase, config->channel)
                            : Cy_PPCA_ADC_Read_AUX_ADC_Data(adcBase, (cy_en_aux_channel_t)auxChannel);
        uint16_t expected = config->expectedCode;
        uint16_t tol      = config->toleranceCode;
        uint16_t lower    = (expected > tol) ? (uint16_t)(expected - tol) : 0U;
        uint16_t upper    = (uint16_t)(expected + tol);

        #if (ERROR_IN_ATOP_ADC)
        /* Fault injection: deliberately corrupt the measured code by more than
         * the acceptance tolerance so a fault-free conversion is forced outside
         * the window and the comparison below reports a failure. */
        code = (uint16_t)((uint32_t)code + (2U * (uint32_t)tol) + 1U);
        #endif

        if ((code < lower) || (code > upper))
        {
            status = ERROR_STATUS;
        }
    }

    /* Disable the converter before writing the saved configuration back so
     * a partially-restored state cannot trigger a spurious conversion on
     * the application's channels. */
    Cy_PPCA_ADC_Disable(adcBase);
    Cy_PPCA_AFE_Disable(afeBase);

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

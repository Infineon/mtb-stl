/*******************************************************************************
* File Name: SelfTest_ATOP_ADC_Filter.c
*
* Description:
*  This file provides the source code for the ATOP ADC digital filter
*  (Median, LINTP, LP, CIC3, AVG, MIN/MAX) self tests according to Class B
*  library.
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
#include "SelfTest_ATOP_ADC_Filter.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

/*******************************************************************************
* Test parameters
*******************************************************************************/

/* ADC channel used to feed the filter under test, and its trigger bitmask. */
#define ATOP_ADC_FILTER_SRC_CH              (0U)
#define ATOP_ADC_FILTER_SRC_CH_MASK         ((uint16_t)((uint16_t)1U << ATOP_ADC_FILTER_SRC_CH))

/* ADC channel-sequence slot value used by the injection personality (mirrors
 * the proven CLB / ATOP DCMP ADC test-mode personality). */
#define ATOP_ADC_FILTER_SEQ_OF_CHANNEL      (16U)

/* Conversion busy-poll timeout in milliseconds (each poll delays 1 ms). */
#define ATOP_ADC_FILTER_CONV_TIMEOUT_MS     (100U)

/* Number of identical samples injected to fully load the 3-sample Median
 * window so the output equals the injected value regardless of window depth. */
#define ATOP_ADC_FILTER_MED_FILL_COUNT      (3U)

/* Gross-fault acceptance window (in codes) around the expected filter output. */
#define ATOP_ADC_FILTER_TOLERANCE           (50U)

/* ADC / filter data width mask (12-bit). The filter output registers sign-
* extend values whose bit 11 is set; mask to the unsigned 12-bit ADC width so
* the read-back can be compared against the injected unsigned test data. */
#define ATOP_ADC_FILTER_DATA_MASK           (0x0FFFU)

/* Two distinct injection values (12-bit range) used to prove the output tracks
 * the input and is not stuck at a constant. */
#define ATOP_ADC_FILTER_TEST_VAL_LOW        (500U)
#define ATOP_ADC_FILTER_TEST_VAL_HIGH       (3000U)

/* Low-Pass filter: number of constant samples injected before the (DC unity-
 * gain) LPF output is read. The LPF reaches its DC value quickly, so a small
 * count is sufficient. */
#define ATOP_ADC_FILTER_LPF_SETTLE_COUNT    (16U)
#define ATOP_ADC_FILTER_LPF_BANDWIDTH       (128U)

/* LPF/AVG outputs carry 4 fractional bits (value stored as code << 4), so shift
 * the read-back right by 4 to recover the integer 12-bit code (ArchTRM Fig 274). */
#define ATOP_ADC_FILTER_FRAC_SHIFT          (4U)

/* AVG filter: number of samples accumulated per average window. */
#define ATOP_ADC_FILTER_AVG_NUM_SAMPLES     (8U)

/* AVG filter: settle samples injected before the first averaging window to warm
 * the ADC / decimation pipeline. They are injected before Trigger_Start, so
 * they are not part of the averaged window; without them the first window is
 * skewed by cold-start samples when this sub-test runs standalone. */
#define ATOP_ADC_FILTER_AVG_SETTLE_COUNT    (16U)

/* CIC3: with scale 0 and decimation 8 the DC gain is 2048; CicToCode divides it
 * out. CIC_SAMPLES fills the 3rd-order pipeline before the output is read. */
#define ATOP_ADC_FILTER_CIC_DEC_FACTOR      (8U)
#define ATOP_ADC_FILTER_CIC_SCALE           (0U)
#define ATOP_ADC_FILTER_CIC_SAMPLES         (200U)

/* CIC3 DC gain for decimation 8, scale 0. */
#define ATOP_ADC_FILTER_CIC_GAIN            (2048)

/* MIN/MAX detection: the detector compares values as signed, so the varying
 * test sequence uses only values < 2048 (bit 11 clear) where the signed value
 * and the unsigned ADC code agree. The minimum injected is MIN_VAL and the
 * maximum is MAX_VAL. */
#define ATOP_ADC_FILTER_MINMAX_MIN_VAL      (500U)
#define ATOP_ADC_FILTER_MINMAX_MAX_VAL      (1800U)

/* LINTP configuration-register integrity: the LINTP interpolator's functional
 * path recovers the sub-sample phase between the free-running sample clock and
 * an independent hardware ADC_SOC trigger, which needs real continuous-sampling
 * timing and cannot be reproduced by digital test-mode injection. The LINTP
 * sub-test therefore verifies the integrity of the writable LIF_CNFG bits
 * (M_FACTOR [2:0] and INTP_EXTP_SEL [31]) with a walking write / read-back
 * pattern, catching stuck-at and addressing faults in the configuration register. */
#define ATOP_ADC_FILTER_LIF_CNFG_RW_MASK \
    (PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_M_FACTOR_Msk | \
     PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_INTP_EXTP_SEL_Msk)

/* Number of filter instances present in each ADC group (index = group). */
static const uint8_t stlAtopFilter_instPerGroup[ATOP_ADC_FILTER_GROUP_COUNT] =
{
    2U, /* GRP_0: ADC_FILT_00, ADC_FILT_01 */
    1U, /* GRP_1: ADC_FILT_10 */
    1U, /* GRP_2: ADC_FILT_20 */
    2U  /* GRP_3: ADC_FILT_30, ADC_FILT_31 */
};

/* ADC AUX slot placeholder. The AUX path is unused by the injection personality
 * (channel 7 stays on the ADC data path), but Cy_PPCA_ADC_Init dereferences
 * aux_slot_config, so a valid object must be provided. */
static cy_stc_aux_slot_config_t stlAtopFilter_adcAuxCfg =
{
    .aux_ch        = 0U,
    .aux_ch_gnd    = 0U,
    .alt_aux_ch    = 0U,
    .ch7_data_path = CY_DATA_PATH_ADC7,
};

/* ADC injection personality: single regular channel (0), arbitrary trigger, so
 * Cy_PPCA_ADC_Trigger() latches test-mode data into ADC_DATA[0]. */
static const cy_stc_ppca_adc_config_t stlAtopFilter_adcCfg =
{
    .calib_gain_mode                 = false,
    .aux_slot                        = false,
    .alt_aux_slot                    = false,
    .channels                        = ATOP_ADC_FILTER_SRC_CH_MASK,
    .channel_type                    = 0U,
    .channel_data_type               = 0U,
    .aux_channel_data_type           = 0U,
    .alt_aux_channel_data_type       = 0U,
    .aux_slot_config                 = &stlAtopFilter_adcAuxCfg,
    .adc_conv_config                 =
    {
        .group_channel               = false,
        .trigger_mode                = CY_ADC_ARBITRARY_TRIGGER,
        .auto_trigger_sample_clck    = 0U,
        .num_of_slot                 = 1U,
        .eos_src                     = false,
        .num_of_aux_slot             = 0U,
        .num_of_alt_aux_slot         = 0U,
        .aux_slot_scan_rate          = 0U,
        .ext_src                     = false,
        .sequence_of_channel         = ATOP_ADC_FILTER_SEQ_OF_CHANNEL,
        .sequence_of_aux_channel     = 0U,
        .sequence_of_alt_aux_channel = 0U,
    },
};

/* Filter personality for the Median sub-test: Median stage active, every other
 * stage bypassed. The unsigned path is selected because the injected test data
 * is unsigned. Sourced from ADC channel 0. */
static const cy_stc_ppca_adc_filter_config_t stlAtopFilter_medianCfg =
{
    .median_filter_enable          = true,
    .linear_filter_enable          = false,
    .average_filter_enable         = false,
    .cic_filter_enable             = false,
    .lpf_filter_enable             = false,
    .median_filter_bypass          = false,
    .linear_filter_bypass          = true,
    .average_filter_bypass         = true,
    .cic_filter_bypass             = true,
    .lpf_filter_bypass             = true,
    .unsign_path_enable            = true,
    .min_max_detection_enable      = false,
    .src_ch                        = ATOP_ADC_FILTER_SRC_CH,
    .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
    .lif_intp_extp_sel             = false,
    .lpf_bandwidth                 = 0U,
    .cic_src_type                  = false,
    .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
    .cic_decimation_factor         = 0U,
    .cic_decimation_offset         = 0U,
    .cic_decimation_scale          = 0U,
    .cic_digital_comparator_enable = false,
    .cic_UB_threshold              = 0U,
    .cic_LB_threshold              = 0U,
    .avg_no_of_samples             = 1U,
    .avg_linear_or_squared         = false,
    .avg_fix_or_dynamic_sample     = false,
    .min_max_source                = false,
    .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
    .src_adc                       = false,
};

/* Filter personality for the Low-Pass sub-test: the Median stage is active as a
 * transparent pass-through to clock the filter pipeline, the LPF stage is active,
 * and all other stages are bypassed. For a constant DC input the settled LPF
 * output equals the input (DC gain 1). */
static const cy_stc_ppca_adc_filter_config_t stlAtopFilter_lpfCfg =
{
    .median_filter_enable          = true,
    .linear_filter_enable          = false,
    .average_filter_enable         = false,
    .cic_filter_enable             = false,
    .lpf_filter_enable             = true,
    .median_filter_bypass          = false,
    .linear_filter_bypass          = true,
    .average_filter_bypass         = true,
    .cic_filter_bypass             = true,
    .lpf_filter_bypass             = false,
    .unsign_path_enable            = true,
    .min_max_detection_enable      = false,
    .src_ch                        = ATOP_ADC_FILTER_SRC_CH,
    .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
    .lif_intp_extp_sel             = false,
    .lpf_bandwidth                 = ATOP_ADC_FILTER_LPF_BANDWIDTH,
    .cic_src_type                  = false,
    .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
    .cic_decimation_factor         = 0U,
    .cic_decimation_offset         = 0U,
    .cic_decimation_scale          = 0U,
    .cic_digital_comparator_enable = false,
    .cic_UB_threshold              = 0U,
    .cic_LB_threshold              = 0U,
    .avg_no_of_samples             = 1U,
    .avg_linear_or_squared         = false,
    .avg_fix_or_dynamic_sample     = false,
    .min_max_source                = false,
    .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
    .src_adc                       = false,
};

/* Filter personality for the Average (AVG) sub-test: only the AVG stage active,
 * fixed-window linear average of N samples. For a constant DC input the average
 * equals the input. */
static const cy_stc_ppca_adc_filter_config_t stlAtopFilter_avgCfg =
{
    .median_filter_enable          = false,
    .linear_filter_enable          = false,
    .average_filter_enable         = true,
    .cic_filter_enable             = false,
    .lpf_filter_enable             = false,
    .median_filter_bypass          = true,
    .linear_filter_bypass          = true,
    .average_filter_bypass         = false,
    .cic_filter_bypass             = true,
    .lpf_filter_bypass             = true,
    .unsign_path_enable            = true,
    .min_max_detection_enable      = false,
    .src_ch                        = ATOP_ADC_FILTER_SRC_CH,
    .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
    .lif_intp_extp_sel             = false,
    .lpf_bandwidth                 = 0U,
    .cic_src_type                  = false,
    .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
    .cic_decimation_factor         = 0U,
    .cic_decimation_offset         = 0U,
    .cic_decimation_scale          = 0U,
    .cic_digital_comparator_enable = false,
    .cic_UB_threshold              = 0U,
    .cic_LB_threshold              = 0U,
    .avg_no_of_samples             = ATOP_ADC_FILTER_AVG_NUM_SAMPLES,
    .avg_linear_or_squared         = false,
    .avg_fix_or_dynamic_sample     = false,
    .min_max_source                = false,
    .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
    .src_adc                       = false,
};

/* Filter personality for the CIC3 sub-test: the Median stage is active as a
 * transparent pass-through (pipeline clock) feeding the internal CIC3 stage;
 * all other stages are bypassed. */
static const cy_stc_ppca_adc_filter_config_t stlAtopFilter_cicCfg =
{
    .median_filter_enable          = true,
    .linear_filter_enable          = false,
    .average_filter_enable         = false,
    .cic_filter_enable             = true,
    .lpf_filter_enable             = false,
    .median_filter_bypass          = false,
    .linear_filter_bypass          = true,
    .average_filter_bypass         = true,
    .cic_filter_bypass             = false,
    .lpf_filter_bypass             = true,
    .unsign_path_enable            = true,
    .min_max_detection_enable      = false,
    .src_ch                        = ATOP_ADC_FILTER_SRC_CH,
    .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
    .lif_intp_extp_sel             = false,
    .lpf_bandwidth                 = 0U,
    .cic_src_type                  = false,
    .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
    .cic_decimation_factor         = ATOP_ADC_FILTER_CIC_DEC_FACTOR,
    .cic_decimation_offset         = 0U,
    .cic_decimation_scale          = ATOP_ADC_FILTER_CIC_SCALE,
    .cic_digital_comparator_enable = false,
    .cic_UB_threshold              = 0U,
    .cic_LB_threshold              = 0U,
    .avg_no_of_samples             = 1U,
    .avg_linear_or_squared         = false,
    .avg_fix_or_dynamic_sample     = false,
    .min_max_source                = false,
    .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
    .src_adc                       = false,
};

/* Filter personality for the MIN/MAX sub-test: MIN/MAX detection active on the
 * Median pass-through output, open search window (accumulates until read). */
static const cy_stc_ppca_adc_filter_config_t stlAtopFilter_minmaxCfg =
{
    .median_filter_enable          = true,
    .linear_filter_enable          = false,
    .average_filter_enable         = false,
    .cic_filter_enable             = false,
    .lpf_filter_enable             = false,
    .median_filter_bypass          = false,
    .linear_filter_bypass          = true,
    .average_filter_bypass         = true,
    .cic_filter_bypass             = true,
    .lpf_filter_bypass             = true,
    .unsign_path_enable            = true,
    .min_max_detection_enable      = true,
    .src_ch                        = ATOP_ADC_FILTER_SRC_CH,
    .lif_m_factor                  = CY_LIF_MFACTOR_SET_TO_2,
    .lif_intp_extp_sel             = false,
    .lpf_bandwidth                 = 0U,
    .cic_src_type                  = false,
    .cic_clck_div                  = CY_CIC_PPCACLK_DIV_BY_4,
    .cic_decimation_factor         = 0U,
    .cic_decimation_offset         = 0U,
    .cic_decimation_scale          = 0U,
    .cic_digital_comparator_enable = false,
    .cic_UB_threshold              = 0U,
    .cic_LB_threshold              = 0U,
    .avg_no_of_samples             = 1U,
    .avg_linear_or_squared         = false,
    .avg_fix_or_dynamic_sample     = false,
    .min_max_source                = false,
    .min_max_sample_search         = CY_SAMPLE_SEARCH_WINDOW_OPEN,
    .src_adc                       = false,
};


/*******************************************************************************
* Private helpers
*******************************************************************************/

/* Resolve the ADC slice-0 base pointer for a group. */
static ATOPSS_ADC_TYPE* GetAdcBase(uint8_t adcGroup)
{
    ATOPSS_ADC_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_ADC_FILTER_GROUP_0:
            base = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC; break;

        case ATOP_ADC_FILTER_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_SLICE0_ADC; break;

        case ATOP_ADC_FILTER_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_SLICE0_ADC; break;

        case ATOP_ADC_FILTER_GROUP_3:
            base = PPCA_ATOPSS_ADC_GRP3_SLICE0_ADC; break;

        default:
            base = NULL; break;
    }

    return base;
}


/* Resolve the ADC filter base pointer for a (group, instance) pair. */
static ATOPSS_ADCFLT_TYPE* GetFilterBase(uint8_t adcGroup, uint8_t filterInst)
{
    ATOPSS_ADCFLT_TYPE* base;

    switch (adcGroup)
    {
        case ATOP_ADC_FILTER_GROUP_0:
            base = (0U == filterInst) ? PPCA_ATOPSS_ADC_GRP0_AFLT0 : PPCA_ATOPSS_ADC_GRP0_AFLT1;
            break;

        case ATOP_ADC_FILTER_GROUP_1:
            base = PPCA_ATOPSS_ADC_GRP1_AFLT0;
            break;

        case ATOP_ADC_FILTER_GROUP_2:
            base = PPCA_ATOPSS_ADC_GRP2_AFLT0;
            break;

        case ATOP_ADC_FILTER_GROUP_3:
            base = (0U == filterInst) ? PPCA_ATOPSS_ADC_GRP3_AFLT0 : PPCA_ATOPSS_ADC_GRP3_AFLT1;
            break;

        default:
            base = NULL;
            break;
    }

    return base;
}


/* Inject one test-mode sample on channel 0 and wait for the conversion to
 * complete. Returns OK_STATUS if the ADC became idle within the timeout. */
static uint8_t InjectSample(ATOPSS_ADC_TYPE* adcBase, uint16_t value)
{
    uint32_t timeout = ATOP_ADC_FILTER_CONV_TIMEOUT_MS;

    Cy_PPCA_ADC_Write_Test_data(adcBase, value);
    Cy_PPCA_ADC_Trigger(adcBase, ATOP_ADC_FILTER_SRC_CH_MASK);
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_Delay(1U);
        timeout--;
    }

    return (0U == timeout) ? ERROR_STATUS : OK_STATUS;
}


/* Inject count identical test-mode samples on channel 0. Returns OK_STATUS if
 * every conversion completed within the timeout. */
static uint8_t InjectConstant(ATOPSS_ADC_TYPE* adcBase, uint16_t value, uint8_t count)
{
    uint8_t status = OK_STATUS;

    for (uint8_t i = 0U; i < count; i++)
    {
        status = InjectSample(adcBase, value);
        if (OK_STATUS != status)
        {
            break;
        }
    }

    return status;
}


/* Absolute-difference within tolerance check. */
static bool WithinTolerance(uint16_t measured, uint16_t expected)
{
    uint16_t diff = (measured > expected) ? (uint16_t)(measured - expected)
                                          : (uint16_t)(expected - measured);
    return (diff <= ATOP_ADC_FILTER_TOLERANCE);
}


/*******************************************************************************
* Function Name: RunMedianTest
********************************************************************************
*
* Median filter sub-test on one filter instance. Assumes the group ADC is
* already brought up in test-injection mode. Configures the filter with the
* Median stage active and all other stages bypassed, injects two distinct
* values, and verifies that the Median output tracks each injected value within
* tolerance (catching stuck-at and broken-datapath faults). Saves and restores
* the filter CTL / CNFG registers it modifies.
*
* \return OK_STATUS on pass, ERROR_STATUS on failure or conversion timeout.
*
*******************************************************************************/
static uint8_t RunMedianTest(ATOPSS_ADC_TYPE* adcBase, ATOPSS_ADCFLT_TYPE* afltBase)
{
    uint8_t  status  = OK_STATUS;
    uint16_t outLow  = 0U;
    uint16_t outHigh = 0U;

    uint32_t savedAfltCtl  = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg = PPCA_ATOP_AFLT_CNFG(afltBase);

    Cy_PPCA_ADC_Filter_Init(afltBase, &stlAtopFilter_medianCfg);

    status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_LOW, ATOP_ADC_FILTER_MED_FILL_COUNT);
    if (OK_STATUS == status)
    {
        outLow = (uint16_t)(Cy_PPCA_ADC_Filter_Median_Output(afltBase) & ATOP_ADC_FILTER_DATA_MASK);
        status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_HIGH, ATOP_ADC_FILTER_MED_FILL_COUNT);
    }
    if (OK_STATUS == status)
    {
        outHigh = (uint16_t)(Cy_PPCA_ADC_Filter_Median_Output(afltBase) & ATOP_ADC_FILTER_DATA_MASK);
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a broken filter datapath: force the high reading to the low value
     * so both the tracking and the tolerance checks below fail. */
    outHigh = outLow;
    #endif

    if (OK_STATUS == status)
    {
        bool tracks = (outHigh > outLow);
        bool lowOk  = WithinTolerance(outLow, ATOP_ADC_FILTER_TEST_VAL_LOW);
        bool highOk = WithinTolerance(outHigh, ATOP_ADC_FILTER_TEST_VAL_HIGH);
        if (!(tracks && lowOk && highOk))
        {
            status = ERROR_STATUS;
        }
    }

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_CTL(afltBase)  = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase) = savedAfltCnfg;

    return status;
}


/*******************************************************************************
* Function Name: RunLpfTest
********************************************************************************
*
* Low-Pass filter sub-test on one filter instance. Injects two distinct constant
* DC levels, letting the IIR settle to each, and verifies the settled LPF output
* tracks each level within tolerance (LPF DC gain is 1). Saves and restores the
* filter CTL / CNFG / LPF_ALFA registers it modifies.
*
* \return OK_STATUS on pass, ERROR_STATUS on failure or conversion timeout.
*
*******************************************************************************/
static uint8_t RunLpfTest(ATOPSS_ADC_TYPE* adcBase, ATOPSS_ADCFLT_TYPE* afltBase)
{
    uint8_t  status  = OK_STATUS;
    uint16_t outLow  = 0U;
    uint16_t outHigh = 0U;

    uint32_t savedAfltCtl     = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg    = PPCA_ATOP_AFLT_CNFG(afltBase);
    uint32_t savedAfltLpfAlfa = PPCA_ATOP_AFLT_LPF_ALFA(afltBase);

    Cy_PPCA_ADC_Filter_Init(afltBase, &stlAtopFilter_lpfCfg);

    status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_LOW, ATOP_ADC_FILTER_LPF_SETTLE_COUNT);
    if (OK_STATUS == status)
    {
        outLow = (uint16_t)(Cy_PPCA_ADC_Filter_LPF_Output(afltBase) >> ATOP_ADC_FILTER_FRAC_SHIFT);
        status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_HIGH, ATOP_ADC_FILTER_LPF_SETTLE_COUNT);
    }
    if (OK_STATUS == status)
    {
        outHigh = (uint16_t)(Cy_PPCA_ADC_Filter_LPF_Output(afltBase) >> ATOP_ADC_FILTER_FRAC_SHIFT);
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a broken filter datapath: force the high reading to the low value
     * so both the tracking and the tolerance checks below fail. */
    outHigh = outLow;
    #endif

    if (OK_STATUS == status)
    {
        bool tracks = (outHigh > outLow);
        bool lowOk  = WithinTolerance(outLow, ATOP_ADC_FILTER_TEST_VAL_LOW);
        bool highOk = WithinTolerance(outHigh, ATOP_ADC_FILTER_TEST_VAL_HIGH);
        if (!(tracks && lowOk && highOk))
        {
            status = ERROR_STATUS;
        }
    }

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_LPF_ALFA(afltBase) = savedAfltLpfAlfa;
    PPCA_ATOP_AFLT_CTL(afltBase)      = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase)     = savedAfltCnfg;

    return status;
}


/* Recover the unsigned 12-bit code from a raw AVG reading. The AVG output is the
 * signed sum of N samples, each stored as code << 4; divide by N, shift right by
 * 4, and mask to 12 bits. */
static uint16_t AvgToCode(uint32_t avgRaw)
{
    return (uint16_t)(((avgRaw / ATOP_ADC_FILTER_AVG_NUM_SAMPLES) >> ATOP_ADC_FILTER_FRAC_SHIFT)
                      & ATOP_ADC_FILTER_DATA_MASK);
}


/* Recover the unsigned 12-bit code from a raw CIC3 output. The CIC3 output is
 * the signed input scaled by the DC gain (4 * DEC^3); dividing the signed value
 * by that gain and masking to 12 bits recovers the unsigned ADC code. */
static uint16_t CicToCode(uint32_t cicRaw)
{
    int32_t value = (int32_t)cicRaw / (int32_t)ATOP_ADC_FILTER_CIC_GAIN;
    return (uint16_t)((uint32_t)value & ATOP_ADC_FILTER_DATA_MASK);
}


/*******************************************************************************
* Function Name: RunAvgTest
********************************************************************************
*
* Average (AVG) filter sub-test on one filter instance. Starts a fresh averaging
* window, injects N constant samples and verifies the recovered average tracks
* the injected value within tolerance. Saves and restores the filter registers
* it modifies.
*
* The AVG output register is the signed sum of N samples, each stored with 4
* fractional bits; \ref AvgToCode recovers the unsigned 12-bit code.
*
* \return OK_STATUS on pass, ERROR_STATUS on failure or conversion timeout.
*
*******************************************************************************/
static uint8_t RunAvgTest(ATOPSS_ADC_TYPE* adcBase, ATOPSS_ADCFLT_TYPE* afltBase)
{
    uint8_t  status  = OK_STATUS;
    uint16_t outLow  = 0U;
    uint16_t outHigh = 0U;

    uint32_t savedAfltCtl     = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg    = PPCA_ATOP_AFLT_CNFG(afltBase);
    uint32_t savedAfltAvgCnfg = PPCA_ATOP_AFLT_AVGF_CNFG(afltBase);

    Cy_PPCA_ADC_Filter_Init(afltBase, &stlAtopFilter_avgCfg);

    /* Warm the ADC / decimation pipeline before opening the first averaging
     * window. These settle samples are injected before Trigger_Start, so they
     * are not part of the averaged window; without them a cold-start first
     * window (e.g. when this sub-test runs standalone) is skewed and fails the
     * tolerance check. The subsequent HIGH window is primed by the preceding
     * LOW window. */
    status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_LOW, ATOP_ADC_FILTER_AVG_SETTLE_COUNT);

    if (OK_STATUS == status)
    {
        Cy_PPCA_ADC_AVG_Filter_Trigger_Start(afltBase);
        status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_LOW, ATOP_ADC_FILTER_AVG_NUM_SAMPLES);
    }
    if (OK_STATUS == status)
    {
        outLow = AvgToCode(Cy_PPCA_ADC_Filter_AVG_Output(afltBase));
        Cy_PPCA_ADC_AVG_Filter_Trigger_Start(afltBase);
        status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_HIGH, ATOP_ADC_FILTER_AVG_NUM_SAMPLES);
    }
    if (OK_STATUS == status)
    {
        outHigh = AvgToCode(Cy_PPCA_ADC_Filter_AVG_Output(afltBase));
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a broken filter datapath: force the high reading to the low value
     * so both the tracking and the tolerance checks below fail. */
    outHigh = outLow;
    #endif

    if (OK_STATUS == status)
    {
        bool tracks = (outHigh > outLow);
        bool lowOk  = WithinTolerance(outLow, ATOP_ADC_FILTER_TEST_VAL_LOW);
        bool highOk = WithinTolerance(outHigh, ATOP_ADC_FILTER_TEST_VAL_HIGH);
        if (!(tracks && lowOk && highOk))
        {
            status = ERROR_STATUS;
        }
    }

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_AVGF_CNFG(afltBase) = savedAfltAvgCnfg;
    PPCA_ATOP_AFLT_CTL(afltBase)       = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase)      = savedAfltCnfg;

    return status;
}


/*******************************************************************************
* Function Name: RunCicTest
********************************************************************************
*
* CIC3 filter sub-test on one filter instance. Injects two distinct constant DC
* levels, lets the 3rd-order pipeline settle, and verifies the recovered output
* tracks each level within tolerance. The CIC3 output is the signed input scaled
* by the DC gain (4 * DEC^3); \ref CicToCode recovers the unsigned 12-bit code.
* Saves and restores the filter registers it modifies.
*
* \return OK_STATUS on pass, ERROR_STATUS on failure or conversion timeout.
*
*******************************************************************************/
static uint8_t RunCicTest(ATOPSS_ADC_TYPE* adcBase, ATOPSS_ADCFLT_TYPE* afltBase)
{
    uint8_t  status  = OK_STATUS;
    uint16_t outLow  = 0U;
    uint16_t outHigh = 0U;

    uint32_t savedAfltCtl  = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg = PPCA_ATOP_AFLT_CNFG(afltBase);
    uint32_t savedCicCnfg  = PPCA_ATOP_AFLT_CICF_CNFG(afltBase);

    Cy_PPCA_ADC_Filter_Init(afltBase, &stlAtopFilter_cicCfg);

    status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_LOW, ATOP_ADC_FILTER_CIC_SAMPLES);
    if (OK_STATUS == status)
    {
        outLow = CicToCode(Cy_PPCA_ADC_Filter_CIC_Output(afltBase));
        status = InjectConstant(adcBase, ATOP_ADC_FILTER_TEST_VAL_HIGH, ATOP_ADC_FILTER_CIC_SAMPLES);
    }
    if (OK_STATUS == status)
    {
        outHigh = CicToCode(Cy_PPCA_ADC_Filter_CIC_Output(afltBase));
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a broken filter datapath: force the high reading to the low value
     * so both the tracking and the tolerance checks below fail. */
    outHigh = outLow;
    #endif

    if (OK_STATUS == status)
    {
        bool tracks = (outHigh > outLow);
        bool lowOk  = WithinTolerance(outLow, ATOP_ADC_FILTER_TEST_VAL_LOW);
        bool highOk = WithinTolerance(outHigh, ATOP_ADC_FILTER_TEST_VAL_HIGH);
        if (!(tracks && lowOk && highOk))
        {
            status = ERROR_STATUS;
        }
    }

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_CICF_CNFG(afltBase) = savedCicCnfg;
    PPCA_ATOP_AFLT_CTL(afltBase)       = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase)      = savedAfltCnfg;

    return status;
}


/*******************************************************************************
* Function Name: RunMinMaxTest
********************************************************************************
*
* MIN/MAX detection sub-test on one filter instance. Injects a known varying
* sample sequence and verifies the detected minimum and maximum match the known
* smallest and largest injected values within tolerance (and are distinct, to
* catch a stuck detector). Saves and restores the filter registers it modifies.
*
* \return OK_STATUS on pass, ERROR_STATUS on failure or conversion timeout.
*
*******************************************************************************/
static uint8_t RunMinMaxTest(ATOPSS_ADC_TYPE* adcBase, ATOPSS_ADCFLT_TYPE* afltBase)
{
    static const uint16_t sequence[] =
    {
        1000U,
        ATOP_ADC_FILTER_MINMAX_MIN_VAL,
        ATOP_ADC_FILTER_MINMAX_MAX_VAL,
        1200U
    };

    uint8_t  status = OK_STATUS;
    uint16_t minVal = 0U;
    uint16_t maxVal = 0U;

    uint32_t savedAfltCtl  = PPCA_ATOP_AFLT_CTL(afltBase);
    uint32_t savedAfltCnfg = PPCA_ATOP_AFLT_CNFG(afltBase);

    Cy_PPCA_ADC_Filter_Init(afltBase, &stlAtopFilter_minmaxCfg);

    /* Reading ADC_MIN_MAX starts / restarts the search window; prime it with a
     * throw-away read so the detector accumulates the injected sequence below. */
    (void)Cy_PPCA_ADC_Filter_MINMAX_Get_MIN_Value(afltBase);

    for (uint8_t i = 0U; i < (sizeof(sequence) / sizeof(sequence[0])); i++)
    {
        status = InjectSample(adcBase, sequence[i]);
        if (OK_STATUS != status)
        {
            break;
        }
    }
    if (OK_STATUS == status)
    {
        minVal = (uint16_t)(Cy_PPCA_ADC_Filter_MINMAX_Get_MIN_Value(afltBase) & ATOP_ADC_FILTER_DATA_MASK);
        maxVal = (uint16_t)(Cy_PPCA_ADC_Filter_MINMAX_Get_MAX_Value(afltBase) & ATOP_ADC_FILTER_DATA_MASK);
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a broken detector: force min = max so the checks below fail. */
    maxVal = minVal;
    #endif

    if (OK_STATUS == status)
    {
        bool minOk    = WithinTolerance(minVal, ATOP_ADC_FILTER_MINMAX_MIN_VAL);
        bool maxOk    = WithinTolerance(maxVal, ATOP_ADC_FILTER_MINMAX_MAX_VAL);
        bool distinct = (maxVal > minVal);
        if (!(minOk && maxOk && distinct))
        {
            status = ERROR_STATUS;
        }
    }

    Cy_PPCA_ADC_Filter_DeInit(afltBase);
    PPCA_ATOP_AFLT_CTL(afltBase)  = savedAfltCtl;
    PPCA_ATOP_AFLT_CNFG(afltBase) = savedAfltCnfg;

    return status;
}


/*******************************************************************************
* Function Name: RunLintpTest
********************************************************************************
*
* Linear-interpolation (LINTP) filter sub-test on one filter instance. The LINTP
* interpolator recovers the sub-sample phase between the free-running sample
* clock and an independent hardware ADC_SOC trigger, so its functional output
* only appears under real continuous-sampling timing and cannot be produced by
* the digital test-mode injection used by the other sub-tests. This sub-test
* instead checks the integrity of the writable LINTP configuration register
* (LIF_CNFG: M_FACTOR and INTP_EXTP_SEL) with a walking write / read-back
* pattern, catching stuck-at and addressing faults in that register. The
* register is saved and restored.
*
* \return OK_STATUS on pass, ERROR_STATUS on a read-back mismatch.
*
*******************************************************************************/
static uint8_t RunLintpTest(ATOPSS_ADCFLT_TYPE* afltBase)
{
    /* Walking patterns over the writable LIF_CNFG bits: all-0, all-1, each field
     * isolated, and alternating M_FACTOR codes (101b / 010b) so every writable
     * bit is toggled both ways. */
    static const uint32_t patterns[] =
    {
        0x00000000U,
        ATOP_ADC_FILTER_LIF_CNFG_RW_MASK,
        PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_M_FACTOR_Msk,
        PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_INTP_EXTP_SEL_Msk,
        ((uint32_t)CY_LIF_MFACTOR_SET_TO_64 << PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_M_FACTOR_Pos),
        ((uint32_t)CY_LIF_MFACTOR_SET_TO_8  << PPCA_ATOPSS_ADC_GRP_AFLT_LIF_CNFG_M_FACTOR_Pos)
    };

    uint8_t  status       = OK_STATUS;
    uint32_t savedLifCnfg = PPCA_ATOP_AFLT_LIF_CNFG(afltBase);

    for (uint8_t i = 0U; i < (sizeof(patterns) / sizeof(patterns[0])); i++)
    {
        PPCA_ATOP_AFLT_LIF_CNFG(afltBase) = patterns[i];
        uint32_t readBack = PPCA_ATOP_AFLT_LIF_CNFG(afltBase) & ATOP_ADC_FILTER_LIF_CNFG_RW_MASK;
        if (readBack != (patterns[i] & ATOP_ADC_FILTER_LIF_CNFG_RW_MASK))
        {
            status = ERROR_STATUS;
            break;
        }
    }

    #if defined(ERROR_IN_ATOP_ADC_FILTER) && (ERROR_IN_ATOP_ADC_FILTER != 0)
    /* Emulate a stuck configuration register so the read-back check fails. */
    status = ERROR_STATUS;
    #endif

    PPCA_ATOP_AFLT_LIF_CNFG(afltBase) = savedLifCnfg;

    return status;
}


/*******************************************************************************
* Function Name: SelfTest_ATOP_ADC_Filter
********************************************************************************
*
* Performs a self-test of the ATOP ADC digital filter chain for the given ADC
* group by running the selected filter-type sub-tests on each filter instance
* of that group.
*
* The group ADC is brought up once in test-injection mode (mirroring the proven
* CLB Test-D personality); each filter instance is then exercised and the full
* ADC / PPCA / filter register state is restored on return so the test can be
* interleaved with normal use of the other ADC groups.
*
* \note All six filter-type sub-tests are implemented. The Median, Low-Pass (LP),
* Average (AVG), CIC3 and MIN/MAX stages are exercised functionally via ADC
* test-mode injection. The Linear Interpolation (LINTP) stage recovers the
* sub-sample phase between the sample clock and an independent hardware ADC_SOC
* trigger, which digital injection cannot reproduce, so it is covered by a
* configuration-register integrity check (LIF_CNFG write / read-back).
*
* \param adcGroup  ADC group selector (ATOP_ADC_FILTER_GROUP_0 .. _GROUP_3).
*
* \param testMask  Bit mask selecting which filter types to run
* (ATOP_ADC_FILTER_TEST_MEDIAN .. _MINMAX, or _ALL). Bits outside the valid
* set are ignored; a mask that selects no valid type returns ERROR_BAD_PARAM.
*
* \return OK_STATUS / ERROR_STATUS / ERROR_BAD_PARAM.
*
*******************************************************************************/
uint8_t SelfTest_ATOP_ADC_Filter(uint8_t adcGroup, uint8_t testMask)
{
    if (adcGroup >= ATOP_ADC_FILTER_GROUP_COUNT)
    {
        return ERROR_BAD_PARAM;
    }

    /* Reject an empty selection so a call never reports success without running
     * at least one sub-test. */
    if ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_ALL) == 0u)
    {
        return ERROR_BAD_PARAM;
    }

    ATOPSS_ADC_TYPE* adcBase = GetAdcBase(adcGroup);
    if (NULL == adcBase)
    {
        return ERROR_BAD_PARAM;
    }

    /* --- Save every group-level register the test modifies --- */
    uint32_t savedPpcaCtrl      = PPCA_CNFG_CTRL(PPCA_CNFG);
    uint32_t savedAdcCnfg       = PPCA_ATOP_ADC_CNFG(adcBase);
    uint32_t savedAdcCtl        = PPCA_ATOP_ADC_CTRL(adcBase);
    uint32_t savedAdcCnvCnfg    = PPCA_ATOP_ADC_CNV_CNFG(adcBase);
    uint32_t savedAdcChCnfg0    = PPCA_ATOP_ADC_CH_CNFG0(adcBase);
    uint32_t savedAdcChCnfg1    = PPCA_ATOP_ADC_CH_CNFG1(adcBase);
    uint32_t savedAdcSignUnsign = PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase);

    /* --- Bring up the analog subsystem and the ADC injection personality --- */
    Cy_PPCA_Enable(PPCA_CNFG);
    if (ATOP_ADC_FILTER_GROUP_0 == adcGroup)
    {
        Cy_PPCA_AREF_Enable(PPCA_ATOPSS_ADC_GRP0_AREF);
    }
    Cy_PPCA_ADC_Init(adcBase, &stlAtopFilter_adcCfg);
    Cy_PPCA_ADC_Enable(adcBase);
    Cy_PPCA_ADC_Set_Test_mode(adcBase, true);
    Cy_PPCA_ADC_Set_Calib_Gain_mode(adcBase, false);

    /* --- Run the implemented filter-type sub-tests on each instance --- */
    uint8_t status    = OK_STATUS;
    uint8_t instCount = stlAtopFilter_instPerGroup[adcGroup];
    for (uint8_t inst = 0U; inst < instCount; inst++)
    {
        ATOPSS_ADCFLT_TYPE* afltBase = GetFilterBase(adcGroup, inst);
        if (NULL == afltBase)
        {
            status = ERROR_BAD_PARAM;
            break;
        }
        /* Median / LP / AVG / CIC3 / MIN-MAX are exercised functionally by
         * injection; LINTP is covered by a config-register integrity check.
         * Each sub-test runs only when its bit is set in testMask. */
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_MEDIAN) != 0u))
        {
            status = RunMedianTest(adcBase, afltBase);
        }
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_LPF) != 0u))
        {
            status = RunLpfTest(adcBase, afltBase);
        }
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_AVG) != 0u))
        {
            status = RunAvgTest(adcBase, afltBase);
        }
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_CIC3) != 0u))
        {
            status = RunCicTest(adcBase, afltBase);
        }
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_MINMAX) != 0u))
        {
            status = RunMinMaxTest(adcBase, afltBase);
        }
        if ((OK_STATUS == status) && ((testMask & (uint8_t)ATOP_ADC_FILTER_TEST_LINTP) != 0u))
        {
            status = RunLintpTest(afltBase);
        }
    }

    /* --- Restore group-level ADC / PPCA state --- */
    Cy_PPCA_ADC_Set_Test_mode(adcBase, false);
    PPCA_ATOP_ADC_SIGN_UNSIGN_CNFG(adcBase) = savedAdcSignUnsign;
    PPCA_ATOP_ADC_CH_CNFG1(adcBase) = savedAdcChCnfg1;
    PPCA_ATOP_ADC_CH_CNFG0(adcBase) = savedAdcChCnfg0;
    PPCA_ATOP_ADC_CNV_CNFG(adcBase) = savedAdcCnvCnfg;
    PPCA_ATOP_ADC_CTRL(adcBase)     = savedAdcCtl;
    PPCA_ATOP_ADC_CNFG(adcBase)     = savedAdcCnfg;
    if (ATOP_ADC_FILTER_GROUP_0 == adcGroup)
    {
        Cy_PPCA_AREF_Disable(PPCA_ATOPSS_ADC_GRP0_AREF);
    }
    PPCA_CNFG_CTRL(PPCA_CNFG) = savedPpcaCtrl;

    return status;
}


#endif /* defined(CY_IP_MXS40PPSS) */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_ATOP_DAC.c
*
* Description:
*  This file provides source code for the APIs to perform ATOP DAC R2R
*  self tests according to the Class B library.
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

#include "SelfTest_ATOP_DAC.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

#include "cy_ppca_adc.h"
#include "cy_ppca_aref.h"
#include "cy_ppca_dcsg.h"
#include "cy_ppca_dacr2r.h"

/*******************************************************************************
* Private Definitions
*******************************************************************************/

/* Maximum valid DAC code (12-bit) */
#define DAC_MAX_CODE                    4095U

/* Number of DAC slices in the device */
#define DAC_NUM                         2U

/* Maximum valid DAC slice */
#define DAC_MAX_SLICE                   (DAC_NUM - 1U)

/* DAC settling time in milliseconds */
#define DAC_SETTLE_TIME_MS              10U

/* ADC timeout in milliseconds */
#define ADC_TIMEOUT_MS                  50U

/* ADC trigger mask for AUX slot (channel 7) */
#define ADC_AUX_SLOT_TRIGGER_MASK       0x80U

/*******************************************************************************
* Static Configuration Structures (based on cycfg_peripherals.c)
*******************************************************************************/

/* AREF configuration */
static const cy_stc_ppca_aref_config_t stlAtopDac_arefConfig =
{
    .aref_mode            = false,
    .enable_current_to_ts = false,
    .analog_ctrl          = CY_AREF_FORCE_POR_0,
    .vref_source_sel      = CY_LOCALLY_GENERATED_VREF,
};

/* DAC configuration - Loopback Single-Ended Mode (0-3.3V) */
static const cy_stc_ppca_dacr2r_config_t stlAtopDac_loopbackConfig =
{
    .dac_buf_enable                = false,
    .dacr2r_cnfg                   = CY_DACR2R_CNFG_LOOPBACK_SINGLE_ENDED_MODE,
    .offset_correction_disable     = false,
    .gain_correction_disable       = false,
    .buf_offset_correction_disable = false,
    .vref_to_buf_enable            = false,
    .dac_out_to_buf_enable         = true,
};

/* ADC AUX slot configuration (template - copied to stack before use) */
static const cy_stc_aux_slot_config_t stlAtopDac_adcAuxConfig =
{
    .aux_ch        = (uint8_t)CY_DAC_R2R0_OUTPUT,
    .aux_ch_gnd    = 0,
    .alt_aux_ch    = 0,
    .ch7_data_path = CY_DATA_PATH_AUX,
};

/* ADC configuration (template - aux_slot_config pointer updated at runtime with stack copy) */
static const cy_stc_ppca_adc_config_t stlAtopDac_adcConfig =
{
    .calib_gain_mode                 = false,
    .aux_slot                        = true,
    .alt_aux_slot                    = false,
    .channels                        = 128,
    .channel_type                    = 0,
    .channel_data_type               = 0,
    .aux_channel_data_type           = 0,
    .alt_aux_channel_data_type       = 0,
    .aux_slot_config                 = NULL,
    .adc_conv_config                 =
    {
        .group_channel               = false,
        .trigger_mode                = CY_ADC_MANUAL_TRIGGER,
        .auto_trigger_sample_clck    = 0,
        .num_of_slot                 = 1,
        .eos_src                     = false,
        .num_of_aux_slot             = 1,
        .num_of_alt_aux_slot         = 0,
        .aux_slot_scan_rate          = 0,
        .ext_src                     = false,
        .sequence_of_channel         = 7,
        .sequence_of_aux_channel     = 0,
        .sequence_of_alt_aux_channel = 0,
    },
};

/* DCSG configuration for DAC clock (template - copied to stack before use) */
static const cy_stc_ppca_dcsg_config_t stlAtopDac_dcsgConfig =
{
    .src_type         = 1,
    .src_ch           = 0,
    .src_ch_type      = CY_DCSG_CH_SRC_TYPE_CH_DIFF_ER,
    .blanking_period  = CY_DCSG_BLANKING_PERIOD_8,
    .dac_clk_blk_rate = CY_DCSG_DAC_RATE_10CLK_8BLANK_CYCLES,
    .refresh_rate     = CY_DCSG_REFRESH_RATE_10000_CYCLES,
    .bypass_refresh   = false,
    .use_user_value   = false,
    .user_value       = 0,
    .hyst_value       = CY_DCSG_HYSTERESIS_IS_DISABLED,
    .debounce_rate    = CY_DCSG_DEBOUNCE_RATE_DISABLED,
    .threshold        = 0,
    .groupNumber      = 0,
};

/*******************************************************************************
* Private Types
*******************************************************************************/

/** Structure to hold saved HW state for restore after test */
typedef struct
{
    uint32_t arefCtl;
    uint32_t arefAnaCtl;
    uint32_t dacCtl;
    uint32_t dacCnfg;
    uint32_t dcsgCtl;
    uint32_t dcsgCnfg;
    uint32_t adcCtl;
    uint32_t adcCnfg;
    uint32_t adcAuxAltAuxCnfg;
    uint32_t adcCnvCnfg;
    uint32_t adcSamplingMask;
} stc_atop_dac_saved_state_t;

/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
static void ATOP_DAC_SaveState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                               PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                               stc_atop_dac_saved_state_t* state);

static void ATOP_DAC_RestoreState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                  PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                  const stc_atop_dac_saved_state_t* state);

static void ATOP_DAC_Init(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                          PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                          const cy_stc_ppca_dcsg_config_t* dcsgCfg,
                          const cy_stc_ppca_adc_config_t* adcCfg);

static uint8_t ATOP_DAC_TestValue(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                  cy_en_aux_channel_t auxChSel,
                                  uint32_t dacVal, uint16_t accuracy);

/*******************************************************************************
* Function Name: ATOP_DAC_SaveState
****************************************************************************//**
*
* Saves the current HW configuration of all peripherals used by the test.
*
* \param dacBase   Pointer to DAC R2R base address
* \param dcsgBase  Pointer to DCSG slice base address
* \param state     Pointer to structure where state will be saved
*
*******************************************************************************/
static void ATOP_DAC_SaveState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                               PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                               stc_atop_dac_saved_state_t* state)
{
    /* Save AREF state */
    state->arefCtl = PPCA_ATOPSS_ADC_GRP0_AREF->AREF_CTL;
    state->arefAnaCtl = PPCA_ATOPSS_ADC_GRP0_AREF->AREF_ANA_CTL;

    /* Save DAC state */
    state->dacCtl = dacBase->DAC_CTL;
    state->dacCnfg = dacBase->DAC_CNFG;

    /* Save DCSG state */
    state->dcsgCtl = dcsgBase->DCSG_CTL;
    state->dcsgCnfg = dcsgBase->DCSG_CNFG;

    /* Save ADC state */
    state->adcCtl = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CTL;
    state->adcCnfg = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CNFG;
    state->adcAuxAltAuxCnfg = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_AUX_ALT_AUX_CNFG;
    state->adcCnvCnfg = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CNV_CNFG;
    state->adcSamplingMask = PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_SAMPLING_MASK;
}


/*******************************************************************************
* Function Name: ATOP_DAC_RestoreState
****************************************************************************//**
*
* Restores the HW configuration saved by ATOP_DAC_SaveState.
*
* \param dacBase   Pointer to DAC R2R base address
* \param dcsgBase  Pointer to DCSG slice base address
* \param state     Pointer to structure with saved state
*
*******************************************************************************/
static void ATOP_DAC_RestoreState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                  PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                  const stc_atop_dac_saved_state_t* state)
{
    /* Disable peripherals in reverse init order before restoring state */
    Cy_PPCA_ADC_Disable(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC);
    Cy_PPCA_DAC_Disable(dacBase);
    Cy_PPCA_DCSG_Disable(dcsgBase);
    Cy_PPCA_AREF_Disable(PPCA_ATOPSS_ADC_GRP0_AREF);

    /* Restore state in init order (respecting HW dependencies) */

    /* Restore AREF state (analog reference - needed by all) */
    PPCA_ATOPSS_ADC_GRP0_AREF->AREF_ANA_CTL = state->arefAnaCtl;
    PPCA_ATOPSS_ADC_GRP0_AREF->AREF_CTL = state->arefCtl;

    /* Restore DCSG state (clock source - needed by DAC) */
    dcsgBase->DCSG_CNFG = state->dcsgCnfg;
    dcsgBase->DCSG_CTL = state->dcsgCtl;

    /* Restore DAC state (depends on DCSG clock) */
    dacBase->DAC_CNFG = state->dacCnfg;
    dacBase->DAC_CTL = state->dacCtl;

    /* Restore ADC state */
    PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_SAMPLING_MASK = state->adcSamplingMask;
    PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CNV_CNFG = state->adcCnvCnfg;
    PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_AUX_ALT_AUX_CNFG = state->adcAuxAltAuxCnfg;
    PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CNFG = state->adcCnfg;
    PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC->ADC_CTL = state->adcCtl;
}


/*******************************************************************************
* Function Name: ATOP_DAC_Init
****************************************************************************//**
*
* Initializes and enables all HW blocks required for DAC loopback test.
*
* \param dacBase  Pointer to DAC R2R base address
* \param dcsgBase Pointer to DCSG slice base address
* \param dcsgCfg  Pointer to DCSG configuration (stack copy with groupNumber set)
* \param adcCfg   Pointer to ADC configuration (stack copy with aux_slot_config set)
*
*******************************************************************************/
static void ATOP_DAC_Init(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                          PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                          const cy_stc_ppca_dcsg_config_t* dcsgCfg,
                          const cy_stc_ppca_adc_config_t* adcCfg)
{
    /* 1. Initialize and enable AREF */
    Cy_PPCA_AREF_Init(PPCA_ATOPSS_ADC_GRP0_AREF, &stlAtopDac_arefConfig);
    Cy_PPCA_AREF_Enable(PPCA_ATOPSS_ADC_GRP0_AREF);

    /* 2. Initialize and enable DCSG (provides clock for DAC) */
    Cy_PPCA_DCSG_Init(dcsgBase, dcsgCfg);
    Cy_PPCA_DCSG_Enable(dcsgBase);

    /* 3. Initialize and enable DAC in loopback mode */
    Cy_PPCA_DAC_Init(dacBase, &stlAtopDac_loopbackConfig);
    Cy_PPCA_DAC_Enable(dacBase);

    /* 4. Initialize and enable ADC with AUX channel */
    Cy_PPCA_ADC_Init(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC, adcCfg);
    Cy_PPCA_ADC_Enable(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC);
}


/*******************************************************************************
* Function Name: ATOP_DAC_TestValue
****************************************************************************//**
*
* Tests a single DAC value by setting output, reading ADC, and validating.
*
* \param dacBase   Pointer to DAC R2R base address
* \param auxChSel  AUX channel selection for ADC readback
* \param dacVal    DAC output code to test (0-4095)
* \param accuracy  Acceptable deviation in ADC codes
*
* \return OK_STATUS on success, ERROR_STATUS on failure, MTB_STL_ERROR_TIMEOUT on timeout
*
*******************************************************************************/
static uint8_t ATOP_DAC_TestValue(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                  cy_en_aux_channel_t auxChSel,
                                  uint32_t dacVal, uint16_t accuracy)
{
    uint8_t ret = ERROR_BAD_PARAM;
    uint32_t timeout;
    int16_t adcResult;
    int32_t difference;

    /* Validate DAC value */
    if (dacVal <= DAC_MAX_CODE)
    {
        /* Default to error for timeout/out-of-tolerance cases */
        ret = ERROR_STATUS;

        /* Set DAC output value */
        Cy_PPCA_DAC_Set_DACOut(dacBase, (uint16_t)dacVal);

        /* Wait for DAC settling */
        Cy_SysLib_Delay(DAC_SETTLE_TIME_MS);

        /* Trigger ADC conversion (AUX slot only - channel 7) */
        Cy_PPCA_ADC_Trigger(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC, ADC_AUX_SLOT_TRIGGER_MASK);

        /* Wait for ADC conversion with timeout */
        timeout = ADC_TIMEOUT_MS;
        while (Cy_PPCA_ADC_Is_ADC_Busy(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC) && (timeout > 0U))
        {
            Cy_SysLib_Delay(1U);
            timeout--;
        }

        /* Check if ADC conversion completed (not timed out) */
        if (!Cy_PPCA_ADC_Is_ADC_Busy(PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC))
        {
            /* Read ADC result from AUX channel */
            adcResult = (int16_t)Cy_PPCA_ADC_Read_AUX_ADC_Data(
                PPCA_ATOPSS_ADC_GRP0_SLICE0_ADC, auxChSel);

            #if ERROR_IN_ATOP_DAC
            /* Error injection for testing */
            adcResult += (int16_t)(2U * (uint32_t)accuracy);
            #endif

            /* Check if result is within tolerance of dacVal */
            difference = (int32_t)adcResult - (int32_t)dacVal;

            if ((difference >= -(int32_t)accuracy) && (difference <= (int32_t)accuracy))
            {
                ret = OK_STATUS;
            }
            else
            {
                ret = ERROR_STATUS;
            }
        }
        else
        {
            /* ADC conversion timed out */
            ret = MTB_STL_ERROR_TIMEOUT;
        }
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTests_ATOP_DAC
*******************************************************************************/
uint8_t SelfTests_ATOP_DAC(uint32_t adcChannel, uint32_t dacSlice,
                           const uint32_t* dacVals, uint32_t numVals,
                           uint16_t accuracy)
{
    uint8_t ret = ERROR_BAD_PARAM;
    uint32_t i;
    stc_atop_dac_saved_state_t savedState;

    /* Peripheral base pointers (only DAC and DCSG vary by slice) */
    PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase;
    PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase;
    cy_en_aux_channel_t auxChSel;

    /* Validate parameters */
    if ((dacSlice <= DAC_MAX_SLICE) && (adcChannel == ATOP_ADC_CHANNEL_0) &&
        (dacVals != NULL) && (numVals > 0U) && (accuracy <= (DAC_MAX_CODE / 2U)))
    {
        /* Create local stack copies of mutable configs for reentrancy */
        cy_stc_ppca_dcsg_config_t localDcsgConfig = stlAtopDac_dcsgConfig;
        cy_stc_aux_slot_config_t localAdcAuxConfig = stlAtopDac_adcAuxConfig;
        cy_stc_ppca_adc_config_t localAdcConfig = stlAtopDac_adcConfig;

        /* Get peripheral base addresses based on DAC slice */
        if (dacSlice == ATOP_DAC_SLICE_0)
        {
            dacBase = PPCA_ATOPSS_DCSG_GRP0_DAC_R2R;
            dcsgBase = PPCA_ATOPSS_DCSG_GRP0_DCSG_SLICE0;
            auxChSel = CY_DAC_R2R0_OUTPUT;
            localDcsgConfig.groupNumber = 0;
        }
        else /* ATOP_DAC_SLICE_1 */
        {
            dacBase = PPCA_ATOPSS_DCSG_GRP1_DAC_R2R;
            dcsgBase = PPCA_ATOPSS_DCSG_GRP1_DCSG_SLICE0;
            auxChSel = CY_DAC_R2R1_OUTPUT;
            localDcsgConfig.groupNumber = 1;
        }

        /* Update AUX channel selection in local ADC config */
        localAdcAuxConfig.aux_ch = (uint8_t)auxChSel;
        localAdcConfig.aux_slot_config = &localAdcAuxConfig;

        /* Save current HW state before modifying peripherals */
        ATOP_DAC_SaveState(dacBase, dcsgBase, &savedState);

        /* Initialize HW blocks */
        ATOP_DAC_Init(dacBase, dcsgBase, &localDcsgConfig, &localAdcConfig);

        /* Execute tests for all values */
        ret = OK_STATUS;
        for (i = 0U; i < numVals; i++)
        {
            ret = ATOP_DAC_TestValue(dacBase, auxChSel, dacVals[i], accuracy);
            if (ret != OK_STATUS)
            {
                break;
            }
        }

        /* Restore original HW state */
        ATOP_DAC_RestoreState(dacBase, dcsgBase, &savedState);
    }

    return ret;
}


#endif /* CY_IP_MXS40PPSS */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_ATOP_DCSG.c
*
* Description:
*  This file provides source code for the APIs to perform ATOP DCSG
*  (Digital Comparator and Slope Generator) self tests according to the
*  Class B library.
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

#include "SelfTest_ATOP_DCSG.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

#include "cy_ppca_cnfg.h"
#include "cy_ppca_aref.h"
#include "cy_ppca_dcsg.h"
#include "cy_ppca_dacr2r.h"

/*******************************************************************************
* Private Definitions
*******************************************************************************/

/** Low threshold value for testing (12-bit, well below max) */
#define ATOP_DCSG_THRESHOLD_LOW         500U

/** High threshold value for testing (12-bit, well above min) */
#define ATOP_DCSG_THRESHOLD_HIGH        3500U

/** DAC maximum value (12-bit) - simulates Vcc input */
#define ATOP_DCSG_DAC_VALUE_MAX         4095U

/** DAC minimum value (12-bit) - simulates GND input */
#define ATOP_DCSG_DAC_VALUE_MIN         0U

/** DAC settling time in milliseconds */
#define ATOP_DCSG_DAC_SETTLE_TIME_MS    10U

/*******************************************************************************
* Static Configuration Structures
*******************************************************************************/

/* AREF configuration */
static const cy_stc_ppca_aref_config_t stlAtopDcsg_arefConfig =
{
    .aref_mode            = false,
    .enable_current_to_ts = false,
    .analog_ctrl          = CY_AREF_FORCE_POR_0,
    .vref_source_sel      = CY_LOCALLY_GENERATED_VREF,
};

/* DAC configuration - Loopback Single-Ended Mode (0-3.3V) */
static const cy_stc_ppca_dacr2r_config_t stlAtopDcsg_dacConfig =
{
    .dac_buf_enable                = false,
    .dacr2r_cnfg                   = CY_DACR2R_CNFG_LOOPBACK_SINGLE_ENDED_MODE,
    .offset_correction_disable     = false,
    .gain_correction_disable       = false,
    .buf_offset_correction_disable = false,
    .vref_to_buf_enable            = false,
    .dac_out_to_buf_enable         = true,
};

/* DCSG configuration - Internal source (from DAC R2R) */
static const cy_stc_ppca_dcsg_config_t stlAtopDcsg_config =
{
    .src_type         = true,
    .src_ch           = 0,
    .src_ch_type      = CY_DCSG_CH_SRC_TYPE_CH_SINGLE_P,
    .blanking_period  = CY_DCSG_BLANKING_PERIOD_8,
    .dac_clk_blk_rate = CY_DCSG_DAC_RATE_10CLK_8BLANK_CYCLES,
    .refresh_rate     = CY_DCSG_REFRESH_RATE_1000_CYCLES,
    .bypass_refresh   = false,
    .use_user_value   = false,
    .user_value       = false,
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
} stc_atop_dcsg_saved_state_t;

/*******************************************************************************
* Private Function Prototypes
*******************************************************************************/
static void ATOP_DCSG_SaveState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                stc_atop_dcsg_saved_state_t* state);

static void ATOP_DCSG_RestoreState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                   PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                   const stc_atop_dcsg_saved_state_t* state);

static void ATOP_DCSG_Init(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                           PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                           const cy_stc_ppca_dcsg_config_t* dcsgCfg);

static uint8_t SelfTests_ATOP_DCSG_RunTest(
    PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
    PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
    uint16_t dacValue,
    uint16_t threshold,
    bool expectedCmpFlag);

/*******************************************************************************
* Function Name: ATOP_DCSG_SaveState
****************************************************************************//**
*
* Saves the current HW configuration of all peripherals used by the test.
*
* \param dacBase   Pointer to DAC R2R base address
* \param dcsgBase  Pointer to DCSG slice base address
* \param state     Pointer to structure where state will be saved
*
*******************************************************************************/
static void ATOP_DCSG_SaveState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                stc_atop_dcsg_saved_state_t* state)
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
}


/*******************************************************************************
* Function Name: ATOP_DCSG_RestoreState
****************************************************************************//**
*
* Restores the HW configuration saved by ATOP_DCSG_SaveState.
*
* \param dacBase   Pointer to DAC R2R base address
* \param dcsgBase  Pointer to DCSG slice base address
* \param state     Pointer to structure with saved state
*
*******************************************************************************/
static void ATOP_DCSG_RestoreState(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                                   PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                                   const stc_atop_dcsg_saved_state_t* state)
{
    /* Disable peripherals in reverse init order before restoring state */
    Cy_PPCA_DCSG_Disable(dcsgBase);
    Cy_PPCA_DAC_Disable(dacBase);
    Cy_PPCA_AREF_Disable(PPCA_ATOPSS_ADC_GRP0_AREF);

    /* Restore state in init order (respecting HW dependencies) */

    /* Restore AREF state (analog reference - needed by all) */
    PPCA_ATOPSS_ADC_GRP0_AREF->AREF_ANA_CTL = state->arefAnaCtl;
    PPCA_ATOPSS_ADC_GRP0_AREF->AREF_CTL = state->arefCtl;

    /* Restore DCSG state */
    dcsgBase->DCSG_CNFG = state->dcsgCnfg;
    dcsgBase->DCSG_CTL = state->dcsgCtl;

    /* Restore DAC state */
    dacBase->DAC_CNFG = state->dacCnfg;
    dacBase->DAC_CTL = state->dacCtl;
}


/*******************************************************************************
* Function Name: ATOP_DCSG_Init
****************************************************************************//**
*
* Initializes and enables all HW blocks required for DCSG test.
*
* \param dacBase  Pointer to DAC R2R base address
* \param dcsgBase Pointer to DCSG slice base address
* \param dcsgCfg  Pointer to DCSG configuration (local copy with groupNumber set)
*
*******************************************************************************/
static void ATOP_DCSG_Init(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                           PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
                           const cy_stc_ppca_dcsg_config_t* dcsgCfg)
{
    /* 1. Initialize and enable AREF */
    Cy_PPCA_AREF_Init(PPCA_ATOPSS_ADC_GRP0_AREF, &stlAtopDcsg_arefConfig);
    Cy_PPCA_AREF_Enable(PPCA_ATOPSS_ADC_GRP0_AREF);

    /* 2. Initialize and enable DAC in loopback mode */
    Cy_PPCA_DAC_Init(dacBase, &stlAtopDcsg_dacConfig);
    Cy_PPCA_DAC_Enable(dacBase);

    /* 3. Initialize and enable DCSG (uses internal DAC as input) */
    Cy_PPCA_DCSG_Init(dcsgBase, dcsgCfg);
    Cy_PPCA_DCSG_Enable(dcsgBase);
}


/*******************************************************************************
* Function Name: SelfTests_ATOP_DCSG
*******************************************************************************/
uint8_t SelfTests_ATOP_DCSG(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                            PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase)
{
    uint8_t ret = ERROR_STATUS;
    stc_atop_dcsg_saved_state_t savedState;

    /* Validate parameters */
    if ((dacBase != NULL) && (dcsgBase != NULL))
    {
        /* Create local copy of DCSG config for thread safety */
        cy_stc_ppca_dcsg_config_t localDcsgConfig = stlAtopDcsg_config;

        /* Save current HW state before modifying peripherals */
        ATOP_DCSG_SaveState(dacBase, dcsgBase, &savedState);

        /***************************************************************************
        * Disable HW Blocks before initialization (ensure clean state)
        ***************************************************************************/
        Cy_PPCA_DCSG_Disable(dcsgBase);
        Cy_PPCA_DAC_Disable(dacBase);

        /* Small delay to ensure hardware is fully stopped */
        Cy_SysLib_DelayUs(1U);

        /***************************************************************************
        * Initialize and Enable HW Blocks
        ***************************************************************************/
        ATOP_DCSG_Init(dacBase, dcsgBase, &localDcsgConfig);

        /***************************************************************************
        * Execute Tests
        ***************************************************************************/

        /*
         * Test Case 1: Input (DAC) = MAX, Threshold = LOW
         * Expected: CMP_FLAG = 1 (input > threshold)
         *
         * DAC output at max (4095) represents ~3.3V (Vcc)
         * Threshold at low value (500) represents ~0.4V
         * Comparator should output HIGH
         */
        ret = SelfTests_ATOP_DCSG_RunTest(
            dacBase, dcsgBase,
            ATOP_DCSG_DAC_VALUE_MAX,    /* DAC value = max */
            ATOP_DCSG_THRESHOLD_LOW,    /* Threshold = low */
            true                         /* Expected CMP_FLAG = 1 */
            );

        if (ret == OK_STATUS)
        {
            /*
             * Test Case 2: Input (DAC) = MIN, Threshold = HIGH
             * Expected: CMP_FLAG = 0 (input < threshold)
             *
             * DAC output at min (0) represents ~0V (GND)
             * Threshold at high value (3500) represents ~2.8V
             * Comparator should output LOW
             */
            ret = SelfTests_ATOP_DCSG_RunTest(
                dacBase, dcsgBase,
                ATOP_DCSG_DAC_VALUE_MIN,    /* DAC value = min */
                ATOP_DCSG_THRESHOLD_HIGH,   /* Threshold = high */
                false                        /* Expected CMP_FLAG = 0 */
                );
        }

        /***************************************************************************
        * Restore original HW state after test
        ***************************************************************************/
        ATOP_DCSG_RestoreState(dacBase, dcsgBase, &savedState);
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTests_ATOP_DCSG_RunTest
*******************************************************************************/
/**
 * \brief Runs a single DCSG comparator test case.
 *
 * \param dacBase       Pointer to DAC R2R instance
 * \param dcsgBase      Pointer to DCSG slice instance
 * \param dacValue      DAC output value (0-4095)
 * \param threshold     DCSG threshold value (0-4095)
 * \param expectedCmpFlag Expected CMP_FLAG result
 *
 * \return OK_STATUS if CMP_FLAG matches expected, ERROR_STATUS otherwise
 */
static uint8_t SelfTests_ATOP_DCSG_RunTest(
    PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
    PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase,
    uint16_t dacValue,
    uint16_t threshold,
    bool expectedCmpFlag)
{
    uint8_t ret = ERROR_STATUS;
    bool cmpFlag;

    /* Set DCSG threshold */
    Cy_PPCA_DCSG_Set_Threshold(dcsgBase, threshold);

    /* Set DAC output value */
    Cy_PPCA_DAC_Set_DACOut(dacBase, dacValue);

    /* Wait for DAC settling */
    Cy_SysLib_Delay(ATOP_DCSG_DAC_SETTLE_TIME_MS);

    /* Read comparator output after DCSG is ready */
    cmpFlag = Cy_PPCA_DCSG_Get_Input_Signal_Status(dcsgBase);

    #if defined(ERROR_IN_ATOP_DCSG) && (ERROR_IN_ATOP_DCSG != 0)
    /* Error injection for testing - invert the result */
    cmpFlag = !cmpFlag;
    #endif

    /* Verify result */
    if (cmpFlag == expectedCmpFlag)
    {
        ret = OK_STATUS;
    }

    return ret;
}


#endif /* CY_IP_MXS40PPSS */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_ADC_Config.h
*
* Description:
*  This file provides function prototypes, constants, and data structures used
*  for the ADC Configuration Protection self tests.
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
/**
 * \defgroup group_adc_config ADC Configuration Test (ADC Configuration Protection STL module)
 * \{
 *
 * This module provides APIs to capture and verify CRC-protected snapshots of
 * SAR ADC and shared PASS-level configuration registers.
 *
 * \ref SelfTest_ADC_Config_Save and \ref SelfTest_ADC_Config_Verify form a
 * pair for capturing and verifying SAR ADC instance configuration snapshots,
 * while \ref SelfTest_ADC_MMIO_Config_Save and \ref SelfTest_ADC_MMIO_Config_Verify
 * form a pair for capturing and verifying shared PASS-level configuration snapshots.
 *
 * Both \ref SelfTest_ADC_Config_Save and \ref SelfTest_ADC_MMIO_Config_Save
 * capture only static configuration registers into caller-owned snapshots.
 * Result, status, interrupt-request, and command registers (for example
 * \c RESULT, \c STATUS, \c CH.INTR, \c CH.TR_CMD, \c CAL_UPD_CMD, \c TEST_CTL)
 * are not captured. Run-time enable bits (\c CTL.ENABLED, \c DIAG_CTL.DIAG_EN,
 * \c CH.ENABLE.CHAN_EN, \c PASS_CTL.DBG_FREEZE_EN) are masked internally, and
 * application-provided masks exclude further dynamic fields.
 *
 * The corresponding \ref SelfTest_ADC_Config_Verify and
 * \ref SelfTest_ADC_MMIO_Config_Verify functions re-read the live registers,
 * apply the stored masks, and compare them against their snapshots.
 *
 * \defgroup group_adc_config_macros Macros
 * \defgroup group_adc_config_data_structures Data Structures
 * \defgroup group_adc_config_functions Functions
 */
#if !defined(SELFTEST_ADC_CONFIG_H)
    #define SELFTEST_ADC_CONFIG_H

#include "cy_pdl.h"
#include "SelfTest_common.h"
#include "SelfTest_CRC_calc.h"

#if defined(CY_IP_MXS40EPASS_ESAR) || defined(CY_DOXYGEN)

/**
 * \addtogroup group_adc_config_macros
 * \{
 */

/** Number of per-channel register slots covered by the snapshot/mask
 *  structures. Matches the fixed \c CH[32] array size of \c PASS_SAR_Type. */
#define STL_ADC_CONFIG_MAX_CHAN          (32u)

/** Number of shared generic trigger input/output selector registers in
*  \c PASS_EPASS_MMIO_Type (one slot per possible SAR ADC instance). */
#define STL_ADC_CONFIG_TR_SEL_NUM        (4u)

/** \} group_adc_config_macros */

/***************************************
*       Data Structures
***************************************/
/** \addtogroup group_adc_config_data_structures
 * \{
 */

/** Application-provided mask describing which SAR ADC instance configuration
 *  fields are legitimately changed by the application at runtime and must be
 *  excluded by \ref SelfTest_ADC_Config_Save and \ref SelfTest_ADC_Config_Verify.
 *
 *  Driver-known-fixed bits (\c CTL.ENABLED, \c DIAG_CTL.DIAG_EN,
 *  \c CH.ENABLE.CHAN_EN) are handled internally and must not be described
 *  here. */
typedef struct
{
    /* Per-ADC instance calibration registers: whole-register skip is
     * sufficient because each register holds only an offset/gain pair that
     * changes together on recalibration. */
    bool     skipAnaCal;     /**< Exclude ANA_CAL entirely. */
    bool     skipDigCal;     /**< Exclude DIG_CAL entirely. */
    bool     skipAnaCalAlt;  /**< Exclude ANA_CAL_ALT entirely. */
    bool     skipDigCalAlt;  /**< Exclude DIG_CAL_ALT entirely. */

    /** SAMPLE_CTL per-channel bitmask. Must NOT mask \c PIN_ADDR/\c PORT_ADDR
     *  (channel routing); expected use is the \c ALT_CAL bit only. */
    uint32_t chSampleCtlMask[STL_ADC_CONFIG_MAX_CHAN];
    /** TR_CTL per-channel bitmask. Must NOT mask \c PRIO/\c PREEMPT_TYPE/
     *  \c GROUP_END/\c DONE_LEVEL; expected use is the \c SEL field only. */
    uint32_t chTrCtlMask[STL_ADC_CONFIG_MAX_CHAN];
    /** POST_CTL per-channel bitmask. Must NOT mask \c POST_PROC/\c LEFT_ALIGN/
     *  \c SIGN_EXT/\c RANGE_MODE; expected use is \c AVG_CNT/\c SHIFT_R only. */
    uint32_t chPostCtlMask[STL_ADC_CONFIG_MAX_CHAN];
    /** INTR_MASK per-channel bitmask: bits set here (per interrupt source)
     *  are excluded from comparison. */
    uint32_t chIntrMaskMask[STL_ADC_CONFIG_MAX_CHAN];

    /** RANGE_CTL per-channel skip: true excludes the whole register. */
    bool     chSkipRangeCtl[STL_ADC_CONFIG_MAX_CHAN];

    /** Coarse per-channel exclusion mask: bit N set to 1 excludes channel N's
     *  entire register set from Save/Verify, e.g. for channels
     *  that are unused or fully re-configured by the application at runtime. */
    uint32_t channelSkipMask;
} stl_adc_config_mask_t;

/** CRC-protected snapshot of one SAR2 ADC instance's configuration
 *  registers. Owned and allocated by the caller.
 *
 *  All per-channel fields of a channel excluded by
 *  \c userMask.channelSkipMask are zero. */
typedef struct
{
    uint32_t ctl;          /**< CTL, with ENABLED masked out. */
    uint32_t diagCtl;      /**< DIAG_CTL, with DIAG_EN masked out. */
    uint32_t precondCtl;   /**< PRECOND_CTL. */
    uint32_t anaCal;       /**< ANA_CAL (0 if userMask.skipAnaCal). */
    uint32_t digCal;       /**< DIG_CAL (0 if userMask.skipDigCal). */
    uint32_t anaCalAlt;    /**< ANA_CAL_ALT (0 if userMask.skipAnaCalAlt). */
    uint32_t digCalAlt;    /**< DIG_CAL_ALT (0 if userMask.skipDigCalAlt). */

    uint32_t chTrCtl[STL_ADC_CONFIG_MAX_CHAN];       /**< Per-channel TR_CTL, with userMask.chTrCtlMask masked out. */
    uint32_t chSampleCtl[STL_ADC_CONFIG_MAX_CHAN];   /**< Per-channel SAMPLE_CTL, with userMask.chSampleCtlMask
                                                        masked out. */
    uint32_t chPostCtl[STL_ADC_CONFIG_MAX_CHAN];     /**< Per-channel POST_CTL, with userMask.chPostCtlMask
                                                        masked out. */
    uint32_t chRangeCtl[STL_ADC_CONFIG_MAX_CHAN];    /**< Per-channel RANGE_CTL (0 if userMask.chSkipRangeCtl). */
    uint32_t chIntrMask[STL_ADC_CONFIG_MAX_CHAN];    /**< Per-channel INTR_MASK, with userMask.chIntrMaskMask
                                                        masked out. */
    uint32_t chEnable[STL_ADC_CONFIG_MAX_CHAN];      /**< Per-channel ENABLE, with CHAN_EN masked out. */

    /** Mask configuration used to create this snapshot. */
    stl_adc_config_mask_t userMask;

    /** CRC32 (\ref SelfTests_CRC32) over every field above. */
    uint32_t crc;
} stl_adc_config_snapshot_t;

/** Application-provided mask for the registers in \ref stl_adc_mmio_config_snapshot_t. */
typedef struct
{
    /** PASS_CTL bitmask (shared/global register): bits set here are excluded
     *  from comparison. Expected use: \c REFBUF_MODE field only -
     *  \c DBG_FREEZE_EN is already handled internally. */
    uint32_t passCtlMask;
    /** SAR_TR_IN_SEL per-slot bitmask, for applications that reprogram
     *  generic trigger input routing at runtime. */
    uint32_t sarTrInSelMask[STL_ADC_CONFIG_TR_SEL_NUM];
    /** SAR_TR_OUT_SEL per-slot bitmask, for applications that reprogram
     *  generic trigger output routing at runtime. */
    uint32_t sarTrOutSelMask[STL_ADC_CONFIG_TR_SEL_NUM];
} stl_adc_mmio_config_mask_t;

/** CRC-protected snapshot of the PASS-level registers shared by every SAR2 ADC
 *  instance (\c PASS0->EPASS_MMIO). */
typedef struct
{
    uint32_t passCtl;      /**< PASS_CTL, with DBG_FREEZE_EN and userMask.passCtlMask masked out. */
    uint32_t sarTrInSel[STL_ADC_CONFIG_TR_SEL_NUM];   /**< SAR_TR_IN_SEL[4], with userMask.sarTrInSelMask masked out. */
    uint32_t sarTrOutSel[STL_ADC_CONFIG_TR_SEL_NUM];  /**< SAR_TR_OUT_SEL[4], with userMask.sarTrOutSelMask
                                                         masked out. */

    /** Mask configuration used to create this snapshot. */
    stl_adc_mmio_config_mask_t userMask;

    /** CRC32 (\ref SelfTests_CRC32) over every field above. */
    uint32_t crc;
} stl_adc_mmio_config_snapshot_t;

/** \} group_adc_config_data_structures */

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_adc_config_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_ADC_Config_Save
****************************************************************************//**
*
* Captures a CRC-protected snapshot of one SAR2 ADC instance's
* configuration registers, applying the driver-known-fixed masks and the
* caller-provided \p userMask before storing each value. Shared PASS-level
* registers are captured separately by \ref SelfTest_ADC_MMIO_Config_Save.
*
* Any channel index with its bit set in \c userMask->channelSkipMask is
* zero-filled in the snapshot and excluded from \ref SelfTest_ADC_Config_Verify.
*
* Call this once after the ADC is fully configured (\c Cy_SAR2_Init and any
* subsequent \c Cy_SAR2_Channel_Init, \c Cy_SAR2_Diag_Init, or calibration
* setter calls), before entering the periodic self-test schedule. Because
* \c CTL.ENABLED and \c CH.ENABLE.CHAN_EN are masked, calling it before or after
* \c Cy_SAR2_Enable gives the same result.
*
* \param base
* The pointer to the SAR ADC instance whose registers are captured.
*
* \param userMask
* Application-provided mask describing fields to exclude from
* future comparisons. Must not be NULL - pass an all-zero \ref stl_adc_config_mask_t
* to compare every field in every channel.
*
* \param snapshot
* Destination for the captured, CRC-protected snapshot. Stored snapshot used for
* later \ref SelfTest_ADC_Config_Verify calls.
*
* \return
*  \ref OK_STATUS (0) - Snapshot captured. <br>
*  \ref ERROR_BAD_PARAM (9) - \p base, \p userMask, or \p snapshot is NULL. <br>
*
*******************************************************************************/
uint8_t SelfTest_ADC_Config_Save(PASS_SAR_Type* base,
                                 const stl_adc_config_mask_t* userMask,
                                 stl_adc_config_snapshot_t* snapshot);

/*******************************************************************************
* Function Name: SelfTest_ADC_MMIO_Config_Save
****************************************************************************//**
*
* Captures a CRC-protected snapshot of the PASS-level registers shared
* by every SAR2 ADC instance, applying the driver-known-fixed mask and
* the caller-provided \p userMask before storing each value.
*
* Call this once for the whole PASS block after initialization (including any
* \c Cy_SAR2_SetGenericTriggerInput, \c Cy_SAR2_SetGenericTriggerOutput, or
* \c Cy_SAR2_SetReferenceBufferMode calls), alongside
* \ref SelfTest_ADC_Config_Save.
*
* \param mmioBase
* The pointer to the shared PASS top-level MMIO block (\c PASS0->EPASS_MMIO).
*
* \param userMask
* Application-provided mask describing fields to exclude from
* future comparisons. Must not be NULL - pass an all-zero \ref stl_adc_mmio_config_mask_t
* to compare every field.
*
* \param snapshot
* Destination for the captured, CRC-protected snapshot. Stored snapshot used for
* later \ref SelfTest_ADC_MMIO_Config_Verify calls.
*
* \return
*  \ref OK_STATUS (0) - Snapshot captured. <br>
*  \ref ERROR_BAD_PARAM (9) - \p mmioBase, \p userMask, or \p snapshot is NULL. <br>
*
*******************************************************************************/
uint8_t SelfTest_ADC_MMIO_Config_Save(PASS_EPASS_MMIO_Type* mmioBase,
                                      const stl_adc_mmio_config_mask_t* userMask,
                                      stl_adc_mmio_config_snapshot_t* snapshot);

/*******************************************************************************
* Function Name: SelfTest_ADC_Config_Verify
****************************************************************************//**
*
* Verifies the live SAR2 ADC configuration registers against a snapshot
* previously captured by \ref SelfTest_ADC_Config_Save.
*
* The snapshot's own CRC is checked first, a CRC mismatch returns
* \ref ERROR_ADC_CONFIG_SNAPSHOT_INVALID without comparing any register.
* If the CRC matches, the live registers are read, masked with the
* driver-known-fixed masks and the snapshot's stored \c userMask, and then
* compared. Channels excluded by \c userMask.channelSkipMask are not compared.
*
* \param base
* The pointer to the SAR ADC instance to verify. Must be the same instance
* passed to \ref SelfTest_ADC_Config_Save when \p snapshot was captured.
*
* \param snapshot
* The snapshot captured by \ref SelfTest_ADC_Config_Save.
*
* \return
*  \ref OK_STATUS (0) - Live configuration matches the snapshot. <br>
*  \ref ERROR_STATUS (1) - A masked comparison mismatch was found; the ADC
*  configuration was unexpectedly changed or corrupted. <br>
*  \ref ERROR_ADC_CONFIG_SNAPSHOT_INVALID (7) - The snapshot failed its own CRC
*  check. <br>
*  \ref ERROR_BAD_PARAM (9) - \p base or \p snapshot is NULL. <br>
*
*******************************************************************************/
uint8_t SelfTest_ADC_Config_Verify(PASS_SAR_Type* base,
                                   const stl_adc_config_snapshot_t* snapshot);

/*******************************************************************************
* Function Name: SelfTest_ADC_MMIO_Config_Verify
****************************************************************************//**
*
* Verifies the live shared PASS-level registers against a snapshot previously
* captured by \ref SelfTest_ADC_MMIO_Config_Save.
*
* The snapshot's own CRC is checked first; a CRC mismatch returns
* \ref ERROR_ADC_CONFIG_SNAPSHOT_INVALID without comparing any register.
* If the CRC matches, the live registers are read, masked with the
* driver-known-fixed mask and the snapshot's stored \c userMask, and then
* compared.
*
* \param mmioBase
* The pointer to the shared PASS top-level MMIO block (\c PASS0->EPASS_MMIO).
*
* \param snapshot
* The snapshot captured by \ref SelfTest_ADC_MMIO_Config_Save.
*
* \return
*  \ref OK_STATUS (0) - Live configuration matches the snapshot. <br>
*  \ref ERROR_STATUS (1) - A masked comparison mismatch was found; the shared
*  PASS configuration was unexpectedly changed or corrupted. <br>
*  \ref ERROR_ADC_CONFIG_SNAPSHOT_INVALID (7) - The snapshot failed its own CRC
*  check. <br>
*  \ref ERROR_BAD_PARAM (9) - \p mmioBase or \p snapshot is NULL. <br>
*
*******************************************************************************/
uint8_t SelfTest_ADC_MMIO_Config_Verify(PASS_EPASS_MMIO_Type* mmioBase,
                                        const stl_adc_mmio_config_snapshot_t* snapshot);

/** \} group_adc_config_functions */

#endif /* defined(CY_IP_MXS40EPASS_ESAR) || defined(CY_DOXYGEN) */

#endif /* SELFTEST_ADC_CONFIG_H */

/** \} group_adc_config */

/* [] END OF FILE */

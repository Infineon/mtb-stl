/*******************************************************************************
* File Name: SelfTest_ATOP_ADC_Core.h
*
* Description:
*  This file provides the constants and parameter values for the ATOP
*  ADC Core (12-bit SAR ADC) self tests.
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
 * \defgroup group_atop_adc_core      ATOP ADC Core             (ATOP ADC Core STL module)
 * \{
 *
 * The ATOP ADC Core self-test validates the 12-bit SAR ADC by performing a
 * conversion of a known internal reference voltage and comparing the result
 * against an expected value: the input is connected to a reference whose
 * nominal value is known, sampled through the converter datapath, and the
 * read-back code is verified to lie within a specified tolerance of the
 * expected code.
 *
 * The device exposes four independent ADC groups, but only group 0 provides an
 * on-chip reference (the 1.2 V VREF) reachable by the SAR through its AUX
 * multiplexer. The self-test therefore covers ADC group 0 only; the other
 * groups have no internal stimulus that the SAR can convert and would require
 * an external known voltage, which is out of scope for this test.
 *
 * The test exercises, on ADC group 0:
 *   - The SAR conversion of a known voltage.
 *   - The internal 1.2 V reference path.
 *   - The AUX channel multiplexer routing (channel 7).
 *   - The ADC trigger / busy / result data path.
 *
 * \section group_atop_adc_core_more_information More Information
 *
 * The test procedure for ADC group 0:
 *
 *      1) Save the ADC and AFE configuration registers that are modified
 *         by the test.
 *      2) Initialise the ADC in manual-trigger, AUX-only mode and route the
 *         internal reference (VREF = 1.2 V) to the AUX slot on channel 7,
 *         with AFE in unity / pass-through gain.
 *      3) Trigger one conversion, poll the ADC busy status with a defensive
 *         timeout, and read the AUX result register.
 *      4) Compare the read-back code against the pre-computed expected code
 *         within a fixed tolerance.
 *      5) If the ERROR_IN_ATOP_ADC error-injection flag is set, corrupt the
 *         measured code by more than the tolerance so that a fault-free run is
 *         forced outside the acceptance window and reported as failed.
 *      6) Restore the original ADC / AFE configuration and return the
 *         result.
 *
 *
 * \defgroup group_atop_adc_core_macros Macros
 * \defgroup group_atop_adc_core_data_structures Data Structures
 * \defgroup group_atop_adc_core_functions Functions
 */

#if !defined(SELFTEST_ATOP_ADC_CORE_H)
    #define SELFTEST_ATOP_ADC_CORE_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN))

/***************************************
* Function Prototypes
***************************************/

/**
 * \addtogroup group_atop_adc_core_macros
 * \{
 */

/** ATOP ADC group 0 index. */
#define ATOP_ADC_CORE_GROUP_0           (0U)
/** ATOP ADC group 1 index. */
#define ATOP_ADC_CORE_GROUP_1           (1U)
/** ATOP ADC group 2 index. */
#define ATOP_ADC_CORE_GROUP_2           (2U)
/** ATOP ADC group 3 index. */
#define ATOP_ADC_CORE_GROUP_3           (3U)
/** Number of ADC groups. */
#define ATOP_ADC_CORE_GROUP_COUNT       (4U)

/** Number of SAR channels on ADC groups 0 and 3 (single-ended channels 0..7). */
#define ATOP_ADC_CORE_CHANNEL_COUNT     (8U)

/** Number of SAR channels on ADC groups 1 and 2 (single-ended channels 0..3). */
#define ATOP_ADC_CORE_CHANNEL_COUNT_GRP12   (4U)

/** ADC channel index (7) onto which the AUX multiplexer is routed; this is
 *  the channel that carries the internal reference. The AUX source itself
 *  (e.g. VREF) is selected separately via the PDL \c cy_en_aux_channel_t. */
#define ATOP_ADC_CORE_AUX_CHANNEL       (7U)

/** Maximum 12-bit SAR conversion code. */
#define ATOP_ADC_CORE_MAX_CODE          (4095U)

/** Expected 12-bit code for the internal 1.2 V bandgap VREF, converted
 *  ratiometrically against VDDA and assuming VDDA = 3.3 V
 *  (round(1.2 / 3.3 * 4095) = 1490). On boards with a different VDDA rail,
 *  pass a supply-specific \c expectedCode in \ref stl_atop_adc_core_config_t
 *  instead of using this default. */
#define ATOP_ADC_CORE_VREF_EXPECTED_CODE        (1490U)
/** Acceptance window (+/- codes) around the VREF expected code. */
#define ATOP_ADC_CORE_VREF_TOLERANCE_CODE       (200U)

/** \} group_atop_adc_core_macros */

/**
 * \addtogroup group_atop_adc_core_data_structures
 * \{
 */

/** Reference source converted and checked by the ADC Core self-test. */
typedef enum
{
    CY_STL_ATOP_ADC_REF_VREF_1V2  = 0U,  /**< Internal 1.2 V bandgap VREF (on-chip reference). */
    CY_STL_ATOP_ADC_REF_EXTERNAL  = 1U   /**< External reference on a regular ADC channel. Converts an
                                          *   application-supplied voltage on the selected channel and
                                          *   checks it against the caller's expectedCode / toleranceCode. */
} stl_atop_adc_ref_source_t;

/** Configuration for one ADC Core self-test run.
 *
 * Any field value not supported on the current silicon is rejected with
 * \c ERROR_BAD_PARAM. Use \ref ATOP_ADC_CORE_CONFIG_GRP0_VREF for the
 * ready-made case. */
typedef struct
{
    uint8_t                   adcGroup;      /**< ADC group selector (\ref ATOP_ADC_CORE_GROUP_0 ..). */
    uint8_t                   channel;       /**< ADC channel carrying the reference. */
    stl_atop_adc_ref_source_t refSource;     /**< Reference source to convert. */
    uint16_t                  expectedCode;  /**< Expected 12-bit conversion code. */
    uint16_t                  toleranceCode; /**< Acceptance window (+/- codes); must be non-zero.
                                              *   \ref SelfTest_ATOP_ADC_Core returns
                                              *   \c ERROR_BAD_PARAM if this is zero. */
} stl_atop_adc_core_config_t;

/** Ready-made configuration: GROUP0 internal 1.2 V VREF self-test. */
#define ATOP_ADC_CORE_CONFIG_GRP0_VREF                                       \
    {                                                                        \
        ATOP_ADC_CORE_GROUP_0, ATOP_ADC_CORE_AUX_CHANNEL,                    \
        CY_STL_ATOP_ADC_REF_VREF_1V2,                                        \
        ATOP_ADC_CORE_VREF_EXPECTED_CODE, ATOP_ADC_CORE_VREF_TOLERANCE_CODE  \
    }

/** \} group_atop_adc_core_data_structures */

/**
 * \addtogroup group_atop_adc_core_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_ATOP_ADC_Core
****************************************************************************//**
*
* Performs a self-test of an ATOP ADC group by converting a known internal
* reference voltage and comparing the result against an expected code.
*
* The SAR conversion datapath, the AUX channel multiplexer, the AFE input
* stage, the trigger / busy logic and the result-register read path are
* exercised. The original ADC and AFE configuration is preserved and
* restored on return so the test can be safely interleaved with normal use
* of the other ADC groups in the system.
*
* The reference source, channel, expected code and tolerance are taken from
* \p config. Use \ref ATOP_ADC_CORE_CONFIG_GRP0_VREF for the ready-made case.
*
* \note Only \ref ATOP_ADC_CORE_GROUP_0 provides an on-chip reference (the
* 1.2 V VREF) reachable by the SAR through its AUX multiplexer, so it is the
* only group with a meaningful internal self-test. Any \p config field not
* supported on the current silicon (other group, non-AUX channel, unknown
* reference source, out-of-range expected code, zero tolerance) is rejected
* with \c ERROR_BAD_PARAM before any HW access.
*
* \note The application is responsible for any system-level allocation of the
* ATOP ADC group to a CPU. The self-test enables the ADC and AFE through the
* standard PDL APIs and restores their previous enable state on return.
*
* \param config
* Pointer to the test configuration \ref stl_atop_adc_core_config_t.
*
* \return
*  OK_STATUS       (0) - Test passed <br>
*  ERROR_STATUS    (1) - HW self-test failed (conversion timeout, busy
*                        stuck, or result out of tolerance) <br>
*  ERROR_BAD_PARAM (9) - NULL \p config or an unsupported configuration
*                        (no HW access performed)
*
*******************************************************************************/
uint8_t SelfTest_ATOP_ADC_Core(const stl_atop_adc_core_config_t* config);

/** \} group_atop_adc_core_functions */

#endif /* (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_ATOP_ADC_CORE_H) */

/** \} group_atop_adc_core */
/* [] END OF FILE */

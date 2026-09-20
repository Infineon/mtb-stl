/*******************************************************************************
* File Name: SelfTest_ATOP_AFE.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the ATOP AFE (Analog Front End) self-tests according to Class B
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
/**
 * \defgroup group_atop_afe ATOP AFE (ATOP AFE STL module)
 * \{
 *
 * PPCA ATOP AFE (Analog Front End) programmable-gain verification self-test.
 *
 * The AFE is a differential programmable-gain amplifier placed in front of the
 * 12-bit SAR ADC. Each of the four ADC groups has one AFE, on the first
 * differential channel (channel 0) of that group:
 *   - ADC group 0: AIN0P / AIN0N
 *   - ADC group 1: AIN4P / AIN4N
 *   - ADC group 2: AIN6P / AIN6N
 *   - ADC group 3: AIN8P / AIN8N
 *
 * The self-test verifies that the amplifier applies the expected gain to a
 * differential input. It converts the same differential input three times:
 *   1. with the AFE off (the raw, un-amplified input),
 *   2. with the AFE at its 8.25x gain setting,
 *   3. with the AFE at its 16.5x gain setting,
 * and checks that the two amplified codes match 8.25x and 16.5x of the raw
 * code (and therefore that the 16.5x code is about twice the 8.25x code),
 * within a caller-supplied tolerance. Because the expected codes are derived
 * from the measured raw code, the check tracks the actual applied stimulus:
 * source or temperature drift moves the raw code and the expected amplified
 * codes together, while the tolerance absorbs residual gain error.
 *
 * \section group_atop_afe_stimulus Required external stimulus
 *
 * The AFE differential input is a dedicated analog pin pair (AINxP / AINxN)
 * that has no on-chip stimulus source, so this is an EXTERNAL-reference test:
 * before calling it, the application must apply a small, stable differential
 * voltage across the selected group's AFE pins, within the AFE input window:
 *   - both pins within [VSSA - 0.25 V, 1 V] (common-mode limit),
 *   - a small differential, so the amplified result stays on-scale. The 16.5x
 *     reading reaches full scale once the raw differential exceeds about
 *     1/16.5 of the ADC range, so keep the raw differential well below that
 *     (a raw code of a few tens is a good operating point).
 * The test reports failure if the raw code is outside the caller's expected
 * window or if either amplified code is saturated, so an absent or out-of-range
 * stimulus is detected rather than silently passing.
 *
 * \section group_atop_afe_more_information More Information
 *
 * The test procedure for one ADC group:
 *
 *      1) Save the ADC and AFE configuration registers that the test modifies.
 *      2) Convert differential channel 0 with the AFE off, obtaining the raw
 *         (un-amplified) code, and confirm it lies inside the caller's
 *         expected raw window (stimulus present and in range).
 *      3) Convert the same channel with the AFE at 8.25x and then at 16.5x.
 *      4) Confirm neither amplified code is saturated, and that each matches
 *         the raw code scaled by its gain (8.25x, 16.5x) and that the 16.5x
 *         code is about twice the 8.25x code, all within the tolerance.
 *      5) If the ERROR_IN_ATOP_AFE error-injection flag is set, corrupt one
 *         amplified code beyond the tolerance so that a fault-free run is
 *         forced outside the acceptance window and reported as failed.
 *      6) Restore the original ADC / AFE configuration and return the result.
 *
 * \note The application is responsible for calling \c Cy_PPCA_CNFG_Init() and
 * \c Cy_PPCA_Enable(), and for bringing up AREF, before calling this function.
 * The ADC and AFE are enabled and restored by the test itself.
 *
 * \defgroup group_atop_afe_macros Macros
 * \defgroup group_atop_afe_data_structures Data Structures
 * \defgroup group_atop_afe_functions Functions
 */

#if !defined(SELFTEST_ATOP_AFE_H)
#define SELFTEST_ATOP_AFE_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

/** \addtogroup group_atop_afe_macros
 * \{
 */

/** ATOP ADC group 0 - AFE differential pair AIN0P / AIN0N. */
#define ATOP_AFE_GROUP_0                (0U)
/** ATOP ADC group 1 - AFE differential pair AIN4P / AIN4N. */
#define ATOP_AFE_GROUP_1                (1U)
/** ATOP ADC group 2 - AFE differential pair AIN6P / AIN6N. */
#define ATOP_AFE_GROUP_2                (2U)
/** ATOP ADC group 3 - AFE differential pair AIN8P / AIN8N. */
#define ATOP_AFE_GROUP_3                (3U)
/** Number of ADC groups, each with one AFE. */
#define ATOP_AFE_GROUP_COUNT            (4U)

/** Numerator of the lower AFE gain (8.25 = 33 / 4). */
#define ATOP_AFE_GAIN_LOW_NUM           (33)
/** Denominator of the lower AFE gain (8.25 = 33 / 4). */
#define ATOP_AFE_GAIN_LOW_DEN           (4)
/** Numerator of the higher AFE gain (16.5 = 33 / 2). */
#define ATOP_AFE_GAIN_HIGH_NUM          (33)
/** Denominator of the higher AFE gain (16.5 = 33 / 2). */
#define ATOP_AFE_GAIN_HIGH_DEN          (2)
/** Expected ratio of the 16.5x reading to the 8.25x reading (16.5 / 8.25). */
#define ATOP_AFE_GAIN_RATIO             (2)

/** Magnitude (12-bit codes) at or above which a signed differential reading is
 *  treated as saturated / out of range. */
#define ATOP_AFE_SATURATION_CODE        (2040)

/** Fixed slack (codes) added to each acceptance window on top of the
 *  percentage tolerance, to absorb quantisation and small-signal noise. */
#define ATOP_AFE_CODE_NOISE_FLOOR       (48)

/** Suggested lower bound of the raw (AFE-off) differential window (codes). */
#define ATOP_AFE_DEFAULT_RAW_MIN        (40)
/** Suggested upper bound of the raw (AFE-off) differential window (codes);
 *  kept below full-scale/16.5 so the 16.5x reading does not saturate. */
#define ATOP_AFE_DEFAULT_RAW_MAX        (120)
/** Suggested +/- tolerance (percent) on the amplified codes and gain ratio. */
#define ATOP_AFE_DEFAULT_TOLERANCE_PCT  (25U)

/** \} group_atop_afe_macros */


/** \addtogroup group_atop_afe_data_structures
 * \{
 */

/** Configuration for one AFE gain self-test run.
 *
 * The raw window and tolerance describe the externally-applied stimulus the
 * caller has set up on the selected group's AFE differential pins. Any
 * unsupported field is rejected with \c ERROR_BAD_PARAM. Use
 * \ref ATOP_AFE_CONFIG_GRP0_DEFAULT for the ready-made group-0 case. */
typedef struct
{
    uint8_t  adcGroup;             /**< ADC group / AFE selector (\ref ATOP_AFE_GROUP_0 ..). */
    int32_t  rawMin;               /**< Minimum accepted AFE-off differential code (signed). */
    int32_t  rawMax;               /**< Maximum accepted AFE-off differential code (signed);
                                    *   must be greater than \c rawMin. */
    uint16_t gainTolerancePercent; /**< Acceptance tolerance (+/- percent) on the amplified
                                    *   codes and the gain ratio; must be in 1..100. */
} stl_atop_afe_config_t;

/** Ready-made configuration: GROUP0 (AIN0P/AIN0N) with the suggested small
 *  positive stimulus window and default tolerance. */
#define ATOP_AFE_CONFIG_GRP0_DEFAULT                                         \
    {                                                                        \
        ATOP_AFE_GROUP_0, ATOP_AFE_DEFAULT_RAW_MIN, ATOP_AFE_DEFAULT_RAW_MAX, \
        ATOP_AFE_DEFAULT_TOLERANCE_PCT                                       \
    }

/** \} group_atop_afe_data_structures */


/** \addtogroup group_atop_afe_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_ATOP_AFE
****************************************************************************//**
*
* Performs the ATOP AFE programmable-gain verification self-test for one ADC
* group, using an externally-applied differential stimulus.
*
* The selected group's differential channel 0 is converted with the AFE off,
* then at 8.25x, then at 16.5x gain. The test passes when the raw (AFE-off)
* code is inside the caller's expected window, neither amplified code is
* saturated, and both amplified codes match the raw code scaled by their gain
* (and the 16.5x code is about twice the 8.25x code) within the caller's
* tolerance. The original ADC and AFE configuration is saved and restored.
*
* \note The AFE differential input has no on-chip stimulus. Before calling
* this function the application must apply a small, stable differential to the
* selected group's AFE pins (AIN0P/AIN0N, AIN4P/AIN4N, AIN6P/AIN6N or
* AIN8P/AIN8N), with both pins within [VSSA - 0.25 V, 1 V] and the differential
* small enough that the 16.5x reading stays on-scale.
*
* \note The application is responsible for calling \c Cy_PPCA_CNFG_Init() and
* \c Cy_PPCA_Enable(), and for bringing up AREF, before calling this function.
*
* \param config
* Pointer to the test configuration \ref stl_atop_afe_config_t.
*
* \return
*  \ref OK_STATUS (0) - AFE gain is within tolerance; test passed. <br>
*  \ref ERROR_STATUS (1) - Raw stimulus out of window, an amplified code
*                     saturated, or a gain out of tolerance. <br>
*  \ref MTB_STL_ERROR_TIMEOUT (10) - An ADC conversion timed out. <br>
*  \ref ERROR_BAD_PARAM (9) - NULL \p config or an unsupported configuration
*                     (no HW access performed). <br>
*
*******************************************************************************/
uint8_t SelfTest_ATOP_AFE(const stl_atop_afe_config_t* config);

/** \} group_atop_afe_functions */

/** \} group_atop_afe */

#endif /* CY_IP_MXS40PPSS || CY_DOXYGEN */

#endif /* !defined(SELFTEST_ATOP_AFE_H) */

/* [] END OF FILE */

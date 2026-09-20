/*******************************************************************************
* File Name: SelfTest_ATOP_ADC_Filter.h
*
* Description:
*  This file provides the constants and parameter values for the ATOP
*  ADC digital filter (Median, LINTP, LP, CIC3, AVG, MIN/MAX) self tests.
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
 * \defgroup group_atop_adc_filter    ATOP ADC Filter           (ATOP ADC digital filter STL module)
 * \{
 *
 * The ATOP ADC Filter self-test validates the digital filter chain that
 * post-processes the ADC converted-data stream: the Median filter, the Linear
 * Interpolation filter (LINTP), the Low-Pass filter (LP), the 3rd-order Comb
 * filter (CIC3), the configurable Average filter (AVG) and the MIN/MAX
 * detection logic.
 *
 * For each filter type a known input sample sequence is applied, the selected
 * filter is run in isolation (the other filter stages are bypassed) and the
 * read-back output is verified to lie within a tolerance of the value expected
 * for that filter's transfer function. Inputs are chosen so the expected output
 * is trivially known (for example, a constant DC level for the LP / AVG / CIC3
 * filters, whose steady-state response to a constant is a known scaling of that
 * constant), which is sufficient for the Class B gross-fault-detection goal.
 *
 * The Median, LP, AVG, CIC3 and MIN/MAX stages are tested functionally this way.
 * The Linear Interpolation (LINTP) stage needs an independent hardware
 * conversion-start trigger that firmware injection cannot provide, so it is
 * covered instead by a configuration-register integrity check.
 *
 * The device exposes six filter instances distributed across the four ADC
 * groups:
 *   - ADC group 0: ADC_FILT_00, ADC_FILT_01
 *   - ADC group 1: ADC_FILT_10
 *   - ADC group 2: ADC_FILT_20
 *   - ADC group 3: ADC_FILT_30, ADC_FILT_31
 *
 * \section group_atop_adc_filter_more_information More Information
 *
 * The test procedure for one filter instance and one filter type:
 *
 *      1) Save the ADC / filter configuration registers that are modified by
 *         the test.
 *      2) Configure the filter with only the filter type under test enabled and
 *         all other stages bypassed, selecting the source ADC channel.
 *      3) Apply the known input sample sequence to the filter input.
 *      4) Read the filter output register and compare it against the
 *         pre-computed expected value within a fixed tolerance.
 *      5) If the ERROR_IN_ATOP_ADC_FILTER error-injection flag is set, corrupt
 *         the measured value by more than the tolerance so that a fault-free run
 *         is forced outside the acceptance window and reported as failed.
 *      6) Restore the original configuration and return the result.
 *
 *
 * \defgroup group_atop_adc_filter_macros Macros
 * \defgroup group_atop_adc_filter_functions Functions
 */

#if !defined(SELFTEST_ATOP_ADC_FILTER_H)
    #define SELFTEST_ATOP_ADC_FILTER_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN))

/**
 * \addtogroup group_atop_adc_filter_macros
 * \{
 */

/** ATOP ADC group 0 index. */
#define ATOP_ADC_FILTER_GROUP_0             (0U)
/** ATOP ADC group 1 index. */
#define ATOP_ADC_FILTER_GROUP_1             (1U)
/** ATOP ADC group 2 index. */
#define ATOP_ADC_FILTER_GROUP_2             (2U)
/** ATOP ADC group 3 index. */
#define ATOP_ADC_FILTER_GROUP_3             (3U)
/** Number of ADC groups. */
#define ATOP_ADC_FILTER_GROUP_COUNT         (4U)

/** Maximum number of filter instances in a single ADC group (groups 0 and 3). */
#define ATOP_ADC_FILTER_MAX_INST_PER_GROUP  (2U)
/** Total number of filter instances across all groups. */
#define ATOP_ADC_FILTER_INST_COUNT          (6U)

/** Test-mask bit: Median filter. */
#define ATOP_ADC_FILTER_TEST_MEDIAN         (0x01U)
/** Test-mask bit: Linear Interpolation filter (LINTP). */
#define ATOP_ADC_FILTER_TEST_LINTP          (0x02U)
/** Test-mask bit: Low-Pass filter (LP). */
#define ATOP_ADC_FILTER_TEST_LPF            (0x04U)
/** Test-mask bit: 3rd-order Comb filter (CIC3). */
#define ATOP_ADC_FILTER_TEST_CIC3           (0x08U)
/** Test-mask bit: configurable Average filter (AVG). */
#define ATOP_ADC_FILTER_TEST_AVG            (0x10U)
/** Test-mask bit: MIN/MAX detection. */
#define ATOP_ADC_FILTER_TEST_MINMAX         (0x20U)
/** Test-mask value selecting every filter type. */
#define ATOP_ADC_FILTER_TEST_ALL            (0x3FU)

/** \} group_atop_adc_filter_macros */

/**
 * \addtogroup group_atop_adc_filter_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_ATOP_ADC_Filter
****************************************************************************//**
*
* Performs a self-test of the ATOP ADC digital filter chain for the given ADC
* group by running the selected filter types on each filter instance of that
* group. The filter types to run are chosen with a bit mask (like
* \ref SelfTest_CLB).
*
* For each selected filter type, a known input sample sequence is applied with
* only that filter enabled (all other stages bypassed), and the read-back output
* is compared against the value expected for the filter's transfer function
* within a tolerance. The original ADC / filter configuration is preserved and
* restored on return so the test can be interleaved with normal use of the other
* ADC groups.
*
* \param adcGroup
* ADC group selector. One of \ref ATOP_ADC_FILTER_GROUP_0
* .. \ref ATOP_ADC_FILTER_GROUP_3.
*
* \param testMask
* Bit mask selecting which filter types to run: any OR-combination of
* \ref ATOP_ADC_FILTER_TEST_MEDIAN, \ref ATOP_ADC_FILTER_TEST_LINTP,
* \ref ATOP_ADC_FILTER_TEST_LPF, \ref ATOP_ADC_FILTER_TEST_CIC3,
* \ref ATOP_ADC_FILTER_TEST_AVG, \ref ATOP_ADC_FILTER_TEST_MINMAX, or
* \ref ATOP_ADC_FILTER_TEST_ALL for all types.
*
* \return
*  OK_STATUS       (0) - All selected filter types passed on every instance <br>
*  ERROR_STATUS    (1) - A filter output was out of tolerance or a timeout occurred <br>
*  ERROR_BAD_PARAM (9) - Invalid \p adcGroup, or \p testMask selects no valid type
*                        (no HW access performed)
*
*******************************************************************************/
uint8_t SelfTest_ATOP_ADC_Filter(uint8_t adcGroup, uint8_t testMask);

/** \} group_atop_adc_filter_functions */

#endif /* (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_ATOP_ADC_FILTER_H) */

/** \} group_atop_adc_filter */
/* [] END OF FILE */

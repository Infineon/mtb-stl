/*******************************************************************************
* File Name: SelfTest_ATOP_DAC.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the ATOP DAC R2R self tests according to Class B library.
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
 * \defgroup group_atop_dac ATOP DAC (ATOP DAC STL module)
 * \{
 *
 * PPCA ATOP DAC R2R internal loopback self-test.
 *
 * This module tests the DAC R2R functionality by setting a digital code,
 * routing the analog output internally to the ADC via Channel 7 auxiliary
 * mux, and verifying the ADC reading matches the expected value.
 *
 * The test uses CY_DACR2R_CNFG_LOOPBACK_SINGLE_ENDED_MODE (0-3.3V) where
 * DAC and ADC share the same reference, providing 1:1 code mapping.
 *
 * \defgroup group_atop_dac_macros Macros
 * \defgroup group_atop_dac_functions Functions
 */

#if !defined(SELFTEST_ATOP_DAC_H)
#define SELFTEST_ATOP_DAC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

/** \addtogroup group_atop_dac_macros
 * \{
 */

/** DAC slice 0 (DAC_R2R_0 in DCSG_GRP_0) */
#define ATOP_DAC_SLICE_0                0U

/** DAC slice 1 (DAC_R2R_1 in DCSG_GRP_1) */
#define ATOP_DAC_SLICE_1                1U

/** ADC channel 0 (ADC_GRP_0) */
#define ATOP_ADC_CHANNEL_0              0U

/** \} group_atop_dac_macros */


/** \addtogroup group_atop_dac_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTests_ATOP_DAC
****************************************************************************//**
*
* Performs ATOP DAC R2R loopback self-test using PDL functions.
*
* This function initializes the DAC/ADC peripherals once and then tests
* multiple DAC values in a loop. For each value, it sets the DAC output,
* triggers ADC conversion, reads the result, and validates against the
* expected value with specified accuracy tolerance.
*
* \param adcChannel
* ADC channel/group to use for measurement. Use ATOP_ADC_CHANNEL_0.
*
* \param dacSlice
* DAC slice to test. Use ATOP_DAC_SLICE_0 or ATOP_DAC_SLICE_1.
*
* \param dacVals
* Pointer to array of DAC output codes to test (0-4095 for 12-bit DAC).
*
* \param numVals
* Number of values in the dacVals array.
*
* \param accuracy
* Acceptable deviation in ADC codes. Test passes if:
* (dacVal - accuracy) <= adcResult <= (dacVal + accuracy)
*
* \return
* \ref OK_STATUS (0) - All tests passed, ADC results within tolerance <br>
* \ref ERROR_STATUS (1) - Test failed (ADC result out of tolerance) <br>
* \ref ERROR_BAD_PARAM (9) - Invalid parameters (bad slice, null pointer, incorrect accuracy) <br>
* \ref MTB_STL_ERROR_TIMEOUT (10) - ADC conversion timed out <br>
*
* \note
* The PPCA subsystem must be enabled by the caller before invoking this
* function (e.g. Cy_PPCA_Enable(PPCA_CNFG)). The test only configures
* AREF, DCSG, DAC, and ADC sub-blocks and restores their state on exit;
* it does not modify the top-level PPCA control register.
*
* \note
* In loopback single-ended mode, DAC and ADC share the same voltage reference,
* so expected ADC result equals dacVal (1:1 code mapping).
*
* \code
*
* static const uint32_t dac_test_values[] = {
*     0U,      // Minimum code (0V)
*     2048U,   // Mid-scale (1.65V)
*     4095U    // Maximum code (3.3V)
* };
* #define DAC_TEST_NUM_VALUES  (sizeof(dac_test_values) / sizeof(dac_test_values[0]))
* #define DAC_TEST_ACCURACY    50U
*
* // Enable PPCA subsystem before calling the self-test
* Cy_PPCA_Enable(PPCA_CNFG);
*
* // Test DAC Slice 0
* uint8_t result = SelfTests_ATOP_DAC(ATOP_ADC_CHANNEL_0, ATOP_DAC_SLICE_0,
*                                      dac_test_values, DAC_TEST_NUM_VALUES,
*                                      DAC_TEST_ACCURACY);
* if (result != OK_STATUS)
* {
*     // Handle error (ERROR_STATUS or ERROR_BAD_PARAM)
* }
* \endcode
*
*******************************************************************************/
uint8_t SelfTests_ATOP_DAC(uint32_t adcChannel, uint32_t dacSlice,
                           const uint32_t* dacVals, uint32_t numVals,
                           uint16_t accuracy);

/** \} group_atop_dac_functions */

/** \} group_atop_dac */

#endif /* CY_IP_MXS40PPSS */

#endif /* SELFTEST_ATOP_DAC_H */

/* [] END OF FILE */

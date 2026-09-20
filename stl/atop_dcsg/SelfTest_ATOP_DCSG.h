/*******************************************************************************
* File Name: SelfTest_ATOP_DCSG.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the ATOP DCSG (Digital Comparator and Slope Generator) self tests
*  according to Class B library.
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
 * \defgroup group_atop_dcsg ATOP DCSG (ATOP DCSG STL module)
 * \{
 *
 * ATOP DCSG (Digital Comparator and Slope Generator) self-test for PPCA block.
 *
 * This module tests the DCSG functionality by:
 * 1. Setting DAC R2R to a known value (simulates Vcc input to comparator)
 * 2. Configuring SLGEN to provide the threshold value
 * 3. Verifying the CMP_FLAG output matches the expected result
 *
 * The test uses the internal DAC R2R output routed to the DCSG comparator input,
 * allowing controlled testing of both comparator states:
 * - Input > Threshold: CMP_FLAG = 1
 * - Input < Threshold: CMP_FLAG = 0
 *
 * \section group_atop_dcsg_more_information More Information
 *
 * Hardware path: DAC_R2R -> DCSG Comparator (via internal routing)
 *                SLGEN -> DCSG Threshold
 *
 * Test validates:
 * - DCSG comparator functionality
 * - SLGEN threshold generation
 * - Internal routing between DAC and DCSG
 *
 * \defgroup group_atop_dcsg_functions Functions
 */

#if !defined(SELFTEST_ATOP_DCSG_H)
#define SELFTEST_ATOP_DCSG_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

/** \addtogroup group_atop_dcsg_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTests_ATOP_DCSG
****************************************************************************//**
*
* Performs ATOP DCSG (Digital Comparator and Slope Generator) self-test.
*
* This function tests the DCSG comparator functionality by:
* 1. Configuring DAC R2R to generate known input voltages
* 2. Configuring SLGEN to provide threshold values
* 3. Verifying CMP_FLAG output matches expected results for both states:
*    - Test 1: DAC = MAX (Vcc), Threshold = LOW -> CMP_FLAG should be 1
*    - Test 2: DAC = MIN (GND), Threshold = HIGH -> CMP_FLAG should be 0
*
* \param dacBase
* Pointer to DAC R2R base address for the DCSG group.
* Example: PPCA_ATOPSS_DCSG_GRP0_DAC_R2R, PPCA_ATOPSS_DCSG_GRP1_DAC_R2R
*
* \param dcsgBase
* Pointer to DCSG slice base address to test.
* Example: PPCA_ATOPSS_DCSG_GRP0_DCSG_SLICE0, &PPCA_ATOPSS_DCSG_GRP0_DCSG_SLICE0[2]
*
* \return
* \ref OK_STATUS (0) - Test passed, DCSG comparator functioning correctly <br>
* \ref ERROR_STATUS (1) - Test failed (CMP_FLAG mismatch or invalid parameters) <br>
*
* \note
* The PPCA subsystem must be enabled by calling Cy_PPCA_Enable(PPCA_CNFG)
* before invoking this function.
*
* Example usage:
* \code
* Cy_PPCA_Enable(PPCA_CNFG);
*
* // Test Group 0, Slice 0
* uint8_t result = SelfTests_ATOP_DCSG(PPCA_ATOPSS_DCSG_GRP0_DAC_R2R,
*                                      PPCA_ATOPSS_DCSG_GRP0_DCSG_SLICE0);
*
* // Test all slices in Group 0 (slices 0-5)
* for (uint32_t i = 0U; i < 6U; i++)
* {
*     result = SelfTests_ATOP_DCSG(PPCA_ATOPSS_DCSG_GRP0_DAC_R2R,
*                                  &PPCA_ATOPSS_DCSG_GRP0_DCSG_SLICE0[i]);
* }
* \endcode
*
*******************************************************************************/
uint8_t SelfTests_ATOP_DCSG(PPCA_ATOPSS_DCSG_GRP_DAC_R2R_Type* dacBase,
                            PPCA_ATOPSS_DCSG_GRP_DCSG_SLICE_Type* dcsgBase);

/** \} group_atop_dcsg_functions */

/** \} group_atop_dcsg */

#endif /* CY_IP_MXS40PPSS */

#endif /* SELFTEST_ATOP_DCSG_H */

/* [] END OF FILE */

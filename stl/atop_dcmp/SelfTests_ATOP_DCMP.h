/*******************************************************************************
* File Name: SelfTests_ATOP_DCMP.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the ATOP DCMP self tests according to Class B library.
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
 * \defgroup group_atop_dcmp ATOP DCMP (ATOP Digital Comparator STL module)
 * \{
 *
 * PPCA ATOP DCMP self-test.
 *
 * This module verifies the DCMP compare result against expected outcomes
 * for multiple ADC test values around the selected threshold.
 *
 * \defgroup group_atop_dcmp_functions Functions
 */

#if !defined(SELFTEST_ATOP_DCMP)
#define SELFTEST_ATOP_DCMP

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

/** \addtogroup group_atop_dcmp_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTests_ATOP_DCMP
****************************************************************************//**
*
* Performs ATOP DCMP self-test using ADC test-data injection.
*
* The function sets an initial DCMP threshold, adapts it for signed mode if
* needed, and then checks DCMP status for three injected ADC values.
*
* \param adcBase
* Pointer to ADC base used for test-data injection and conversion trigger.
*
* \param dcmpBase
* Pointer to DCMP base used for threshold setup and status read.
*

* \return
* \ref OK_STATUS (0) - All comparator results match expected values. <br>
* \ref ERROR_STATUS (1) - At least one comparator result mismatches. <br>
* \ref MTB_STL_ERROR_TIMEOUT (10) if ADC conversion times out. <br>
*
*******************************************************************************/
uint8_t SelfTests_ATOP_DCMP(ATOPSS_ADC_TYPE* adcBase, ATOPSS_DCMP_TYPE* dcmpBase);

/** \} group_atop_dcmp_functions */

#endif // if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

#endif // if !defined(SELFTEST_ATOP_DCMP)

/** \} group_atop_dcmp */

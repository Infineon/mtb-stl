/*******************************************************************************
* File Name: SelfTest_APSR.h
*
* Description:
*  This file provides the function prototype and constants used for the APSR
*  register self-test.
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
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
 * \defgroup group_apsr APSR (APSR register STL module)
 * \{
 *
 * The APSR (Application Program Status Register) test detects stuck-at faults in the
 * APSR bits that are implemented and writable on the core: the condition flags N, Z, C,
 * and V, and, where the core has them, the sticky saturation flag Q and the
 * greater-than-or-equal flags GE[3:0].
 *
 * \section group_apsr_more_information More Information
 *
 * The test performs the following major tasks: <br>
 *
 *      1) Saves the APSR value.
 *      2) Writes a known pattern to the writable APSR bits, checks the N, Z, C, and V
 *         flags with conditional branches, reads APSR back, masks the result to the
 *         implemented bits, and compares it with the pattern.
 *      3) Repeats step 2 with the complementary pattern, so that every tested bit is
 *         checked at 0 and at 1.
 *      4) Restores the saved APSR value, and checks the restored value.
 *
 * The tested bits depend on the core, and are selected at compile time:
 *
 *  Core                                              | Tested bits
 *  ------------------------------------------------- | -----------------
 *  Armv6-M and Armv8-M Baseline (Cortex-M0+, M23)    | N, Z, C, V
 *  Armv7-M (Cortex-M3)                               | N, Z, C, V, Q
 *  Cores with the DSP extension (Cortex-M4, M7, M33) | N, Z, C, V, Q, GE
 *
 * \defgroup group_apsr_functions Functions
 */

#if !defined(SELFTEST_APSR_H)
#define SELFTEST_APSR_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

/** \cond INTERNAL */
/* Implemented in TOOLCHAIN_<toolchain>/SelfTest_APSR_<toolchain>.S */
uint8_t SelfTest_APSR_Asm(uint32_t apsrMask);
/** \endcond */

/**
 * \addtogroup group_apsr_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_APSR
****************************************************************************//**
*
* This function tests the APSR bits that are implemented and writable on the
* core, with a pattern and its complement.
*
* The function restores the original APSR value and checks it. On return, Q and GE
* (where implemented) hold their original values. N, Z, C, and V aren't preserved,
* as the Procedure Call Standard for the Arm Architecture doesn't require a called
* function to preserve them.
*
* \note
* The function doesn't need privileged access and doesn't mask interrupts: the
* APSR value is saved and restored on every exception entry and return.
*
* \note
* This test runs only in blocking mode.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed, or the original APSR value wasn't restored <br>
*
*******************************************************************************/
uint8_t SelfTest_APSR(void);

/** \} group_apsr_functions */

/** \} group_apsr */

#endif /* SELFTEST_APSR_H */

/* [] END OF FILE */

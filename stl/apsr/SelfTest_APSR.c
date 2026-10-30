/*******************************************************************************
* File Name: SelfTest_APSR.c
*
* Description:
*  This file provides the source code for the APSR register self-test.
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

#include "SelfTest_APSR.h"

/* APSR bits tested. N, Z, C, and V exist on every Cortex-M core. Q exists on Armv7-M and
 * Armv8-M Mainline (Thumb-2 cores), GE only with the DSP extension. Only compiler macros decide
 * this: no device headers. */
/* The mask selects the bits to test. The pattern and its complement are defined in the assembly
 * code: the conditional branch checks there depend on the N, Z, C, and V values of both. */
#define SELFTEST_APSR_NZCV_Msk              (0xF0000000UL)
#define SELFTEST_APSR_Q_Msk                 (0x08000000UL)
#define SELFTEST_APSR_GE_Msk                (0x000F0000UL)

#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
    #define SELFTEST_APSR_TEST_MASK         (SELFTEST_APSR_NZCV_Msk | SELFTEST_APSR_Q_Msk | \
                                             SELFTEST_APSR_GE_Msk)
#elif defined(__ARM_ARCH_ISA_THUMB) && (__ARM_ARCH_ISA_THUMB == 2)
    #define SELFTEST_APSR_TEST_MASK         (SELFTEST_APSR_NZCV_Msk | SELFTEST_APSR_Q_Msk)
#else
    #define SELFTEST_APSR_TEST_MASK         (SELFTEST_APSR_NZCV_Msk)
#endif

/*******************************************************************************
 * Function Name: SelfTest_APSR
 *******************************************************************************
 *
 * Summary:
 *  This function tests the implemented and writable APSR bits with a pattern
 *  and its complement, and restores APSR.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test: OK_STATUS - pass; ERROR_STATUS - fail.
 *
 ******************************************************************************/
uint8_t SelfTest_APSR(void)
{
    return SelfTest_APSR_Asm(SELFTEST_APSR_TEST_MASK);
}


/* [] END OF FILE */

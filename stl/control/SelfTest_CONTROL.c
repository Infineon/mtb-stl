/*******************************************************************************
* File Name: SelfTest_CONTROL.c
*
* Description:
*  This file provides the source code for the CONTROL register self-test.
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

#include "SelfTest_CONTROL.h"

/* CONTROL bits tested. SPSEL is always tested. FPCA is tested when the compiler generates FPU
 * code, SFPA only in a Secure image as well (Non-secure writes to SFPA are ignored).
 * SELFTEST_CONTROL_SKIP_FP_BITS removes both. nPRIV is only read (precondition check), never
 * written, and untested bits keep their original values. Only compiler macros decide this: no
 * device headers. */
#define SELFTEST_CONTROL_nPRIV_Msk          (1UL << 0U)
#define SELFTEST_CONTROL_SPSEL_Msk          (1UL << 1U)
#define SELFTEST_CONTROL_FPCA_Msk           (1UL << 2U)
#define SELFTEST_CONTROL_SFPA_Msk           (1UL << 3U)

#if (defined(__ARM_FP) && (__ARM_FP != 0)) && (SELFTEST_CONTROL_SKIP_FP_BITS == 0)
    #if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)
        #define SELFTEST_CONTROL_TEST_MASK  (SELFTEST_CONTROL_SPSEL_Msk | \
                                             SELFTEST_CONTROL_FPCA_Msk  | \
                                             SELFTEST_CONTROL_SFPA_Msk)
    #else
        #define SELFTEST_CONTROL_TEST_MASK  (SELFTEST_CONTROL_SPSEL_Msk | \
                                             SELFTEST_CONTROL_FPCA_Msk)
    #endif
#else
    #define SELFTEST_CONTROL_TEST_MASK      (SELFTEST_CONTROL_SPSEL_Msk)
#endif

/* The core can have an FPU, even if this image doesn't generate FPU code: FPCA can already be 1
 * when the test starts (mixed soft/softfp objects, or a Secure image without FPU code while the
 * Non-secure side uses the FPU), and pattern 1 keeps it. The size is therefore chosen by
 * architecture (Armv7E-M, Armv8-M Mainline), not only by __ARM_FP. */
#if (defined(__ARM_FP) && (__ARM_FP != 0)) || \
    (defined(__ARM_ARCH_ISA_THUMB) && (__ARM_ARCH_ISA_THUMB == 2) && defined(__ARM_ARCH) && \
    ((__ARM_ARCH >= 8) || (defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP != 0))))
#define SELFTEST_CONTROL_FPU_POSSIBLE       (1)
#else
#define SELFTEST_CONTROL_FPU_POSSIBLE       (0)
#endif

/* The test stack must hold the probe word, 8-byte alignment padding, and one
 * exception frame (NMI or HardFault while PSP is selected), rounded up to a
 * power of two:
 *  - basic frame (32 bytes): 40 bytes -> 64
 *  - extended FP frame (104 bytes): 112 bytes -> 128
 *  - basic frame + Secure additional state context (40 bytes): 80 bytes -> 128
 *  - extended FP frame + Secure context + S16-S31 (64 bytes): 216 bytes -> 256 */
#if (SELFTEST_CONTROL_FPU_POSSIBLE == 1) && \
    (defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3))
#define SELFTEST_CONTROL_TEST_STACK_SIZE    (256u)
#elif (SELFTEST_CONTROL_FPU_POSSIBLE == 1) || \
    (defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3))
#define SELFTEST_CONTROL_TEST_STACK_SIZE    (128u)
#else
#define SELFTEST_CONTROL_TEST_STACK_SIZE    (64u)
#endif
#define SELFTEST_CONTROL_TEST_STACK_DWORDS  (SELFTEST_CONTROL_TEST_STACK_SIZE / 8u)

/* uint64_t elements keep the stack 8-byte aligned, as AAPCS requires. */
static uint64_t stlControl_testStack[SELFTEST_CONTROL_TEST_STACK_DWORDS];

/*******************************************************************************
 * Function Name: SelfTest_CONTROL
 *******************************************************************************
 *
 * Summary:
 *  This function tests the implemented and writable CONTROL register bits and
 *  checks that SPSEL switches the active stack pointer.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test: OK_STATUS - pass; ERROR_STATUS - fail;
 *  ERROR_INCORRECT_USAGE_STATUS - not called from privileged Thread mode.
 *
 ******************************************************************************/
uint8_t SelfTest_CONTROL(void)
{
    uint8_t ret = ERROR_INCORRECT_USAGE_STATUS;

    /* The checks must run before interrupts are masked: CPSID is ignored in
     * unprivileged Thread mode. */
    if ((0UL == __get_IPSR()) && (0UL == (__get_CONTROL() & SELFTEST_CONTROL_nPRIV_Msk)))
    {
        uint32_t stackLimit = (uint32_t)(uintptr_t)&stlControl_testStack[0];
        uint32_t primask = __get_PRIMASK();

        __disable_irq();

        ret = SelfTest_CONTROL_Asm(stackLimit + SELFTEST_CONTROL_TEST_STACK_SIZE, stackLimit,
                                   SELFTEST_CONTROL_TEST_MASK);

        __set_PRIMASK(primask);
    }

    return ret;
}


/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_APSR_GCC.S
*
* Description: This file contains the function that performs the APSR
* register self-test.
*
* Hardware Dependency:
*  Cortex-M cores (Armv6-M, Armv7-M, Armv8-M, Armv8.1-M)
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

#include "SelfTest_ErrorInjection.h"

.syntax unified

.text
.thumb

/* MSR field mask that selects the APSR bits to write: the GE bits exist only with the DSP
 * extension */
#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
#define SELFTEST_APSR_FIELDS     APSR_nzcvqg
#else
#define SELFTEST_APSR_FIELDS     APSR_nzcvq
#endif

/* Return values. They must match OK_STATUS and ERROR_STATUS in SelfTest_common.h, which isn't
 * included here. */
.equ STL_PASS,          0
.equ STL_FAIL,          1

/* Pattern for the APSR bits (N = 0, Z = 1, C = 0, V = 1, Q = 0, GE = 0101). It is masked with
 * the bits implemented on the core. The N, Z, C, and V values are required by the conditional
 * branch checks. */
.equ APSR_PATTERN,      0x50050000

/* ERROR_IN_APSR_REGISTER (SelfTest_ErrorInjection.h) clears the Z flag in APSR after the flag
 * checks, before the read-back. Expected result: ERROR_STATUS, with APSR restored. */


/*----------------------------------------------------------------------------------*/
/* SelfTest_APSR_Asm                                                                */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Writes a pattern and its complement to the APSR bits in apsrMask. After each   */
/*   write, it reads APSR back, masks the result with apsrMask, and compares it     */
/*   with the written value. The N, Z, C, and V flags are also checked with         */
/*   conditional branches. Restores APSR and checks the restored value.             */
/*   The pattern is APSR_PATTERN & apsrMask (N = 0, Z = 1, C = 0, V = 1), and the   */
/*   complement pattern is apsrMask ^ pattern (N = 1, Z = 0, C = 1, V = 0). The     */
/*   branch checks depend on these flag values.                                     */
/*                                                                                  */
/* Parameters:                                                                      */
/*   R0 - apsrMask: APSR bits to test                                               */
/*                                                                                  */
/* Return:                                                                          */
/*   STL_PASS (0) - No error                                                        */
/*   STL_FAIL (1) - Error detected                                                  */
/*                                                                                  */
/* Register use:                                                                    */
/*   R3 - original APSR                                                             */
/*   R4 - apsrMask                                                                  */
/*   R5 - pattern                                                                   */
/*   R6 - complement pattern                                                        */
/*   R7 - failure flag                                                              */
/*----------------------------------------------------------------------------------*/

/* uint8_t SelfTest_APSR_Asm(uint32_t apsrMask); */
.global SelfTest_APSR_Asm
.func SelfTest_APSR_Asm, SelfTest_APSR_Asm
.type SelfTest_APSR_Asm, %function
.thumb_func
SelfTest_APSR_Asm:

    PUSH  {R4-R7, LR}
    MOVS  R4, R0
    MOVS  R7, #STL_PASS

    /* Pattern = APSR_PATTERN & apsrMask, complement pattern = apsrMask ^ pattern */
    LDR   R5, =APSR_PATTERN
    ANDS  R5, R5, R4
    MOVS  R6, R4
    EORS  R6, R6, R5

    /* Save APSR */
    MRS   R3, APSR

    /* Pattern: N = 0, Z = 1, C = 0, V = 1 */
    MSR   SELFTEST_APSR_FIELDS, R5
    BMI   apsr_test_fail
    BNE   apsr_test_fail
    BCS   apsr_test_fail
    BVC   apsr_test_fail
#if (ERROR_IN_APSR_REGISTER)
    MOVS  R1, #1
#endif
    MRS   R1, APSR
    ANDS  R1, R1, R4
    CMP   R1, R5
    BNE   apsr_test_fail

    /* Complement pattern: N = 1, Z = 0, C = 1, V = 0 */
    MSR   SELFTEST_APSR_FIELDS, R6
    BPL   apsr_test_fail
    BEQ   apsr_test_fail
    BCC   apsr_test_fail
    BVS   apsr_test_fail
    MRS   R1, APSR
    ANDS  R1, R1, R4
    CMP   R1, R6
    BNE   apsr_test_fail
    B     apsr_test_restore

apsr_test_fail:
    MOVS  R7, #STL_FAIL

apsr_test_restore:
    /* Restore APSR and check that the tested bits are back to their original values */
    MSR   SELFTEST_APSR_FIELDS, R3
    MRS   R0, APSR
    ANDS  R0, R0, R4
    ANDS  R3, R3, R4
    CMP   R0, R3
    BEQ   apsr_test_done
    MOVS  R7, #STL_FAIL

apsr_test_done:
    /* Return the failure flag: STL_PASS or STL_FAIL */
    MOVS  R0, R7
    POP   {R4-R7, PC}
    .ltorg

.endfunc
.size SelfTest_APSR_Asm, . - SelfTest_APSR_Asm

.end

/* [] END OF FILE */

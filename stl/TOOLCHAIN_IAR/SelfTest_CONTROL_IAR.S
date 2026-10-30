/*******************************************************************************
* File Name: SelfTest_CONTROL_IAR.S
*
* Description: This file contains the function that performs the CONTROL
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

  SECTION .text:CODE:ROOT(4)
  PUBLIC SelfTest_CONTROL_Asm
  THUMB

/* Armv8-M Mainline cores have a PSP stack limit register (PSPLIM).
   __CORE__ is used because the IAR assembler can't evaluate __ARM_ARCH in #if
   for Armv8.1-M, and doesn't define __ARM_ARCH_8M_MAIN__. */
#if defined(__CORE__) && ((__CORE__ == __ARM8M_MAINLINE__) || (__CORE__ == __ARM8EM_MAINLINE__))
#define CONTROL_TEST_PSPLIM     1
#else
#define CONTROL_TEST_PSPLIM     0
#endif

/* ERROR_IN_CONTROL_REGISTER and ERROR_IN_CONTROL_RESTORE (SelfTest_ErrorInjection.h) inject errors:
   - ERROR_IN_CONTROL_REGISTER: a CONTROL read-back error.
     Expected result: ERROR_STATUS, with CONTROL and the stacks restored.
   - ERROR_IN_CONTROL_RESTORE: a restore check error.
     Expected result: endless loop with interrupts disabled (watchdog reset). */


/*----------------------------------------------------------------------------------*/
/* SelfTest_CONTROL_Asm                                                             */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Tests the CONTROL bits in testMask with the patterns 0 and testMask, checks    */
/*   that SPSEL selects MSP and PSP, restores CONTROL, PSP (and PSPLIM), and checks */
/*   the restored state. CONTROL bits outside testMask keep their values.           */
/*   Must be called from privileged Thread mode with PRIMASK = 1.                   */
/*                                                                                  */
/* Parameters:                                                                      */
/*   R0 - stackTop: top of the test stack, 8-byte aligned                           */
/*   R1 - stackLimit: bottom of the test stack                                      */
/*   R2 - testMask: CONTROL bits to test, must contain SPSEL                        */
/*                                                                                  */
/* Return:                                                                          */
/*   0 - No error                                                                   */
/*   1 - Error detected                                                             */
/*   Does not return if the restored state can't be confirmed.                     */
/*                                                                                  */
/* Register use:                                                                    */
/*   R3 - stackLimit                                                                */
/*   R4 - original CONTROL                                                          */
/*   R5 - original active SP                                                        */
/*   R6 - original PSP                                                              */
/*   R7 - failure flag                                                              */
/*   R8 - original PSPLIM (Armv8-M Mainline only)                                   */
/*   R12 - stackTop                                                                 */
/*----------------------------------------------------------------------------------*/

/* uint8_t SelfTest_CONTROL_Asm(uint32_t stackTop, uint32_t stackLimit, uint32_t testMask); */
SelfTest_CONTROL_Asm:

#if CONTROL_TEST_PSPLIM
    PUSH  {R4-R8, LR}
#else
    PUSH  {R4-R7, LR}
#endif
    MOVS  R7, #0
    MOV   R12, R0
    MOVS  R3, R1

    /* Save CONTROL, the active SP, PSP and PSPLIM */
    MRS   R4, CONTROL
    MOV   R5, SP
    MRS   R6, PSP
#if CONTROL_TEST_PSPLIM
    MRS   R8, PSPLIM
#endif

    /* Pattern 0: tested bits cleared, SPSEL = 0 selects MSP */
    MOVS  R0, R4
    BICS  R0, R0, R2
    MSR   CONTROL, R0
    ISB
    MRS   R1, CONTROL
    ANDS  R1, R1, R2
#if (ERROR_IN_CONTROL_REGISTER)
    ADDS  R1, R1, #1
#endif
    CMP   R1, #0
    BNE   control_test_fail

    /* The active SP must be MSP */
    MOV   R1, SP
    MRS   R0, MSP
    CMP   R0, R1
    BNE   control_test_fail

    /* Point PSP (and PSPLIM) to the test stack while MSP is still active */
#if CONTROL_TEST_PSPLIM
    MSR   PSPLIM, R3
#endif
    MOV   R0, R12
    MSR   PSP, R0

    /* Seed the probe slot with stackTop, so a probe left by an earlier call can't pass */
    MOV   R1, R12
    SUBS  R1, R1, #4
    STR   R0, [R1]

    /* Pattern 1: tested bits set, SPSEL = 1 selects PSP */
    MOVS  R1, R4
    ORRS  R1, R1, R2
    MSR   CONTROL, R1
    ISB
    MRS   R1, CONTROL
    ANDS  R1, R1, R2
    CMP   R1, R2
    BNE   control_test_fail

    /* The active SP must be the test stack */
    MOV   R1, SP
    CMP   R1, R12
    BNE   control_test_fail

    /* Push a probe word and check that it was written to the test stack */
    MVNS  R0, R0
    PUSH  {R0}
    MOV   R1, R12
    SUBS  R1, R1, #4
    LDR   R1, [R1]
    CMP   R1, R0
    POP   {R1}
    BEQ   control_test_restore

control_test_fail:
    MOVS  R7, #1

control_test_restore:
    /* Back to MSP, then restore PSP, PSPLIM and the original CONTROL */
    MOVS  R0, R4
    BICS  R0, R0, R2
    MSR   CONTROL, R0
    ISB
    MSR   PSP, R6
#if CONTROL_TEST_PSPLIM
    MSR   PSPLIM, R8
#endif
    MSR   CONTROL, R4
    ISB

    /* Check that CONTROL, the active SP, PSP and PSPLIM are back to their original values */
    MRS   R0, CONTROL
#if (ERROR_IN_CONTROL_RESTORE)
    ADDS  R0, R0, #1
#endif
    CMP   R0, R4
    BNE   control_restore_fail
    MOV   R0, SP
    CMP   R0, R5
    BNE   control_restore_fail
    MRS   R0, PSP
    CMP   R0, R6
    BNE   control_restore_fail
#if CONTROL_TEST_PSPLIM
    MRS   R0, PSPLIM
    CMP   R0, R8
    BNE   control_restore_fail
#endif

    /* Return the failure flag: 0 - pass, 1 - fail */
    MOVS  R0, R7
#if CONTROL_TEST_PSPLIM
    POP   {R4-R8, PC}
#else
    POP   {R4-R7, PC}
#endif

    /* The stack can't be trusted: stay here with interrupts disabled */
control_restore_fail:
    CPSID I
control_safe_state:
    B     control_safe_state

  END

/* [] END OF FILE */
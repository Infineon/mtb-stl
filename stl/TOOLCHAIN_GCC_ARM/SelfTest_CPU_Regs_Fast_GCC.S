/*******************************************************************************
* File Name: SelfTest_CPU_Regs_Fast_GCC.S
* Version 1.0.0
*
* Description: This file checks one CPU register per call.
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

.syntax unified
.thumb

.equ ERROR_IN_CPU_REGISTERS_FAST, 0x00u

#if defined(STL_CPU_FAST_RUN_FROM_RAM) && defined(STL_CPU_FAST_RUN_FROM_ITCM)
#error "Select only one CPU step execution section"
#elif defined(STL_CPU_FAST_RUN_FROM_ITCM)
.section .cy_itcm, "ax", %progbits
#elif defined(STL_CPU_FAST_RUN_FROM_RAM)
.section .cy_ramfunc, "ax", %progbits
#else
.text
#endif

.macro SP_CHECKERBOARD fail
    MOV   R1, SP
    LDR   R0, =0xAAAAAAA8
    MOV   SP, R0
    CMP   SP, R0
    MOV   SP, R1
    BNE   \fail
    LDR   R0, =0x55555554
    MOV   SP, R0
    CMP   SP, R0
    MOV   SP, R1
    BNE   \fail
.endm

.macro REG_CHECKERBOARD reg, fail
    MOV   R1, \reg
    LDR   R0, =0xAAAAAAAA
    MOV   \reg, R0
    CMP   \reg, R0
    MOV   \reg, R1
    BNE   \fail
    LDR   R0, =0x55555555
    MOV   \reg, R0
    CMP   \reg, R0
    MOV   \reg, R1
    BNE   \fail
.endm

.equ CPU_STEP_SIGNATURE_BASE, 0x40
#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
.equ CPU_STEP_LAST, 17
#else
.equ CPU_STEP_LAST, 16
#endif

/*----------------------------------------------------------------------------------*/
/* SelfTest_CPU_Registers_Fast                                                      */
/*----------------------------------------------------------------------------------*/
/* Summary:                                                                         */
/*   Tests one CPU register per call.                                               */
/* Parameters:                                                                      */
/*   R0 - Pointer to the last attempted step; initialize to 0xFF.                   */
/* Return:                                                                          */
/*   R0 - 0 on pass, 1 on failure; records the tested step on either result.        */
/* Destroys:                                                                        */
/*   R0-R2 and NZCV flags; R3-R7 and LR are restored.                               */
/*----------------------------------------------------------------------------------*/
.global SelfTest_CPU_Registers_Fast
.type SelfTest_CPU_Registers_Fast, %function
.thumb_func
SelfTest_CPU_Registers_Fast:
    CMP   R0, #0
    BNE   fast_valid
    MOVS  R0, #1
    BX    LR
fast_valid:
    /* Preserve the caller's registers and interrupt mask across one phase. */
    PUSH  {R3-R7, LR}
    MOV   R4, R0
    MRS   R5, PRIMASK
    CPSID i
    LDRB  R6, [R4]
    CMP   R6, #0xFF
    BEQ   fast_sp
    CMP   R6, #CPU_STEP_LAST
    BHI   fast_invalid
    BEQ   fast_sp
    ADDS  R6, R6, #1
    B     fast_dispatch
fast_sp:
    /* Test SP register. */
    MOVS  R6, #0
    SP_CHECKERBOARD fast_fail
    MOVS  R0, #CPU_STEP_SIGNATURE_BASE
fast_verify:
    /* Report PASS only if the phase returned its expected signature. */
    MOVS  R7, #CPU_STEP_SIGNATURE_BASE
    ADDS  R7, R7, R6
    CMP   R0, R7
    BNE   fast_fail
    STRB  R6, [R4]
    MOVS  R0, #0
    MSR   PRIMASK, R5
    POP   {R3-R7, PC}
fast_fail:
    /* Record the attempted phase before reporting a test failure. */
    STRB  R6, [R4]
fast_invalid:
    MOVS  R0, #1
    MSR   PRIMASK, R5
    POP   {R3-R7, PC}
fast_dispatch:
    ADR   R3, fast_table
    LSLS  R2, R6, #2
    LDR   R3, [R3, R2]
    BX    R3

.macro INLINE_REG name, signature
fast_\name:
    REG_CHECKERBOARD \name, fast_\name\()_fail
    MOVS  R0, #\signature
    B     fast_verify
fast_\name\()_fail:
    B     fast_fail
.endm

/* R8 register test. */
INLINE_REG R8, 0x41
/* R9 register test. */
INLINE_REG R9, 0x42
/* R10 register test. */
INLINE_REG R10, 0x43
/* R11 register test. */
INLINE_REG R11, 0x44
/* R12 register test. */
INLINE_REG R12, 0x45
/* LR register test. */
INLINE_REG LR, 0x46

/* R0 register test. */
fast_R0:
    LDR   R1, =0xAAAAAAAA
    MOV   R0, R1
    CMP   R0, R1
    BNE   fast_R0_fail
    LDR   R1, =0x55555555
    MOV   R0, R1
    CMP   R0, R1
    BNE   fast_R0_fail
    MOVS  R0, #0x47
    B     fast_verify
fast_R0_fail:
    B     fast_fail

/* R1 register test. */
fast_R1:
    LDR   R0, =0xAAAAAAAA
    MOV   R1, R0
.if (ERROR_IN_CPU_REGISTERS_FAST)
    ADDS  R1, R1, #1
.endif
    CMP   R1, R0
    BNE   fast_R1_fail
    LDR   R0, =0x55555555
    MOV   R1, R0
    CMP   R1, R0
    BNE   fast_R1_fail
    MOVS  R0, #0x48
    B     fast_verify
fast_R1_fail:
    B     fast_fail

/* R2 register test. */
INLINE_REG R2, 0x49
/* R3 register test. */
INLINE_REG R3, 0x4A
/* R4 register test. */
INLINE_REG R4, 0x4B
/* R5 register test. */
INLINE_REG R5, 0x4C
/* R6 register test. */
INLINE_REG R6, 0x4D
/* R7 register test. */
INLINE_REG R7, 0x4E

/* PRIMASK test. */
fast_primask:
    MOVS  R0, #0x1
    MSR   PRIMASK, R0
    MRS   R2, PRIMASK
    CMP   R2, #0x1
    BNE   fast_primask_fail

    MOVS  R0, #0x0
    MSR   PRIMASK, R0
    MRS   R2, PRIMASK

    CPSID i
    CMP   R2, #0x0
    BNE   fast_primask_fail

    MOVS  R0, #0x4F
    B     fast_verify
fast_primask_fail:
    B     fast_fail

/* CONTROL test. */
fast_control:
    MRS   R0, PSP
    CMP   R0, #0
    BEQ   fast_control_no_psp
    MRS   R2, CONTROL

    MOVS  R0, #0x2
    MSR   CONTROL, R0
    MRS   R1, CONTROL
    CMP   R1, #0x2
    BNE   fast_control_fail

    MOVS  R0, #0x0
    MSR   CONTROL, R0
    MRS   R1, CONTROL
    CMP   R1, #0x0
    BNE   fast_control_fail

    MSR   CONTROL, R2
    MOVS  R0, #0x50
    B     fast_verify
fast_control_no_psp:
    B     fast_fail
fast_control_fail:
    MSR   CONTROL, R2
    B     fast_fail

#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
/* APSR flags test. */
fast_apsr:
    MRS   R2, APSR
    LDR   R0, =0x50050000
    MSR   APSR_nzcvqg, R0
    BNE   fast_apsr_fail
    BMI   fast_apsr_fail
    BVC   fast_apsr_fail
    BCS   fast_apsr_fail
    MRS   R1, APSR
    CMP   R1, R0
    BNE   fast_apsr_fail
    LDR   R0, =0xA80A0000
    MSR   APSR_nzcvqg, R0
    BEQ   fast_apsr_fail
    BPL   fast_apsr_fail
    BVS   fast_apsr_fail
    BCC   fast_apsr_fail
    MRS   R1, APSR
    CMP   R1, R0
    BNE   fast_apsr_fail
    MSR   APSR_nzcvqg, R2
    MOVS  R0, #0x51
    B     fast_verify
fast_apsr_fail:
    MSR   APSR_nzcvqg, R2
    B     fast_fail
#endif

    .align 2
fast_table:
    .word fast_sp + 1
    .word fast_R8 + 1
    .word fast_R9 + 1
    .word fast_R10 + 1
    .word fast_R11 + 1
    .word fast_R12 + 1
    .word fast_LR + 1
    .word fast_R0 + 1
    .word fast_R1 + 1
    .word fast_R2 + 1
    .word fast_R3 + 1
    .word fast_R4 + 1
    .word fast_R5 + 1
    .word fast_R6 + 1
    .word fast_R7 + 1
    .word fast_primask + 1
    .word fast_control + 1
#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
    .word fast_apsr + 1
#endif
    .ltorg
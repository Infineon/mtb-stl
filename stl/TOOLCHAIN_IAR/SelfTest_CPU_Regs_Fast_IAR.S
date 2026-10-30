/*******************************************************************************
* File Name: SelfTest_CPU_Regs_Fast_IAR.S
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

#define ERROR_IN_CPU_REGISTERS_FAST 0x00

#if defined(STL_CPU_FAST_RUN_FROM_RAM) && defined(STL_CPU_FAST_RUN_FROM_ITCM)
#error "Select only one CPU step execution section"
#elif defined(STL_CPU_FAST_RUN_FROM_ITCM)
    SECTION .cy_itcm:CODE:ROOT(4)
#elif defined(STL_CPU_FAST_RUN_FROM_RAM)
    SECTION .cy_ramfunc:CODE:ROOT(4)
#else
    SECTION .text:CODE:ROOT(4)
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
    PUBLIC SelfTest_CPU_Registers_Fast
    THUMB

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
#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
    CMP   R6, #17
#else
    CMP   R6, #16
#endif
    BHI   fast_invalid
    BEQ   fast_sp
    ADDS  R6, R6, #1
    B     fast_dispatch
fast_sp:
    /* Test SP register. */
    MOVS  R6, #0
    MOV   R1, SP
    LDR   R0, =0xAAAAAAA8
    MOV   SP, R0
    CMP   SP, R0
    MOV   SP, R1
    BNE   fast_fail
    LDR   R0, =0x55555554
    MOV   SP, R0
    CMP   SP, R0
    MOV   SP, R1
    BNE   fast_fail
    MOVS  R0, #0x40
fast_verify:
    /* Report PASS only if the phase returned its expected signature. */
    MOVS  R7, #0x40
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
    LDR   R3, =fast_table
    LSLS  R2, R6, #2
    LDR   R3, [R3, R2]
    MOVS  R0, #1
    ORRS  R3, R0
    BX    R3

/* R8 register test. */
fast_R8:
    MOV   R1, R8
    LDR   R0, =0xAAAAAAAA
    MOV   R8, R0
    CMP   R8, R0
    MOV   R8, R1
    BNE   fast_R8_fail
    LDR   R0, =0x55555555
    MOV   R8, R0
    CMP   R8, R0
    MOV   R8, R1
    BNE   fast_R8_fail
    MOVS  R0, #0x41
    B     fast_verify
fast_R8_fail:
    B     fast_fail

/* R9 register test. */
fast_R9:
    MOV   R1, R9
    LDR   R0, =0xAAAAAAAA
    MOV   R9, R0
    CMP   R9, R0
    MOV   R9, R1
    BNE   fast_R9_fail
    LDR   R0, =0x55555555
    MOV   R9, R0
    CMP   R9, R0
    MOV   R9, R1
    BNE   fast_R9_fail
    MOVS  R0, #0x42
    B     fast_verify
fast_R9_fail:
    B     fast_fail

/* R10 register test. */
fast_R10:
    MOV   R1, R10
    LDR   R0, =0xAAAAAAAA
    MOV   R10, R0
    CMP   R10, R0
    MOV   R10, R1
    BNE   fast_R10_fail
    LDR   R0, =0x55555555
    MOV   R10, R0
    CMP   R10, R0
    MOV   R10, R1
    BNE   fast_R10_fail
    MOVS  R0, #0x43
    B     fast_verify
fast_R10_fail:
    B     fast_fail

/* R11 register test. */
fast_R11:
    MOV   R1, R11
    LDR   R0, =0xAAAAAAAA
    MOV   R11, R0
    CMP   R11, R0
    MOV   R11, R1
    BNE   fast_R11_fail
    LDR   R0, =0x55555555
    MOV   R11, R0
    CMP   R11, R0
    MOV   R11, R1
    BNE   fast_R11_fail
    MOVS  R0, #0x44
    B     fast_verify
fast_R11_fail:
    B     fast_fail

/* R12 register test. */
fast_R12:
    MOV   R1, R12
    LDR   R0, =0xAAAAAAAA
    MOV   R12, R0
    CMP   R12, R0
    MOV   R12, R1
    BNE   fast_R12_fail
    LDR   R0, =0x55555555
    MOV   R12, R0
    CMP   R12, R0
    MOV   R12, R1
    BNE   fast_R12_fail
    MOVS  R0, #0x45
    B     fast_verify
fast_R12_fail:
    B     fast_fail

/* LR register test. */
fast_LR:
    MOV   R1, LR
    LDR   R0, =0xAAAAAAAA
    MOV   LR, R0
    CMP   LR, R0
    MOV   LR, R1
    BNE   fast_LR_fail
    LDR   R0, =0x55555555
    MOV   LR, R0
    CMP   LR, R0
    MOV   LR, R1
    BNE   fast_LR_fail
    MOVS  R0, #0x46
    B     fast_verify
fast_LR_fail:
    B     fast_fail

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
#if (ERROR_IN_CPU_REGISTERS_FAST)
    ADDS  R1, R1, #1
#endif
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
fast_R2:
    MOV   R1, R2
    LDR   R0, =0xAAAAAAAA
    MOV   R2, R0
    CMP   R2, R0
    MOV   R2, R1
    BNE   fast_R2_fail
    LDR   R0, =0x55555555
    MOV   R2, R0
    CMP   R2, R0
    MOV   R2, R1
    BNE   fast_R2_fail
    MOVS  R0, #0x49
    B     fast_verify
fast_R2_fail:
    B     fast_fail

/* R3 register test. */
fast_R3:
    MOV   R1, R3
    LDR   R0, =0xAAAAAAAA
    MOV   R3, R0
    CMP   R3, R0
    MOV   R3, R1
    BNE   fast_R3_fail
    LDR   R0, =0x55555555
    MOV   R3, R0
    CMP   R3, R0
    MOV   R3, R1
    BNE   fast_R3_fail
    MOVS  R0, #0x4A
    B     fast_verify
fast_R3_fail:
    B     fast_fail

/* R4 register test. */
fast_R4:
    MOV   R1, R4
    LDR   R0, =0xAAAAAAAA
    MOV   R4, R0
    CMP   R4, R0
    MOV   R4, R1
    BNE   fast_R4_fail
    LDR   R0, =0x55555555
    MOV   R4, R0
    CMP   R4, R0
    MOV   R4, R1
    BNE   fast_R4_fail
    MOVS  R0, #0x4B
    B     fast_verify
fast_R4_fail:
    B     fast_fail

/* R5 register test. */
fast_R5:
    MOV   R1, R5
    LDR   R0, =0xAAAAAAAA
    MOV   R5, R0
    CMP   R5, R0
    MOV   R5, R1
    BNE   fast_R5_fail
    LDR   R0, =0x55555555
    MOV   R5, R0
    CMP   R5, R0
    MOV   R5, R1
    BNE   fast_R5_fail
    MOVS  R0, #0x4C
    B     fast_verify
fast_R5_fail:
    B     fast_fail

/* R6 register test. */
fast_R6:
    MOV   R1, R6
    LDR   R0, =0xAAAAAAAA
    MOV   R6, R0
    CMP   R6, R0
    MOV   R6, R1
    BNE   fast_R6_fail
    LDR   R0, =0x55555555
    MOV   R6, R0
    CMP   R6, R0
    MOV   R6, R1
    BNE   fast_R6_fail
    MOVS  R0, #0x4D
    B     fast_verify
fast_R6_fail:
    B     fast_fail

/* R7 register test. */
fast_R7:
    MOV   R1, R7
    LDR   R0, =0xAAAAAAAA
    MOV   R7, R0
    CMP   R7, R0
    MOV   R7, R1
    BNE   fast_R7_fail
    LDR   R0, =0x55555555
    MOV   R7, R0
    CMP   R7, R0
    MOV   R7, R1
    BNE   fast_R7_fail
    MOVS  R0, #0x4E
    B     fast_verify
fast_R7_fail:
    B     fast_fail

/* PRIMASK test. */
fast_primask:
    MOVS  R0, #1
    MSR   PRIMASK, R0
    MRS   R2, PRIMASK
    CMP   R2, #1
    BNE   fast_primask_fail
    MOVS  R0, #0
    MSR   PRIMASK, R0
    MRS   R2, PRIMASK

    CPSID i
    CMP   R2, #0
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

    LTORG
    SECTION .rodata:CONST:ROOT(2)
fast_table:
    DC32 fast_sp, fast_R8, fast_R9, fast_R10, fast_R11
    DC32 fast_R12, fast_LR, fast_R0, fast_R1, fast_R2
    DC32 fast_R3, fast_R4, fast_R5, fast_R6, fast_R7
    DC32 fast_primask, fast_control
#if defined(__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1)
    DC32 fast_apsr
#endif

    END
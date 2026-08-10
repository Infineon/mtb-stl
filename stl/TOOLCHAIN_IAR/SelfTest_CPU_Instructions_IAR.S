/*******************************************************************************
* File Name: SelfTest_CPU_Instructions_IAR.S
* Version 1.0.0
*
* Description: This file contains functions to perform CPU instructions self
* test for IAR toolchain.
*
* Related Document:
*
* Hardware Dependency:
*  PSoC 4100S Device
*  PSoC 4100S Plus 256k Device
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
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

        SECTION .text:CODE:NOROOT:REORDER(2)
        PUBLIC  SelfTest_CPU_Instructions_IAR

/*----------------------------------------------------------------------------------*/
/* SelfTest_CPU_Instructions_IAR                                                    */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function performs basic checks on CPU logic and arithmetic instructions.  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   None.                                                                          */
/*                                                                                  */
/* Return:                                                                          */
/*  0 - No error                                                                    */
/*  Not 0 - Error detected                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0                                                                             */
/*----------------------------------------------------------------------------------*/
SelfTest_CPU_Instructions_IAR:
        PUSH    {R1-R7, LR}

        /* Initialize r0 to 1 (assume FAIL) */
        MOVS    R0, #1

        /* --- ADD test: 1 + 2 = 3 --- */
        MOVS    R1, #1
        MOVS    R2, #2
        MOVS    R3, R1
        ADDS    R3, R2
        CMP     R3, #3
        BNE     cpu_test_fail

        /* --- SUB test: 5 - 3 = 2 --- */
        MOVS    R1, #5
        MOVS    R2, #3
        MOVS    R3, R1
        SUBS    R3, R2
        CMP     R3, #2
        BNE     cpu_test_fail

        /* --- AND test: 0xAA & 0x55 = 0x00 --- */
        LDR     R1, =0xAA
        LDR     R2, =0x55
        MOV     R3, R1
        ANDS    R3, R2
        CMP     R3, #0
        BNE     cpu_test_fail

        /* --- ORR test: 0xAA | 0x55 = 0xFF --- */
        LDR     R1, =0xAA
        LDR     R2, =0x55
        MOV     R3, R1
        ORRS    R3, R2
        LDR     R4, =0xFF
        CMP     R3, R4
        BNE     cpu_test_fail

        /* --- EOR test: 0xF0 ^ 0x0F = 0xFF --- */
        LDR     R1, =0xF0
        LDR     R2, =0x0F
        MOV     R3, R1
        EORS    R3, R2
        LDR     R4, =0xFF
        CMP     R3, R4
        BNE     cpu_test_fail

        /* --- MOV/MVN test: ~0x0F = 0xFFFFFFF0 (for 32-bit) --- */
        LDR     R1, =0x0F
        MVNS    R2, R1
        LDR     R3, =0xFFFFFFF0
        CMP     R2, R3
        BNE     cpu_test_fail

        /* --- LSL test: 1 << 3 = 8 --- */
        MOVS    R1, #1
        LSLS    R2, R1, #3
        CMP     R2, #8
        BNE     cpu_test_fail

        /* --- LSR test: 0x80 >> 7 = 1 --- */
        LDR     R1, =0x80
        LSRS    R2, R1, #7
        CMP     R2, #1
        BNE     cpu_test_fail

        /* --- ASR test: 0xFFFFFFF0 >> 4 = 0xFFFFFFFF (arithmetic shift) --- */
        LDR     R1, =0xFFFFFFF0
        ASRS    R2, R1, #4
        LDR     R3, =0xFFFFFFFF
        CMP     R2, R3
        BNE     cpu_test_fail

        /* --- CMP/CMN test: 5 == 5, 5 + 5 == 10 --- */
        MOVS    R1, #5
        MOVS    R2, #5
        CMP     R1, R2
        BNE     cpu_test_fail
        MOVS    R3, R1
        ADDS    R3, R2
        CMP     R3, #10
        BNE     cpu_test_fail

        /* --- Addition with carry test: 0xFFFFFFFF + 1 = 0 (overflow to 0) --- */
        MOVS    R4, #0
        SUBS    R4, R4, #1         ; Clear carry (C=0): 0 - 1 causes borrow
        LDR     R1, =0xFFFFFFFF
        MOVS    R2, #1
        MOVS    R3, R1
        ADCS    R3, R2             ; 0xFFFFFFFF + 1 + 0 = 0
        CMP     R3, #0
        BNE     cpu_test_fail
        /* --- Subtraction with carry test: 1 - 1 = 0 (with carry) --- */
        MOVS    R4, #0
        CMP     R4, R4             ; Set carry (C=1): 0 >= 0, no borrow
        MOVS    R1, #1
        MOVS    R2, #1
        MOVS    R3, R1
        SBCS    R3, R2             ; 1 - 1 - 0 = 0
        CMP     R3, #0
        BNE     cpu_test_fail
        MOVS    R4, #0
        SUBS    R4, R4, R4         ; Clean carry for next tests

        /* --- ROR test: 0x80000001 ror 1 = 0xC0000000 --- */
        LDR     R1, =0x80000001
        LSRS    R2, R1, #1         ; R2 = R1 >> 1
        LSLS    R4, R1, #31        ; R4 = R1 << 31
        ORRS    R2, R4             ; R2 = (R1 >> 1) | (R1 << 31)
        LDR     R3, =0xC0000000
        CMP     R2, R3
        BNE     cpu_test_fail

        /* --- BIC test: 0xFF & ~0xF0 = 0x0F --- */
        LDR     R1, =0xFF
        LDR     R2, =0xF0
        MOV     R3, R1
        BICS    R3, R2
        LDR     R4, =0x0F
        CMP     R3, R4
        BNE     cpu_test_fail

        /* --- REV test: 0x12345678 -> 0x78563412 --- */
#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
        LDR     R1, =0x12345678
        REV     R2, R1
        LDR     R3, =0x78563412
        CMP     R2, R3
        BNE     cpu_test_fail
#endif

        /* --- CLZ test: 0x0000F000 = 16 leading zeros --- */
#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
        LDR     R1, =0x0000F000
        CLZ     R2, R1
        CMP     R2, #16
        BNE     cpu_test_fail
#endif

        /* --- Passed all tests --- */
        MOVS    R0, #0
        B       cpu_test_end

cpu_test_fail
        /* r0 already 1, just return */

cpu_test_end
        POP     {R1, R2, R3, R4, R5, R6, R7, PC}

        END

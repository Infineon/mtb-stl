;-------------------------------------------------------------------------------
; FILENAME: SelfTest_CPU_Regs_ARM.s
; Version 1.1
;
;  DESCRIPTION:
;    Assembly routines for ARM compiler.
;
;  Owner:
;   JOBI
;
;  Related Document:
;   AN89056: PSoC 4 - IEC 60730 Class B and IEC 61508 Safety Software Library
;
; Hardware Dependency:
;  CY8C40XX, CY8C42XX, CY8C42XXM, CY8C42XXL, CY8C40XXS, CY8C41XXS and
;  CY8C41XXS Plus Devices
;
;-------------------------------------------------------------------------------
; (c) (2018-2025), Infineon Technologies AG, or an affiliate of Infineon
; Technologies AG. All rights reserved.
; This software, associated documentation and materials ("Software") is
; owned by Infineon Technologies AG or one of its affiliates ("Infineon")
; and is protected by and subject to worldwide patent protection, worldwide
; copyright laws, and international treaty provisions. Therefore, you may use
; this Software only as provided in the license agreement accompanying the
; software package from which you obtained this Software. If no license
; agreement applies, then any use, reproduction, modification, translation, or
; compilation of this Software is prohibited without the express written
; permission of Infineon.
;
; Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
; IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
; THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
; SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
; Infineon reserves the right to make changes to the Software without notice.
; You are responsible for properly designing, programming, and testing the
; functionality and safety of your intended application of the Software, as
; well as complying with any legal requirements related to its use. Infineon
; does not guarantee that the Software will be free from intrusion, data theft
; or loss, or other breaches ("Security Breaches"), and Infineon shall have
; no liability arising out of any Security Breaches. Unless otherwise
; explicitly approved by Infineon, the Software may not be used in any
; application where a failure of the Product or any consequences of the use
; thereof can reasonably be expected to result in personal injury.
;-------------------------------------------------------------------------------

    AREA |.text|,CODE
    THUMB

; Set this const to 0x01 to generate error for registers test
ERROR_IN_CPU_REGISTERS      EQU     0x00

;-------------------------------------------------------------------------------
; Function Name: SelfTest_CPU_Regs_ARM
;-------------------------------------------------------------------------------
; Summary:
;   Tests all ARM M0 CPU registers except PC.
;
; Parameters:
;   None
;
; Return:
;   void.
;
;-------------------------------------------------------------------------------
; void SelfTest_CPU_Regs_ARM(void)
SelfTest_CPU_Regs_ARM FUNCTION

    EXPORT SelfTest_CPU_Regs_ARM

        PUSH  {R1-R7, LR}

        ; Test SP register
        ; Note: SP ignores writes to the lower two bits,
        ; so they are intentionally set to 0

        ; Save SP to R1
        MOV   R1, SP

        ; Load SP with 0xAAAAAAA8
        LDR   R0, =0xAAAAAAA8
        MOV   SP, R0
        CMP   SP, R0

        ; Restore SP and return failure
        MOV   SP, R1
        BNE   __test_stack_fail

        ;Load SP with 0x55555554
        LDR   R0, =0x55555554
        MOV   SP, R0
        CMP   SP, R0

        ; Restore SP and return failure
        MOV   SP, R1
        BNE   __test_stack_fail

        B     __test_stack_ok

__test_stack_fail
        B    __test_asm_fail

__test_stack_ok
        ; R8 registers test
        MOV   R1, R8
        LDR   R0, =0xAAAAAAAA
        MOV   R8, R0
        CMP   R8, R0
        MOV   R8, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   R8, R0
        CMP   R8, R0
        MOV   R8, R1
        BNE   __test_high_reg_fail

        ; R9 registers test
        MOV   R1, R9
        LDR   R0, =0xAAAAAAAA
        MOV   R9, R0
        CMP   R9, R0
        MOV   R9, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   R9, R0
        CMP   R9, R0
        MOV   R9, R1
        BNE   __test_high_reg_fail

        ; R10 registers test
        MOV   R1, R10
        LDR   R0, =0xAAAAAAAA
        MOV   R10, R0
        CMP   R10, R0
        MOV   R10, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   R10, R0
        CMP   R10, R0
        MOV   R10, R1
        BNE   __test_high_reg_fail

        ; R11 registers test
        MOV   R1, R11
        LDR   R0, =0xAAAAAAAA
        MOV   R11, R0
        CMP   R11, R0
        MOV   R11, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   R11, R0
        CMP   R11, R0
        MOV   R11, R1
        BNE   __test_high_reg_fail

        ; R12 registers test
        MOV   R1, R12
        LDR   R0, =0xAAAAAAAA
        MOV   R12, R0
        CMP   R12, R0
        MOV   R12, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   R12, R0
        CMP   R12, R0
        MOV   R12, R1
        BNE   __test_high_reg_fail

        ; LR registers test
        MOV   R1, LR
        LDR   R0, =0xAAAAAAAA
        MOV   LR, R0
        CMP   LR, R0
        MOV   LR, R1
        BNE   __test_high_reg_fail

        LDR   R0, =0x55555555
        MOV   LR, R0
        CMP   LR, R0
        MOV   LR, R1
        BNE   __test_high_reg_fail

        B     __test_high_reg_ok

__test_high_reg_fail
        B    __test_asm_fail

__test_high_reg_ok
        ; R0 registers test
        LDR   R1, =0xAAAAAAAA
        MOV   R0, R1
        CMP   R0, R1
        BNE   __test_asm_fail

        LDR   R1, =0x55555555
        MOV   R0, R1
        CMP   R0, R1
        BNE   __test_asm_fail

        ; R1 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R1, R0

        IF (ERROR_IN_CPU_REGISTERS == 1)
        ADDS  R1, R1, #0x01
        MOV   R1, R1
        ENDIF

        CMP   R1, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R1, R0
        CMP   R1, R0
        BNE   __test_asm_fail

        ; R2 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R2, R0
        CMP   R2, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R2, R0
        CMP   R2, R0
        BNE   __test_asm_fail

        ; R3 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R3, R0
        CMP   R3, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R3, R0
        CMP   R3, R0
        BNE   __test_asm_fail

        ; R4 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R4, R0
        CMP   R4, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R4, R0
        CMP   R4, R0
        BNE   __test_asm_fail

        ; R5 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R5, R0
        CMP   R5, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R5, R0
        CMP   R5, R0
        BNE   __test_asm_fail

        ; R6 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R6, R0
        CMP   R6, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R6, R0
        CMP   R6, R0
        BNE   __test_asm_fail

        ; R7 registers test
        LDR   R0, =0xAAAAAAAA
        MOV   R7, R0
        CMP   R7, R0
        BNE   __test_asm_fail

        LDR   R0, =0x55555555
        MOV   R7, R0
        CMP   R7, R0
        BNE   __test_asm_fail

        ; Return success
        MOVS   R0, #0
        POP   {R1-R7, PC}

        ; Return failure
__test_asm_fail
        MOVS   R0, #0x01
        POP   {R1-R7, PC}

    ENDFUNC

    END
;[] END OF FILE
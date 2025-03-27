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
; Copyright (2018-2019), Cypress Semiconductor Corporation.
;-------------------------------------------------------------------------------
; This software is owned by Cypress Semiconductor Corporation (Cypress) and is
; protected by and subject to worldwide patent protection (United States and
; foreign), United States copyright laws and international treaty provisions.
; Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
; license to copy, use, modify, create derivative works of, and compile the
; Cypress Source Code and derivative works for the sole purpose of creating
; custom software in support of licensee product to be used only in conjunction
; with a Cypress integrated circuit as specified in the applicable agreement.
; Any reproduction, modification, translation, compilation, or representation of
; this software except as specified above is prohibited without the express
; written permission of Cypress.
;
; Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
; REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
; Cypress reserves the right to make changes without further notice to the
; materials described herein. Cypress does not assume any liability arising out
; of the application or use of any product or circuit described herein. Cypress
; does not authorize its products for use as critical components in life-support
; systems where a malfunction or failure may reasonably be expected to result in
; significant injury to the user. The inclusion of Cypress product in a life
; support systems application implies that the manufacturer assumes all risk of
; such use and in doing so indemnifies Cypress against all charges. Use may be
; limited by and subject to the applicable Cypress software license agreement.
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
;-------------------------------------------------------------------------------
; FILENAME: SelfTest_FPU_Regs_ARM.s
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
ERROR_IN_FPU_REGISTERS EQU 0x00


;---------------------------------------------------------------------------------- 
; SelfTest_FPU_Regs_GCC                                                             
;---------------------------------------------------------------------------------- 
;                                                                                   
; Summary:                                                                          
;   This function performs checkerboard test for all FPU registers for PSoC 6        
;   and XMC                                                                         
;                                                                                   
; Parameters:                                                                       
;   None.                                                                           
;                                                                                   
; Return:                                                                           
;  0 - No error                                                                     
;  Not 0 - Error detected                                                           
;                                                                                   
; Destroys:                                                                         
;   r0                                                                              
;---------------------------------------------------------------------------------- 

; uint8 SelfTest_FPU_Regs_GCC(void);  

SelfTest_FPU_Regs_GCC FUNCTION
    EXPORT SelfTest_FPU_Regs_GCC
    PUSH      {R1, R2, R3, LR}
    VPUSH.32  {S0,S1}
    VMRS      R1, FPSCR

;Assuming that the CPU test is successfull.
; Very first time the S0 is checked using the CPU registers as we are now aware that the 
; FPU register are working properly or not.


    ; S0 registers test  
    MOV    R0, #0xAAAAAAAA
    VMOV   S0, R0
    VMOV   R2, S0
    CMP    R2, R0
    BNE.w    __test_asm_fail

    MOV    R0, #0x55555555
    VMOV   S0, R0
    VMOV   R2, S0
    CMP    R2, R0
    BNE.w    __test_asm_fail


    ; S1 registers test  
    MOV    R0, #0xAAAAAAAA
    VMOV   S1, R0
    VMOV   R2, S1
    CMP    R2, R0
    BNE.w    __test_asm_fail

    MOV    R0, #0x55555555
    VMOV   S1, R0
    VMOV   R2, S1
    CMP    R2, R0
    BNE.w    __test_asm_fail

    ; S2 registers test  
    VMOV.F32      S0, S2
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S2, S1
    VCMP.F32  S2, S1
    VMOV.F32      S2,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S2, S1
    VCMP.F32   S2, S1
    VMOV.F32       S2, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S3 registers test  
    VMOV.F32      S0, S3
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S3, S1
    VCMP.F32  S3, S1
    VMOV.F32      S3,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S3, S1
    VCMP.F32   S3, S1
    VMOV.F32       S3, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S4 registers test  
    VMOV.F32      S0, S4
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S4, S1
    VCMP.F32  S4, S1
    VMOV.F32      S4,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S4, S1
    VCMP.F32   S4, S1
    VMOV.F32       S4, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S5 registers test  
    VMOV.F32      S0, S5
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S5, S1
    VCMP.F32.F32  S5, S1
    VMOV.F32      S5,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S5, S1
    VCMP.F32   S5, S1
    VMOV.F32       S5, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail


    ; S6 registers test  
    VMOV.F32      S0, S6
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S6, S1
    VCMP.F32  S6, S1
    VMOV.F32      S6,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S6, S1
    VCMP.F32   S6, S1
    VMOV.F32       S6, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S7 registers test  
    VMOV.F32      S0, S7
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S7, S1
    VCMP.F32  S7, S1
    VMOV.F32      S7,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S7, S1
    VCMP.F32   S7, S1
    VMOV.F32       S7, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S8 registers test  
    VMOV.F32      S0, S8
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S8, S1
    VCMP.F32  S8, S1
    VMOV.F32      S8,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S8, S1
    VCMP.F32   S8, S1
    VMOV.F32       S8, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S9 registers test  
    VMOV.F32      S0, S9
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S9, S1
    VCMP.F32  S9, S1
    VMOV.F32      S9,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S9, S1
    VCMP.F32   S9, S1
    VMOV.F32       S9, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S10 registers test  
    VMOV.F32      S0, S10
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S10, S1
    VCMP.F32  S10, S1
    VMOV.F32      S10,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S10, S1
    VCMP.F32   S10, S1
    VMOV.F32       S10, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S11 registers test  
    VMOV.F32      S0, S11
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S11, S1
    VCMP.F32  S11, S1
    VMOV.F32      S11,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S11, S1
    VCMP.F32   S11, S1
    VMOV.F32       S11, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S12 registers test  
    VMOV.F32      S0, S12
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S12, S1
    VCMP.F32  S12, S1
    VMOV.F32      S12,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S12, S1
    VCMP.F32   S12, S1
    VMOV.F32       S12, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail




    ; S13 registers test  
    VMOV.F32      S0, S13
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S13, S1
    VCMP.F32  S13, S1
    VMOV.F32      S13,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S13, S1
    VCMP.F32   S13, S1
    VMOV.F32       S13, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S14 registers test  
    VMOV.F32      S0, S14
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S14, S1
    VCMP.F32  S14, S1
    VMOV.F32      S14,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S14, S1
    VCMP.F32   S14, S1
    VMOV.F32       S14, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S15 registers test  
    VMOV.F32      S0, S15
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S15, S1
    VCMP.F32  S15, S1
    VMOV.F32      S15,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S15, S1
    VCMP.F32   S15, S1
    VMOV.F32       S15, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S16 registers test  
    VMOV.F32      S0, S16
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S16, S1
    VCMP.F32  S16, S1
    VMOV.F32      S16,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S16, S1
    VCMP.F32   S16, S1
    VMOV.F32       S16, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S17 registers test  
    VMOV.F32      S0, S17
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S17, S1
    VCMP.F32  S17, S1
    VMOV.F32      S17,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S17, S1
    VCMP.F32   S17, S1
    VMOV.F32       S17, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S18 registers test  
    VMOV.F32      S0, S18
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S18, S1
    VCMP.F32  S18, S1
    VMOV.F32      S18,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S18, S1
    VCMP.F32   S18, S1
    VMOV.F32       S18, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S19 registers test  
    VMOV.F32      S0, S19
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S19, S1
    VCMP.F32  S19, S1
    VMOV.F32      S19,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S19, S1
    VCMP.F32   S19, S1
    VMOV.F32       S19, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S20 registers test  
    VMOV.F32      S0, S20
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S20, S1
    VCMP.F32  S20, S1
    VMOV.F32      S20,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S20, S1
    VCMP.F32   S20, S1
    VMOV.F32       S20, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S21 registers test  
    VMOV.F32      S0, S21
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S21, S1
    VCMP.F32  S21, S1
    VMOV.F32      S21,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S21, S1
    VCMP.F32   S21, S1
    VMOV.F32       S21, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S22 registers test  
    VMOV.F32      S0, S22
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S22, S1
    VCMP.F32  S22, S1
    VMOV.F32      S22,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S22, S1
    VCMP.F32   S22, S1
    VMOV.F32       S22, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail



    ; S23 registers test  
    VMOV.F32      S0, S23
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S23, S1
    VCMP.F32  S23, S1
    VMOV.F32      S23,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S23, S1
    VCMP.F32   S23, S1
    VMOV.F32       S23, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail
    
    

    ; S24 registers test  
    VMOV.F32      S0, S24
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S24, S1
    VCMP.F32  S24, S1
    VMOV.F32      S24,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S24, S1
    VCMP.F32   S24, S1
    VMOV.F32       S24, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail
    
    
    
    ; S25 registers test  
    VMOV.F32      S0, S25
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S25, S1
    VCMP.F32  S25, S1
    VMOV.F32      S25,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S25, S1
    VCMP.F32   S25, S1
    VMOV.F32       S25, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail
    
    
    ; S26 registers test  
    VMOV.F32      S0, S26
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S26, S1
    VCMP.F32  S26, S1
    VMOV.F32      S26,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S26, S1
    VCMP.F32   S26, S1
    VMOV.F32       S26, S0
    VMRS       APSR_nzcv, FPSCR
    BNE.w        __test_asm_fail


    ; S27 registers test  
    VMOV.F32      S0, S27
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S27, S1
    VCMP.F32  S27, S1
    VMOV.F32      S27,S0
    VMRS      APSR_nzcv, FPSCR
    BNE.w       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S27, S1
    VCMP.F32   S27, S1
    VMOV.F32       S27, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        __test_asm_fail


    ; S28 registers test  
    VMOV.F32      S0, S28
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S28, S1
    VCMP.F32  S28, S1
    VMOV.F32      S28,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S28, S1
    VCMP.F32   S28, S1
    VMOV.F32       S28, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        __test_asm_fail



    ; S29 registers test  
    VMOV.F32      S0, S29
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S29, S1
    VCMP.F32  S29, S1
    VMOV.F32      S29,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S29, S1
    VCMP.F32   S29, S1
    VMOV.F32       S29, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        __test_asm_fail

    ; S30 registers test  
    VMOV.F32      S0, S30
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S30, S1
    VCMP.F32  S30, S1
    VMOV.F32      S30,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S30, S1
    VCMP.F32   S30, S1
    VMOV.F32       S30, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        __test_asm_fail

    ; S31 registers test  
    VMOV.F32      S0, S31
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV.F32      S31, S1
    VCMP.F32  S31, S1
    VMOV.F32      S31,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       __test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV.F32       S31, S1
    IF ERROR_IN_FPU_REGISTERS == 1
    VADD.F32   S31, S31, S1
    ENDIF
    VCMP.F32   S31, S1
    VMOV.F32       S31, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        __test_asm_fail

    ; Return success  
    MOVS      R0, #0
    VMSR      FPSCR, R1
    VPOP.32   {S0,S1}
    POP       {R1, R2, R3, PC}

    ; Return failure  
__test_asm_fail
    MOVS      R0, #0x01
    VMSR      FPSCR, R1
    VPOP.32   {S0,S1}
    POP       {R1, R2, R3, PC}
    
    ENDFUNC

    END
; [] END OF FILE  

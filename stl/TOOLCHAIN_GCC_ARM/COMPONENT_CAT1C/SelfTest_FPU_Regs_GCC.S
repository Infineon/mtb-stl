/*******************************************************************************
* File Name: SelfTest_FPU_Regs_GCC.s
* Version 2.0
*
* Description: This file contains functions to perform FPU register self test.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/


.syntax unified
.text
.thumb

/* Set this const to 0x01 to generate error for registers test */
.equ ERROR_IN_FPU_REGISTERS,  0x00u


/*----------------------------------------------------------------------------------*/
/* SelfTest_FPU_Regs_GCC                                                            */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function performs checkerboard test for all FPU registers for PSoC 6      */ 
/*   and XMC                                                                        */
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

/* uint8 SelfTest_FPU_Regs_GCC(void); */
.global SelfTest_FPU_Regs_GCC
.func SelfTest_FPU_Regs_GCC, SelfTest_FPU_Regs_GCC
.type SelfTest_FPU_Regs_GCC, %function
.thumb_func
SelfTest_FPU_Regs_GCC:
    PUSH      {R1, R2, R3, LR}
    VPUSH.32  {S0,S1}
    VMRS      R1, FPSCR
    
/* Assuming that the CPU test is successfull.
   Very first time the S0 is checked using the CPU registers as we are now aware that the 
   FPU register are working properly or not. */


    /* S0 registers test */
    MOV    R0, #0xAAAAAAAA
    VMOV   S0, R0
    VMOV   R2, S0
    CMP    R2, R0
    BNE    test_asm_fail

    MOV    R0, #0x55555555
    VMOV   S0, R0
    VMOV   R2, S0
    CMP    R2, R0
    BNE    test_asm_fail


    /* S1 registers test */
    MOV    R0, #0xAAAAAAAA
    VMOV   S1, R0
    VMOV   R2, S1
    CMP    R2, R0
    BNE    test_asm_fail

    MOV    R0, #0x55555555
    VMOV   S1, R0
    VMOV   R2, S1
    CMP    R2, R0
    BNE    test_asm_fail


    /* S2 registers test */
    VMOV      S0, S2
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S2, S1
    VCMP.F32  S2, S1
    VMOV      S2,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S2, S1
    VCMP.F32   S2, S1
    VMOV       S2, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S3 registers test */
    VMOV      S0, S3
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S3, S1
    VCMP.F32  S3, S1
    VMOV      S3,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S3, S1
    VCMP.F32   S3, S1
    VMOV       S3, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S4 registers test */
    VMOV      S0, S4
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S4, S1
    VCMP.F32  S4, S1
    VMOV      S4,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S4, S1
    VCMP.F32   S4, S1
    VMOV       S4, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S5 registers test */
    VMOV      S0, S5
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S5, S1
    VCMP.F32  S5, S1
    VMOV      S5,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S5, S1
    VCMP.F32   S5, S1
    VMOV       S5, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail


    /* S6 registers test */
    VMOV      S0, S6
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S6, S1
    VCMP.F32  S6, S1
    VMOV      S6,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S6, S1
    VCMP.F32   S6, S1
    VMOV       S6, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S7 registers test */
    VMOV      S0, S7
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S7, S1
    VCMP.F32  S7, S1
    VMOV      S7,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S7, S1
    VCMP.F32   S7, S1
    VMOV       S7, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S8 registers test */
    VMOV      S0, S8
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S8, S1
    VCMP.F32  S8, S1
    VMOV      S8,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S8, S1
    VCMP.F32   S8, S1
    VMOV       S8, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S9 registers test */
    VMOV      S0, S9
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S9, S1
    VCMP.F32  S9, S1
    VMOV      S9,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S9, S1
    VCMP.F32   S9, S1
    VMOV       S9, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S10 registers test */
    VMOV      S0, S10
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S10, S1
    VCMP.F32  S10, S1
    VMOV      S10,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S10, S1
    VCMP.F32   S10, S1
    VMOV       S10, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S11 registers test */
    VMOV      S0, S11
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S11, S1
    VCMP.F32  S11, S1
    VMOV      S11,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S11, S1
    VCMP.F32   S11, S1
    VMOV       S11, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S12 registers test */
    VMOV      S0, S12
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S12, S1
    VCMP.F32  S12, S1
    VMOV      S12,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S12, S1
    VCMP.F32   S12, S1
    VMOV       S12, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail




    /* S13 registers test */
    VMOV      S0, S13
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S13, S1
    VCMP.F32  S13, S1
    VMOV      S13,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S13, S1
    VCMP.F32   S13, S1
    VMOV       S13, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S14 registers test */
    VMOV      S0, S14
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S14, S1
    VCMP.F32  S14, S1
    VMOV      S14,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S14, S1
    VCMP.F32   S14, S1
    VMOV       S14, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S15 registers test */
    VMOV      S0, S15
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S15, S1
    VCMP.F32  S15, S1
    VMOV      S15,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S15, S1
    VCMP.F32   S15, S1
    VMOV       S15, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S16 registers test */
    VMOV      S0, S16
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S16, S1
    VCMP.F32  S16, S1
    VMOV      S16,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S16, S1
    VCMP.F32   S16, S1
    VMOV       S16, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S17 registers test */
    VMOV      S0, S17
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S17, S1
    VCMP.F32  S17, S1
    VMOV      S17,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S17, S1
    VCMP.F32   S17, S1
    VMOV       S17, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S18 registers test */
    VMOV      S0, S18
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S18, S1
    VCMP.F32  S18, S1
    VMOV      S18,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S18, S1
    VCMP.F32   S18, S1
    VMOV       S18, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S19 registers test */
    VMOV      S0, S19
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S19, S1
    VCMP.F32  S19, S1
    VMOV      S19,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S19, S1
    VCMP.F32   S19, S1
    VMOV       S19, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S20 registers test */
    VMOV      S0, S20
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S20, S1
    VCMP.F32  S20, S1
    VMOV      S20,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S20, S1
    VCMP.F32   S20, S1
    VMOV       S20, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S21 registers test */
    VMOV      S0, S21
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S21, S1
    VCMP.F32  S21, S1
    VMOV      S21,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S21, S1
    VCMP.F32   S21, S1
    VMOV       S21, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S22 registers test */
    VMOV      S0, S22
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S22, S1
    VCMP.F32  S22, S1
    VMOV      S22,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S22, S1
    VCMP.F32   S22, S1
    VMOV       S22, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail



    /* S23 registers test */
    VMOV      S0, S23
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S23, S1
    VCMP.F32  S23, S1
    VMOV      S23,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S23, S1
    VCMP.F32   S23, S1
    VMOV       S23, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail
    
    

    /* S24 registers test */
    VMOV      S0, S24
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S24, S1
    VCMP.F32  S24, S1
    VMOV      S24,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S24, S1
    VCMP.F32   S24, S1
    VMOV       S24, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail
    
    
    
    /* S25 registers test */
    VMOV      S0, S25
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S25, S1
    VCMP.F32  S25, S1
    VMOV      S25,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S25, S1
    VCMP.F32   S25, S1
    VMOV       S25, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail
    
    
    /* S26 registers test */
    VMOV      S0, S26
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S26, S1
    VCMP.F32  S26, S1
    VMOV      S26,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S26, S1
    VCMP.F32   S26, S1
    VMOV       S26, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* S27 registers test */
    VMOV      S0, S27
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S27, S1
    VCMP.F32  S27, S1
    VMOV      S27,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S27, S1
    VCMP.F32   S27, S1
    VMOV       S27, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* S28 registers test */
    VMOV      S0, S28
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S28, S1
    VCMP.F32  S28, S1
    VMOV      S28,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S28, S1
    VCMP.F32   S28, S1
    VMOV       S28, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* S29 registers test */
    VMOV      S0, S29
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S29, S1
    VCMP.F32  S29, S1
    VMOV      S29,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S29, S1
    VCMP.F32   S29, S1
    VMOV       S29, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* S30 registers test */
    VMOV      S0, S30
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S30, S1
    VCMP.F32  S30, S1
    VMOV      S30,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S30, S1
    VCMP.F32   S30, S1
    VMOV       S30, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* S31 registers test */
    VMOV      S0, S31
    MOV       R0, #0xAAAAAAAA
    VMOV      S1, R0
    VMOV      S31, S1
    VCMP.F32  S31, S1
    VMOV      S31,S0
    VMRS      APSR_nzcv, FPSCR
    BNE       test_asm_fail


    MOV        R0, #0x55555555
    VMOV       S1, R0
    VMOV       S31, S1
.if (ERROR_IN_FPU_REGISTERS)
    VADD.F32   S31, S31, S1
.endif
    VCMP.F32   S31, S1
    VMOV       S31, S0
    VMRS       APSR_nzcv, FPSCR
    BNE        test_asm_fail

    /* Return success */
    MOVS      R0, #0
    VMSR      FPSCR, R1
    VPOP.32   {S0,S1}
    POP       {R1, R2, R3, PC}

    /* Return failure */
test_asm_fail:
    MOVS      R0, #0x01
    VMSR      FPSCR, R1
    VPOP.32   {S0,S1}
    POP       {R1, R2, R3, PC}

.endfunc

.end
/* [] END OF FILE */

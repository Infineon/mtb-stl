/*******************************************************************************
* File Name: SelfTest_CPU_Regs_GCC.s
* Version 1.0.0
*
* Description: This file contains functions to perform CPU register self test
* using March method.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
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
.equ ERROR_IN_CPU_REGISTERS,  0x00u


/*----------------------------------------------------------------------------------*/
/* SelfTest_CPU_Regs_GCC                                                            */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function performs checkerboard test for all CPU registers for PSoC 4      */ 
/*   except PC register.                                                            */
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

/* uint8 SelfTest_CPU_Regs_GCC(void); */
.global SelfTest_CPU_Regs_GCC
.func SelfTest_CPU_Regs_GCC, SelfTest_CPU_Regs_GCC
.type SelfTest_CPU_Regs_GCC, %function
.thumb_func
SelfTest_CPU_Regs_GCC:

    PUSH  {R1-R7, LR}
    /* Test SP register */
    /* Note: SP ignores writes to the lower two bits, */ 
    /* so they are intentionally set to 0 */
                   
    /* Save SP to R1 */
    MOV   R1, SP
        
    /* Load SP with 0xAAAAAAA8 */
    LDR   R0, =0xAAAAAAA8
    MOV   SP, R0
    CMP   SP, R0
        
    /* Restore SP and return failure */
    MOV   SP, R1
    BNE   test_stack_fail
        
    /* Load SP with 0x55555554 */
    LDR   R0, =0x55555554
    MOV   SP, R0
    CMP   SP, R0
        
    /* Restore SP and return failure */
    MOV   SP, R1
    BNE   test_stack_fail
        
    B     test_stack_ok
        
test_stack_fail:
    B    test_asm_fail
        
test_stack_ok:
    
    /* R8 registers test */
    MOV   R1, R8
    LDR   R0, =0xAAAAAAAA
    MOV   R8, R0
    CMP   R8, R0
    MOV   R8, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R8, R0
    CMP   R8, R0
    MOV   R8, R1
    BNE   test_asm_fail
    
    /* R9 registers test */
    MOV   R1, R9
    LDR   R0, =0xAAAAAAAA
    MOV   R9, R0
    CMP   R9, R0
    MOV   R9, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R9, R0
    CMP   R9, R0
    MOV   R9, R1
    BNE   test_asm_fail
    
    /* R10 registers test */
    MOV   R1, R10
    LDR   R0, =0xAAAAAAAA
    MOV   R10, R0
    CMP   R10, R0
    MOV   R10, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R10, R0
    CMP   R10, R0
    MOV   R10, R1
    BNE   test_asm_fail
    
    /* R11 registers test */
    MOV   R1, R11
    LDR   R0, =0xAAAAAAAA
    MOV   R11, R0
    CMP   R11, R0
    MOV   R11, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R11, R0
    CMP   R11, R0
    MOV   R11, R1
    BNE   test_asm_fail
    
    /* R12 registers test */
    MOV   R1, R12
    LDR   R0, =0xAAAAAAAA
    MOV   R12, R0
    CMP   R12, R0
    MOV   R12, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R12, R0
    CMP   R12, R0
    MOV   R12, R1
    BNE   test_asm_fail
    
    /* LR registers test */
    MOV   R1, LR
    LDR   R0, =0xAAAAAAAA
    MOV   LR, R0
    CMP   LR, R0
    MOV   LR, R1
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   LR, R0
    CMP   LR, R0
    MOV   LR, R1
    BNE   test_asm_fail
    
    /* R0 registers test */
    LDR   R1, =0xAAAAAAAA
    MOV   R0, R1
    CMP   R0, R1
    BNE   test_asm_fail

    LDR   R1, =0x55555555
    MOV   R0, R1
    CMP   R0, R1
    BNE   test_asm_fail
    
    /* R1 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R1, R0
    
.if (ERROR_IN_CPU_REGISTERS)
    adds  R1, R1, #1
    MOV   R1, R1
.endif
    
    CMP   R1, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R1, R0
    CMP   R1, R0
    BNE   test_asm_fail
    
    /* R2 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R2, R0
    CMP   R2, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R2, R0
    CMP   R2, R0
    BNE   test_asm_fail
    
    /* R3 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R3, R0
    CMP   R3, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R3, R0
    CMP   R3, R0
    BNE   test_asm_fail
    
    /* R4 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R4, R0
    CMP   R4, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R4, R0
    CMP   R4, R0
    BNE   test_asm_fail
    
    /* R5 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R5, R0
    CMP   R5, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R5, R0
    CMP   R5, R0
    BNE   test_asm_fail
    
    /* R6 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R6, R0
    CMP   R6, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R6, R0
    CMP   R6, R0
    BNE   test_asm_fail
    
    /* R7 registers test */
    LDR   R0, =0xAAAAAAAA
    MOV   R7, R0
    CMP   R7, R0
    BNE   test_asm_fail

    LDR   R0, =0x55555555
    MOV   R7, R0
    CMP   R7, R0
    BNE   test_asm_fail
    
    /* Return success */
    MOVS   R0, #0
    POP   {R1-R7, PC}
        
    /* Return failure */
test_asm_fail:
    MOVS   R0, #0x01
    POP   {R1-R7, PC}
    
.endfunc

.end

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_CPU_Regs_IAR.s
* Version 2.0
*
* Description: This file contains functions to perform CPU register self test
*  using March method.
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
*
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

  SECTION .text:CODE:ROOT(4)
  PUBLIC SelfTest_CPU_Regs_IAR
  THUMB
    
  ; Set this const to 0x01 to generate error for registers test
  #define ERROR_IN_CPU_REGISTERS 0x00

/*----------------------------------------------------------------------------------*/
/* SelfTest_CPU_Regs_IAR							                                */
/*----------------------------------------------------------------------------------*/
/* 										                                            */
/* Summary:									                                        */
/*   This function checks all the Cortex M0+ CPU registers for stuck at faults      */
/*										                                            */
/* Parameters:									                                    */
/*   None                                                			                */
/*										                                            */
/* Return:									                                        */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.			            */
/*										                                            */
/* Destroys:									                                    */
/*   r0         								                                    */
/*----------------------------------------------------------------------------------*/        
  SelfTest_CPU_Regs_IAR:
    PUSH   {R1-R7, LR}
    
    MOVS R7, #0

	/* Test SP register */
	/*   Note: SP ignores writes to the lower two bits, 
	so they are intentionally set to 0 */

	/* Save SP to R1 */           
	MOV   R1, SP                

	/* Load SP with 0xAAAAAAA8 */
	LDR   R0, =0xAAAAAAA8       
	MOV   SP, R0                
	CMP   SP, R0               

	/* Restore SP and return failure */
	MOV   SP, R1               
	BNE   __test_stack_fail    

	/* Load SP with 0x55555554 */
	LDR   R0, =0x55555554       
	MOV   SP, R0                
	CMP   SP, R0               

	/* Restore SP and return failure */
	MOV   SP, R1               
	BNE   __test_stack_fail    

	B     __test_stack_ok      

        __test_stack_fail:         
	B    __test_asm_fail       

        __test_stack_ok:           
	/* CPU_REGISTER_TEST_HI(R8) */
	MOV   R1, R8               
	LDR   R0, =0xAAAAAAAA       
	MOV   R8, R0               
	CMP   R8, R0               
	MOV   R8, R1               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R8, R0               
	CMP   R8, R0               
	MOV   R8, R1               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_HI(R9) */
	MOV   R1, R9               
	LDR   R0, =0xAAAAAAAA      
	MOV   R9, R0               
	CMP   R9, R0               
	MOV   R9, R1               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R9, R0               
	CMP   R9, R0               
	MOV   R9, R1               
	BNE   __test_asm_fail      


	/* CPU_REGISTER_TEST_HI(R10) */
	MOV   R1, R10              
	LDR   R0, =0xAAAAAAAA      
	MOV   R10, R0              
	CMP   R10, R0              
	MOV   R10, R1              
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R10, R0              
	CMP   R10, R0              
	MOV   R10, R1              
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_HI(R11) */
	MOV   R1, R11              
	LDR   R0, =0xAAAAAAAA      
	MOV   R11, R0              
	CMP   R11, R0              
	MOV   R11, R1              
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R11, R0              
	CMP   R11, R0              
	MOV   R11, R1              
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_HI(R12) */
	MOV   R1, R12              
	LDR   R0, =0xAAAAAAAA      
	MOV   R12, R0              
	CMP   R12, R0              
	MOV   R12, R1              
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R12, R0              
	CMP   R12, R0              
	MOV   R12, R1              
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_HI(LR) */
	MOV   R1, LR               
	LDR   R0, =0xAAAAAAAA      
	MOV   LR, R0               
	CMP   LR, R0               
	MOV   LR, R1               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   LR, R0               
	CMP   LR, R0               
	MOV   LR, R1               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_R0(R0) */
	LDR   R1, =0xAAAAAAAA      
	MOV   R0, R1               
	CMP   R0, R1               
	BNE   __test_asm_fail      

	LDR   R1, =0x55555555      
	MOV   R0, R1               
	CMP   R0, R1               
	BNE   __test_asm_fail      

	#if (ERROR_IN_CPU_REGISTERS)

        /* CPU_REGISTER_TEST_LO_ERROR(R1) */
        LDR   R0, =0xAAAAAAAA      
        MOV   R1, R0               
        ADDS   R1, R1, #0x01        
        CMP   R1, R0               
        BNE   __test_asm_fail      

        LDR   R0, =0x55555555      
        MOV   R1, R0               
        CMP   R1, R0               
        BNE   __test_asm_fail      
	#else 

	/*CPU_REGISTER_TEST_LO(R1) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R1, R0               
	CMP   R1, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R1, R0               
	CMP   R1, R0               
	BNE   __test_asm_fail      
	#endif  

	/* CPU_REGISTER_TEST_LO(R2) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R2, R0               
	CMP   R2, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R2, R0               
	CMP   R2, R0               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_LO(R3) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R3, R0               
	CMP   R3, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R3, R0               
	CMP   R3, R0               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_LO(R4) */ 
	LDR   R0, =0xAAAAAAAA      
	MOV   R4, R0               
	CMP   R4, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R4, R0               
	CMP   R4, R0               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_LO(R5) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R5, R0               
	CMP   R5, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R5, R0               
	CMP   R5, R0               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_LO(R6) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R6, R0               
	CMP   R6, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R6, R0               
	CMP   R6, R0               
	BNE   __test_asm_fail      

	/* CPU_REGISTER_TEST_LO(R7) */
	LDR   R0, =0xAAAAAAAA      
	MOV   R7, R0               
	CMP   R7, R0               
	BNE   __test_asm_fail      

	LDR   R0, =0x55555555      
	MOV   R7, R0               
	CMP   R7, R0               
	BNE   __test_asm_fail      

	/* Return success */
	MOVS   R0, #0x00           
	POP   {R1-R7, PC}

	/* Return failure */
        __test_asm_fail:           
	MOVS   R0, #0x01           
	POP   {R1-R7, PC}
        
        END
        
        
/* [] END OF FILE */
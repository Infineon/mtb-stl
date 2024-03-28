/*------------------------------------------------------------------------------
* FILENAME: SelfTest_SRAM_March_IAR.s
* Version 2.0
*
* DESCRIPTION:
*  This file contains functions to perform SRAM self test using March method.
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

  IMPORT test_Stack_Addr
  IMPORT test_SRAM_Addr
  SECTION .text:CODE:ROOT(4)
  PUBLIC March_Test_Block
  PUBLIC March_Test_Stack_Init
  PUBLIC March_Test_Stack
  PUBLIC March_Test_Init
  PUBLIC March_Test_SRAM
  THUMB
  #include <SelfTest_RAM_Config_IAR.inc>

/* Enable this define to do intentional error during SRAM March test */
#define ERROR_IN_SRAM_MARCH 0x00

/* Enable this define to do intentional error during STACK March test */
#define ERROR_IN_STACK_MARCH 0x00

/* Set Test Block size for SRAM test */
#define TEST_BLOCK_SRAM_SIZE 0x100u

/* Set Test Block size for Stack test */
#define TEST_BLOCK_STACK_SIZE 0x15u

/* Choose maximum block size for reserved area */
#if TEST_BLOCK_SRAM_SIZE > TEST_BLOCK_STACK_SIZE
    #define TEST_BLOCK_SIZE (TEST_BLOCK_SRAM_SIZE) 
#else
    #define TEST_BLOCK_SIZE (TEST_BLOCK_STACK_SIZE)
#endif

MARCH_SRAM_BASE EQU           (CY_SRAM_BASE)
MARCH_STACK_BASE EQU          (CY_SRAM_BASE + CY_SRAM_SIZE - CY_STACK_SIZE)
MARCH_STACK_END EQU           (CY_SRAM_BASE + CY_SRAM_SIZE)
MARCH_BUFF_ADDR_START EQU     (MARCH_STACK_BASE - TEST_BLOCK_SIZE)    
MARCH_BUFF_ADDR_END EQU       (MARCH_BUFF_ADDR_START + TEST_BLOCK_SRAM_SIZE)
MARCH_BUFF_ADDR_STACK_END EQU (MARCH_BUFF_ADDR_START + TEST_BLOCK_STACK_SIZE)


/* Status flags to return function results */
OK_STATUS EQU   	       0x00
ERROR_STATUS EQU  	       0x01
PASS_STILL_TESTING_STATUS EQU  0x02 
PASS_COMPLETE_STATUS EQU       0x03 


/*----------------------------------------------------------------------------------*/
/* March_Test_Block																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function perform block of RAM self test using March method. 				*/
/*																					*/
/* Parameters:																		*/
/*   r0 - used to set address of first block byte. 									*/
/*   r1 - used to set address of last block byte. 									*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r3, r4, r10																	*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Block(void); */

March_Test_Block:
    mov r10, LR

    /* Save r0 */
    mov r4, r0
    
    /* Write 0x00 to all block of SRAM */
    bl March_Test_Write_0
    mov r0, r4
    
    /* Read byte, compare with 0x00 and write 0xFF to all block of SRAM  */
    bl March_Test_Read0_Write1_Inc
    cmp r7, #0
    bne End0
    mov r0, r4
    
    /* Read byte, compare with 0xFF and write 0x00 to all block of SRAM */
    bl March_Test_Read1_Write0_Inc
    cmp r7, #0
    bne End0
    mov r0, r4
    
    /* Read byte, compare with 0x00 to all block of SRAM */  
    bl March_Test_Read_0
    cmp r7, #0
    bne End0
    mov r0, r4
    
    /* Save r1*/
    mov r4, r1
    
    /* Read byte, compare with 0x00 and write 0xFF to all block of SRAM */
    bl March_Test_Read0_Write1_Dec
    cmp r7, #0
    bne End0
    mov r1, r4
    
    /* Read byte, compare with 0xFF and write 0x00 to all block of SRAM */ 
    bl March_Test_Read1_Write0_Dec    
    cmp r7, #0
    bne End0
    mov r1, r4

    /* Save r0*/
    mov r4, r0

    /* Read byte, compare with 0x00 to all block of SRAM */  
    bl March_Test_Read_0
    cmp r7, #0
    bne End0  
    mov r0, r4
    
    End0: 

    mov LR, r10
    
    BX lr
    
/*----------------------------------------------------------------------------------*/
/* Copy_Block_To_Buffer																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function save "memory block under test" to reserved location.				*/
/*   Function copies data from address r2 to r0. Size of copying size is (r1 - r0). */
/*																					*/
/* Parameters:																		*/
/*   r0 - high byte of start address for copying. 									*/
/*   r1 - high byte of end address for copying. 							    	*/
/*   r2 - high byte of buffer address.   											*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r2, r3,																	*/
/*----------------------------------------------------------------------------------*/
/* void Copy_Block_To_Buffer(void); */

Copy_Block_To_Buffer:
	
    IRAM_copy0:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Copt byte of SRAM from r3 to reserved area */
        STRB r3, [r2]

        /* Increment address of copied area */
        adds r0, r0, #1

        /* Increment address of reserved area */
        adds r2, r2, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_copy0    

    BX lr    
/*------------------------------------------------------------------------------------*/
/* Copy_Block_From_Buffer                                                             */
/*------------------------------------------------------------------------------------*/
/*                                                                                    */
/* Summary:                                                                           */
/*   This function restore "memory block under test" from buffer location.            */
/*   Function copies data from address r2 to r0. Size of copying size is (r1 - r0).   */
/*                                                                                    */
/* Parameters:                                                                        */
/*   r0 - high byte of start address for copying.                                     */
/*   r1 - high byte of end address for copying.                                       */
/*   r2 - high byte of buffer address.                                                */
/* Return:                                                                            */
/*   void.                                                                            */
/*                                                                                    */
/* Destroys:                                                                          */
/*   r0, r2, r3                                                                       */
/*------------------------------------------------------------------------------------*/
/* void Copy_Block_From_Buffer(void); */

Copy_Block_From_Buffer:

    IRAM_copy1:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r2]
        
        /* Copt byte of SRAM from r3 to reserved area */
        STRB r3, [r0]

        /* Increment address of copied area */
        adds r0, r0, #1

        /* Increment address of reserved area */
        adds r2, r2, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_copy1  	
	
    BX lr  	
	
/*----------------------------------------------------------------------------------*/
/* March_Test_Write_0																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Write 0x00 to block of SRAM. Address of memory increnets.				    	*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3																			*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Write_0(void); */
March_Test_Write_0:
        
	movs r3, #0x00
		  		
	IRAM_chk0:
        
		STRB r3, [r0]

		adds r0, r0, #1

		/* Check if it is not end of SRAM block */
		cmp r0, r1
        
		blt IRAM_chk0
		
	/* Check if intentional error should be made for testing */
	IF (ERROR_IN_SRAM_MARCH | ERROR_IN_STACK_MARCH)
        
		/* Set 0x01 for writing to RAM */
		movs r3, #0x01     
		
		/* Set address of last byte in block */
		subs r0, r0, #1
		
		/* Write 0x01 to RAM */
		STRB r3, [r0]
		
		/* Restore original address in r0 */
		adds r0, r0, #1
			
	ENDIF  		

	BX lr
        
/*----------------------------------------------------------------------------------*/
/* March_Test_Write_1																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Write 0xFF to block of SRAM. Address of memory increnets.						*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3																			*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Write_1(void); */

March_Test_Write_1:

    movs r3, #0xFF
    IRAM_chk1:
    
        STRB r3, [r0]

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk1

    BX lr
   
/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Inc														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00 and write 0xFF to block.							*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																	    */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Inc(void); */
March_Test_Read0_Write1_Inc:

	IRAM_chk2:
        
        /* Write byte of SRAM to r3 */
    	LDRB r3, [r0]
		
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_0xFF
        
        /* return ERROR_STATUS */
        movs r7, #ERROR_STATUS    

        BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0xFF:
        
        movs r3, #0xFF
        STRB r3, [r0]

        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk2

    BX lr
 
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Inc														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0xFF and write 0x00 to block.							*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																		*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Inc(void); */


March_Test_Read1_Write0_Inc:

    IRAM_chk3:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0xFF 
        
        beq Write_0x00
        
        /* return ERROR_STATUS */
        movs r7, #ERROR_STATUS    

        BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0x00:
        
        movs r3, #0x00
        STRB r3, [r0]

        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk3

    BX lr
 
/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Dec														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00 and write 0xFF to block.							*/
/*   Address of memory decreneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r1, r3, r7																	    */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Dec(void); */
March_Test_Read0_Write1_Dec:

    subs r1, r1, #1

    IRAM_chk5:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r1]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_0xFF_1
        
        /* return ERROR_STATUS */
        movs r7, #ERROR_STATUS    

        BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0xFF_1:
        
        movs r3, #0xFF
        STRB r3, [r1]

        /* Decrenemt SRAM pointer register */
        subs r1, r1, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        ble IRAM_chk5

    BX lr
 
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Dec														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0xFF and write 0x00 to block.							*/
/*   Address of memory decreneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r1, r3, r7																	    */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Dec(void); */
March_Test_Read1_Write0_Dec:

    subs r1, r1, #1

    IRAM_chk6:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r1]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0xFF 
        
        beq Write_0x00_1
        
        /* return ERROR_STATUS */
        movs r7, #ERROR_STATUS    

        BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0x00_1:
        
        movs r3, #0
        STRB r3, [r1]

        /* Increnemt SRAM pointer register */
        subs r1, r1, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        ble IRAM_chk6

    BX lr     
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Read_0																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00.													*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																		*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read_0(void); */
March_Test_Read_0:

    IRAM_chk4:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_next_loop
        
        /* return ERROR_STATUS */
        movs r7, #ERROR_STATUS    

        BX lr 

        Write_next_loop:

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_chk4

    BX lr

/*----------------------------------------------------------------------------------*/
/* March_Test_Init																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function initializes the SRAM base address.								*/
/*																					*/
/* Parameters:																		*/
/*   Function set to variable "test_SRAM_Addr" address of first SRAM byte for test. */
/*   "test_SRAM_Addr" = MARCH_SRAM_BASE + R0. R0 uses to set shift of first 		*/
/*   byte for test.										  							*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   none.																		*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Init(void); */

March_Test_Init:

    PUSH  {R1, R2}

    /* Set base address for March checkerboard test */
    LDR r1, =MARCH_SRAM_BASE

	/* Set shift of start test address */ 
	ADD r1, r1, r0
	
 	LDR r2, =test_SRAM_Addr
	STR r1, [r2, #0]
	
    POP  {R1, R2}	
    
    BX lr
    
/*----------------------------------------------------------------------------------*/
/* March_Test_SRAM																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*  This function perform block of RAM self test using March method. This function  */
/*  first duplicates data in the area to be tested as this test is destructive.		*/
/*  At the end of this test the original data is restored to its correct location 	*/
/*  in memory. 																		*/
/*																					*/
/* Parameters:																		*/	
/*   Function uses variable "test_SRAM_Addr" to set start address of tested block	*/
/*																					*/
/* Return:																			*/
/*   Result of test:  "1" - fail test;												*/
/*	                  "2" - pass, but still testing status							*/
/*                    "3" - pass and complete status								*/
/*																					*/
/* Destroys:																		*/
/*   r0																				*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_SRAM(uint8_t shift); */

March_Test_SRAM:

    PUSH  {R1-R7, LR}

    /* Clear test status register */
    movs r7, #0 
	
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_BUFF_ADDR_START
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_BUFF_ADDR_END

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
#if (!ERROR_IN_STACK_MARCH | ERROR_IN_SRAM_MARCH)

    /* Run Test of reserved RAM area. */
    bl March_Test_Block
#endif  

    /* Check result of reserved RAM testing */ 
    cmp r7, #0
    bne ret_result     
    /*----------------------------------------------------------------------------------*/


    /*-----------------------------  Copy RAM block  -----------------------------------*/	
	
    /* Set parameters to copy tested block of RAM to reserved area */ 
	
    /* Set address of first byte in testing block */  
    LDR r0, =test_SRAM_Addr
	LDR r0, [r0]
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_SRAM_SIZE
    ADD r1, r1, r0

    /* Set address of buffer first byte */
    LDR r2, =MARCH_BUFF_ADDR_START
    
    /* Check if not end of RAM */
    CMP r2, r1
    BPL Not_end_ram
    
    /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
    MOV r1, r2
		
Not_end_ram:
	            
	/* Save r0 */
	MOV r5, r0
	
    /* Copy block of SRAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
    /*-----------------------------------------------------------------------------------*/
   
   
    /*-----------------------------  Test block of RAM  ---------------------------------*/
    /* Set address of first byte in testing block */  
    mov r0, r5

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
#if (!ERROR_IN_STACK_MARCH | ERROR_IN_SRAM_MARCH)

    /* Test Block of RAM */
    bl March_Test_Block
    /*-----------------------------------------------------------------------------------*/  
#endif	
	
    /*-----------------------------  Restore block  -----------------------------------*/
    /* Set address of buffer first byte */
    LDR r2, =MARCH_BUFF_ADDR_START	
   
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer      
    /*-----------------------------------------------------------------------------------*/

    /* Check result of RAM block testing */
    cmp r7, #0
    bne ret_result 

    /*-------------------------  Check boundaries  ----------------------------------*/

	
    /* Set address of buffer first byte (this is second last byte for test ) */
    LDR r2, =MARCH_BUFF_ADDR_START	
		
    /* Check if not end of SRAM */
    cmp r2, r1		 	
    bne Not_end_of_ram


    /*----------------------------  Check results  -------------------------------------*/
    movs r7, #PASS_COMPLETE_STATUS 
    bl ret_result
    
Not_end_of_ram:
    
	/* Calculate and save address of first byte for next RAM block */
 	LDR r3, =test_SRAM_Addr
	STR r1, [r3, #0]					 
    movs r7, #PASS_STILL_TESTING_STATUS
    
ret_result:
    
    /* Return result */
    mov r0, r7     
    
    POP   {R1-R7, PC}

/*-------------------------------------------------------------------------------------------*/
/* March_Test_Stack_Init                                                                     */
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/* Summary:                                                                                  */
/*   This function initializes the test STACK base address.                                  */
/*                                                                                           */
/* Parameters:                                                                               */
/*   Function set to variable "test_Stack_Addr" address of first STACK byte for test.        */         
/*   "test_Stack_Addr" = MARCH_STACK_BASE + R0. R0 uses to set shift of first byte for test. */
/*                                                                                           */
/* Return:                                                                                   */
/*   void.                                                                                   */
/*                                                                                           */
/* Destroys:                                                                                 */
/*   none.                                                                                   */
/*-------------------------------------------------------------------------------------------*/
/* void March_Test_Stack_Init(uint8_t shift); */

March_Test_Stack_Init:

    PUSH  {R1, R2}

	/* Set base address for March checkerboard test */
	LDR r1, =MARCH_STACK_BASE	
	
	/* Set shift of start test address */ 
	ADD r1, r1, r0
	
 	LDR r2, =test_Stack_Addr
	STR r1, [r2, #0]
    
	POP  {R1, R2}
	
    BX lr
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Stack																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*  This function perform Stack RAM self test using March method. This function 	*/
/*  first duplicates data in the area to be tested as this test is destructive.		*/
/*  At the end of this test the original data is restored to its correct location 	*/
/*  in memory. 																		*/
/*																					*/
/* Parameters:																		*/
/*																					*/
/* Return:																			*/
/*   Result of test:  "1" - fail test;												*/
/*	                  "2" - pass, but still testing status							*/
/*                    "3" - pass and complete status								*/
/* Destroys:																		*/
/*   r0																				*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Stack(void); */

March_Test_Stack:

    PUSH  {R1-R7, LR}

    /* Clear test status register */
    movs r7, #0 
	
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_BUFF_ADDR_START	
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_BUFF_ADDR_STACK_END

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
#if (!ERROR_IN_SRAM_MARCH | ERROR_IN_STACK_MARCH)

    /* Test reserved RAM area. */
     bl March_Test_Block
#endif    
	
    /* Check result of reserved RAM testing */
    cmp r7, #0
    bne Error_detected_1     
    /*----------------------------------------------------------------------------------*/
	
	
    /*-------------------------  Copy block  ----------------------------------------*/
    
    /* Set parameters to copy tested block of RAM to reserved area */
    
    /* Set address of first byte in testing block */  
    LDR r0, =test_Stack_Addr
    LDR r0, [r0]
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_STACK_SIZE	
    ADD r1, r1, r0

    /* Set address of buffer first byte */
    LDR r2, =MARCH_STACK_END	

    /* Check if not end of RAM */
    CMP r2, r1	
    BPL Not_end_stack

    /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
    MOV r1, r2
		
Not_end_stack:
	    
    /* Set parameters in r2 to copy tested block of RAM to reserved area */
    LDR r2, =MARCH_BUFF_ADDR_START	
              
    /* Save r0 */
    MOV r5, r0
	
    /* Copy block of stack RAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
    /*--------------------------------------------------------------------------------
		
    /*-------------------------  Test block  ---------------------------------------*/
    /* Set address of first byte in testing block */
    mov r0, r5

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
#if (!ERROR_IN_SRAM_MARCH| ERROR_IN_STACK_MARCH)

    /* Test block of stack RAM */
    bl March_Test_Block
    /*-------------------------------------------------------------------------------*/
#endif	
	
    /*-------------------------  Restore block  ------------------------------------*/
    /* Write to R0 and R1 parameters for function "Copy_Block_To_Buffer" */
    LDR r2, =MARCH_BUFF_ADDR_START	
    
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer      		
    /*-------------------------------------------------------------------------------*/

    /* Return error */
    cmp r7, #0
    bne Error_detected_1     
					
    /*-------------------------  Check boundaries  ----------------------------------*/
    /* Set address of  second last byte of STACK */
    LDR r2, =MARCH_STACK_END

    /* Check if not end of SRAM	*/
    CMP r2, r1
    BNE Not_end_of_Stack
	    	
    /*----------------------------  Check results  -----------------------------------*/	
    movs r7, #PASS_COMPLETE_STATUS    
    bl Error_detected_1
		
Not_end_of_Stack:
    
    /* Calculate and save address of first byte for next RAM block */
    LDR r3, =test_Stack_Addr
    STR r1, [r3, #0]	
    movs r7, #PASS_STILL_TESTING_STATUS
	
Error_detected_1:

    /* Return result */
    mov r0, r7 
    mov r0, r0
	
    POP   {R1-R7, PC}
    
  END

  
/* [] END OF FILE */

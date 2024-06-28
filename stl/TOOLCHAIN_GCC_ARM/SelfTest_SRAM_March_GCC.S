/*------------------------------------------------------------------------------
* FILENAME: SelfTest_SRAM_March_GCC.s
* Version 1.0.0
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

.data   

Test_Stack_Addr:
.align
.word  0x00000000 

Test_SRAM_Addr:
.align
.word  0x00000000 

.syntax unified

.text
.thumb

#if COMPONENT_CAT1
#if COMPONENT_CAT1A

    #if TARGET_CY8CPROTO_062_4343W || TARGET_APP_CY8CPROTO_062_4343W
        .equ CY_SRAM_BASE,          0x08002000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x000FD800
    #elif TARGET_CY8CPROTO_062S3_4343W  || TARGET_APP_CY8CPROTO_062S3_4343W
        .equ CY_SRAM_BASE,          0x08002000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x0003D800
    #elif TARGET_CY8CKIT_062S4 || TARGET_APP_CY8CKIT_062S4
        .equ CY_SRAM_BASE,          0x08002000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x0001D800
    #elif TARGET_CY8CKIT_062_WIFI_BT || TARGET_APP_CY8CKIT_062_WIFI_BT
        .equ CY_SRAM_BASE,          0x08002000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x00045800
    #else
        /* Update these macros as per the device being tested */
        .equ CY_SRAM_BASE,          0x08002000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x00045800
    #endif
#endif

#if COMPONENT_CAT1C
    #if TARGET_KIT_XMC71_EVK_LITE_V1 || TARGET_APP_KIT_XMC71_EVK_LITE_V1
        .equ CY_SRAM_BASE,          0x28004000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x000BC000
    #elif TARGET_KIT_XMC72_EVK || TARGET_APP_KIT_XMC72_EVK
        .equ CY_SRAM_BASE,          0x28004000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x000FC000
    #else
       /* Update these macros as per the device being tested */
        .equ CY_SRAM_BASE,          0x28004000
        .equ CY_STACK_SIZE,         0x00001000
        .equ CY_SRAM_SIZE,          0x000FC000
    #endif
#endif

#endif

#if COMPONENT_CAT2

    #if TARGET_CY8CKIT_041S_MAX || TARGET_APP_CY8CKIT_041S_MAX
        .equ CY_SRAM_BASE,          0x20000000
        .equ CY_STACK_SIZE,         0x00000400
        .equ CY_SRAM_SIZE,          0x00008000
    
    #else
       /* Update these macros as per the device being tested */
        .equ CY_SRAM_BASE,          0x20000000
        .equ CY_STACK_SIZE,         0x00000400
        .equ CY_SRAM_SIZE,          0x00002000
    #endif
#endif

/* Enable this define to do intentional error during SRAM MARCH or GALPAT test */
.equ ERROR_IN_SRAM_MARCH_GALPAT,  0x00

/* Enable this define to do intentional error during STACK MARCH or GALPAT test */
.equ ERROR_IN_STACK_MARCH_GALPAT, 0x00

/* Set Test Block size for SRAM test */
.equ TEST_BLOCK_SRAM_SIZE,  0x00000100

/* Set Test Block size for Stack test */
.equ TEST_BLOCK_STACK_SIZE, 0x00000015


/* Choose maximum block size for reserved area */
.if (TEST_BLOCK_SRAM_SIZE > TEST_BLOCK_STACK_SIZE)
    .equ TEST_BLOCK_SIZE,    TEST_BLOCK_SRAM_SIZE 
.else
    .equ TEST_BLOCK_SIZE,    TEST_BLOCK_STACK_SIZE 
.endif


.equ MARCH_GALPAT_SRAM_BASE,           (CY_SRAM_BASE)
.equ MARCH_GALPAT_STACK_BASE,          (CY_SRAM_BASE + CY_SRAM_SIZE - CY_STACK_SIZE)
.equ MARCH_GALPAT_STACK_END,           (CY_SRAM_BASE + CY_SRAM_SIZE)
.equ MARCH_GALPAT_BUFF_ADDR_START,     (MARCH_GALPAT_STACK_BASE - TEST_BLOCK_SIZE)    
.equ MARCH_GALPAT_BUFF_ADDR_END,       (MARCH_GALPAT_BUFF_ADDR_START + TEST_BLOCK_SRAM_SIZE)
.equ MARCH_GALPAT_BUFF_ADDR_STACK_END, (MARCH_GALPAT_BUFF_ADDR_START + TEST_BLOCK_STACK_SIZE)


/* Status flags to return function results */
.equ OK_STATUS,                 0x00
.equ ERROR_STATUS,              0x01
.equ PASS_STILL_TESTING_STATUS, 0x02 
.equ PASS_COMPLETE_STATUS,      0x03 


/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_Block                                                                 */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function perform block of RAM self test using GALPAT method.               */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - used to set address of first block byte.                                  */
/*   r1 - used to set address of last block byte.                                   */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r3, r4, r5, r10                                                                */
/*----------------------------------------------------------------------------------*/
/* uint8 GALPAT_Test_Block(void); */
.global GALPAT_Test_Block
.func GALPAT_Test_Block, GALPAT_Test_Block
.type GALPAT_Test_Block, %function
.thumb_func
GALPAT_Test_Block:

    mov r10, LR

    /* Save r0 */
    mov r4, r0
    
    /* keep track of inverted byte */
    mov r5, r0

    /* Write 0x00 to all block of SRAM */
    bl MARCH_GALPAT_Test_Write_0
    mov r0, r4
    
    /* Invert the r5^th byte */
    Repeate_procedure_1:
        /* Read byte, compare with 0x00 and also compare the inverted byte every time  */
        bl GALPAT_Test_Read0_by_Inverting_1byte
        cmp r7, #0
        bne End1
        movs r3, #0x00
        STRB r3, [r5]
        adds r5, r5, #1
        mov r0, r4
        cmp r5, r1
    
    blt Repeate_procedure_1 

    /* Write 0xFF to all block of SRAM */
    bl MARCH_GALPAT_Test_Write_1
    mov r0, r4    
    /* keep track of inverted byte */
    mov r5, r4

    Repeate_procedure_2:
        /* Read byte, compare with 0x00 and write 0xFF to all block of SRAM  */
        bl GALPAT_Test_Read1_by_Inverting_1byte
        cmp r7, #0
        bne End1
        movs r3, #0xFF
        STRB r3, [r5]
        adds r5, r5, #1
        mov r0, r4
        cmp r5, r1
    
    blt Repeate_procedure_2


    End1: 

    mov LR, r10
    
    BX lr
    
.endfunc


/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_Read0_by_Inverting_1byte                                             */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Invert the r5^th byte, Check block of SRAM with 0x00 except r5th byte.         */
/*   r5^th byte is checked with 0xFF after every byte read andcompared with 0x00.   */
/*   Address of memory increneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*   r5 - register used to set address of inverted test byte                        */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 GALPAT_Test_Read0_by_Inverting_1byte(void); */

.global GALPAT_Test_Read0_by_Inverting_1byte
.func GALPAT_Test_Read0_by_Inverting_1byte, GALPAT_Test_Read0_by_Inverting_1byte
.type GALPAT_Test_Read0_by_Inverting_1byte, %function
.thumb_func
GALPAT_Test_Read0_by_Inverting_1byte:

    IRAM_chk_galpat:
        
        movs r3, #0xFF
        STRB r3, [r5]

        cmp r0, r5
        beq Check_inverted_byte

        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Check_inverted_byte
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 

        Check_inverted_byte:
        LDRB r3, [r5]
        cmp r3, #0xFF
        beq Incr_byte
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr


        /* Write 0xFF to byte in SRAM */
        Incr_byte: 
        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk_galpat

    BX lr
 
.endfunc   

/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_Read1_by_Inverting_1byte                                             */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Invert the r5^th byte, Check block of SRAM with 0xFF except r5th byte.         */
/*   r5^th byte is checked with 0x00 after every byte read and compared with 0xFF.  */
/*   Address of memory increneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*   r5 - register used to set address of inverted test byte                        */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 GALPAT_Test_Read1_by_Inverting_1byte(void); */

.global GALPAT_Test_Read1_by_Inverting_1byte
.func GALPAT_Test_Read1_by_Inverting_1byte, GALPAT_Test_Read1_by_Inverting_1byte
.type GALPAT_Test_Read1_by_Inverting_1byte, %function
.thumb_func
GALPAT_Test_Read1_by_Inverting_1byte:

    IRAM_chk_galpat_1:
        
        movs r3, #0x00
        STRB r3, [r5]

        cmp r0, r5
        beq Check_inverted_byte_1

        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0xFF */
        cmp r3, #0xFF 
        
        beq Check_inverted_byte_1
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 

        Check_inverted_byte_1:
        LDRB r3, [r5]
        cmp r3, #0x00
        beq Incr_byte_1
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr

        Incr_byte_1: 
        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk_galpat_1

    BX lr
 
.endfunc   


/*----------------------------------------------------------------------------------*/
/* March_Test_Block                                                                 */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function perform block of RAM self test using March method.               */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - used to set address of first block byte.                                  */
/*   r1 - used to set address of last block byte.                                   */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r3, r4, r10                                                                    */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Block(void); */
.global March_Test_Block
.func March_Test_Block, March_Test_Block
.type March_Test_Block, %function
.thumb_func
March_Test_Block:

    mov r10, LR

    /* Save r0 */
    mov r4, r0
    
    /* Write 0x00 to all block of SRAM */
    bl MARCH_GALPAT_Test_Write_0
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
    
.endfunc


/*----------------------------------------------------------------------------------*/
/* Copy_Block_To_Buffer                                                             */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function save "memory block under test" to reserved location.             */
/*   Function copies data from address r2 to r0. Size of copying size is (r1 - r0). */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - high byte of start address for copying.                                   */
/*   r1 - high byte of end address for copying.                                     */
/*   r2 - high byte of buffer address.                                              */
/* Return:                                                                          */
/*   void.                                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r2, r3,                                                                    */
/*----------------------------------------------------------------------------------*/
/* void Copy_Block_To_Buffer(void); */
.global Copy_Block_To_Buffer
.func Copy_Block_To_Buffer, Copy_Block_To_Buffer
.type Copy_Block_To_Buffer, %function
.thumb_func
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

.endfunc  


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
.global Copy_Block_From_Buffer
.func Copy_Block_From_Buffer, Copy_Block_From_Buffer
.type Copy_Block_From_Buffer, %function
.thumb_func
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
    
.endfunc  
    

/*----------------------------------------------------------------------------------*/
/* MARCH_GALPAT_Test_Write_0                                                               */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Write 0x00 to block of SRAM. Address of memory increnets.                      */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   void.                                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3                                                                         */
/*----------------------------------------------------------------------------------*/
/* void MARCH_GALPAT_Test_Write_0(void); */
.global MARCH_GALPAT_Test_Write_0
.func MARCH_GALPAT_Test_Write_0, MARCH_GALPAT_Test_Write_0
.type MARCH_GALPAT_Test_Write_0, %function
.thumb_func
    
MARCH_GALPAT_Test_Write_0:
        
    movs r3, #0x00
                  
    IRAM_chk0:
        
        STRB r3, [r0]

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
        
        blt IRAM_chk0
        
    /* Check if intentional error should be made for testing */
    .if (ERROR_IN_SRAM_MARCH_GALPAT | ERROR_IN_STACK_MARCH_GALPAT)
        
        /* Set 0x01 for writing to RAM */
        movs r3, #0x01     
        
        /* Set address of last byte in block */
        subs r0, r0, #1
        
        /* Write 0x01 to RAM */
        STRB r3, [r0]
        
        /* Restore original address in r0 */
        adds r0, r0, #1
            
    .endif          

    BX lr
        
.endfunc   
  

 
/*----------------------------------------------------------------------------------*/
/* MARCH_GALPAT_Test_Write_1                                                               */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Write 0xFF to block of SRAM. Address of memory increnets.                      */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   void.                                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3                                                                         */
/*----------------------------------------------------------------------------------*/
/* void MARCH_GALPAT_Test_Write_1(void); */
.global MARCH_GALPAT_Test_Write_1
.func MARCH_GALPAT_Test_Write_1, MARCH_GALPAT_Test_Write_1
.type MARCH_GALPAT_Test_Write_1, %function
.thumb_func
MARCH_GALPAT_Test_Write_1:

    movs r3, #0xFF
    IRAM_chk1:
    
        STRB r3, [r0]

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk1

    BX lr
    
.endfunc   


/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Inc                                                      */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Check block of SRAM with 0x00 and write 0xFF to block.                         */
/*   Address of memory increneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Inc(void); */

.global March_Test_Read0_Write1_Inc
.func March_Test_Read0_Write1_Inc, March_Test_Read0_Write1_Inc
.type March_Test_Read0_Write1_Inc, %function
.thumb_func
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
 
.endfunc   
    
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Inc                                                      */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Check block of SRAM with 0xFF and write 0x00 to block.                         */
/*   Address of memory increneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Inc(void); */

.global March_Test_Read1_Write0_Inc
.func March_Test_Read1_Write0_Inc, March_Test_Read1_Write0_Inc
.type March_Test_Read1_Write0_Inc, %function
.thumb_func
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
 
.endfunc      
  

/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Dec                                                      */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Check block of SRAM with 0x00 and write 0xFF to block.                         */
/*   Address of memory decreneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r1, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Dec(void); */

.global March_Test_Read0_Write1_Dec
.func March_Test_Read0_Write1_Dec, March_Test_Read0_Write1_Dec
.type March_Test_Read0_Write1_Dec, %function
.thumb_func
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
 
.endfunc        
    
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Dec                                                      */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Check block of SRAM with 0xFF and write 0x00 to block.                         */
/*   Address of memory decreneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r1, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Dec(void); */

.global March_Test_Read1_Write0_Dec
.func March_Test_Read1_Write0_Dec, March_Test_Read1_Write0_Dec
.type March_Test_Read1_Write0_Dec, %function
.thumb_func
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
    
.endfunc  


/*----------------------------------------------------------------------------------*/
/* March_Test_Read_0                                                                */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   Check block of SRAM with 0x00.                                                 */
/*   Address of memory increneted.                                                  */
/*                                                                                  */
/* Parameters:                                                                      */
/*   r0 - register used to set address of first test byte                           */
/*   r1 - register used to set address of last test byte                            */
/*                                                                                  */
/* Return:                                                                          */
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.                        */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0, r3, r7                                                                     */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read_0(void); */

.global March_Test_Read_0
.func March_Test_Read_0, March_Test_Read_0
.type March_Test_Read_0, %function
.thumb_func
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

.endfunc  


/*----------------------------------------------------------------------------------*/
/* March_Test_Init                                                                  */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function initializes the SRAM base address.                               */
/*                                                                                  */
/* Parameters:                                                                      */
/*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. */
/*   "Test_SRAM_Addr" = MARCH_GALPAT_SRAM_BASE + R0. R0 uses to set shift of first         */
/*   byte for test.                                                                 */
/*                                                                                  */
/* Return:                                                                          */
/*   void.                                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   none.                                                                          */
/*----------------------------------------------------------------------------------*/
/* void March_Test_Init(uint8_t shift); */
.global March_Test_Init
.func March_Test_Init, March_Test_Init
.type March_Test_Init, %function
.thumb_func
March_Test_Init:

    PUSH  {R1, R2}

    /* Set base address for March checkerboard test */
    LDR r1, =MARCH_GALPAT_SRAM_BASE
    
    /* Set shift of start test address */ 
    ADD r1, r1, r0
    
     LDR r2, =Test_SRAM_Addr
    STR r1, [r2, #0]
    
    POP  {R1, R2}    
    
    BX lr
    
.endfunc    


/*----------------------------------------------------------------------------------*/
/* March_Test_SRAM                                                                  */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*  This function perform block of RAM self test using March method. This function  */
/*  first duplicates data in the area to be tested as this test is destructive.     */
/*  At the end of this test the original data is restored to its correct location   */
/*  in memory.                                                                      */
/*                                                                                  */
/* Parameters:                                                                      */    
/*   Function uses variable "Test_SRAM_Addr" to set start address of tested block   */
/*                                                                                  */
/* Return:                                                                          */
/*   Result of test:  "1" - fail test;                                              */
/*                    "2" - pass, but still testing status                          */
/*                    "3" - pass and complete status                                */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0                                                                             */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_SRAM(void); */
.global March_Test_SRAM
.func March_Test_SRAM, March_Test_SRAM
.type March_Test_SRAM, %function
.thumb_func
March_Test_SRAM:

    PUSH  {R1-R7, LR}


    /* Clear test status register */
    movs r7, #0 

        
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_GALPAT_BUFF_ADDR_START
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_GALPAT_BUFF_ADDR_END

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH_GALPAT enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH_GALPAT | ERROR_IN_SRAM_MARCH_GALPAT)

    /* Run Test of reserved RAM area. */
    bl March_Test_Block
.endif  

    /* Check result of reserved RAM testing */ 
    cmp r7, #0
        bne ret_result     
    /*----------------------------------------------------------------------------------*/


    /*-----------------------------  Copy RAM block  -----------------------------------*/    
    
    /* Set parameters to copy tested block of RAM to reserved area */ 
    
    /* Set address of first byte in testing block */  
    LDR r0, =Test_SRAM_Addr
    LDR r0, [r0]  
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_SRAM_SIZE
    ADD r1, r1, r0
    
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START
    
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

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH_GALPAT enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH_GALPAT | ERROR_IN_SRAM_MARCH_GALPAT)

    /* Test Block of RAM */
    bl March_Test_Block
    /*-----------------------------------------------------------------------------------*/  
.endif    
    
    /*-----------------------------  Restore block  -----------------------------------*/
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
   
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer      
    /*-----------------------------------------------------------------------------------*/


    /* Check result of RAM block testing */
    cmp r7, #0
        bne ret_result 


    /*-------------------------  Check boundaries  ----------------------------------*/
    
    /* Set address of buffer first byte (this is second last byte for test ) */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
        
    /* Check if not end of SRAM */
    cmp r2, r1             
        bne Not_end_of_ram


    /*----------------------------  Check results  -------------------------------------*/
        movs r7, #PASS_COMPLETE_STATUS 
        bl ret_result
    
Not_end_of_ram:
    
    /* Calculate and save address of first byte for next RAM block */
     LDR r3, =Test_SRAM_Addr
    STR r1, [r3, #0]
    movs r7, #PASS_STILL_TESTING_STATUS
    
ret_result:
    
    /* Return result */
    mov r0, r7     
    
    POP   {R1-R7, PC}

.endfunc


/*-------------------------------------------------------------------------------------------*/
/* March_Test_Stack_Init                                                                     */
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/* Summary:                                                                                  */
/*   This function initializes the test STACK base address.                                  */
/*                                                                                           */
/* Parameters:                                                                               */
/*   Function set to variable "Test_Stack_Addr" address of first STACK byte for test.        */         
/*   "Test_Stack_Addr" = MARCH_GALPAT_STACK_BASE + R0. R0 uses to set shift of first byte for test. */
/*                                                                                           */
/* Return:                                                                                   */
/*   void.                                                                                   */
/*                                                                                           */
/* Destroys:                                                                                 */
/*   none.                                                                                   */
/*-------------------------------------------------------------------------------------------*/
/* void March_Test_Stack_Init(uint8_t shift); */
.global March_Test_Stack_Init
.func March_Test_Stack_Init, March_Test_Stack_Init
.type March_Test_Stack_Init, %function
.thumb_func
March_Test_Stack_Init:

    PUSH  {R1, R2}

    /* Set base address for March checkerboard test */
    LDR r1, =MARCH_GALPAT_STACK_BASE    
    
    /* Set shift of start test address */ 
    ADD r1, r1, r0
    
     LDR r2, =Test_Stack_Addr
    STR r1, [r2, #0]
    
    POP  {R1, R2}
    
    BX lr
    
.endfunc
    

/*----------------------------------------------------------------------------------*/
/* March_Test_Stack                                                                 */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*  This function perform Stack RAM self test using March method. This function     */
/*  first duplicates data in the area to be tested as this test is destructive.     */
/*  At the end of this test the original data is restored to its correct location   */
/*  in memory.                                                                      */
/*                                                                                  */
/* Parameters:                                                                      */
/*                                                                                  */
/* Return:                                                                          */
/*   Result of test:  "1" - fail test;                                              */
/*                    "2" - pass, but still testing status                          */
/*                    "3" - pass and complete status                                */
/* Destroys:                                                                        */
/*   r0                                                                             */
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Stack(void); */
.global March_Test_Stack
.func March_Test_Stack, March_Test_Stack
.type March_Test_Stack, %function
.thumb_func
March_Test_Stack:

    PUSH  {R1-R7, LR}

    /* Clear test status register */
    movs r7, #0 
    
    
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_GALPAT_BUFF_ADDR_START    
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_GALPAT_BUFF_ADDR_STACK_END

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH_GALPAT enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH_GALPAT | ERROR_IN_STACK_MARCH_GALPAT)

    /* Test reserved RAM area. */
     bl March_Test_Block
.endif    
    
    /* Check result of reserved RAM testing */
     cmp r7, #0
        bne Error_detected_1     
    /*----------------------------------------------------------------------------------*/
    
    
    /*-------------------------  Copy block  ----------------------------------------*/
    
    /* Set parameters to copy tested block of RAM to reserved area */
    
    /* Set address of first byte in testing block */  
    LDR r0, =Test_Stack_Addr
    LDR r0, [r0]
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_STACK_SIZE    
    ADD r1, r1, r0
    
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_STACK_END    
    
    /* Check if not end of RAM */
    CMP r2, r1    
    BPL Not_end_stack
    
        /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
        MOV r1, r2
        
Not_end_stack:
        
    /* Set parameters in r2 to copy tested block of RAM to reserved area */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
              
    /* Save r0 */
    MOV r5, r0
    
    /* Copy block of stack RAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
    /*------------------------------------------------------------------------------*/
        
   
    /*-------------------------  Test block  ---------------------------------------*/
    /* Set address of first byte in testing block */
    mov r0, r5

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH_GALPAT enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH_GALPAT | ERROR_IN_STACK_MARCH_GALPAT)

    /* Test block of stack RAM */
    bl March_Test_Block
    /*-------------------------------------------------------------------------------*/
.endif    
    
    /*-------------------------  Restore block  ------------------------------------*/
    /* Write to R0 and R1 parameters for function "Copy_Block_To_Buffer" */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
    
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer              
    /*-------------------------------------------------------------------------------*/

    /* Return error */
    cmp r7, #0
        bne Error_detected_1     
            
            
    /*-------------------------  Check boundaries  ----------------------------------*/
    /* Set address of  second last byte of STACK */
    LDR r2, =MARCH_GALPAT_STACK_END
    
    /* Check if not end of SRAM    */
    CMP r2, r1
    BNE Not_end_of_Stack
        
        
    /*----------------------------  Check results  -----------------------------------*/    
        movs r7, #PASS_COMPLETE_STATUS    
        bl Error_detected_1
        
Not_end_of_Stack:
    
    /* Calculate and save address of first byte for next RAM block */
     LDR r3, =Test_Stack_Addr
    STR r1, [r3, #0]    
    movs r7, #PASS_STILL_TESTING_STATUS
    
Error_detected_1:

    /* Return result */
    mov r0, r7 
    mov r0, r0
    
    POP   {R1-R7, PC}
    
.endfunc



/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_Init                                                                 */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*   This function initializes the SRAM base address.                               */
/*                                                                                  */
/* Parameters:                                                                      */
/*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. */
/*   "Test_SRAM_Addr" = MARCH_GALPAT_SRAM_BASE + R0. R0 uses to set shift of first         */
/*   byte for test.                                                                 */
/*                                                                                  */
/* Return:                                                                          */
/*   void.                                                                          */
/*                                                                                  */
/* Destroys:                                                                        */
/*   none.                                                                          */
/*----------------------------------------------------------------------------------*/
/* void GALPAT_Test_Init(uint8_t shift); */
.global GALPAT_Test_Init
.func GALPAT_Test_Init, GALPAT_Test_Init
.type GALPAT_Test_Init, %function
.thumb_func
GALPAT_Test_Init:

    PUSH  {R1, R2}

    /* Set base address for Galpat checkerboard test */
    LDR r1, =MARCH_GALPAT_SRAM_BASE
    
    /* Set shift of start test address */ 
    ADD r1, r1, r0
    
     LDR r2, =Test_SRAM_Addr
    STR r1, [r2, #0]
    
    POP  {R1, R2}    
    
    BX lr
    
.endfunc    


/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_SRAM                                                                  */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*  This function perform block of RAM self test using GALPAT method. This function  */
/*  first duplicates data in the area to be tested as this test is destructive.     */
/*  At the end of this test the original data is restored to its correct location   */
/*  in memory.                                                                      */
/*                                                                                  */
/* Parameters:                                                                      */    
/*   Function uses variable "Test_SRAM_Addr" to set start address of tested block   */
/*                                                                                  */
/* Return:                                                                          */
/*   Result of test:  "1" - fail test;                                              */
/*                    "2" - pass, but still testing status                          */
/*                    "3" - pass and complete status                                */
/*                                                                                  */
/* Destroys:                                                                        */
/*   r0                                                                             */
/*----------------------------------------------------------------------------------*/
/* uint8 GALPAT_Test_SRAM(void); */
.global GALPAT_Test_SRAM
.func GALPAT_Test_SRAM, GALPAT_Test_SRAM
.type GALPAT_Test_SRAM, %function
.thumb_func
GALPAT_Test_SRAM:

    PUSH  {R1-R7, LR}


    /* Clear test status register */
    movs r7, #0 

        
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_GALPAT_BUFF_ADDR_START
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_GALPAT_BUFF_ADDR_END

/* Disable SRAM GALPAT test, if ERROR_IN_STACK_MARCH_GALPAT enabled */
/* We need it because in both tests used function "GALPAT_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH_GALPAT | ERROR_IN_SRAM_MARCH_GALPAT)

    /* Run Test of reserved RAM area. */
    bl GALPAT_Test_Block
.endif  

    /* Check result of reserved RAM testing */ 
    cmp r7, #0
        bne ret_result_galpat     
    /*----------------------------------------------------------------------------------*/


    /*-----------------------------  Copy RAM block  -----------------------------------*/    
    
    /* Set parameters to copy tested block of RAM to reserved area */ 
    
    /* Set address of first byte in testing block */  
    LDR r0, =Test_SRAM_Addr
    LDR r0, [r0]  
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_SRAM_SIZE
    ADD r1, r1, r0
    
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START
    
    /* Check if not end of RAM */
    CMP r2, r1
    BPL Not_end_ram_galpat
    
        /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
        MOV r1, r2
        
Not_end_ram_galpat:
                
    /* Save r0 */
    MOV r5, r0
    
    /* Copy block of SRAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
    /*-----------------------------------------------------------------------------------*/
   
   
    /*-----------------------------  Test block of RAM  ---------------------------------*/
    /* Set address of first byte in testing block */  
    mov r0, r5

/* Disable SRAM GALPAT test, if ERROR_IN_STACK_MARCH_GALPAT enabled */
/* We need it because in both tests used function "GALPAT_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH_GALPAT | ERROR_IN_SRAM_MARCH_GALPAT)

    /* Test Block of RAM */
    bl GALPAT_Test_Block
    /*-----------------------------------------------------------------------------------*/  
.endif    
    
    /*-----------------------------  Restore block  -----------------------------------*/
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
   
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer      
    /*-----------------------------------------------------------------------------------*/


    /* Check result of RAM block testing */
    cmp r7, #0
        bne ret_result_galpat 


    /*-------------------------  Check boundaries  ----------------------------------*/
    
    /* Set address of buffer first byte (this is second last byte for test ) */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
        
    /* Check if not end of SRAM */
    cmp r2, r1             
        bne Not_end_of_ram_galpat


    /*----------------------------  Check results  -------------------------------------*/
        movs r7, #PASS_COMPLETE_STATUS 
        bl ret_result_galpat
    
Not_end_of_ram_galpat:
    
    /* Calculate and save address of first byte for next RAM block */
     LDR r3, =Test_SRAM_Addr
    STR r1, [r3, #0]
    movs r7, #PASS_STILL_TESTING_STATUS
    
ret_result_galpat:
    
    /* Return result */
    mov r0, r7     
    
    POP   {R1-R7, PC}

.endfunc


/*-------------------------------------------------------------------------------------------*/
/* GALPAT_Test_Stack_Init                                                                     */
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/* Summary:                                                                                  */
/*   This function initializes the test STACK base address.                                  */
/*                                                                                           */
/* Parameters:                                                                               */
/*   Function set to variable "Test_Stack_Addr" address of first STACK byte for test.        */         
/*   "Test_Stack_Addr" = MARCH_GALPAT_STACK_BASE + R0. R0 uses to set shift of first byte for test. */
/*                                                                                           */
/* Return:                                                                                   */
/*   void.                                                                                   */
/*                                                                                           */
/* Destroys:                                                                                 */
/*   none.                                                                                   */
/*-------------------------------------------------------------------------------------------*/
/* void GALPAT_Test_Stack_Init(uint8_t shift); */
.global GALPAT_Test_Stack_Init
.func GALPAT_Test_Stack_Init, GALPAT_Test_Stack_Init
.type GALPAT_Test_Stack_Init, %function
.thumb_func
GALPAT_Test_Stack_Init:

    PUSH  {R1, R2}

    /* Set base address for Galpat checkerboard test */
    LDR r1, =MARCH_GALPAT_STACK_BASE    
    
    /* Set shift of start test address */ 
    ADD r1, r1, r0
    
     LDR r2, =Test_Stack_Addr
    STR r1, [r2, #0]
    
    POP  {R1, R2}
    
    BX lr
    
.endfunc
    

/*----------------------------------------------------------------------------------*/
/* GALPAT_Test_Stack                                                                 */
/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/* Summary:                                                                         */
/*  This function perform Stack RAM self test using GALPAT method. This function     */
/*  first duplicates data in the area to be tested as this test is destructive.     */
/*  At the end of this test the original data is restored to its correct location   */
/*  in memory.                                                                      */
/*                                                                                  */
/* Parameters:                                                                      */
/*                                                                                  */
/* Return:                                                                          */
/*   Result of test:  "1" - fail test;                                              */
/*                    "2" - pass, but still testing status                          */
/*                    "3" - pass and complete status                                */
/* Destroys:                                                                        */
/*   r0                                                                             */
/*----------------------------------------------------------------------------------*/
/* uint8 GALPAT_Test_Stack(void); */
.global GALPAT_Test_Stack
.func GALPAT_Test_Stack, GALPAT_Test_Stack
.type GALPAT_Test_Stack, %function
.thumb_func
GALPAT_Test_Stack:

    PUSH  {R1-R7, LR}

    /* Clear test status register */
    movs r7, #0 
    
    
    /*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_GALPAT_BUFF_ADDR_START    
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_GALPAT_BUFF_ADDR_STACK_END

/* Disable STACK Galpat test, if ERROR_IN_SRAM_MARCH_GALPAT enabled */
/* We need it because in both tests used function "GALPAT_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH_GALPAT | ERROR_IN_STACK_MARCH_GALPAT)

    /* Test reserved RAM area. */
     bl GALPAT_Test_Block
.endif    
    
    /* Check result of reserved RAM testing */
     cmp r7, #0
        bne Error_detected_galpat     
    /*----------------------------------------------------------------------------------*/
    
    
    /*-------------------------  Copy block  ----------------------------------------*/
    
    /* Set parameters to copy tested block of RAM to reserved area */
    
    /* Set address of first byte in testing block */  
    LDR r0, =Test_Stack_Addr
    LDR r0, [r0]
    
    /* Set address of last byte in testing block */  
    LDR r1, =TEST_BLOCK_STACK_SIZE    
    ADD r1, r1, r0
    
    /* Set address of buffer first byte */
    LDR r2, =MARCH_GALPAT_STACK_END    
    
    /* Check if not end of RAM */
    CMP r2, r1    
    BPL Not_end_stack_galpat
    
        /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
        MOV r1, r2
        
Not_end_stack_galpat:
        
    /* Set parameters in r2 to copy tested block of RAM to reserved area */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
              
    /* Save r0 */
    MOV r5, r0
    
    /* Copy block of stack RAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
    /*------------------------------------------------------------------------------*/
        
   
    /*-------------------------  Test block  ---------------------------------------*/
    /* Set address of first byte in testing block */
    mov r0, r5

/* Disable STACK Galpat test, if ERROR_IN_SRAM_MARCH_GALPAT enabled */
/* We need it because in both tests used function "GALPAT_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH_GALPAT | ERROR_IN_STACK_MARCH_GALPAT)

    /* Test block of stack RAM */
    bl GALPAT_Test_Block
    /*-------------------------------------------------------------------------------*/
.endif    
    
    /*-------------------------  Restore block  ------------------------------------*/
    /* Write to R0 and R1 parameters for function "Copy_Block_To_Buffer" */
    LDR r2, =MARCH_GALPAT_BUFF_ADDR_START    
    
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer              
    /*-------------------------------------------------------------------------------*/

    /* Return error */
    cmp r7, #0
        bne Error_detected_galpat     
            
            
    /*-------------------------  Check boundaries  ----------------------------------*/
    /* Set address of  second last byte of STACK */
    LDR r2, =MARCH_GALPAT_STACK_END
    
    /* Check if not end of SRAM    */
    CMP r2, r1
    BNE Not_end_of_Stack_galpat
        
        
    /*----------------------------  Check results  -----------------------------------*/    
        movs r7, #PASS_COMPLETE_STATUS    
        bl Error_detected_galpat
        
Not_end_of_Stack_galpat:
    
    /* Calculate and save address of first byte for next RAM block */
     LDR r3, =Test_Stack_Addr
    STR r1, [r3, #0]    
    movs r7, #PASS_STILL_TESTING_STATUS
    
Error_detected_galpat:

    /* Return result */
    mov r0, r7 
    mov r0, r0
    
    POP   {R1-R7, PC}
    
.endfunc


.end


/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_RAM_March_GCC.S
*
* Description:
*  This file contains assembly-based SRAM March test functions for GCC ARM
*  compiler. Implements stack-free RAM testing suitable for both variable
*  RAM and stack regions.
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

#include "SelfTest_ErrorInjection.h"

    .syntax unified
    .thumb

/*******************************************************************************
* Constants
*******************************************************************************/
    .equ PATTERN_55,        0x55555555
    .equ PATTERN_AA,        0xAAAAAAAA
    .equ STL_PASS,          0
    .equ STL_FAIL,          1
    .equ MARCH_C_MINUS,     0
    .equ MARCH_X,           1

/*******************************************************************************
* Code Section
*******************************************************************************/
    .section .text
    .align 4

/*******************************************************************************
* Function Name: SelfTest_SRAM_March_Full_Asm
********************************************************************************
* Summary:
*   Full SRAM test using March algorithm. Tests entire range from startAddress
*   to endAddress. Assembly implementation with no stack usage during test.
*
* Parameters:
*   R0 - startAddress (must be 4-byte aligned)
*   R1 - endAddress (must be 4-byte aligned)
*   R2 - blockSize (must be 4-byte aligned)
*   R3 - backupAddress (must be 4-byte aligned, buffer size >= blockSize)
*   [SP+0x00] - mode (at function entry; accessed at [SP+0x24] after saving 9 registers)
*
* Return:
*   R0 - 0 = pass, 1 = fail
*
* NOTE: All addresses and sizes MUST be 4-byte aligned. Unaligned values
*   may cause access beyond intended memory regions or HardFault.
*   The backup buffer must be allocated with size >= blockSize.
*******************************************************************************/
    .global SelfTest_SRAM_March_Full_Asm
    .type SelfTest_SRAM_March_Full_Asm, %function
    .thumb_func
SelfTest_SRAM_March_Full_Asm:
    /* Save context */
    PUSH    {R4-R7, LR}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    PUSH    {R4-R7}

    /* Store parameters in high registers */
    MOV     R8, R0              /* startAddress */
    MOV     R9, R1              /* endAddress */
    MOV     R10, R3             /* backupAddress */
    MOV     R11, R2             /* blockSize */
    
    /* Load mode from stack (9 regs pushed = 0x24 offset) */
    LDR     R7, [SP, #0x24]

    /***************************************************************************
    * Test Backup Area First
    ***************************************************************************/
    MOV     R0, R10             /* backupAddress */
    MOV     R1, R11             /* blockSize */
    MOV     R2, R7              /* mode */
    BL      March_Segment_Internal
    /* R4-R7 clobbered by March_Segment_Internal (stack-free internal function) */
    
    CMP     R0, #STL_FAIL
    BEQ     full_test_exit
    
    /* Reload mode from stack (R7 was clobbered) */
    LDR     R7, [SP, #0x24]

    /***************************************************************************
    * Main Test Loop: startAddress -> endAddress
    ***************************************************************************/
    MOV     R4, R8              /* currentAddr = startAddress */

    /* Clamp blockSize to region size if needed (handles blockSize > region) */
    MOV     R0, R9
    MOV     R1, R8
    SUBS    R0, R0, R1          /* regionSize = endAddress - startAddress */
    MOV     R1, R11
    CMP     R0, R1              /* regionSize < blockSize? */
    BHS     full_test_loop
    MOV     R11, R0             /* blockSize = regionSize */

full_test_loop:
    /* Copy block to backup */
    MOV     R0, R4              /* src = currentAddr */
    MOV     R1, R11             /* size = blockSize */
    MOV     R2, R10             /* dst = backupAddress */
    BL      Copy_Block_Internal

    /* Test block - save R4 before call (March_Segment_Internal is stack-free, clobbers R4-R7) */
    MOV     R12, R4             /* Save currentAddr to R12 */
    MOV     R0, R4              /* addr = currentAddr */
    MOV     R1, R11             /* size = blockSize */
    MOV     R2, R7              /* mode */
    BL      March_Segment_Internal
    /* R4-R7 clobbered, R0 = result, R12 = saved currentAddr */
    MOV     R5, R0              /* Save result in R5 */
    MOV     R4, R12             /* Restore currentAddr from R12 */

    /* Restore block from backup */
    MOV     R0, R10             /* src = backupAddress */
    MOV     R1, R11             /* size = blockSize */
    MOV     R2, R4              /* dst = currentAddr */
    BL      Copy_Block_Internal

    LDR     R7, [SP, #0x24]     /* Reload mode from stack */

    /* Check result (now in R5) */
    CMP     R5, #STL_FAIL
    BEQ     full_test_fail

    /* Advance to next block */
    MOV     R0, R11
    ADDS    R4, R4, R0          /* currentAddr += blockSize */
    
    /* Check remaining size */
    MOV     R0, R9
    SUBS    R0, R0, R4          /* remaining = endAddress - currentAddr */
    CMP     R0, #0
    BLE     full_test_pass
    
    /* Adjust blockSize if last block is smaller */
    MOV     R1, R11
    CMP     R0, R1
    BHS     full_test_loop
    MOV     R11, R0             /* Use remaining as blockSize */
    B       full_test_loop

full_test_pass:
    MOVS    R0, #STL_PASS
    B       full_test_exit

full_test_fail:
    MOVS    R0, #STL_FAIL

full_test_exit:
    /* Restore context */
    MOV     R4, R0              /* Save return value */
    POP     {R0-R3}             /* Pop saved R8-R11 into R0-R3 */
    MOV     R11, R3
    MOV     R10, R2
    MOV     R9, R1
    MOV     R8, R0
    MOV     R0, R4              /* Restore return value */
    POP     {R4-R7, PC}
    .size SelfTest_SRAM_March_Full_Asm, . - SelfTest_SRAM_March_Full_Asm


/*******************************************************************************
* Function Name: SelfTest_SRAM_March_Runtime_Asm
********************************************************************************
* Summary:
*   Runtime SRAM test. Tests one block per call, maintains state via
*   currentAddress pointer. Suitable for periodic testing without blocking.
*
* Parameters:
*   R0 - startAddress (must be 4-byte aligned)
*   R1 - endAddress (must be 4-byte aligned)
*   R2 - currentAddress pointer (must point to 4-byte aligned value)
*   R3 - blockSize (must be 4-byte aligned)
*   [SP+0x00] - backupAddress (at function entry; accessed at [SP+0x24] after saving 9 registers)
*   [SP+0x04] - mode (at function entry; accessed at [SP+0x28] after saving 9 registers)
*
* Return:
*   R0 - 0 = pass, 1 = fail
*
* NOTE: All addresses and sizes MUST be 4-byte aligned. Unaligned values
*   may cause access beyond intended memory regions or HardFault.
*   The backup buffer must be allocated with size >= blockSize.
*   The currentAddress variable can be placed anywhere except inside the
*   backup buffer region [backupAddress, backupAddress + blockSize).
*******************************************************************************/
    .global SelfTest_SRAM_March_Runtime_Asm
    .type SelfTest_SRAM_March_Runtime_Asm, %function
    .thumb_func
SelfTest_SRAM_March_Runtime_Asm:
    /* Save context */
    PUSH    {R4-R7, LR}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    PUSH    {R4-R7}

    /* Store parameters */
    MOV     R8, R0              /* startAddress */
    MOV     R9, R1              /* endAddress */
    MOV     R11, R3             /* blockSize */
    
    /* Load backupAddress from stack BEFORE saving &currentAddress */
    LDR     R6, [SP, #0x24]     /* backupAddress from stack */
    MOV     R10, R6             /* backupAddress -> R10 */
    
    /* Save &currentAddress (R2) to stack for later access */
    STR     R2, [SP, #0x24]     /* Now [SP+0x24] = &currentAddress */
    
    /* Load mode and dereference currentAddress */
    LDR     R7, [SP, #0x28]     /* mode */
    LDR     R4, [R2]            /* currentAddress value */

    /* Check if first iteration (currentAddress == startAddress) */
    CMP     R4, R8
    BNE     runtime_test_block

    /***************************************************************************
    * First Iteration: Test Backup Area
    ***************************************************************************/
    MOV     R0, R10             /* backupAddress */
    MOV     R1, R11             /* blockSize */
    MOV     R2, R7              /* mode */
    BL      March_Segment_Internal
    /* R4-R7 clobbered by March_Segment_Internal (stack-free), R0 = result */
    MOV     R5, R0              /* Save result (writing new value to clobbered R5) */

    /* Only advance currentAddress when no backup fault */
    CMP     R5, #STL_FAIL
    BEQ     runtime_backup_done

    /* Update currentAddress = startAddress + blockSize */
    MOV     R0, R8
    MOV     R1, R11
    ADDS    R0, R0, R1
    LDR     R6, [SP, #0x24]     /* Reload &currentAddress (writing new value to clobbered R6) */
    STR     R0, [R6]            /* Store new currentAddress */

runtime_backup_done:
    MOV     R0, R5              /* Return result */
    B       runtime_exit

    /***************************************************************************
    * Other Iterations: Test RAM Block
    ***************************************************************************/
runtime_test_block:
    /* Determine block to test (currentAddress - blockSize points to untested block) */
    MOV     R0, R11
    SUBS    R4, R4, R0          /* blockToTest = currentAddress - blockSize */

    /* Calculate actual block size (may be smaller for last block) */
    MOV     R6, R11             /* actualBlockSize = blockSize */
    MOV     R0, R9
    SUBS    R0, R0, R4          /* remaining = endAddress - blockToTest */
    CMP     R0, R6
    BHS     runtime_copy_to_backup
    MOV     R6, R0              /* actualBlockSize = remaining */

runtime_copy_to_backup:
    /* Copy block to backup */
    MOV     R0, R4              /* src = blockToTest */
    MOV     R1, R6              /* size = actualBlockSize */
    MOV     R2, R10             /* dst = backupAddress */
    BL      Copy_Block_Internal

    /* Test block - save R4 before call (March_Segment_Internal is stack-free, clobbers R4-R7) */
    MOV     R12, R4             /* Save blockToTest to R12 */
    MOV     R0, R4              /* addr = blockToTest */
    MOV     R1, R6              /* size = actualBlockSize */
    MOV     R2, R7              /* mode */
    BL      March_Segment_Internal
    /* R4-R7 clobbered, R0 = result, R12 = saved blockToTest */
    MOV     R5, R0              /* Save result in R5 */
    MOV     R4, R12             /* Restore blockToTest from R12 */
    
    /* Recalculate actualBlockSize (R6 was clobbered) */
    MOV     R6, R11             /* actualBlockSize = blockSize */
    MOV     R0, R9
    SUBS    R0, R0, R4          /* remaining = endAddress - blockToTest */
    CMP     R0, R6
    BHS     runtime_do_restore
    MOV     R6, R0              /* actualBlockSize = remaining */

runtime_do_restore:
    /* Restore block from backup */
    MOV     R0, R10             /* src = backupAddress */
    MOV     R1, R6              /* size = actualBlockSize */
    MOV     R2, R4              /* dst = blockToTest */
    BL      Copy_Block_Internal

    /* Check if we just tested the last block in this pass.
     * Wrap when: blockToTest + actualBlockSize >= endAddress
     * This ensures partial blocks at the end are tested before wrapping.
     * (R4 = blockToTest, R6 = actualBlockSize - both preserved by Copy_Block_Internal) */
    MOV     R0, R4              /* blockToTest */
    ADDS    R0, R0, R6          /* blockToTest + actualBlockSize */
    CMP     R0, R9              /* >= endAddress? */
    BHS     runtime_wrap        /* Yes, completed this pass */

    /* More blocks to test, advance currentAddress normally */
    LDR     R3, [SP, #0x24]     /* &currentAddress into R3 */
    LDR     R0, [R3]            /* Load current value */
    MOV     R1, R11
    ADDS    R0, R0, R1          /* currentAddress += blockSize */
    B       runtime_store_addr

runtime_wrap:
    /* Completed pass through entire range, wrap to start */
    LDR     R3, [SP, #0x24]     /* &currentAddress into R3 */
    MOV     R0, R8              /* currentAddress = startAddress */

runtime_store_addr:
    STR     R0, [R3]            /* Store updated currentAddress */
    MOV     R0, R5              /* Return test result */

runtime_exit:
    /* Restore context */
    MOV     R4, R0
    POP     {R0-R3}
    MOV     R11, R3
    MOV     R10, R2
    MOV     R9, R1
    MOV     R8, R0
    MOV     R0, R4
    POP     {R4-R7, PC}
    .size SelfTest_SRAM_March_Runtime_Asm, . - SelfTest_SRAM_March_Runtime_Asm


/*******************************************************************************
* Function Name: March_Segment_Internal
********************************************************************************
* Summary:
*   Internal function to perform March test on a memory segment.
*   Selects March C- or March X based on mode parameter.
*   STACK-FREE: Caller must save R4-R7 if needed. Returns via BX LR.
*
* Parameters:
*   R0 - startAddress
*   R1 - blockSize
*   R2 - mode (0 = March C-, 1 = March X)
*
* Return:
*   R0 - 0 = pass, 1 = fail
*
* Clobbers:
*   R4-R7 (caller must save if needed)
*******************************************************************************/
    .type March_Segment_Internal, %function
    .thumb_func
March_Segment_Internal:
    /* NO PUSH - stack-free leaf function. */
    
    /* Save mode to R4 temporarily (will be moved to R3 later) */
    MOV     R4, R2

    /* Calculate end address (caller must ensure 4-byte aligned blockSize) */
    ADDS    R3, R0, R1          /* endAddr = startAddr + blockSize */
    
    /* Save startAddr and endAddr */
    MOV     R6, R0              /* R6 = startAddr */
    MOV     R7, R3              /* R7 = endAddr */
    
    /* Move saved mode to R3 for later comparison */
    MOV     R3, R4              /* R3 = mode */
    
    /* Load patterns */
    LDR     R4, =PATTERN_55
    LDR     R5, =PATTERN_AA

    /* Force literal pool */
    B       march_check_mode
    .ltorg
march_check_mode:
    /* Check which algorithm to use (mode is in R3) */
    CMP     R3, #MARCH_X
    BEQ     march_x_algo

    /***************************************************************************
    * March C- Algorithm (10n complexity)
    * Step 1: W(0x55) ascending
    * Step 2: R(0x55) W(0xAA) ascending
    * Step 3: R(0xAA) W(0x55) ascending
    * Step 4: R(0x55) W(0xAA) descending
    * Step 5: R(0xAA) W(0x55) descending
    * Step 6: R(0x55) descending
    ***************************************************************************/

    /* Step 1: Write 0x55555555 ascending */
    MOV     R0, R6
march_c_step1:
#if (ERROR_IN_SRAM_MARCH_STACKFREE == 1u)
    /* Error injection (controlled by ERROR_IN_SRAM_MARCH_STACKFREE in SelfTest_ErrorInjection.h):
     * write wrong pattern at first address */
    CMP     R0, R6              /* Is this the first word? */
    BNE     march_c_step1_normal
    STR     R5, [R0]            /* Write 0xAA instead of 0x55 - will fail in Step 2 */
    B       march_c_step1_next
march_c_step1_normal:
#endif
    STR     R4, [R0]
march_c_step1_next:
    ADDS    R0, R0, #4
    CMP     R0, R7
    BLO     march_c_step1

    /* Step 2: Read 0x55, Write 0xAA ascending */
    MOV     R0, R6
march_c_step2:
    LDR     R1, [R0]
    CMP     R1, R4
    BNE     march_fail
    STR     R5, [R0]
    ADDS    R0, R0, #4
    CMP     R0, R7
    BLO     march_c_step2

    /* Step 3: Read 0xAA, Write 0x55 ascending */
    MOV     R0, R6
march_c_step3:
    LDR     R1, [R0]
    CMP     R1, R5
    BNE     march_fail
    STR     R4, [R0]
    ADDS    R0, R0, #4
    CMP     R0, R7
    BLO     march_c_step3

    /* Step 4: Read 0x55, Write 0xAA descending */
    MOV     R0, R7
    SUBS    R0, R0, #4
march_c_step4:
    LDR     R1, [R0]
    CMP     R1, R4
    BNE     march_fail
    STR     R5, [R0]
    CMP     R0, R6
    BEQ     march_c_step5_init
    SUBS    R0, R0, #4
    B       march_c_step4

march_c_step5_init:
    /* Step 5: Read 0xAA, Write 0x55 descending */
    MOV     R0, R7
    SUBS    R0, R0, #4
march_c_step5:
    LDR     R1, [R0]
    CMP     R1, R5
    BNE     march_fail
    STR     R4, [R0]
    CMP     R0, R6
    BEQ     march_c_step6_init
    SUBS    R0, R0, #4
    B       march_c_step5

march_c_step6_init:
    /* Step 6: Read 0x55 descending */
    MOV     R0, R7
    SUBS    R0, R0, #4
march_c_step6:
    LDR     R1, [R0]
    CMP     R1, R4
    BNE     march_fail
    CMP     R0, R6
    BEQ     march_pass
    SUBS    R0, R0, #4
    B       march_c_step6

    /***************************************************************************
    * March X Algorithm (6n complexity)
    * Step 1: W(0x55) ascending
    * Step 2: R(0x55) W(0xAA) ascending
    * Step 3: R(0xAA) W(0x55) descending
    * Step 4: R(0x55) descending
    ***************************************************************************/
march_x_algo:
    /* Step 1: Write 0x55555555 ascending */
    MOV     R0, R6
march_x_step1:
#if (ERROR_IN_SRAM_MARCH_STACKFREE == 1u)
    /* Error injection (controlled by ERROR_IN_SRAM_MARCH_STACKFREE in SelfTest_ErrorInjection.h):
     * write wrong pattern at first address */
    CMP     R0, R6              /* Is this the first word? */
    BNE     march_x_step1_normal
    STR     R5, [R0]            /* Write 0xAA instead of 0x55 - will fail in Step 2 */
    B       march_x_step1_next
march_x_step1_normal:
#endif
    STR     R4, [R0]
march_x_step1_next:
    ADDS    R0, R0, #4
    CMP     R0, R7
    BLO     march_x_step1

    /* Step 2: Read 0x55, Write 0xAA ascending */
    MOV     R0, R6
march_x_step2:
    LDR     R1, [R0]
    CMP     R1, R4
    BNE     march_fail
    STR     R5, [R0]
    ADDS    R0, R0, #4
    CMP     R0, R7
    BLO     march_x_step2

    /* Step 3: Read 0xAA, Write 0x55 descending */
    MOV     R0, R7
    SUBS    R0, R0, #4
march_x_step3:
    LDR     R1, [R0]
    CMP     R1, R5
    BNE     march_fail
    STR     R4, [R0]
    CMP     R0, R6
    BEQ     march_x_step4_init
    SUBS    R0, R0, #4
    B       march_x_step3

march_x_step4_init:
    /* Step 4: Read 0x55 descending */
    MOV     R0, R7
    SUBS    R0, R0, #4
march_x_step4:
    LDR     R1, [R0]
    CMP     R1, R4
    BNE     march_fail
    CMP     R0, R6
    BEQ     march_pass
    SUBS    R0, R0, #4
    B       march_x_step4

march_pass:
    MOVS    R0, #STL_PASS
    BX      LR

march_fail:
    MOVS    R0, #STL_FAIL
    BX      LR
    .size March_Segment_Internal, . - March_Segment_Internal


/*******************************************************************************
* Function Name: Copy_Block_Internal
********************************************************************************
* Summary:
*   Copy memory block (32-bit word aligned). Copies word-by-word from low to
*   high addresses. Source and destination regions must not overlap.
*
* Parameters:
*   R0 - source address
*   R1 - size in bytes
*   R2 - destination address
*
* Return:
*   None
*
* Clobbers:
*   R0, R1, R2, R3
*******************************************************************************/
    .type Copy_Block_Internal, %function
    .thumb_func
Copy_Block_Internal:
    ADDS    R1, R0, R1          /* R1 = endAddr = srcAddr + size */

copy_loop:
    LDR     R3, [R0]
    STR     R3, [R2]
    ADDS    R0, R0, #4
    ADDS    R2, R2, #4
    CMP     R0, R1
    BLO     copy_loop
    
    BX      LR
    .size Copy_Block_Internal, . - Copy_Block_Internal

    .end


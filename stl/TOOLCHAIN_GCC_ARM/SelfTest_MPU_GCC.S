/*******************************************************************************
* File Name: SelfTest_MPU_GCC.S
*
* Description: Toolchain-specific recoverable-fault helper for the ARM
* Cortex-M core MPU self-test (SelfTest_MPU.c): the denied-write helper and
* the resume-point label used by the application-owned MemManage_Handler
* (via SelfTest_MPU_HandleMemManage) to skip the faulted STR.
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

.syntax unified
.text
.thumb

#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_8M_MAIN__)

/*----------------------------------------------------------------------------------*/
/* stlMpu_triggerWrite / stlMpu_resumePoint                                         */
/*----------------------------------------------------------------------------------*/
/* void stlMpu_triggerWrite(volatile uint32_t *address, uint32_t value);            */
/*                                                                                  */
/* Stores value (R1) to address (R0). If the ARM core MPU denies the store,         */
/* the application-owned MemManage_Handler forwards to                              */
/* SelfTest_MPU_HandleMemManage (SelfTest_MPU.c), which rewrites the stacked        */
/* return address to stlMpu_resumePoint (exported below), so execution              */
/* resumes at the BX LR instead of the faulted STR.                                 */
/*----------------------------------------------------------------------------------*/
.global stlMpu_triggerWrite
.type stlMpu_triggerWrite, %function
.thumb_func
stlMpu_triggerWrite:
    STR   R1, [R0]

.global stlMpu_resumePoint
.type stlMpu_resumePoint, %function
.thumb_func
stlMpu_resumePoint:
    BX    LR

#endif /* defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_8M_MAIN__) */

.end

/* [] END OF FILE */

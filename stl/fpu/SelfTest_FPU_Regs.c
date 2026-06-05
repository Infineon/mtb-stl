/*******************************************************************************
* File Name: SelfTest_FPU_Regs.c
*
* Description:
*  This file provides the source code for the FPU register self tests.
*
*******************************************************************************
* (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
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

#include "cy_pdl.h"
#include "SelfTest_FPU_Regs.h"

#if (defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U))

/*******************************************************************************
 * Function Name: SelfTest_FPU_REG
 *******************************************************************************
 *
 * Summary:
 *  This function performs the checkerboard test for all FPU registers.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_FPU_Registers(void)
{
    uint8_t ret;

    /* Enable the global interrupts */
    __disable_irq();
    #if defined(__GNUC__)
    ret = SelfTest_FPU_Regs_GCC();
    #elif defined(__ICCARM__)
    ret = SelfTest_FPU_Regs_IAR();
    #elif defined (__ARMCC_VERSION)
    ret = SelfTest_FPU_Regs_ARM();
    #else
    ret = ERROR_STATUS;
    #endif
    /* Enable the global interrupts */
    __enable_irq();

    return ret;
}


#endif /* if (defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) */

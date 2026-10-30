/*******************************************************************************
* File Name: SelfTest_PRIMASK.c
*
* Description:
*  This file provides the source code for the PRIMASK register self-test.
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
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

#include "SelfTest_PRIMASK.h"
#include "SelfTest_ErrorInjection.h"

#define SELFTEST_PRIMASK_PM_Msk             (1UL << 0U)
#define SELFTEST_PRIMASK_nPRIV_Msk          (1UL << 0U)

/*******************************************************************************
 * Function Name: SelfTest_PRIMASK
 *******************************************************************************
 *
 * Summary:
 *  This function tests the PM bit of the PRIMASK register with the values 0 and
 *  1, and attempts to restore the original PRIMASK value.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test: OK_STATUS - pass; ERROR_STATUS - fail;
 *  ERROR_INCORRECT_USAGE_STATUS - called from unprivileged Thread mode.
 *
 ******************************************************************************/
uint8_t SelfTest_PRIMASK(void)
{
    uint8_t ret = ERROR_INCORRECT_USAGE_STATUS;

    /* Handler mode is always privileged. A PRIMASK write from unprivileged Thread mode is ignored. */
    if ((0UL != __get_IPSR()) || (0UL == (__get_CONTROL() & SELFTEST_PRIMASK_nPRIV_Msk)))
    {
        const uint32_t original = __get_PRIMASK();
        uint32_t readBack;

        ret = OK_STATUS;

        /* PM = 0 */
        __set_PRIMASK(0UL);
        #if (ERROR_IN_PRIMASK_REGISTER)
        /* Force PM to 1 after the write, as if the bit were stuck at 1 */
        __set_PRIMASK(SELFTEST_PRIMASK_PM_Msk);
        #endif
        readBack = __get_PRIMASK() & SELFTEST_PRIMASK_PM_Msk;
        if (0UL != readBack)
        {
            ret = ERROR_STATUS;
        }

        /* PM = 1 */
        __set_PRIMASK(SELFTEST_PRIMASK_PM_Msk);
        readBack = __get_PRIMASK() & SELFTEST_PRIMASK_PM_Msk;
        if (SELFTEST_PRIMASK_PM_Msk != readBack)
        {
            ret = ERROR_STATUS;
        }

        /* Restore the original value, and check it */
        __set_PRIMASK(original);
        readBack = __get_PRIMASK() & SELFTEST_PRIMASK_PM_Msk;
        if ((original & SELFTEST_PRIMASK_PM_Msk) != readBack)
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


/* [] END OF FILE */

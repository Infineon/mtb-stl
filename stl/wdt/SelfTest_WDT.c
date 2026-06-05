/*******************************************************************************
* File Name: SelfTest_WDT.c
*
* Description:
*  This file provides the source code for the watchdog timer
*  Class B self tests.
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
#include "SelfTest_WDT.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_S8SRSSLT)

/*****************************************************************************
* Function Name: SelfTest_WDT
******************************************************************************
*
* Summary:
*  This function performs the Watchdog test. the function runs WDT and waits for a defined
*  period of time. If the PSOC was not reset, the function returns the ERROR status.
*  If the PSOC was reset, bit 1 in CyResetStatus must be set to 1. If
*  this bit was set, the function returns the OK status.
*
* Parameters:
* NONE
*
* Return:
*  0 - test pass
*  1 - test failed
*
* Note:
* This function provides a watchdog reset or stops the software execution if
* the watchdog fails.
*
*****************************************************************************/
uint8_t SelfTest_WDT(void)
{
    uint8_t ret;
    uint16_t guardCnt = 0u;


    /* If MCU was reset by WDT */
    if ((Cy_SysLib_GetResetReason() & CY_SYSLIB_RESET_HWWDT) != 0u)
    {
        /* If WDT caused a reset, return the OK status */
        ret = OK_STATUS;
    }
    else
    {
        /* Check if an intentional error should be made for testing */
        /* if "yes" - WDT will not run */
        #if (ERROR_IN_WDT == 0)
        /* Initializing WDT */

        /* Write the ignore bits */
        Cy_WDT_SetIgnoreBits(IGNORE_BITS);
        if (Cy_WDT_GetIgnoreBits() != IGNORE_BITS)
        {
            CY_ASSERT(0);
        }

        /* Clear the match event interrupt, if any */
        Cy_WDT_ClearInterrupt();

        /* Enable ILO */
        Cy_SysClk_IloEnable();

        /* Wait for the proper start-up of ILO */
        Cy_SysLib_Delay(ILO_START_UP_TIME);

        /* Write the match value if periodic Interrupt mode selected */
        Cy_WDT_SetMatch(WDT_PERIOD);
        if (Cy_WDT_GetMatch() != WDT_PERIOD)
        {
            CY_ASSERT(0);
        }

        /* Enable WDT */
        Cy_WDT_Enable();
        if (Cy_WDT_IsEnabled() == false)
        {
            CY_ASSERT(0);
        }

        #endif /* End !(ERROR_IN_WDT == 1) */


        /* Reset the PSoC by WDT */
        /* WDT_DATA_TIME defined in SelfTest_WDT.h */
        /* During period of next cycle WDT must reset the PSoC */
        /* If the PSoC was not reset, WDT works incorrect */
        while ((guardCnt < WDT_DATA_TIME))
        {
            /* Wait for WDT reset */
            guardCnt++;
            Cy_SysLib_Delay(1u);
        }

        /* If the PSoC was not reset, WDT works incorrect */
        /* Return ERROR status  */
        ret = ERROR_STATUS;
    }

    return ret;
}


#elif (defined (CY_IP_MXS40SRSS) || defined (CY_IP_MXS40SSRSS))

uint8_t SelfTest_WDT(void)
{
    uint8_t ret;
    uint16_t guardCnt = 0u;

    if (CY_SYSLIB_RESET_HWWDT == Cy_SysLib_GetResetReason())
    {
        Cy_SysLib_ClearResetReason();
        /* If WDT cause a reset return OK status */
        ret = OK_STATUS;
    }
    else
    {
        Cy_SysLib_ClearResetReason();

        /* Initialize WDT */

        /* Step 1- Unlock WDT */
        Cy_WDT_Unlock();

        #if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2))
        /* Step 2- Write the ignore bits - operate with only 14 bits */
        Cy_WDT_SetIgnoreBits(IGNORE_BITS);

        /* Step 3- Write match value */
        Cy_WDT_SetMatch(WDT_PERIOD);

        #elif (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION >= 2))
        Cy_WDT_SetUpperLimit(WDT_PERIOD); /* Reset after 3 seconds */
        Cy_WDT_SetUpperAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_RESET);
        #elif defined (CY_IP_MXS40SSRSS)
        /* Step 2- Write the match bits - operate with only 14 bits */
        Cy_WDT_SetMatchBits(MATCH_BITS);
        /* Step 3- Write match value */
        Cy_WDT_SetMatch(WDT_PERIOD);
        #endif /* if (defined (CY_IP_MXS40SRSS) && (CY_IP_MXS40SRSS_VERSION < 2)) */

        /* Step 4- Clear match event interrupt, if any */
        Cy_WDT_ClearInterrupt();
        #if (ERROR_IN_WDT == 0)
        /* Step 5- Enable WDT */
        Cy_WDT_Enable();
        #endif
        /* Step 6- Lock WDT configuration */
        Cy_WDT_Lock();

        while ((guardCnt < WDT_DATA_TIME))
        {
            /* Wait for WDT reset */
            guardCnt++;
            Cy_SysLib_Delay(500u);
        }

        /* If the PSoC was not reset, WDT works incorrect */
        /* Return ERROR status  */
        ret = ERROR_STATUS;
    }
    return ret;
}


#endif /* if defined(CY_IP_S8SRSSLT) */

/* [] END OF FILE */

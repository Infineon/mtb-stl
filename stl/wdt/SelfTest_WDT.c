/*******************************************************************************
* File Name: SelfTest_WDT.c
* Version 1.0.0
*
* Description:
*  This file provides the source code for the watchdog timer
*  Class B support tests.
*
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "cy_pdl.h"
#include "SelfTest_WDT.h"
#include "SelfTest_ErrorInjection.h"

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 8.6', 1, \
'Only one defination will be used during compilation.')
#if CY_CPU_CORTEX_M0P

/*****************************************************************************
* Function Name: SelfTest_WDT
******************************************************************************
*
* Summary:
*  This function perform Watchdog test. Function runs WDT and wait defined
*  period of time. If the PSoC was not reset function return ERROR status.
*  If the PSoC was reset bit "1" in CyResetStatus must be set to "1". If
*  this bit was set function return OK status.
*
* Parameters:
* NONE
*
* Return:
*  0 - test pass
*  1 - test failed
*
* Note:
* This function provides watchdog reset or stops software execution if
* watchdog fails
*
*****************************************************************************/
uint8_t SelfTest_WDT(void)
{
    uint8_t ret;
    uint16_t guardCnt = 0u;


    /* If MCU was reset by WDT */
    if ((Cy_SysLib_GetResetReason() & CY_SYSLIB_RESET_HWWDT) != 0u)
    {
        /* If WDT cause a reset return OK status */
        ret = OK_STATUS;
    }
    else
    {
        /* Check if intentional error should be made for testing */
        /* if "yes" - WDT will not run */
        #if (ERROR_IN_WDT == 0)
        /* Initializing WDT */

        /* Write the ignore bits */
        Cy_WDT_SetIgnoreBits(IGNORE_BITS);
        if (Cy_WDT_GetIgnoreBits() != IGNORE_BITS)
        {
            CY_ASSERT(0);
        }

        /* Clear match event interrupt, if any */
        Cy_WDT_ClearInterrupt();

        /* Enable ILO */
        Cy_SysClk_IloEnable();

        /* Waiting for proper start-up of ILO */
        Cy_SysLib_Delay(ILO_START_UP_TIME);

        /* Write match value if periodic interrupt mode selected */
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

#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7 ||CY_CPU_CORTEX_M33 )

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

#if CY_CPU_CORTEX_M4
        /* Step 2- Write the ignore bits - operate with only 14 bits */
        Cy_WDT_SetIgnoreBits(IGNORE_BITS);

        /* Step 3- Write match value */
        Cy_WDT_SetMatch(WDT_PERIOD);

#elif CY_CPU_CORTEX_M7
        Cy_WDT_SetUpperLimit(WDT_PERIOD); /* Reset after 3 seconds */
        Cy_WDT_SetUpperAction(CY_WDT_LOW_UPPER_LIMIT_ACTION_RESET);
#elif CY_CPU_CORTEX_M33
        /* Step 2- Write the match bits - operate with only 14 bits */
        Cy_WDT_SetMatchBits(MATCH_BITS);
        /* Step 3- Write match value */
        Cy_WDT_SetMatch(WDT_PERIOD);
#endif

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

#endif
CY_MISRA_BLOCK_END('MISRA C-2012 Rule 8.6')

/* [] END OF FILE */

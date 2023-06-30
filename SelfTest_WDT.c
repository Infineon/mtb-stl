/*******************************************************************************
* File Name: SelfTest_WDT.c
* Version 2.0
*
* Description:
*  This file provides the source code for the watchdog timer
*  Class B support tests.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
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

#include "cy_pdl.h"
#include "SelfTest_WDT.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

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


/* [] END OF FILE */

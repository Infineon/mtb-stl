/*******************************************************************************
* File Name: SelfTest_ConfigRegisters.c
* Version 2.0
*
* Description:
*  This file provides the source code to the APIs for configuration register self
*  tests for PSoC 4.
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
#include "SelfTest_CRC_calc.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_ConfigRegisters.h"
#include "SelfTest_Config.h"

#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)
/* Table of register 32Bit registers to be tested */
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),
    &(HSIOM_PRT5->PORT_SEL),
    &(HSIOM_PRT6->PORT_SEL),
    &(HSIOM_PRT7->PORT_SEL),
    &(HSIOM_PRT8->PORT_SEL),
    &(HSIOM_PRT9->PORT_SEL),
    &(HSIOM_PRT10->PORT_SEL),
    &(HSIOM_PRT11->PORT_SEL),
    &(HSIOM_PRT12->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
    &(GPIO_PRT5->PC),
    &(GPIO_PRT5->PC2),
    &(GPIO_PRT6->PC),
    &(GPIO_PRT6->PC2),
    &(GPIO_PRT7->PC),
    &(GPIO_PRT7->PC2),

    &(GPIO_PRT8->PC),
    &(GPIO_PRT8->PC2),
    &(GPIO_PRT9->PC),
    &(GPIO_PRT9->PC2),
    &(GPIO_PRT10->PC),
    &(GPIO_PRT10->PC2),
    &(GPIO_PRT11->PC),
    &(GPIO_PRT11->PC2),
    &(GPIO_PRT12->PC),
    &(GPIO_PRT12->PC2),
};
#endif /*  4100S MAX  */

/* Table of register 32Bit registers to be tested */
#if (defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS) && (CY_FLASH_SIZE == 0x00040000UL)) || \
    (defined(CY_DEVICE_SERIES_PSOC_4500S))
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),
    &(HSIOM_PRT5->PORT_SEL),
    &(HSIOM_PRT6->PORT_SEL),
    &(HSIOM_PRT7->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
    &(GPIO_PRT5->PC),
    &(GPIO_PRT5->PC2),
    &(GPIO_PRT6->PC),
    &(GPIO_PRT6->PC2),
    &(GPIO_PRT7->PC),
    &(GPIO_PRT7->PC2),
};
#endif /* 4500S & 4100S Plus 256k */

#if defined(CY_DEVICE_SERIES_PSOC_4100S)
static volatile uint32_t* regs32_ToTest[] =
{
    /* Clock registers */
    &(SRSSLT->CLK_IMO_SELECT),
    &(SRSSLT->CLK_IMO_TRIM1),
    &(SRSSLT->CLK_IMO_TRIM2),
    &(SRSSLT->CLK_IMO_TRIM3),
    &(SRSSLT->CLK_IMO_CONFIG),
    &(SRSSLT->CLK_ILO_CONFIG),
    &(PERI->PCLK_CTL[2]),
    &(PERI->PCLK_CTL[5]),
    &(PERI->DIV_16_CTL[0]),
    &(PERI->DIV_16_CTL[1]),
    &(WCO->WDT_CONFIG),

    /* Analog routing regs */
    &(PASS0_DSAB->DSAB_CTRL),
    &(SAR0->PUMP_CTRL),
    &(CTBM0->CTB_CTRL),

    /* HSIOM registers */
    &(HSIOM_PRT0->PORT_SEL),
    &(HSIOM_PRT1->PORT_SEL),
    &(HSIOM_PRT2->PORT_SEL),
    &(HSIOM_PRT3->PORT_SEL),
    &(HSIOM_PRT4->PORT_SEL),

    /* IO Pin registers */
    &(GPIO_PRT0->PC),
    &(GPIO_PRT0->PC2),
    &(GPIO_PRT1->PC),
    &(GPIO_PRT1->PC2),
    &(GPIO_PRT2->PC),
    &(GPIO_PRT2->PC2),
    &(GPIO_PRT3->PC),
    &(GPIO_PRT3->PC2),
    &(GPIO_PRT4->PC),
    &(GPIO_PRT4->PC2),
};
#endif /* 4100S */

/* Buffer to store Flash row */
static uint32_t flashRowData[CY_FLASH_SIZEOF_ROW/(sizeof(uint32_t))];

#if (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
static uint8_t SelfTests_Check_StartUp_Cfg_CRC(uint32_t Current_CRC);
#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE) */


#if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)

/*******************************************************************************
 * Function Name: SelfTests_Save_StartUp_ConfigReg(void)
 *******************************************************************************
 *
 * Summary:
 *  This function stores configuration registers to FlashRowData array
 *  and writes this array to flash
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  0 - writing to flash is successful
 *  >=1 - writing to flash is not successful. Refer to Flash Driver PDL documentation
 *  for error codes.
 *
 ******************************************************************************/
cy_en_flashdrv_status_t SelfTests_Save_StartUp_ConfigReg(void)
{
    /* Fill flash row with "0" */
    (void)memset(flashRowData, 0x00, sizeof(flashRowData));

    /* Store 32Bit registers */
    for (uint8_t i = 0u; i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])); i++)
    {
        flashRowData[i] = CY_GET_REG32(regs32_ToTest[i]);
    }

    return Cy_Flash_WriteRow(CONF_REG_FIRST_ROW_ADDR, flashRowData);
}


#elif (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
/*******************************************************************************
 * Function Name: SelfTests_Check_StartUp_Cfg_CRC
 *******************************************************************************
 *
 * Summary:
 *  This function compares the calculated CRC (uint32_t Current_CRC) with the
 *  CRC previously stored in FLASH if the status semaphore for a stored CRC is
 *  set.
 *  If status semaphore is not set, it calculates an new CRC
 *  (uint32_t Current_CRC) and stores it to FLASH and sets the status semaphore.
 *
 * Parameters:
 *  uint32_t Current_CRC - CRC to compare with saved CRC
 *
 * Return:
 *  0 - No error, CRC matched or FLASH write successful.
 *  Not 0 - Error detected, CRC didn't match or FLASH write fail.
 *
 ******************************************************************************/
static uint8_t SelfTests_Check_StartUp_Cfg_CRC(uint32_t Current_CRC)
{
    uint8_t* regPointer;
    uint8_t  ret = ERROR_STATUS;
    cy_en_flashdrv_status_t status;

    /* Pointer to FLASH base */
    regPointer = (uint8_t*)CY_FLASH_BASE;

    /* Check if FLASH CRC semaphore is set */
    if (regPointer[CY_FLASH_SIZE - CRC_STARTUP_SEMAPHORE_SHIFT] == CRC_STARTUP_SEMAPHORE)
    {
        /* Check if intentional error should be made for testing */
        #if (ERROR_IN_STARTUP_CONF_REG)

        /* Make artificial error in CRC */
        Current_CRC++;
        #endif /* End (ERROR_IN_STARTUP_CONF_REG) */
        /* if "yes" - check calculated FLASH CRC from CRC component with CRC stored in FLASH */
        if ((regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 0u] == ((Current_CRC >> 0u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 1u] == ((Current_CRC >> 8u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 2u] == ((Current_CRC >> 16u) & 0xFFuL)) &&
            (regPointer[CY_FLASH_SIZE - CRC_STARTUP_LO + 3u] == ((Current_CRC >> 24u) & 0xFFuL)))
        {
            ret = PASS_COMPLETE_STATUS;
        }
    }
    else
    {
        /* Read last row from FLASH */
        for (uint16_t i = 0; i < CY_FLASH_SIZEOF_ROW; i++)
        {
            ((uint8_t*)flashRowData)[i] = regPointer[LAST_ROW_IN_FLASH_OFFSET + i];
        }


        /* If "no" - store calculated FLASH CRC and status byte to last bytes in FLASH with defined
         * shift */
        for (uint8_t i = 0; i < sizeof(Current_CRC); i++)
        {
            ((uint8_t*)flashRowData)[CY_FLASH_SIZEOF_ROW  - CRC_STARTUP_LO +
                                     i] = (uint8_t)((Current_CRC >> (8u * i)) & 0xFFuL);
        }

        ((uint8_t*)flashRowData)[CY_FLASH_SIZEOF_ROW -
                                 CRC_STARTUP_SEMAPHORE_SHIFT] = CRC_STARTUP_SEMAPHORE;

        /* Write array to last FLASH row */
        status = Cy_Flash_WriteRow(LAST_ROW_IN_FLASH_ADDR, flashRowData);
        if (CY_FLASH_DRV_SUCCESS == status)
        {
            /* Return status that CRC is already stored */
            ret = CRC_SAVED_STATUS;
        }
    }

    return ret;
}


#endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */


/*******************************************************************************
 * Function Name: SelfTests_StartUp_ConfigReg(void)
 *******************************************************************************
 *
 * Summary:
 *  This function call checks configuration registers by comparing the value
 *  stored in flash with current the current configuration registers value.
 *  If values are different function returns fail.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  0 - pass test.
 *  1 - fail test.
 *
 ******************************************************************************/
uint8_t SelfTests_StartUp_ConfigReg(void)
{
    #if (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE)
    uint32_t* cfgRegPointer;
    uint8_t ret = OK_STATUS;
    uint32_t tmp;

    /* Set base address to flash where configuration registers stored */
    cfgRegPointer = (uint32_t*)(CONF_REG_FIRST_ROW_ADDR);

    /* Compare register values with saved values */
    for (uint8_t i = 0u;
         ((i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u]))) && (ret == OK_STATUS)); i++)
    {
        tmp = cfgRegPointer[i];

        /* Check if intentional error should be made for testing */
        #if (ERROR_IN_STARTUP_CONF_REG)

        /* Make artificial error in configuration register */
        tmp++;
        #endif /* End (ERROR_IN_STARTUP_CONF_REG) */

        if (tmp != CY_GET_REG32(regs32_ToTest[i]))
        {
            ret = ERROR_STATUS;
        }
    }

    /* Return OK status if no error detected */
    return ret;
    #elif (STARTUP_CFG_REGS_MODE == CFG_REGS_CRC_MODE)
    uint32_t calculated_CRC;

    /* Fill flash row with "0" */
    (void)memset(flashRowData, 0x00, sizeof(flashRowData));

    /* Store 32Bit registers */
    for (uint8_t i = 0u; i < (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])); i++)
    {
        flashRowData[i] = CY_GET_REG32(regs32_ToTest[i]);
    }

    /* Calculate CRC of Start Up registers */
    calculated_CRC =
        SelfTests_CRC32((uint32_t)flashRowData,
                        (sizeof(regs32_ToTest) / sizeof(regs32_ToTest[0u])));

    /* Compare calculated CRC with previously saved and return status of matches */
    return SelfTests_Check_StartUp_Cfg_CRC(calculated_CRC);
    #endif /* End (STARTUP_CFG_REGS_MODE == CFG_REGS_TO_FLASH_MODE) */
}


/* [] END OF FILE */

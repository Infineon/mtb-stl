/*******************************************************************************
* File Name: SelfTest_ECC.c
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
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
#include "SelfTest_ECC.h"

#if (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))

uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t error);

uint32_t* flashAddrPtr = (uint32_t *) CY_FLASH_ADDR;
uint32_t readVal = 0;
cy_en_SysFault_source_t source = (cy_en_SysFault_source_t) CY_SYSFAULT_NO_FAULT;

cy_stc_SysFault_t sysFault_cfg = 
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false,
};

cy_stc_sysint_t sysFault_IRQ_cfg =
{
    .intrSrc = ((NvicMux3_IRQn << CY_SYSINT_INTRSRC_MUXIRQ_SHIFT) | cpuss_interrupts_fault_0_IRQn),
    .intrPriority = 0UL
};

/* Interrupt Handler */
void irqFaultReportHandler(void)
{
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    /* Get error source */
    source = Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    /* Clear fault status */
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
    /* Disable ECC */
    Cy_Flashc_MainECCDisable();
}

void Cy_SysLib_ProcessingFault(void)
{
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    /* Get error source */
    source = Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    /* Clear fault status */
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
    /* Disable ECC */
    Cy_Flashc_MainECCDisable();
}

/* ECC Configuration */
void configureECC(cy_en_ecc_error_mode_t eccErrorMode)
{
    uint32_t index;
    uint32_t data[(CY_FLASH_SIZE_ROW/4U)];
    cy_en_flashdrv_status_t res;
    for(index = 0; index < (CY_FLASH_SIZE_ROW/4U); index++)
    {
        data[index] = (uint32_t)0x5A5A5A5A;
    }

    /* Flash configuration */
    Cy_Flash_Init();
    Cy_Flashc_SetMainBankMode(CY_FLASH_SINGLE_BANK_MODE);
    Cy_Flashc_WorkWriteEnable();
    Cy_Flashc_MainWriteEnable();
    Cy_Flashc_MainECCEnable();
    FLASHC_FLASH_CTL |= FLASHC_FLASH_CTL_MAIN_ECC_INJ_EN_Msk;

    /* Configure error injection */
    if (eccErrorMode == CY_ECC_C_ERROR)
    {
        res = Cy_Flashc_InjectECC(CY_FLASH_MAIN_REGION,CY_FLASH_ADDR, CY_ECC_C_ERROR_PARITY);
    }
    if (eccErrorMode == CY_ECC_NC_ERROR)
    {
        res = Cy_Flashc_InjectECC(CY_FLASH_MAIN_REGION,CY_FLASH_ADDR, CY_ECC_NC_ERROR_PARITY);
    }

    /* Write operation */
    res = Cy_Flash_StartEraseSector((uint32_t)CY_FLASH_ADDR);
    while(Cy_Flash_IsOperationComplete() != CY_FLASH_DRV_SUCCESS)
    {
    }
    res = Cy_Flash_ProgramRow(CY_FLASH_ADDR , (const uint32_t *)data);

    /* Read operation */
    flashAddrPtr = (uint32_t *) CY_FLASH_ADDR ;
    readVal = *flashAddrPtr;
    (void) readVal;
    (void) res;
}


/*******************************************************************************
* Function Name: SelfTest_ECC
****************************************************************************//**
*
* Summary:
*  This function performs the ECC hardware self test to detect, correct the single
*  bit error and reports fault for double bit error.
*
* Parameters:
*  Error injection mode
*
* Return:
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/

uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode)
{
    uint8_t ret = ERROR_STATUS;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); // clear status
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFAULT_FLASHC_MAIN_C_ECC);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFAULT_FLASHC_MAIN_NC_ECC);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
    cy_en_SysFault_status_t faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &sysFault_cfg);
    cy_en_sysint_status_t intrStatus = Cy_SysInt_Init(&sysFault_IRQ_cfg, &irqFaultReportHandler);
    NVIC_SetPriority((IRQn_Type) NvicMux3_IRQn,  2UL);
    NVIC_EnableIRQ((IRQn_Type) NvicMux3_IRQn);

    /* ECC configuration */
    configureECC(eccErrorMode);

    /* Wait for the Fault to trigger */
    Cy_SysLib_Delay(2000);
    #if (ERROR_IN_ECC == 1)
    source = CY_SYSFAULT_MPU_0;
    #endif
    /*Verify result */
    if ((eccErrorMode == CY_ECC_NO_ERROR)  && (source == CY_SYSFAULT_NO_FAULT))
    {
        ret = OK_STATUS;
    }
    else if ((eccErrorMode == CY_ECC_C_ERROR)  && (source == CY_SYSFAULT_FLASHC_MAIN_C_ECC))
    {
        ret = OK_STATUS;
    }
    else if ((eccErrorMode == CY_ECC_NC_ERROR)  && (source == CY_SYSFAULT_FLASHC_MAIN_NC_ECC))
    {
        ret = OK_STATUS;
    }
    else
    {
        ret = ERROR_STATUS;
    }

    (void) faultStatus;
    (void) intrStatus;

    return ret;
}

#endif
/* [] END OF FILE */

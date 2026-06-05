/*******************************************************************************
* File Name: SelfTest_ECC.c
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
#include "SelfTest_ECC.h"

#if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT))

#define TIMEOUT_FOR_FAULT 2000UL

static volatile uint32_t readVal = 0;
static volatile uint8_t source = (uint8_t)CY_SYSFAULT_NO_FAULT;

#if defined(CY_IP_MXFAULT)
static uint32_t pattern_a[512] = {[0] = 0x2400F000UL, [1] = 0x02014267UL, [2] = 0x02014241UL, [3] = 0x02014249UL };
static uint32_t pattern_b[512] = {[0] = 0x34004000UL, [1] = 0x120032DFUL, [2] = 0x120032B9UL, [3] = 0x120032C1UL };

uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t error);
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t error);
#else /* if defined(CY_IP_MXFAULT) */
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t error);
static volatile uint32_t* flashAddrPtr = (volatile uint32_t*)CY_FLASH_ADDR;
#endif /* if defined(CY_IP_MXFAULT) */

static cy_stc_SysFault_t sysFault_cfg =
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false,
};
#if defined(CY_IP_M7CPUSS) || defined(CY_IP_M4CPUSS)
static cy_stc_sysint_t sysFault_IRQ_cfg =
{
    .intrSrc      =
        ((NvicMux3_IRQn << CY_SYSINT_INTRSRC_MUXIRQ_SHIFT) | cpuss_interrupts_fault_0_IRQn),
    .intrPriority = 0UL
};
#else
static cy_stc_sysint_t sysFault_IRQ_cfg =
{
    .intrSrc      = (cpuss_interrupts_fault_0_IRQn),
    .intrPriority = 0UL
};
#endif /* if defined(CY_IP_M7CPUSS) || defined(CY_IP_M4CPUSS) */

/* Interrupt Handler */
static void irqFaultReportHandler(void)
{
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    /* Get error source */
    source = (uint8_t)Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    /* Clear fault status */
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
}


void Cy_SysLib_ProcessingFault(void)
{
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    /* Get error source */
    source = (uint8_t)Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    #if defined(CY_IP_MXS40FLASHC)
    Cy_Flashc_ECCDisable();
    #endif
}


#if defined (CY_IP_MXS40FAULT)
/* ECC Configuration */
static void configureECC(cy_en_ecc_error_mode_t eccErrorMode)
{
    uint32_t index;
    CY_ALIGN(4) static uint32_t data[(CY_FLASH_SIZE_ROW/4U)];
    for (index = 0; index < (CY_FLASH_SIZE_ROW/4U); index++)
    {
        data[index] = 0x5A5A5A5AUL;
    }

    /* Flash configuration */
    Cy_Flash_Init();
    Cy_Flashc_SetMainBankMode(CY_FLASH_SINGLE_BANK_MODE);
    Cy_Flashc_WorkWriteEnable();
    Cy_Flashc_MainWriteEnable();
    Cy_Flashc_MainECCEnable();
    FLASHC_FLASH_CTL |= FLASHC_FLASH_CTL_MAIN_ECC_INJ_EN_Msk;

    #if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT)
    /* Suppress the AHB bus error on a non-correctable ECC read so it does not
     * escalate to HardFault. The fault is still latched by SysFault and
     * triggers the IRQ. */
    FLASHC_FLASH_CTL |= FLASHC_V2_FLASH_CTL_MAIN_ERR_SILENT_Msk;
    #endif

    /* Configure error injection */
    if (eccErrorMode == CY_ECC_NC_ERROR)
    {
        (void)Cy_Flashc_InjectECC(CY_FLASH_MAIN_REGION, CY_FLASH_ADDR, CY_ECC_NC_ERROR_PARITY);
    }

    /* Write operation */
    (void)Cy_Flash_StartEraseSector((uint32_t)CY_FLASH_ADDR);
    while (Cy_Flash_IsOperationComplete() != CY_FLASH_DRV_SUCCESS)
    {
    }
    (void)Cy_Flash_ProgramRow(CY_FLASH_ADDR, (const uint32_t*)data);

    /* Read operation - this will trigger the ECC fault */
    flashAddrPtr = (uint32_t*)CY_FLASH_ADDR;
    readVal = *flashAddrPtr;
    (void)readVal;
}


/*******************************************************************************
* Function Name: SelfTest_ECC
********************************************************************************
*
* Summary:
*  This function performs the ECC hardware self test to report fault for double bit error.
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
    uint8_t localSource;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_FAULT);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);

    (void)Cy_SysInt_Init(&sysFault_IRQ_cfg, &irqFaultReportHandler);
    (void)Cy_SysFault_Init(FAULT_STRUCT0, &sysFault_cfg);

    NVIC_SetPriority((IRQn_Type)NvicMux3_IRQn, 2UL);
    NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);

    source = (uint8_t)CY_SYSFAULT_NO_FAULT;

    /* ECC configuration */
    configureECC(eccErrorMode);

    uint32_t timeout = 0UL;
    /* Wait for the Fault to trigger */
    for (timeout = 0; timeout <= TIMEOUT_FOR_FAULT; timeout++)
    {
        localSource = source;
        if (localSource != (uint8_t)CY_SYSFAULT_NO_FAULT)
        {
            break;
        }
        Cy_SysLib_Delay(1);
    }

    #if (ERROR_IN_ECC == 1)
    localSource = (uint8_t)CY_SYSFAULT_MPU_0;
    #endif

    /* Verify result */
    if ((eccErrorMode == CY_ECC_NC_ERROR) && (localSource == (uint8_t)CY_ECC_NC_FAULT))
    {
        ret = OK_STATUS;
    }

    return ret;
}


#endif /* if defined (CY_IP_MXS40FAULT) */

#if defined(CY_IP_MXFAULT)
/*******************************************************************************
* Function Name: SelfTest_ECC_Flash
********************************************************************************
*
* Summary:
*  This function performs the ECC hardware self test for Flash memory
*  to report a fault for the double-bit error
*
* Parameters:
* addr - Aligned flash row address.
* eccErrorMode - Error injection mode
*
* Return:
*  0 - Test passed <br>
*  1 - Test failed
*
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode)
{
    uint8_t ret = ERROR_STATUS;
    uint8_t localSource;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_FAULT);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
    cy_en_sysint_status_t intrStatus = Cy_SysInt_Init(&sysFault_IRQ_cfg, &irqFaultReportHandler);

    cy_en_SysFault_status_t faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &sysFault_cfg);
    NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);

    /* ECC configuration */
    cy_en_flashdrv_status_t res;

    (void)Cy_Flash_Init(false);
    Cy_Flashc_ECCDisable();

    /* Configure error injection */
    cy_en_flashdrv_status_t flashWriteStatus = Cy_Flash_EraseRow((uint32_t)addr);
    if (flashWriteStatus == CY_FLASH_DRV_SUCCESS)
    {
        if (CY_FLASH_DRV_SUCCESS == Cy_Flash_ProgramRow((uint32_t)addr, (uint32_t*)&pattern_a))
        {
            (void)Cy_Flash_ProgramRow((uint32_t)addr, (uint32_t*)&pattern_b);
        }
    }
    /* When try to access corrupted data it will signal a fault */
    Cy_Flashc_ECCEnable();

    /* Read operation */
    volatile uint32_t* ptr = (uint32_t*)addr;
    readVal = *ptr;

    /* Wait for the Fault to trigger */
    Cy_SysLib_Delay(2000);
    localSource = source;
    #if (ERROR_IN_ECC == 1)
    localSource = (uint8_t)CY_SYSFAULT_MPU_0;
    #endif
    /*Verify result */
    if ((eccErrorMode == CY_ECC_NC_ERROR) && (localSource == (uint8_t)CY_ECC_NC_FAULT))
    {
        ret = OK_STATUS;
    }
    else
    {
        ret = ERROR_STATUS;
    }

    (void)readVal;
    (void)res;
    (void)faultStatus;
    (void)intrStatus;

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_ECC_Ram
********************************************************************************
*
* Summary:
*  This function performs the ECC hardware self test for Ram memory
*  to report a fault for the double-bit error.
*
* Parameters:
*  addr -Ram address.
*  eccErrorMode - Error injection mode.
*
* Return:
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode)
{
    uint8_t ret = ERROR_STATUS;
    uint8_t localSource;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_RAM_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_RAM_FAULT);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
    cy_en_sysint_status_t intrStatus = Cy_SysInt_Init(&sysFault_IRQ_cfg, &irqFaultReportHandler);

    cy_en_SysFault_status_t faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &sysFault_cfg);
    NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);

    /* ECC configuration */
    RAMC0->ECC_CTL = 0x00000000;
    CY_SET_REG32(addr, 0x5A5A5A5A);
    CY_SET_REG32(addr, 0x7A7A7A7A);


    /* Sram Error EN =1 */
    RAMC0->ECC_CTL |= 0x00000001UL;

    /* Enable ECC Check */
    RAMC0->ECC_CTL |= 0x00000008UL;
    CY_GET_REG32(addr);


    /* Wait for the Fault to trigger */
    Cy_SysLib_Delay(2000);
    uint32_t* ptr = (uint32_t*)addr;
    readVal = *ptr;

    localSource = (uint8_t)Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    if ((eccErrorMode == CY_ECC_NC_ERROR) && (localSource == (uint8_t)CY_ECC_NC_RAM_FAULT))
    {
        ret = OK_STATUS;
    }
    else
    {
        ret = ERROR_STATUS;
    }
    (void)readVal;
    (void)faultStatus;
    (void)intrStatus;

    return ret;
}


#endif /* if defined(CY_IP_MXFAULT) */

#endif /* if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT)) */
/* [] END OF FILE */

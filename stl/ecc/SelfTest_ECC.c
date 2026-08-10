/*******************************************************************************
* File Name: SelfTest_ECC.c
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
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
#include "cy_pdl.h"
#include "SelfTest_ECC.h"

#if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT))

#define TIMEOUT_FOR_FAULT 2000UL

static volatile uint32_t stlEcc_readVal = 0;
static volatile cy_en_SysFault_source_t stlEcc_source = (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;
static const cy_en_SysFault_source_t stlEcc_noFaultSrc = (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;

#if defined(CY_IP_MXFAULT)
static uint32_t stlEcc_patternA[CY_FLASH_SIZE_ROW /
                                sizeof(uint32_t)] =
    {[0] = 0x2400F000UL, [1] = 0x02014267UL, [2] = 0x02014241UL, [3] = 0x02014249UL };
static uint32_t stlEcc_patternB[CY_FLASH_SIZE_ROW /
                                sizeof(uint32_t)] =
    {[0] = 0x34004000UL, [1] = 0x120032DFUL, [2] = 0x120032B9UL, [3] = 0x120032C1UL };

uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t error);
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t error);
#else /* if defined(CY_IP_MXFAULT) */
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t error);
static volatile uint32_t* stlEcc_flashAddrPtr = (volatile uint32_t*)CY_FLASH_ADDR;
#endif /* if defined(CY_IP_MXFAULT) */

static cy_stc_SysFault_t stlEcc_sysFaultConfig =
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false,
};
#if defined(CY_IP_M7CPUSS) || defined(CY_IP_M4CPUSS)
static cy_stc_sysint_t stlEcc_sysFaultIrqConfig =
{
    .intrSrc      =
        ((NvicMux3_IRQn << CY_SYSINT_INTRSRC_MUXIRQ_SHIFT) | cpuss_interrupts_fault_0_IRQn),
    .intrPriority = 0UL
};
#else
static cy_stc_sysint_t stlEcc_sysFaultIrqConfig =
{
    .intrSrc      = (cpuss_interrupts_fault_0_IRQn),
    .intrPriority = 0UL
};
#endif /* if defined(CY_IP_M7CPUSS) || defined(CY_IP_M4CPUSS) */

/* Interrupt Handler */
static void irqFaultReportHandler(void)
{
    /* Get error source */
    stlEcc_source = Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    /* Clear fault status */
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
}


void Cy_SysLib_ProcessingFault(void)
{
    /* Get error source */
    stlEcc_source = Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
    /* Clear Interrupt flag */
    Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);
    #if defined(CY_IP_MXS40FLASHC)
    Cy_Flashc_ECCDisable();
    #endif
}


#if defined (CY_IP_MXS40FAULT)
/* ECC Configuration */
static uint8_t configureECC(cy_en_ecc_error_mode_t eccErrorMode)
{
    uint8_t ret = OK_STATUS;
    uint32_t index;
    cy_en_flashdrv_status_t flashStatus = CY_FLASH_DRV_SUCCESS;
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

    /* Suppress the AHB bus error on a non-correctable ECC read so it does not
     * escalate to HardFault. The fault is still latched by SysFault and
     * triggers the IRQ. */
    #if defined(FLASHC_V2_FLASH_CTL_MAIN_ERR_SILENT_Msk)
    FLASHC_FLASH_CTL |= FLASHC_V2_FLASH_CTL_MAIN_ERR_SILENT_Msk;
    #elif defined(FLASHC_FLASH_CTL_MAIN_ERR_SILENT_Msk)
    FLASHC_FLASH_CTL |= FLASHC_FLASH_CTL_MAIN_ERR_SILENT_Msk;
    #else
    #error Unsupported FLASHC header: MAIN_ERR_SILENT mask not defined
    #endif

    /* Configure error injection */
    if (eccErrorMode == CY_ECC_NC_ERROR)
    {
        flashStatus = Cy_Flashc_InjectECC(CY_FLASH_MAIN_REGION, CY_FLASH_ADDR, CY_ECC_NC_ERROR_PARITY);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    /* Write operation */
    if (ret == OK_STATUS)
    {
        flashStatus = Cy_Flash_EraseSector((uint32_t)CY_FLASH_ADDR);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        flashStatus = Cy_Flash_ProgramRow(CY_FLASH_ADDR, (const uint32_t*)data);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    /* Read operation - this will trigger the ECC fault */
    if (ret == OK_STATUS)
    {
        stlEcc_flashAddrPtr = (uint32_t*)CY_FLASH_ADDR;
        stlEcc_readVal = *stlEcc_flashAddrPtr;
    }

    (void)stlEcc_readVal;
    return ret;
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
    uint8_t ret = OK_STATUS;
    cy_en_SysFault_source_t localSource = stlEcc_noFaultSrc;
    cy_en_sysint_status_t intrStatus;
    cy_en_SysFault_status_t faultStatus;
    uint32_t timeout = 0UL;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_FAULT);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);

    intrStatus = Cy_SysInt_Init(&stlEcc_sysFaultIrqConfig, &irqFaultReportHandler);
    if (intrStatus != CY_SYSINT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &stlEcc_sysFaultConfig);
    if (faultStatus != CY_SYSFAULT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    if (ret == OK_STATUS)
    {
        NVIC_SetPriority((IRQn_Type)NvicMux3_IRQn, 2UL);
        NVIC_EnableIRQ((IRQn_Type)NvicMux3_IRQn);

        stlEcc_source = stlEcc_noFaultSrc;

        /* ECC configuration */
        ret = configureECC(eccErrorMode);
    }


    if (ret == OK_STATUS)
    {
        /* Wait for the Fault to trigger */
        for (timeout = 0UL; timeout < TIMEOUT_FOR_FAULT; timeout++)
        {
            localSource = stlEcc_source;
            if (localSource != stlEcc_noFaultSrc)
            {
                break;
            }
            Cy_SysLib_Delay(1);
        }

        /* Timeout error branch */
        if (timeout >= TIMEOUT_FOR_FAULT)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        #if (ERROR_IN_ECC == 1)
        localSource = CY_SYSFAULT_MPU_0;
        #endif

        /* Verify result */
        if ((eccErrorMode != CY_ECC_NC_ERROR) || (localSource != CY_ECC_NC_FAULT))
        {
            ret = ERROR_STATUS;
        }
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
    uint8_t ret = OK_STATUS;
    uint32_t timeout = 0UL;
    cy_en_SysFault_source_t localSource = stlEcc_noFaultSrc;
    cy_en_sysint_status_t intrStatus;
    cy_en_SysFault_status_t faultStatus;
    cy_en_flashdrv_status_t flashStatus = CY_FLASH_DRV_SUCCESS;
    stlEcc_source = stlEcc_noFaultSrc;

    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_FAULT);
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
    intrStatus = Cy_SysInt_Init(&stlEcc_sysFaultIrqConfig, &irqFaultReportHandler);
    if (intrStatus != CY_SYSINT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &stlEcc_sysFaultConfig);
    if (faultStatus != CY_SYSFAULT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    if (ret == OK_STATUS)
    {
        NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);

        /* ECC configuration */
        flashStatus = Cy_Flash_Init(false);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        Cy_Flashc_ECCDisable();

        /* Configure error injection */
        flashStatus = Cy_Flash_EraseRow((uint32_t)addr);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        flashStatus = Cy_Flash_ProgramRow((uint32_t)addr, stlEcc_patternA);
        if (flashStatus != CY_FLASH_DRV_SUCCESS)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        (void)Cy_Flash_ProgramRow((uint32_t)addr, stlEcc_patternB);
    }

    /* When try to access corrupted data it will signal a fault */
    if (ret == OK_STATUS)
    {
        Cy_Flashc_ECCEnable();

        /* Read operation */
        volatile uint32_t* ptr = (uint32_t*)addr;
        stlEcc_readVal = *ptr;
    }

    if (ret == OK_STATUS)
    {
        /* Wait for the Fault to trigger */
        for (timeout = 0UL; timeout < TIMEOUT_FOR_FAULT; timeout++)
        {
            localSource = stlEcc_source;
            if (localSource != stlEcc_noFaultSrc)
            {
                break;
            }
            Cy_SysLib_Delay(1);
        }

        /* Timeout error branch */
        if (timeout >= TIMEOUT_FOR_FAULT)
        {
            ret = ERROR_STATUS;
        }
    }

    if (ret == OK_STATUS)
    {
        #if (ERROR_IN_ECC == 1)
        localSource = stlEcc_noFaultSrc;
        #endif

        /*Verify result */
        if ((eccErrorMode != CY_ECC_NC_ERROR) || (localSource != CY_ECC_NC_FAULT))
        {
            ret = ERROR_STATUS;
        }

        (void)stlEcc_readVal;
    }
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
    uint8_t ret = OK_STATUS;
    RAMC_Type* ramcBase = NULL;
    cy_en_SysFault_source_t localSource = stlEcc_noFaultSrc;
    cy_en_sysint_status_t intrStatus;
    cy_en_SysFault_status_t faultStatus;
    stlEcc_source = stlEcc_noFaultSrc;
    uint32_t timeout = 0UL;

    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    if ((addr > CY_SRAM_BASE) && (addr < (CY_SRAM_BASE + (CY_SRAM_SIZE / 2U))))
    {
        ramcBase = RAMC0;
    }
    else
    {
        ramcBase = RAMC1;
    }
    #else
    ramcBase = RAMC0;
    #endif
    /* Configure Fault registers*/
    Cy_SysFault_ClearStatus(FAULT_STRUCT0); /* clear status */
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_RAM_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_RAM_FAULT);
    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_RAM1_FAULT);
    Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_RAM1_FAULT);
    #endif
    Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
    intrStatus = Cy_SysInt_Init(&stlEcc_sysFaultIrqConfig, &irqFaultReportHandler);
    if (intrStatus != CY_SYSINT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    faultStatus = Cy_SysFault_Init(FAULT_STRUCT0, &stlEcc_sysFaultConfig);
    if (faultStatus != CY_SYSFAULT_SUCCESS)
    {
        ret = ERROR_STATUS;
    }

    if (ret == OK_STATUS)
    {
        NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);

        /* Write data to the RAM with ECC enabled to save initial state of crc */
        CY_SET_REG32(addr, CY_RAM_MEMORY_DATA);
        /* ECC disable */
        ramcBase->ECC_CTL = 0x00000000;
        __DSB();
        __ISB();
        /* Write data to the RAM with ECC disabled to inject non-correctable error */
        CY_SET_REG32(addr, CY_RAM_CORRUPT_DATA);
        __DSB();
        __ISB();
        /* Sram Error EN =1 */
        ramcBase->ECC_CTL |= RAMC_ECC_CTL_EN_Msk;
        __DSB();
        __ISB();
        /* Enable ECC Check */
        ramcBase->ECC_CTL |= RAMC_ECC_CTL_CHECK_EN_Msk;
        __DSB();
        __ISB();
        /* Trigger ECC check by reading corrupted data */
        (void)CY_GET_REG32(addr);
        __DSB();
        __ISB();
        for (timeout = 0UL; timeout < TIMEOUT_FOR_FAULT; timeout++)
        {
            localSource = stlEcc_source;
            if (localSource != stlEcc_noFaultSrc)
            {
                break;
            }
            Cy_SysLib_Delay(1);
        }

        /* Timeout error branch */
        if (timeout >= TIMEOUT_FOR_FAULT)
        {
            ret = ERROR_STATUS;
        }
    }

    /* Poll fault source up to TIMEOUT_FOR_FAULT ms instead of fixed wait. */
    if (ret == OK_STATUS)
    {
        bool isRamFault = (localSource == CY_ECC_NC_RAM_FAULT);
        #if (CY_IP_MXSRAMC_INSTANCES == 2U)
        isRamFault = isRamFault || (localSource == CY_ECC_NC_RAM1_FAULT);
        #endif
        if ((eccErrorMode != CY_ECC_NC_ERROR) || (!isRamFault))
        {
            ret = ERROR_STATUS;
        }
    }

    (void)stlEcc_readVal;

    return ret;
}


#endif /* if defined(CY_IP_MXFAULT) */
#endif /* if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT)) */
/* [] END OF FILE */

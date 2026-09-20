/*******************************************************************************
* File Name: SelfTest_ECC_Flash_Runtime.c
*
* Description:
*  This file provides the application-managed runtime Flash ECC configuration,
*  verification and event decoding APIs for PSOC Control C3 devices.
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

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME)
#if (defined(CY_IP_MXS40FLASHC) && defined(CY_IP_MXFAULT))

/* Two patterns that differ in more than one bit per ECC word. Programming the
 * second one without an erase leaves the stored parity of the first. */
static const uint32_t stlEccFlash_patternA[CY_FLASH_SIZE_ROW / sizeof(uint32_t)] =
    {[0] = 0x2400F000UL, [1] = 0x02014267UL, [2] = 0x02014241UL, [3] = 0x02014249UL };
static const uint32_t stlEccFlash_patternB[CY_FLASH_SIZE_ROW / sizeof(uint32_t)] =
    {[0] = 0x34004000UL, [1] = 0x120032DFUL, [2] = 0x120032B9UL, [3] = 0x120032C1UL };

static volatile uint32_t stlEccFlash_readVal = 0UL;

/*******************************************************************************
* Function Name: isRequestSupported
****************************************************************************//**
*
* Checks the configuration request against the contract this module supports.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_ConfigVerify(const stl_ecc_flash_cfg_t* expected)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((expected != NULL) && (expected->eccEnable) && (!expected->rbusErrSilent))
    {
        uint32_t flashCtl = FLASHC_FLASH_CTL;
        uint32_t injEn    = FLASHC_FLASH_ECC_INJ_EN;
        bool     match    = true;

        if ((flashCtl & FLASHC_FLASH_CTL_ECC_EN_Msk) == 0UL)
        {
            match = false;
        }

        if ((flashCtl & FLASHC_FLASH_CTL_RBUS_ERR_SILENT_Msk) != 0UL)
        {
            match = false;
        }

        /* ECC_INJ_PC stays latched after injection is disabled, so it is not compared. */
        if ((injEn & (FLASHC_ECC_INJ_EN_ECC_INJ_ENABLE_Msk | FLASHC_ECC_INJ_EN_ECC_ERROR_Msk)) != 0UL)
        {
            match = false;
        }

        if (expected->checkFlashMacroEncoder)
        {
            if ((FLASHC->FM_CTL.ANA_CTL0 & FLASHC_FM_CTL_ANA_CTL0_ECC_ENC_DIS_Msk) != 0UL)
            {
                match = false;
            }
        }

        ret = (match) ? OK_STATUS : ERROR_STATUS;
    }

    return ret;
}


uint8_t SelfTest_ECC_Flash_SnapshotCapture(stl_ecc_flash_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if (snapshot != NULL)
    {
        snapshot->faultValid  = ((FAULT_STRUCT0->STATUS & FAULT_STRUCT_STATUS_VALID_Msk) != 0UL);
        snapshot->faultSource = (uint32_t)Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
        snapshot->data[0]     = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA0);
        snapshot->data[1]     = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA1);
        snapshot->data[2]     = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA2);
        snapshot->data[3]     = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA3);
        snapshot->flashCtl    = FLASHC_FLASH_CTL;
        snapshot->eccLogir    = FLASHC->ECC_LOGIR;

        ret = OK_STATUS;
    }

    return ret;
}


uint8_t SelfTest_ECC_Flash_Decode(const stl_ecc_flash_snapshot_t* snapshot,
                                  stl_ecc_flash_event_t* event)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((snapshot != NULL) && (event != NULL))
    {
        event->kind         = STL_ECC_FLASH_EVENT_NONE;
        event->wordAddress  = 0UL;
        event->offset       = 0UL;
        event->address      = 0UL;
        event->syndrome     = 0U;
        event->ecc1Cnt      = (uint16_t)((snapshot->eccLogir & FLASHC_ECC_LOGIR_ECC1CNT_Msk) >>
                                         FLASHC_ECC_LOGIR_ECC1CNT_Pos);
        event->eccThreshold = (uint16_t)(snapshot->eccLogir & FLASHC_ECC_LOGIR_ECCTHRESHOLD_Msk);

        if (snapshot->faultValid)
        {
            if (snapshot->faultSource == (uint32_t)CY_ECC_C_FAULT)
            {
                event->kind = STL_ECC_FLASH_EVENT_CORRECTABLE;
            }
            else if (snapshot->faultSource == (uint32_t)CY_ECC_NC_FAULT)
            {
                event->kind = STL_ECC_FLASH_EVENT_NON_CORRECTABLE;
            }
            else
            {
                /* Another fault source shares the record; not a Flash ECC event. */
            }
        }

        if (event->kind != STL_ECC_FLASH_EVENT_NONE)
        {
            event->wordAddress = snapshot->data[0] & STL_ECC_FLASH_DATA0_WORD_ADDR_Msk;
            event->offset      = event->wordAddress << STL_ECC_FLASH_WORD_SHIFT;
            event->address     = (uint32_t)STL_ECC_FLASH_MAIN_BASE + event->offset;
            event->syndrome    = (uint16_t)(snapshot->data[1] & STL_ECC_FLASH_DATA1_SYNDROME_Msk);
        }

        ret = OK_STATUS;
    }

    return ret;
}


void SelfTest_ECC_Flash_Accumulate(stl_ecc_flash_counters_t* counters,
                                   const stl_ecc_flash_event_t* event)
{
    if ((counters != NULL) && (event != NULL))
    {
        uint32_t* slot;

        if (event->kind == STL_ECC_FLASH_EVENT_CORRECTABLE)
        {
            slot            = &counters->correctable;
            counters->last  = *event;
        }
        else if (event->kind == STL_ECC_FLASH_EVENT_NON_CORRECTABLE)
        {
            slot            = &counters->nonCorrectable;
            counters->last  = *event;
        }
        else
        {
            slot = &counters->other;
        }

        if (*slot < UINT32_MAX)
        {
            (*slot)++;
        }
    }
}


uint8_t SelfTest_ECC_Flash_ProvokeNc(uint32_t addr)
{
    uint8_t ret = ERROR_STATUS;

    if (Cy_Flash_Init(false) == CY_FLASH_DRV_SUCCESS)
    {
        Cy_Flashc_ECCDisable();

        if (Cy_Flash_EraseRow(addr) == CY_FLASH_DRV_SUCCESS)
        {
            if (Cy_Flash_ProgramRow(addr, stlEccFlash_patternA) == CY_FLASH_DRV_SUCCESS)
            {
                /* Boot ROM reports the missing erase, but the mismatch is stored. */
                (void)Cy_Flash_ProgramRow(addr, stlEccFlash_patternB);

                Cy_Flashc_ECCEnable();
                __DSB();
                __ISB();

                stlEccFlash_readVal = *(volatile const uint32_t*)addr;
                __DSB();
                __ISB();

                ret = OK_STATUS;
            }
        }
    }

    return ret;
}


uint8_t SelfTest_ECC_Flash_RestoreRow(uint32_t addr)
{
    uint8_t ret = ERROR_STATUS;

    if (Cy_Flash_Init(false) == CY_FLASH_DRV_SUCCESS)
    {
        if (Cy_Flash_EraseRow(addr) == CY_FLASH_DRV_SUCCESS)
        {
            if (Cy_Flash_ProgramRow(addr, stlEccFlash_patternA) == CY_FLASH_DRV_SUCCESS)
            {
                ret = OK_STATUS;
            }
        }
    }

    return ret;
}


#endif /* if (defined(CY_IP_MXS40FLASHC) && defined(CY_IP_MXFAULT)) */
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */

/* [] END OF FILE */

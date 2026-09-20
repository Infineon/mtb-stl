/*******************************************************************************
* File Name: SelfTest_ECC_Ram_Runtime.c
*
* Description:
*  This file provides the application-managed runtime RAM ECC configuration,
*  verification and event decoding APIs for PSOC Control C3 devices.
*
*******************************************************************************
* (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is
* protected by and subject to worldwide patent protection, worldwide copyright
* laws, and international treaty provisions. Therefore, you may use this
* Software only as provided in the license agreement accompanying the software
* package from which you obtained this Software. If no license agreement applies,
* then any use, reproduction, modification, translation, or compilation of this
* Software is prohibited without the express written permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-
* PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have no
* liability arising out of any Security Breaches. Unless otherwise explicitly
* approved by Infineon, the Software may not be used in any application where a
* failure of the Product or any consequences of the use thereof can reasonably
* be expected to result in personal injury.
*******************************************************************************/
#include "cy_pdl.h"
#include "SelfTest_ECC.h"

#define STL_ECC_RAM_ADDRESS_VIEW_MASK    (0x0FFFFFFFUL)

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME)
#if (defined(CY_IP_MXSRAMC) && defined(CY_IP_MXFAULT))

typedef struct
{
    uint32_t correctable;
    uint32_t nonCorrectable;
} stl_ecc_ram_fault_sources_t;

static const stl_ecc_ram_fault_sources_t stlEccRam_faultSources[STL_ECC_RAM_INSTANCES] =
{
    { (uint32_t)CY_ECC_C_RAM_FAULT,  (uint32_t)CY_ECC_NC_RAM_FAULT  },
    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    { (uint32_t)CY_ECC_C_RAM1_FAULT, (uint32_t)CY_ECC_NC_RAM1_FAULT },
    #endif
};

static volatile uint32_t stlEccRam_readVal = 0UL;

static RAMC_Type* getRamc(uint32_t ramcIdx)
{
    RAMC_Type* ramc = RAMC0;

    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    if (ramcIdx == 1U)
    {
        ramc = RAMC1;
    }
    #else
    (void)ramcIdx;
    #endif

    return ramc;
}


static bool isSupportedRamc(RAMC_Type* ramc)
{
    bool supported = (ramc == RAMC0);

    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    supported = supported || (ramc == RAMC1);
    #endif

    return supported;
}


static bool isAddressOwnedByRamc(RAMC_Type* ramc, uint32_t address)
{
    uint32_t canonicalAddress = address & STL_ECC_RAM_ADDRESS_VIEW_MASK;
    uint32_t canonicalSramBase = CY_SRAM_BASE & STL_ECC_RAM_ADDRESS_VIEW_MASK;
    uint32_t sramOffset;
    uint32_t ramcSize = CY_SRAM_SIZE / STL_ECC_RAM_INSTANCES;

    if (canonicalAddress < canonicalSramBase)
    {
        return false;
    }

    sramOffset = canonicalAddress - canonicalSramBase;
    if (sramOffset >= CY_SRAM_SIZE)
    {
        return false;
    }

    if (ramc == RAMC0)
    {
        return (sramOffset < ramcSize);
    }

    #if (CY_IP_MXSRAMC_INSTANCES == 2U)
    if (ramc == RAMC1)
    {
        return (sramOffset >= ramcSize);
    }
    #endif

    return false;
}


static bool isRequestSupported(RAMC_Type* ramc, const stl_ecc_ram_cfg_t* cfg)
{
    return (isSupportedRamc(ramc) && (cfg != NULL) &&
            (cfg->eccEnable) && (cfg->autoCorrect));
}


static uint32_t getRuntimeEccCtl(bool checkEnable)
{
    uint32_t eccCtl = RAMC_ECC_CTL_EN_Msk | RAMC_ECC_CTL_AUTO_CORRECT_Msk;

    if (checkEnable)
    {
        eccCtl |= RAMC_ECC_CTL_CHECK_EN_Msk;
    }

    return eccCtl;
}


uint8_t SelfTest_ECC_Ram_ConfigVerify(RAMC_Type* ramc, const stl_ecc_ram_cfg_t* expected)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if (isRequestSupported(ramc, expected))
    {
        uint32_t eccCtl = ramc->ECC_CTL;
        bool match = true;

        if ((eccCtl & RAMC_ECC_CTL_EN_Msk) == 0UL)
        {
            match = false;
        }

        if ((eccCtl & RAMC_ECC_CTL_AUTO_CORRECT_Msk) == 0UL)
        {
            match = false;
        }

        if (((eccCtl & RAMC_ECC_CTL_CHECK_EN_Msk) != 0UL) != expected->checkEnable)
        {
            match = false;
        }

        if ((eccCtl & RAMC_ECC_CTL_INJ_EN_Msk) != 0UL)
        {
            match = false;
        }

        ret = (match) ? OK_STATUS : ERROR_STATUS;
    }

    return ret;
}


uint8_t SelfTest_ECC_Ram_SnapshotCapture(stl_ecc_ram_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;
    uint32_t ramcIdx;

    if (snapshot != NULL)
    {
        snapshot->faultValid = ((FAULT_STRUCT0->STATUS & FAULT_STRUCT_STATUS_VALID_Msk) != 0UL);
        snapshot->faultSource = (uint32_t)Cy_SysFault_GetErrorSource(FAULT_STRUCT0);
        snapshot->data[0] = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA0);
        snapshot->data[1] = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA1);
        snapshot->data[2] = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA2);
        snapshot->data[3] = Cy_SysFault_GetFaultData(FAULT_STRUCT0, CY_SYSFAULT_DATA3);

        for (ramcIdx = 0UL; ramcIdx < STL_ECC_RAM_INSTANCES; ramcIdx++)
        {
            RAMC_Type* ramc = getRamc(ramcIdx);

            snapshot->eccCtl[ramcIdx] = ramc->ECC_CTL;
            snapshot->eccStatus0[ramcIdx] = ramc->ECC_STATUS0;
            snapshot->eccStatus1[ramcIdx] = ramc->ECC_STATUS1;
        }

        ret = OK_STATUS;
    }

    return ret;
}


uint8_t SelfTest_ECC_Ram_Decode(const stl_ecc_ram_snapshot_t* snapshot,
                                stl_ecc_ram_event_t* event)
{
    uint8_t ret = ERROR_BAD_PARAM;
    uint32_t ramcIdx;

    if ((snapshot != NULL) && (event != NULL))
    {
        event->kind = STL_ECC_RAM_EVENT_NONE;
        event->ramcIdx = 0UL;
        event->address = 0UL;
        event->syndrome = 0U;

        if (snapshot->faultValid)
        {
            for (ramcIdx = 0UL; ramcIdx < STL_ECC_RAM_INSTANCES; ramcIdx++)
            {
                if (snapshot->faultSource == stlEccRam_faultSources[ramcIdx].correctable)
                {
                    event->kind = STL_ECC_RAM_EVENT_CORRECTABLE;
                    event->ramcIdx = ramcIdx;
                    break;
                }

                if (snapshot->faultSource == stlEccRam_faultSources[ramcIdx].nonCorrectable)
                {
                    event->kind = STL_ECC_RAM_EVENT_NON_CORRECTABLE;
                    event->ramcIdx = ramcIdx;
                    break;
                }
            }
        }

        if (event->kind != STL_ECC_RAM_EVENT_NONE)
        {
            event->address = snapshot->data[0];
            event->syndrome = (uint16_t)(snapshot->data[1] & STL_ECC_RAM_DATA1_SYNDROME_Msk);
        }

        ret = OK_STATUS;
    }

    return ret;
}


void SelfTest_ECC_Ram_Accumulate(stl_ecc_ram_counters_t* counters,
                                 const stl_ecc_ram_event_t* event)
{
    if ((counters != NULL) && (event != NULL))
    {
        uint32_t* slot;

        if (event->kind == STL_ECC_RAM_EVENT_CORRECTABLE)
        {
            slot = &counters->correctable;
            counters->last = *event;
        }
        else if (event->kind == STL_ECC_RAM_EVENT_NON_CORRECTABLE)
        {
            slot = &counters->nonCorrectable;
            counters->last = *event;
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


uint8_t SelfTest_ECC_Ram_Provoke(RAMC_Type* ramc,
                                 uint32_t wordAddr,
                                 stl_ecc_ram_inject_mode_t mode)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if (isSupportedRamc(ramc) && isAddressOwnedByRamc(ramc, wordAddr) &&
        (wordAddr != 0UL) &&
        ((wordAddr % STL_ECC_RAM_GRANULE_BYTES) == 0UL) &&
        ((mode == STL_ECC_RAM_INJECT_CORRECTABLE) ||
         (mode == STL_ECC_RAM_INJECT_NON_CORRECTABLE)))
    {
        volatile uint32_t* word = (volatile uint32_t*)wordAddr;
        uint32_t savedEccCtl = ramc->ECC_CTL;
        uint32_t flip = (mode == STL_ECC_RAM_INJECT_CORRECTABLE) ?
                        STL_ECC_RAM_PROVOKE_FLIP_C : STL_ECC_RAM_PROVOKE_FLIP_NC;
        uint32_t runtimeEccCtl = getRuntimeEccCtl(true);
        uint32_t savedPrimask = __get_PRIMASK();

        ramc->ECC_CTL = runtimeEccCtl;
        __DSB();
        *word = STL_ECC_RAM_PROVOKE_DATA;
        __DSB();

        __disable_irq();
        /* Keep the ECC-off window explicit and bounded (steps 4-7). */
        ramc->ECC_CTL = 0UL;
        __DSB();
        __ISB();
        *word = STL_ECC_RAM_PROVOKE_DATA ^ flip;
        __DSB();
        ramc->ECC_CTL = runtimeEccCtl;
        __DSB();
        __ISB();
        __set_PRIMASK(savedPrimask);

        stlEccRam_readVal = *word;
        __DSB();
        __ISB();

        ramc->ECC_CTL = RAMC_ECC_CTL_EN_Msk;
        __DSB();
        *word = STL_ECC_RAM_PROVOKE_DATA;
        __DSB();
        ramc->ECC_CTL = savedEccCtl;
        __DSB();
        __ISB();

        ret = OK_STATUS;
    }

    return ret;
}


#endif /* if (defined(CY_IP_MXSRAMC) && defined(CY_IP_MXFAULT)) */
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */

/* [] END OF FILE */

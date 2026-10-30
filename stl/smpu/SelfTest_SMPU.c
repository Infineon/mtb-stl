/*******************************************************************************
* File Name: SelfTest_SMPU.c
*
* Description: This file provides the source code for the API to perform the
* CPUSS SMPU (shared memory protection unit) self-test according to the
* Class B library.
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
#include "SelfTest_SMPU.h"

#if defined(SELFTEST_XMC7X_FAMILY)

/* BFSR of the deliberate fault: imprecise on bufferable memory, precise with a valid BFAR otherwise. */
#define SMPU_TEST_IMPRECISE_BFSR      (SCB_CFSR_IMPRECISERR_Msk)
#define SMPU_TEST_PRECISE_BFSR        (SCB_CFSR_PRECISERR_Msk | SCB_CFSR_BFARVALID_Msk)

/* Thumb function addresses have bit 0 set; a stacked PC never does, and an odd
 * stacked return address is UNPREDICTABLE on ARMv7-M exception return. */
#define SMPU_TEST_THUMB_BIT           (1u)
#define SMPU_TEST_CLEAR_THUMB_BIT(fn) (((uint32_t)(fn)) & ~SMPU_TEST_THUMB_BIT)

/* Toolchain-specific helper (stl/TOOLCHAIN_X/SelfTest_SMPU_X): performs the
 * denied store and exports the "stlSmpu_resumePoint" label right after it. */
extern void stlSmpu_triggerWrite(volatile uint32_t* address, uint32_t value);
extern void stlSmpu_resumePoint(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Shared state consumed by SelfTest_SMPU and the application-owned
 * BusFault_Handler wrapper (via SelfTest_SMPU_HandleBusFault). Written only
 * while SelfTest_SMPU owns the fault path (see stlSmpu_active). */
static volatile bool                    stlSmpu_active;
static volatile bool                    stlSmpu_recovered;
static volatile uint32_t                stlSmpu_faultPc;
static volatile uint32_t                stlSmpu_resumePc;
static volatile uint32_t                stlSmpu_testAddress;
static volatile uint32_t                stlSmpu_observedCfsr;
static volatile uint32_t                stlSmpu_observedBfar;

static cy_stc_SysFault_t stlSmpu_faultCfg =
{
    .ResetEnable   = false,
    .OutputEnable  = false,
    .TriggerEnable = false,
};


/*******************************************************************************
* Function Name: SelfTest_SMPU_HandleBusFault
*******************************************************************************/
bool SelfTest_SMPU_HandleBusFault(uint32_t* stack)
{
    bool handled = false;
    bool active  = stlSmpu_active;

    if ((stack != NULL) && active)
    {
        uint32_t faultPc   = stlSmpu_faultPc;
        uint32_t resumePc  = stlSmpu_resumePc;
        uint32_t stackedPc = stack[6];
        uint32_t cfsr      = SCB->CFSR;
        uint32_t bfsr      = cfsr & SCB_CFSR_BUSFAULTSR_Msk;
        bool     ownFault  = false;

        /* An imprecise fault may be taken at any helper instruction up to the resume point. */
        if ((stackedPc >= faultPc) && (stackedPc <= resumePc))
        {
            if (bfsr == SMPU_TEST_IMPRECISE_BFSR)
            {
                ownFault = true;
            }
            else if ((bfsr == SMPU_TEST_PRECISE_BFSR) && (stackedPc == faultPc))
            {
                uint32_t bfar     = SCB->BFAR;
                uint32_t testAddr = stlSmpu_testAddress;

                ownFault = (bfar == testAddr);
            }
            else
            {
                /* Any other BusFault signature is not the STL's. */
            }
        }

        if (ownFault)
        {
            stlSmpu_observedCfsr = cfsr;
            stlSmpu_observedBfar = SCB->BFAR;

            /* Write-1-to-clear only the bits this fault owns. */
            SCB->CFSR = bfsr;
            __DSB();
            __ISB();

            stack[6] = resumePc;
            stlSmpu_recovered = true;
            handled = true;
        }
    }

    return handled;
}


/*******************************************************************************
* Function Name: SelfTest_SMPU
*******************************************************************************/
uint8_t SelfTest_SMPU(const stl_smpu_entry_t* entry)
{
    uint8_t ret = ERROR_BAD_PARAM;
    /* IDENTITY.PC is the protection context of this read. */
    uint32_t activePc = _FLD2VAL(CPUSS_IDENTITY_PC, CPUSS_IDENTITY);

    if ((entry != NULL) &&
        (entry->testAddress != NULL) &&
        (entry->smpuStruct != NULL) &&
        (entry->faultStruct != NULL) &&
        (entry->writeValue != entry->expectedValue) &&
        (entry->allowedPc >= SMPU_TEST_MIN_PC) &&
        (entry->allowedPc <= SMPU_TEST_MAX_PC) &&
        ((uint32_t)entry->expectedFault < (uint32_t)CY_SYSFAULT_NO_FAULT))
    {
        uint32_t regionBase  = (uint32_t)entry->testAddress;
        bool     structInUse = (_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT0_ENABLED,
                                         PROT_SMPU_SMPU_STRUCT_ATT0(entry->smpuStruct)) != 0u);

        /* The access is only denied if the active PC differs from the single PC the region allows. */
        if (((regionBase & (SMPU_TEST_BLOCK_BYTES - 1u)) == 0u) &&
            !structInUse &&
            (activePc != entry->allowedPc))
        {
            ret = ERROR_STATUS;

            /* PC0 cannot be restricted, so no SMPU configuration could deny the write. */
            if (activePc == 0u)
            {
                ret = ERROR_INCORRECT_USAGE_STATUS;
            }
            else if (*(entry->testAddress) == entry->expectedValue)
            {
                cy_stc_smpu_cfg_t   slaveCfg;
                cy_en_prot_status_t cfgStatus;
                bool busFaultWasEnabled = ((SCB->SHCSR & SCB_SHCSR_BUSFAULTENA_Msk) != 0u);
                bool ownViolation       = false;
                bool disarmed           = false;
                uint32_t before;
                uint32_t after          = 0u;

                stlSmpu_active        = false;
                stlSmpu_recovered     = false;
                stlSmpu_faultPc       = 0u;
                stlSmpu_resumePc      = 0u;
                stlSmpu_testAddress   = 0u;
                stlSmpu_observedCfsr  = 0u;
                stlSmpu_observedBfar  = 0u;

                /* Capture only the expected source while the test runs. */
                (void)Cy_SysFault_Init(entry->faultStruct, &stlSmpu_faultCfg);
                Cy_SysFault_ClearInterruptMask(entry->faultStruct);
                FAULT_MASK0(entry->faultStruct) = 0u;
                FAULT_MASK1(entry->faultStruct) = 0u;
                FAULT_MASK2(entry->faultStruct) = 0u;
                Cy_SysFault_SetMaskByIdx(entry->faultStruct, entry->expectedFault);

                /* A fault already captured, or a pending one captured once unmasked, is not this
                 * test's: let it settle and do not trigger. */
                Cy_SysLib_DelayUs(1u);
                if (Cy_SysFault_GetErrorSource(entry->faultStruct) ==
                    (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT)
                {
                    /* Force BusFault reporting so the denied access does not
                     * escalate to HardFault. */
                    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;

                    /* Drop stale BFSR bits (W1C) so only this test's fault is observed. */
                    SCB->CFSR = SCB_CFSR_BUSFAULTSR_Msk;
                    __DSB();

                    /* Configure the borrowed slave struct to allow only a different
                     * PC, denying the current one. Leave it disabled for now. */
                    slaveCfg.address        = (uint32_t*)regionBase;
                    slaveCfg.regionSize     = CY_PROT_SIZE_256B;
                    slaveCfg.subregions     = 0u;
                    slaveCfg.userPermission = CY_PROT_PERM_RW;
                    slaveCfg.privPermission = CY_PROT_PERM_RW;
                    slaveCfg.secure         = false;
                    slaveCfg.pcMatch        = false;
                    slaveCfg.pcMask         = (uint16_t)(1u << (entry->allowedPc - 1u));

                    /* Baseline read while the restriction is disabled. */
                    before = *(entry->testAddress);

                    cfgStatus = Cy_Prot_ConfigSmpuSlaveStruct(entry->smpuStruct, &slaveCfg);
                    if (cfgStatus == CY_PROT_SUCCESS)
                    {
                        /* Arm only around the write window */
                        cfgStatus = Cy_Prot_EnableSmpuSlaveStruct(entry->smpuStruct);
                    }

                    /* Do not attempt unprotected write. */
                    if (cfgStatus == CY_PROT_SUCCESS)
                    {
                        stlSmpu_recovered   = false;
                        /* Thumb bit cleared: compared against / written to the stacked PC. */
                        stlSmpu_faultPc     = SMPU_TEST_CLEAR_THUMB_BIT(&stlSmpu_triggerWrite);
                        stlSmpu_resumePc    = SMPU_TEST_CLEAR_THUMB_BIT(&stlSmpu_resumePoint);
                        stlSmpu_testAddress = regionBase;
                        stlSmpu_active      = true;

                        stlSmpu_triggerWrite(entry->testAddress, entry->writeValue);

                        stlSmpu_active = false;

                        /* The helper's DSB has already pushed the denied store to the bus, so the
                         * violation is latched; close the window before polling for it. */
                        disarmed = (Cy_Prot_DisableSmpuSlaveStruct(entry->smpuStruct) == CY_PROT_SUCCESS);

                        /* Poll the fault structure STATUS for the expected source. */
                        uint32_t elapsed = 0u;
                        cy_en_SysFault_source_t observed =
                            (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;
                        while ((observed == (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT) &&
                               (elapsed < SMPU_TEST_TIMEOUT_US))
                        {
                            observed = Cy_SysFault_GetErrorSource(entry->faultStruct);
                            Cy_SysLib_DelayUs(1u);
                            elapsed++;
                        }

                        /* With the window still armed, this read would fault outside the test window. */
                        if (disarmed)
                        {
                            after = *(entry->testAddress);
                        }

                        bool     recovered     = stlSmpu_recovered;
                        uint32_t observedBfsr  = stlSmpu_observedCfsr & SCB_CFSR_BUSFAULTSR_Msk;
                        uint32_t observedBfar  = stlSmpu_observedBfar;
                        uint32_t violationAddr = Cy_SysFault_GetFaultData(entry->faultStruct,
                                                                          CY_SYSFAULT_DATA0);
                        bool     cpuFaultOk    = (observedBfsr == SMPU_TEST_IMPRECISE_BFSR) ||
                                                 ((observedBfsr == SMPU_TEST_PRECISE_BFSR) &&
                                                  (observedBfar == regionBase));

                        ownViolation = (observed == entry->expectedFault) &&
                                       (violationAddr == regionBase);

                        /* Re-checks the handler's criteria and the SMPU's own violation record. */
                        if (recovered &&
                            cpuFaultOk &&
                            ownViolation &&
                            disarmed &&
                            (after == before))
                        {
                            ret = OK_STATUS;
                        }
                    }

                    /* Restore the prior BusFault-enable state. */
                    if (!busFaultWasEnabled)
                    {
                        SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
                    }
                }

                /* Mask first, so nothing more is captured after the cleanup below. */
                Cy_SysFault_ClearMaskByIdx(entry->faultStruct, entry->expectedFault);

                /* Clear only the test's own violation; any other capture is left for the application. */
                if (ownViolation)
                {
                    Cy_SysFault_ClearStatus(entry->faultStruct);
                    Cy_SysFault_ClearInterrupt(entry->faultStruct);
                }

                stlSmpu_active      = false;
                stlSmpu_recovered   = false;
                stlSmpu_faultPc     = 0u;
                stlSmpu_resumePc    = 0u;
                stlSmpu_testAddress = 0u;
            }
            else
            {
                /* Scratch word not primed: ERROR_STATUS. */
            }
        }
    }

    return ret;
}


#endif /* defined(SELFTEST_XMC7X_FAMILY) */

/* [] END OF FILE */

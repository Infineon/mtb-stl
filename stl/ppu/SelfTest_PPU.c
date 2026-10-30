/*******************************************************************************
* File Name: SelfTest_PPU.c
*
* Description: This file provides the source code for the API to perform the
* Peripheral Protection Unit (PPU) self-test according to the Class B library.
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
#include "SelfTest_PPU.h"

#if defined(SELFTEST_XMC7X_FAMILY)

/*******************************************************************************
* Function Name: SelfTest_PPU
*******************************************************************************/
uint8_t SelfTest_PPU(const stl_ppu_entry_t* entry)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((entry != NULL) &&
        (entry->testReg != NULL) &&
        (entry->faultStruct != NULL) &&
        ((uint32_t)entry->expectedFault < (uint32_t)CY_SYSFAULT_NO_FAULT) &&
        ((!entry->checkIntegrity) || (entry->writeValue != entry->expectedValue)))
    {
        cy_stc_SysFault_t faultCfg =
        {
            .ResetEnable   = false,
            .OutputEnable  = false,
            .TriggerEnable = false,
        };
        FAULT_STRUCT_Type* fault = entry->faultStruct;
        cy_en_SysFault_source_t observed = (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT;
        uint32_t faultAddr;
        uint32_t before = 0u;
        uint32_t after  = 0u;
        bool     integrityOk  = true;
        bool     ownViolation = false;

        ret = ERROR_STATUS;

        /* Capture only the expected source, with reset, IO and trigger outputs
         * and the fault interrupt disabled, so the deliberate fault has no
         * effect outside this function. */
        (void)Cy_SysFault_Init(fault, &faultCfg);
        Cy_SysFault_ClearInterruptMask(fault);
        fault->MASK0 = 0u;
        fault->MASK1 = 0u;
        fault->MASK2 = 0u;
        Cy_SysFault_SetMaskByIdx(fault, entry->expectedFault);

        /* A fault already captured, or a pending one captured once unmasked, is not
         * this test's: let it settle and do not trigger. */
        Cy_SysLib_DelayUs(1u);
        if (Cy_SysFault_GetErrorSource(fault) == (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT)
        {
            /* Baseline for the data-integrity check, taken only when the register
             * is readable at the calling core's protection context. */
            if (entry->checkIntegrity)
            {
                before = *(entry->testReg);
            }

            /* Attempt the unauthorized write. The PPU denies it; with the bus-error
             * conditions met, the violation is captured without a CPU fault. */
            *(entry->testReg) = entry->writeValue;
            __DSB();
            __ISB();

            /* Poll the fault structure for the captured violation. */
            for (uint32_t elapsedUs = 0u; elapsedUs < PPU_TEST_POLL_TIMEOUT_US; elapsedUs++)
            {
                observed = Cy_SysFault_GetErrorSource(fault);
                if (observed != (cy_en_SysFault_source_t)CY_SYSFAULT_NO_FAULT)
                {
                    break;
                }
                Cy_SysLib_DelayUs(1u);
            }
            faultAddr = Cy_SysFault_GetFaultData(fault, CY_SYSFAULT_DATA0);

            if (entry->checkIntegrity)
            {
                after = *(entry->testReg);
                integrityOk = ((before == entry->expectedValue) && (after == before));
            }

            ownViolation = (observed == entry->expectedFault) &&
                           (faultAddr == (uint32_t)entry->testReg);

            if (ownViolation && integrityOk)
            {
                ret = OK_STATUS;
            }
        }

        /* Masks are cleared before STATUS so no further fault is captured into
         * the released structure. */
        fault->MASK0 = 0u;
        fault->MASK1 = 0u;
        fault->MASK2 = 0u;

        /* Clear only the test's own violation; any other capture is left for the application. */
        if (ownViolation)
        {
            Cy_SysFault_ClearStatus(fault);
            Cy_SysFault_ClearInterrupt(fault);
        }
    }

    return ret;
}


#endif /* defined(SELFTEST_XMC7X_FAMILY) */

/* [] END OF FILE */

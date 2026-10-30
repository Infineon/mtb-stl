/*******************************************************************************
* File Name: SelfTest_MPU.c
*
* Description: This file provides the source code for the API to perform the
* ARM Cortex-M core MPU self-test according to the Class B library.
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
#include "SelfTest_MPU.h"

#if defined(SELFTEST_XMC7X_FAMILY) && (defined(__MPU_PRESENT) && (__MPU_PRESENT == 1U))

/* MMFSR signature of the deliberate fault: precise data access violation, MMFAR valid, nothing else. */
#define MPU_TEST_EXPECTED_MMFSR      (SCB_CFSR_DACCVIOL_Msk | SCB_CFSR_MMARVALID_Msk)

/* Thumb function addresses have bit 0 set; a stacked PC never does, and an odd
 * stacked return address is UNPREDICTABLE on ARMv7-M exception return. */
#define MPU_TEST_THUMB_BIT           (1u)
#define MPU_TEST_CLEAR_THUMB_BIT(fn) (((uint32_t)(fn)) & ~MPU_TEST_THUMB_BIT)

/* MPU->TYPE (offset 0) */
#define MPU_TEST_TYPE_REG            (*(const volatile uint32_t*)MPU_BASE)

/* Toolchain-specific helper (stl/TOOLCHAIN_X/SelfTest_MPU_X): performs the
 * denied store and exports the "stlMpu_resumePoint" label right after it. */
extern void stlMpu_triggerWrite(volatile uint32_t* address, uint32_t value);
extern void stlMpu_resumePoint(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Shared state consumed by SelfTest_MPU and the application-owned
 * MemManage_Handler wrapper (via SelfTest_MPU_HandleMemManage). Written only
 * while SelfTest_MPU owns the fault path (see stlMpu_active). */
static volatile bool     stlMpu_active;
static volatile bool     stlMpu_recovered;
static volatile uint32_t stlMpu_faultPc;
static volatile uint32_t stlMpu_resumePc;
static volatile uint32_t stlMpu_testAddress;
static volatile uint32_t stlMpu_observedCfsr;
static volatile uint32_t stlMpu_observedMmfar;


/*******************************************************************************
* Function Name: SelfTest_MPU_HandleMemManage
*******************************************************************************/
bool SelfTest_MPU_HandleMemManage(uint32_t* stack)
{
    bool handled = false;
    bool active  = stlMpu_active;

    if ((stack != NULL) && active)
    {
        uint32_t faultPc = stlMpu_faultPc;
        uint32_t cfsr    = SCB->CFSR;

        /* stack[6] is the stacked PC; MemManage faults are precise. */
        if ((stack[6] == faultPc) &&
            ((cfsr & SCB_CFSR_MEMFAULTSR_Msk) == MPU_TEST_EXPECTED_MMFSR))
        {
            uint32_t mmfar    = SCB->MMFAR;
            uint32_t testAddr = stlMpu_testAddress;

            if (mmfar == testAddr)
            {
                stlMpu_observedCfsr  = cfsr;
                stlMpu_observedMmfar = mmfar;

                /* Write-1-to-clear only the bits this fault owns. */
                SCB->CFSR = MPU_TEST_EXPECTED_MMFSR;
                __DSB();
                __ISB();

                stack[6] = stlMpu_resumePc;
                stlMpu_recovered = true;
                handled = true;
            }
        }
    }

    return handled;
}


/*******************************************************************************
* Function Name: SelfTest_MPU
*******************************************************************************/
uint8_t SelfTest_MPU(const stl_mpu_entry_t* entry)
{
    uint8_t  ret        = ERROR_BAD_PARAM;
    /* Number of MPU regions implemented by this core (0 if no MPU). */
    uint32_t mpuRegions = (MPU_TEST_TYPE_REG & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;

    if ((entry != NULL) &&
        (entry->testAddress != NULL) &&
        (entry->writeValue != entry->expectedValue) &&
        (entry->regionNumber >= MPU_TEST_MIN_REGION_NUM) &&
        ((uint32_t)entry->regionNumber < mpuRegions))
    {
        uint32_t regionBase = (uint32_t)entry->testAddress;
        uint32_t savedRnr   = MPU->RNR;
        bool     regionInUse;

        MPU->RNR    = (uint32_t)entry->regionNumber;
        regionInUse = ((MPU->RASR & MPU_RASR_ENABLE_Msk) != 0u);

        if (((regionBase & (MPU_TEST_BLOCK_BYTES - 1u)) == 0u) && !regionInUse)
        {
            uint32_t mpuCtrl = MPU->CTRL;

            ret = ERROR_STATUS;

            /* MPU off: the write would land and corrupt the scratch word. */
            if ((mpuCtrl & MPU_CTRL_ENABLE_Msk) == 0u)
            {
                ret = ERROR_INCORRECT_USAGE_STATUS;
            }
            else if (*(entry->testAddress) == entry->expectedValue)
            {
                uint32_t before;
                uint32_t after;
                bool     memFaultWasEnabled = ((SCB->SHCSR & SCB_SHCSR_MEMFAULTENA_Msk) != 0u);

                stlMpu_active         = false;
                stlMpu_recovered      = false;
                stlMpu_faultPc        = 0u;
                stlMpu_resumePc       = 0u;
                stlMpu_testAddress    = 0u;
                stlMpu_observedCfsr   = 0u;
                stlMpu_observedMmfar  = 0u;

                /* Force MemManage reporting so the denied access does not
                 * escalate to HardFault. */
                SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

                /* Drop stale MMFSR bits (W1C) so only this test's fault is observed. */
                SCB->CFSR = SCB_CFSR_MEMFAULTSR_Msk;
                __DSB();

                before = *(entry->testAddress);

                /* Borrow the spare region: read-only over the scratch word.
                 * Per-region update; the rest of the MPU config, including
                 * region 0, is left enabled and undisturbed. */
                __DMB();
                ARM_MPU_SetRegionEx(entry->regionNumber, regionBase,
                                    ARM_MPU_RASR(0uL, (uint32_t)ARM_MPU_AP_RO,
                                                 0uL, 1uL, 1uL,
                                                 0uL, 0uL,
                                                 (uint32_t)ARM_MPU_REGION_SIZE_32B));
                __DSB();
                __ISB();

                stlMpu_recovered   = false;
                /* Thumb bit cleared: compared against / written to the stacked PC. */
                stlMpu_faultPc     = MPU_TEST_CLEAR_THUMB_BIT(&stlMpu_triggerWrite);
                stlMpu_resumePc    = MPU_TEST_CLEAR_THUMB_BIT(&stlMpu_resumePoint);
                stlMpu_testAddress = regionBase;
                stlMpu_active      = true;

                stlMpu_triggerWrite(entry->testAddress, entry->writeValue);

                stlMpu_active = false;

                /* Release the borrowed region. */
                __DMB();
                ARM_MPU_SetRegionEx(entry->regionNumber, 0u, 0u);
                __DSB();
                __ISB();

                after = *(entry->testAddress);

                if (!memFaultWasEnabled)
                {
                    SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
                }

                bool     recovered     = stlMpu_recovered;
                uint32_t observedCfsr  = stlMpu_observedCfsr;
                uint32_t observedMmfar = stlMpu_observedMmfar;

                /* Re-checks the handler's acceptance criteria independently of it. */
                if (recovered &&
                    ((observedCfsr & SCB_CFSR_MEMFAULTSR_Msk) == MPU_TEST_EXPECTED_MMFSR) &&
                    (observedMmfar == regionBase) &&
                    (after == before))
                {
                    ret = OK_STATUS;
                }
            }
            else
            {
                /* Scratch word not primed: ERROR_STATUS. */
            }
        }

        MPU->RNR = savedRnr;
    }

    return ret;
}


#endif /* defined(SELFTEST_XMC7X_FAMILY) && __MPU_PRESENT */

/* [] END OF FILE */

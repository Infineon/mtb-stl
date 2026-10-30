/*******************************************************************************
* File Name: SelfTest_MPU.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values
*  used for the ARM Cortex-M core MPU self-test according to Class B library.
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

/**
 * \defgroup group_mpu MPU (ARM core MPU STL module)
 * \{
 *
 * Functional self-test for the ARM Cortex-M core MPU (CMSIS `MPU->RBAR/RASR`,
 * `ARM_MPU_*`).
 *
 * \section group_mpu_more_information More Information
 *
 * The self-test borrows one spare, currently-unused MPU region and temporarily
 * marks it read-only over an application-provided 32-byte scratch block. It
 * then attempts a privileged write to the first word of that block and
 * verifies that:
 * -# the CPU raises a MemManage fault with `CFSR.DACCVIOL` set and `MMFAR`
 *    equal to the scratch address, and
 * -# the scratch word content is unchanged by the denied write.
 *
 * The denied write is recovered in place: the application's MemManage
 * exception handler forwards the fault to \ref SelfTest_MPU_HandleMemManage,
 * which rewrites the stacked return address so execution resumes immediately
 * after the write, without resetting or halting the device.
 *
 * \note
 * The application owns \c MemManage_Handler. This module does not install
 * a strong \c MemManage_Handler and does not override the application's
 * vector table. The application must provide a trampoline that forwards the
 * exception stack frame to \ref SelfTest_MPU_HandleMemManage. For the
 * trampoline and dispatcher code and the link-time-optimization safeguards,
 * see \ref stl_usage_memory_tests.
 *
 * \defgroup group_mpu_macros Macros
 * \defgroup group_mpu_data_structures Data Structures
 * \defgroup group_mpu_functions Functions
 */

#if !defined(SELFTEST_MPU_H)
    #define SELFTEST_MPU_H
#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(SELFTEST_XMC7X_FAMILY) && (defined(__MPU_PRESENT) && (__MPU_PRESENT == 1U))) || defined(CY_DOXYGEN)

/**
 * \addtogroup group_mpu_data_structures
 * \{
 */

/*******************************************************************************
* Function Name: stl_mpu_entry_t
****************************************************************************//**
*
* Entry parameters for \ref SelfTest_MPU(), supplied by the application.
*
*******************************************************************************/
typedef struct
{
    /** First word of a dedicated \ref MPU_TEST_BLOCK_BYTES block, aligned to
     *  \ref MPU_TEST_BLOCK_BYTES, that holds no other data and no stack, for
     *  example <tt>CY_ALIGN(32) static volatile uint32_t scratch[8];</tt>.
     *  The whole block is read-only while the test runs; any other write to
     *  it (for example from an ISR) raises a MemManage fault that the STL
     *  does not consume. Any normal memory (cacheable or non-cacheable) is
     *  supported: the ARM core MPU is checked before the D-cache. */
    volatile uint32_t* testAddress;

    /** Value \ref stl_mpu_entry_t::testAddress must hold before the test
     *  runs, for example 0x5A5A5A5A. The application must prime the scratch
     *  word with this value before every call to \ref SelfTest_MPU(). If the
     *  denied write does not fault, \ref stl_mpu_entry_t::writeValue remains
     *  in the scratch word. */
    uint32_t expectedValue;

    /** Value the self-test attempts to write. Must differ from
     *  \ref stl_mpu_entry_t::expectedValue. */
    uint32_t writeValue;

    /** Spare MPU region index to borrow for the duration of the test, from
     *  \ref MPU_TEST_MIN_REGION_NUM to the highest region implemented by the
     *  core (\c MPU->TYPE.DREGION - 1, checked at run time; 15 on XMC7000 CM7).
     *  Region 0 is excluded because BSPs for cache-enabled cores (for
     *  example XMC7000 CM7) typically reserve it for a non-cacheable memory
     *  window. The region must be disabled when \ref SelfTest_MPU() is called,
     *  its \c RBAR and \c RASR are overwritten and left disabled after the test.
     *
     *  When enabled regions overlap, ARMv7-M applies the attributes of the
     *  highest-numbered one. A higher-numbered enabled region that allows
     *  privileged writes over the scratch block therefore overrides the test
     *  region: the write succeeds and \ref SelfTest_MPU() returns
     *  \ref ERROR_STATUS, which cannot be differentiated from an MPU defect. Use
     *  the highest free region number, or make sure no higher-numbered region
     *  overlaps the scratch block. */
    uint8_t regionNumber;
} stl_mpu_entry_t;
/** \} group_mpu_data_structures */

/**
 * \addtogroup group_mpu_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_MPU
****************************************************************************//**
*
* Runs the functional self-test for the ARM Cortex-M core MPU: temporarily
* configures a spare MPU region read-only over the supplied scratch word,
* attempts a denied write, recovers from the resulting MemManage fault, and
* verifies both the fault cause and the data integrity of the scratch word.
*
* \param entry
*  Pointer to \ref stl_mpu_entry_t describing the scratch word under test and
*  the spare MPU region to use.
*
* \return
*  \ref OK_STATUS       - The denied write raised a MemManage fault with
*                          \c CFSR.MMFSR equal to \c DACCVIOL | \c MMARVALID
*                          and \c MMFAR equal to \c testAddress, and the
*                          scratch word content is unchanged. <br>
*  \ref ERROR_STATUS    - The scratch word did not hold \c expectedValue on
*                          entry, the expected fault was not observed, or the
*                          scratch word content changed during the test. A
*                          higher-numbered region overlapping the scratch block
*                          also causes this result (see
*                          \ref stl_mpu_entry_t::regionNumber). <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS - The ARM core MPU is disabled
*                          (\c MPU->CTRL.ENABLE = 0). Nothing is written,
*                          including the scratch word. <br>
*  \ref ERROR_BAD_PARAM - Invalid entry: null pointers, \c writeValue equal to
*                          \c expectedValue, \c regionNumber out of range or
*                          selecting an enabled region, or \c testAddress not
*                          aligned to \ref MPU_TEST_BLOCK_BYTES. The MPU
*                          configuration and the scratch word are unchanged.
*
* \note
* Before calling this function the ARM core MPU must be enabled, the
* scratch word must hold \ref stl_mpu_entry_t::expectedValue, and the
* application must have installed its \c MemManage_Handler trampoline. See
* \ref stl_usage_memory_tests.
*
* \note
* Side effects on entry:
*  - \c SCB->SHCSR.MEMFAULTENA is forced on for the duration of the call (its
*    prior state is saved and restored on exit) so the denied access reports
*    as a MemManage fault instead of escalating to HardFault.
*  - All \c SCB->CFSR.MMFSR bits (and so \c MMFAR validity) left over from an
*    earlier MemManage fault are cleared. \c BFSR and \c UFSR are untouched.
*  - The MPU region selected by \ref stl_mpu_entry_t::regionNumber, which must
*    be disabled, is reprogrammed read-only over the
*    \ref MPU_TEST_BLOCK_BYTES block at \ref stl_mpu_entry_t::testAddress.
*
* \note
* Side effects on exit:
*  - The borrowed MPU region is released (disabled); the rest of the MPU
*    configuration is left untouched.
*  - \c SCB->SHCSR.MEMFAULTENA is restored to its value on entry.
*  - \c MPU->RNR is restored to its value on entry.
*  - The application's \c MemManage_Handler stays installed; after this call
*    returns, \ref SelfTest_MPU_HandleMemManage() will return \c false for
*    any further MemManage fault so the application's normal policy runs.
*
* \note
* This is a startup test: call it once during initialization (before an RTOS
* scheduler starts, if one is used) and before the final operational
* protection policy is armed. It is blocking (the denied write and its
* recovery complete before the function returns), is not reentrant, and must
* not be called from an ISR or any other exception handler.
*
* \note
* Interrupts are not masked during the test. An interrupt cannot cause a
* false \ref OK_STATUS. An interrupt that reprograms the borrowed region,
* disables the MPU or writes to the scratch block causes \ref ERROR_STATUS or
* a MemManage fault handled by the application's policy.
* MemManage must not be masked when the function is called: no \c PRIMASK or
* \c FAULTMASK (for example inside \c __disable_irq() or
* \c Cy_SysLib_EnterCriticalSection()) and no \c BASEPRI at or above the
* MemManage priority. These conditions are not checked at runtime. If
* MemManage is masked, the deliberate fault escalates to HardFault, which
* cannot produce a false \ref OK_STATUS.
*
* \note
* Any other MemManage fault raised while this function runs is not consumed
* by \ref SelfTest_MPU_HandleMemManage() and reaches the application's
* MemManage policy.
*
*******************************************************************************/
uint8_t SelfTest_MPU(const stl_mpu_entry_t* entry);

/*******************************************************************************
* Function Name: SelfTest_MPU_HandleMemManage
****************************************************************************//**
*
* MemManage dispatch entry for the application's \c MemManage_Handler
* trampoline. Recovers the deliberate denied write driven by \ref SelfTest_MPU
* (rewrites the stacked PC to the STL resume point) and returns \c true.
*
* A fault is accepted as the deliberate one only if all of the following are true:
*  - \ref SelfTest_MPU is inside its test window,
*  - the stacked PC is the denied \c STR in the STL write helper,
*  - \c CFSR.MMFSR is exactly \c DACCVIOL | \c MMARVALID, and
*  - \c MMFAR equals \ref stl_mpu_entry_t::testAddress.
*
* Any other MemManage fault, inside or outside the test window, is left
* untouched and \c false is returned so the application's normal policy
* handles it.
*
* \param stack
*  Pointer to the exception stack frame (r0, r1, r2, r3, r12, lr, pc, xpsr)
*  that was active when the fault occurred, selected from MSP or PSP by the
*  application trampoline. \c NULL is rejected (returns \c false).
*
* \return
*  \c true  - Fault was the STL's deliberate denied write; \c stack[6] has
*             been rewritten to resume execution after the store and
*             \c CFSR.DACCVIOL / \c CFSR.MMARVALID have been cleared (other
*             \c CFSR bits are preserved). The application trampoline must
*             return normally so the exception return uses the (unchanged)
*             \c EXC_RETURN value in \c LR. <br>
*  \c false - \c stack is \c NULL, or the fault does not match the criteria
*             above. \c stack, \c SCB->CFSR, \c SCB->MMFAR and the STL state
*             are untouched. The application trampoline must invoke its own
*             MemManage policy.
*
* \note
* This function is safe to call from the MemManage exception context. It
* must not be called from ordinary code and must not be treated as a general
* test API.
*
*******************************************************************************/
bool SelfTest_MPU_HandleMemManage(uint32_t* stack);
/** \} group_mpu_functions */

/**
 * \addtogroup group_mpu_macros
 * \{
 */
/** Lowest spare MPU region index accepted by \ref SelfTest_MPU(). Region 0 is
 *  reserved (see \ref stl_mpu_entry_t::regionNumber). */
#define MPU_TEST_MIN_REGION_NUM      (1u)

/** Size in bytes of the read-only region placed over
 *  \ref stl_mpu_entry_t::testAddress (ARMv7-M minimum region size). */
#define MPU_TEST_BLOCK_BYTES         (32u)
/** \} group_mpu_macros */

#endif /* (defined(SELFTEST_XMC7X_FAMILY) && __MPU_PRESENT) || defined(CY_DOXYGEN) */
#endif /* !defined(SELFTEST_MPU_H) */
/** \} group_mpu */
/* [] END OF FILE */

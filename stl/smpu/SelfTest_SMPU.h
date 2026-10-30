/*******************************************************************************
* File Name: SelfTest_SMPU.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values
*  used for the CPUSS SMPU (shared memory protection unit) self-test according
*  to Class B library.
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
 * \defgroup group_smpu SMPU (CPUSS shared memory protection STL module)
 * \{
 *
 * Functional self-test for the CPUSS SMPU (shared memory protection unit)
 * programmed through the PDL \c Cy_Prot_*SmpuSlaveStruct() API.
 *
 * The SMPU denied write is reported by two independent mechanisms that
 * the self-test relies on:
 * -# a peripheral \c CPUSS_MPU_VIO SysFault latched in a FAULT_STRUCT (STATUS
 *    carries the source and \c DATA0 the violating address), which the
 *    self-test reads back by polling the fault structure directly - this
 *    proves the SMPU detected the violation, and
 * -# a CPU BusFault, which the application-owned \c BusFault_Handler
 *    forwards to \ref SelfTest_SMPU_HandleBusFault so execution recovers in
 *    place instead of escalating to a HardFault.
 *
 * \section group_smpu_more_information More Information
 *
 * The self-test borrows one low, disabled SMPU slave struct and temporarily
 * configures it over an application-provided 256-byte scratch block so that
 * only a protection context *different* from the one the accessing bus master
 * currently runs at is allowed. It then attempts a privileged write to the
 * first word of that block and verifies that:
 * -# the expected \c CPUSS_MPU_VIO SysFault source is observed, and
 * -# the scratch word content is unchanged by the denied write.
 *
 * \section group_smpu_coverage Diagnostic coverage
 *
 * Fault model: the SMPU fails to deny a write it is configured to deny, or
 * fails to report the violation. One call checks one path end to end at
 * startup:
 * -# SMPU slave-struct evaluation for one struct and one 256-byte region:
 *    address/size match, \c ENABLED, and the \c PC_MASK check that denies the
 *    calling core's active protection context while one other context is
 *    allowed.
 * -# Write blocking at the memory: the scratch word is unchanged.
 * -# Violation reporting for the tested memory area: the
 *    \ref stl_smpu_entry_t::expectedFault SysFault with \c DATA0 equal to the
 *    accessed address, and the bus error returned to the CPU as a BusFault.
 *
 * Other cases are not covered, in particular: denial by the permission
 * attributes (user/privileged read, write, execute), the \c secure attribute,
 * \c pcMatch, subregions, other region sizes, the application's own SMPU slave
 * structs and their priority, SMPU master structs, and accesses by other bus
 * masters. Call \ref SelfTest_SMPU() once per safety-related memory area, with
 * a scratch block in that area and the matching
 * \ref stl_smpu_entry_t::expectedFault.
 *
 * The self-test verifies the SMPU at startup only. Faults that occur after
 * startup are not detected by this self-test.
 *
 * \note
 * Non-cacheable scratch is mandatory. On a cache-enabled core (for example
 * XMC7000 CM7, D-cache enabled at startup) a denied store to a *cacheable*
 * buffer hits the D-cache and never reaches the bus, so the SMPU never sees it:
 * the write appears to succeed and no fault is raised. The scratch word in
 * \ref stl_smpu_entry_t::testAddress MUST reside in non-cacheable memory; refer
 * to the application linker script for the non-cacheable SRAM region.
 *
 * \note
 * The application owns \c BusFault_Handler. This module does not install a
 * strong \c BusFault_Handler and does not override the application's vector
 * table. The application must provide a small assembly-level trampoline that
 * uses \c EXC_RETURN to pick the active exception stack (MSP or PSP) and
 * forwards the frame pointer to a C dispatcher that calls
 * \ref SelfTest_SMPU_HandleBusFault():
 *
 * \code
 * // GCC / ARM
 * __attribute__((naked, noinline, used))
 * void BusFault_Handler(void)
 * {
 *     __asm volatile(
 *         "tst   lr, #4                         \n"
 *         "ite   eq                             \n"
 *         "mrseq r0, msp                        \n"
 *         "mrsne r0, psp                        \n"
 *         "b     Application_BusFault_Dispatch  \n");
 * }
 *
 * // IAR
 * #pragma required=Application_BusFault_Dispatch
 * #pragma inline=never
 * __stackless __root void BusFault_Handler(void)
 * {
 *     asm volatile(
 *         "tst   lr, #4                         \n"
 *         "ite   eq                             \n"
 *         "mrseq r0, msp                        \n"
 *         "mrsne r0, psp                        \n"
 *         "b     Application_BusFault_Dispatch  \n");
 * }
 *
 * // Portable C dispatcher (mark noinline so it is never merged into the naked
 * // trampoline above under LTO).
 * #if defined(__ICCARM__)
 * #pragma inline=never
 * #else
 * __attribute__((noinline))
 * #endif
 * void Application_BusFault_Dispatch(uint32_t* stack)
 * {
 *     if (SelfTest_SMPU_HandleBusFault(stack))
 *     {
 *         return; // STL consumed the deliberate SMPU-test fault
 *     }
 *     Application_UnexpectedBusFault(stack); // real application fault
 * }
 * \endcode
 *
 * \ref SelfTest_SMPU_HandleBusFault returns \c true only for the deliberate
 * denied write driven by \ref SelfTest_SMPU. For every other BusFault it
 * returns \c false with \c CFSR and \c BFAR untouched, so the application's
 * normal fault policy can inspect and handle it.
 *
 * \note
 * LTO / link-time-optimization safeguards. Whole-program optimization can
 * inline, reorder, or discard code around naked functions in ways that silently
 * break exception handling. Refer to your toolchain manual for details.
 * Suggested attributes/pragmas:
 * -# \c noinline / \c inline=never on both the trampoline and its C dispatcher.
 * -# \c used (GCC / ARM) or \c __root (IAR) on the trampoline so the linker
 *    keeps it even when the vector-table reference is a pure data relocation
 *    that LTO does not see as a code reference.
 * -# \c \#pragma \c required=&lt;dispatcher-name&gt; (IAR only) to keep the C
 *    dispatcher from being dead-code-eliminated when nothing but the naked
 *    trampoline references it.
 *
 * \defgroup group_smpu_macros Macros
 * \defgroup group_smpu_data_structures Data Structures
 * \defgroup group_smpu_functions Functions
 */

#if !defined(SELFTEST_SMPU_H)
    #define SELFTEST_SMPU_H
#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(SELFTEST_XMC7X_FAMILY) || defined(CY_DOXYGEN)

/**
 * \addtogroup group_smpu_data_structures
 * \{
 */

/*******************************************************************************
* Function Name: stl_smpu_entry_t
****************************************************************************//**
*
* Entry parameters for \ref SelfTest_SMPU(), supplied by the application.
*
*******************************************************************************/
typedef struct
{
    /** First word of a dedicated \ref SMPU_TEST_BLOCK_BYTES block, aligned to
     *  \ref SMPU_TEST_BLOCK_BYTES, in non-cacheable memory. The block must hold
     *  no other data and no stack. Every access to the block from the active
     *  protection context is denied while the test runs. */
    volatile uint32_t* testAddress;

    /** Value \ref stl_smpu_entry_t::testAddress must hold before the test
     *  runs, for example 0x5A5A5A5A. The application must prime the scratch
     *  word with this value before every call to \ref SelfTest_SMPU(): if the
     *  write is not denied, \ref stl_smpu_entry_t::writeValue remains in the
     *  scratch word. */
    uint32_t expectedValue;

    /** Value the self-test attempts to write. Must differ from
     *  \ref stl_smpu_entry_t::expectedValue. */
    uint32_t writeValue;

    /** Low SMPU slave struct to borrow for the test.
     *  It must be disabled when \ref SelfTest_SMPU() is called. Its
     *  configuration is overwritten and left disabled after the test. High
     *  structs (13/14/15) are boot-reserved/locked and must not be used.
     *
     *  When enabled SMPU structs overlap, the highest-numbered one decides the
     *  access. A higher-numbered enabled struct that allows the active
     *  protection context over the scratch block therefore overrides the test
     *  struct: the write succeeds and \ref SelfTest_SMPU() returns
     *  \ref ERROR_STATUS, which cannot be differentiated from an SMPU defect.
     *  Make sure no higher-numbered enabled struct overlaps the scratch block. */
    PROT_SMPU_SMPU_STRUCT_Type* smpuStruct;

    /** Protection context the SMPU region allows, 1-7. Must be different
     *  from the active PC of the core calling \ref SelfTest_SMPU() so the
     *  access at the active PC is denied. The active PC must be non-zero (PC0
     *  is unrestrictable and cannot be denied). */
    uint32_t allowedPc;

    /** Application-provided fault structure used to observe the SMPU
     *  \c CPUSS_MPU_VIO SysFault (for example \c FAULT_STRUCT0). */
    FAULT_STRUCT_Type* faultStruct;

    /** Device/area-specific expected SysFault source for a denied write to the
     *  tested memory area (for example \c CPUSS_MPU_VIO_14 for SRAM on
     *  XMC7200). The self-test polls the fault structure STATUS for this source. */
    cy_en_SysFault_source_t expectedFault;
} stl_smpu_entry_t;
/** \} group_smpu_data_structures */

/**
 * \addtogroup group_smpu_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_SMPU
****************************************************************************//**
*
* Runs the functional self-test for the CPUSS SMPU: temporarily configures a
* spare SMPU slave struct over the supplied scratch word so the active
* protection context of the calling core is denied, attempts a denied write,
* recovers from the resulting BusFault, and verifies both the SysFault source
* and the data integrity of the scratch word.
*
* \param entry
*  Pointer to \ref stl_smpu_entry_t describing the scratch word under test, the
*  spare SMPU slave struct, the fault structure, and the expected fault source.
*
* \return
*  \ref OK_STATUS       - The denied write raised a BusFault with the expected
*                          \c CFSR.BFSR signature (see
*                          \ref SelfTest_SMPU_HandleBusFault), the expected
*                          \c CPUSS_MPU_VIO SysFault with \c DATA0 equal to
*                          \c testAddress, and the scratch word content is
*                          unchanged. <br>
*  \ref ERROR_STATUS    - The scratch word did not hold \c expectedValue on
*                          entry, the SMPU configuration failed, the expected
*                          fault was not observed, the scratch word content
*                          changed during the test, or a fault was already
*                          latched when the function was called (captured in
*                          \c faultStruct, or \c expectedFault pending): the
*                          write is then not attempted and the fault is left
*                          in \c faultStruct for the application. <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS - The active protection context of the
*                          calling core is 0, which the SMPU cannot restrict.
*                          Nothing is written, including the scratch word. <br>
*  \ref ERROR_BAD_PARAM - Invalid entry: null pointers, \c writeValue equal to
*                          \c expectedValue, \c expectedFault not a valid
*                          SysFault source, \c testAddress not aligned to
*                          \ref SMPU_TEST_BLOCK_BYTES, \c smpuStruct already
*                          enabled, \c allowedPc out of range, or \c allowedPc
*                          equal to the active protection context. Nothing is
*                          written in this case.
*
* \note
* Before calling this function the application MUST:
* -# Run during system initialization, before the final protection policy is
*    locked.
* -# Install an application-owned \c BusFault_Handler trampoline that calls
*    \ref SelfTest_SMPU_HandleBusFault() with the exception stack-frame pointer
*    and falls back to the application's normal BusFault policy when the STL
*    declines the fault (see the module note in \ref group_smpu).
* -# Provide a dedicated \ref SMPU_TEST_BLOCK_BYTES scratch block in
*    non-cacheable memory, with its first word primed with
*    \ref stl_smpu_entry_t::expectedValue.
* -# Provide a low, disabled SMPU slave struct and the device/area
*    -specific \ref stl_smpu_entry_t::expectedFault source.
* -# Ensure the calling core runs at a non-zero protection context different
*    from \ref stl_smpu_entry_t::allowedPc.
* -# Handle (read and clear) any fault captured in
*    \ref stl_smpu_entry_t::faultStruct and any pending
*    \ref stl_smpu_entry_t::expectedFault (for example a violation at boot, or
*    one left by an earlier failed call). Otherwise the self-test returns
*    \ref ERROR_STATUS without attempting the write; it does not discard faults
*    it did not cause.
*
* \note
* Side effects on entry:
*  - The function reinitializes the provided fault structure: \c CTL, the
*    interrupt mask and \c MASK0..2 are overwritten so that only
*    \ref stl_smpu_entry_t::expectedFault is captured, with the fault
*    interrupt, reset request, \c fault_out and trigger outputs disabled.
*    Any prior application configuration of that fault structure is lost.
*  - \c SCB->SHCSR.BUSFAULTENA is forced on for the duration of the call (its
*    prior state is saved and restored on exit) so the denied access reports as
*    a BusFault instead of escalating to HardFault.
*  - All \c SCB->CFSR.BFSR bits (and so \c BFAR validity) left over from an
*    earlier BusFault are cleared. \c MMFSR and \c UFSR are untouched.
*  - The SMPU slave struct selected by \ref stl_smpu_entry_t::smpuStruct, which
*    must be disabled, is reprogrammed over the \ref SMPU_TEST_BLOCK_BYTES block
*    at \ref stl_smpu_entry_t::testAddress.
*
* \note
* Side effects on exit:
*  - The borrowed SMPU slave struct is left disabled; the application is
*    responsible for programming its final operational protection policy. If
*    the disable fails (for example the struct got locked), \ref ERROR_STATUS
*    is returned and the struct may stay enabled, denying access to the
*    scratch block at the calling core's protection context.
*  - \c SCB->SHCSR.BUSFAULTENA is restored to its value on entry.
*  - The fault-structure \c MASK0..2 are left cleared; the application must set
*    up its own fault policy after the self-test returns.
*  - The fault-structure STATUS/INTR are cleared only if they hold the test's
*    own violation (\ref stl_smpu_entry_t::expectedFault at
*    \ref stl_smpu_entry_t::testAddress). Any other captured fault is left for
*    the application. If the induced violation was not captured (for example
*    a wrong \ref stl_smpu_entry_t::expectedFault), it stays pending.
*  - The application's \c BusFault_Handler stays installed; after this call
*    returns, \ref SelfTest_SMPU_HandleBusFault() returns \c false for any
*    further BusFault so the application's normal policy runs.
*
* \note
* This is a startup test: call it once during initialization (before an RTOS
* scheduler starts, if one is used) and before the final operational
* protection policy is armed. It is blocking (polls up to
* \ref SMPU_TEST_TIMEOUT_US microseconds), is not reentrant, and must not be
* called from an ISR or any other exception handler.
*
* \note
* Interrupts are not masked during the test. An interrupt can never cause a
* false \ref OK_STATUS. An interrupt that accesses the scratch block while the
* struct is enabled, or reprograms the borrowed struct or the fault structure,
* causes \ref ERROR_STATUS or a BusFault handled by the application's policy.
* BusFault must not be masked when the function is called: no \c PRIMASK or
* \c FAULTMASK (for example inside \c __disable_irq() or
* \c Cy_SysLib_EnterCriticalSection()) and no \c BASEPRI at or above the
* BusFault priority. These conditions are not checked at runtime. If BusFault
* is masked, a precise fault escalates to HardFault. An imprecise fault stays
* pending: the function returns \ref ERROR_STATUS and the pending BusFault is
* taken later, outside the test window, by the application's BusFault policy.
* Neither case can produce a false \ref OK_STATUS.
*
* \note
* Any other BusFault raised while this function runs is not consumed by
* \ref SelfTest_SMPU_HandleBusFault() and reaches the application's BusFault
* policy.
*
*******************************************************************************/
uint8_t SelfTest_SMPU(const stl_smpu_entry_t* entry);

/*******************************************************************************
* Function Name: SelfTest_SMPU_HandleBusFault
****************************************************************************//**
*
* BusFault dispatch entry for the application's \c BusFault_Handler trampoline.
* Recovers the deliberate denied write driven by \ref SelfTest_SMPU (rewrites
* the stacked PC to the STL resume point) and returns \c true.
*
* A fault is accepted as the deliberate one only if all of the following are true:
*  - \ref SelfTest_SMPU is inside its test window,
*  - the stacked PC lies within the STL write helper (from the denied \c STR
*    up to the resume point), and
*  - \c CFSR.BFSR is exactly \c IMPRECISERR, or exactly
*    \c PRECISERR | \c BFARVALID with the stacked PC at the \c STR and \c BFAR
*    equal to \ref stl_smpu_entry_t::testAddress.
*
* Any other BusFault, inside or outside the test window, is left untouched and
* \c false is returned so the application's normal policy handles it.
*
* \param stack
*  Pointer to the exception stack frame (r0, r1, r2, r3, r12, lr, pc, xpsr)
*  that was active when the fault occurred, selected from MSP or PSP by the
*  application trampoline. \c NULL is rejected (returns \c false).
*
* \return
*  \c true  - Fault was the STL's deliberate denied write; \c stack[6] has been
*             rewritten to resume execution after the store and the accepted
*             \c CFSR.BFSR bits have been cleared (other \c CFSR bits are
*             preserved). The application trampoline must return normally so
*             the exception return uses the (unchanged) \c EXC_RETURN value in
*             \c LR. <br>
*  \c false - \c stack is \c NULL, or the fault does not match the criteria
*             above. \c stack, \c SCB->CFSR, \c SCB->BFAR and the STL state are
*             untouched. The application trampoline must invoke its own
*             BusFault policy.
*
* \note
* This function is safe to call from the BusFault exception context. It must
* not be called from ordinary code and must not be treated as a general test
* API.
*
*******************************************************************************/
bool SelfTest_SMPU_HandleBusFault(uint32_t* stack);
/** \} group_smpu_functions */

/**
 * \addtogroup group_smpu_macros
 * \{
 */
/** Lowest protection context accepted for \ref stl_smpu_entry_t::allowedPc. */
#define SMPU_TEST_MIN_PC             (1u)

/** Highest protection context accepted for \ref stl_smpu_entry_t::allowedPc. */
#define SMPU_TEST_MAX_PC             (PERI_PC_NR - 1u)

/** Size in bytes of the SMPU region placed over
 *  \ref stl_smpu_entry_t::testAddress (SMPU minimum region size). */
#define SMPU_TEST_BLOCK_BYTES        (256u)

/** Blocking timeout, in microseconds, \ref SelfTest_SMPU() polls the fault
 *  structure STATUS for the induced \c CPUSS_MPU_VIO source. */
#define SMPU_TEST_TIMEOUT_US         (1000u)
/** \} group_smpu_macros */

#endif /* defined(SELFTEST_XMC7X_FAMILY) || defined(CY_DOXYGEN) */
#endif /* !defined(SELFTEST_SMPU_H) */
/** \} group_smpu */
/* [] END OF FILE */

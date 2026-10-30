/*******************************************************************************
* File Name: SelfTest_PPU.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values
*  used for the Peripheral Protection Unit (PPU) self-test according to the
*  Class B library.
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
 * \defgroup group_ppu PPU (Peripheral Protection Unit STL module)
 * \{
 *
 * Functional self-test for the Peripheral Protection Unit (PPU) of the
 * `PERI_MS` block (CAT1C / XMC7000).
 *
 * \section group_ppu_more_information More Information
 *
 * On XMC7000 platform the PPU is configured from CM0+ (PC0) during startup,
 * before CM7 is enabled: the CM7 application cores cannot program a PPU at run
 * time. The self-test therefore does not configure any PPU. It
 * verifies the protection CM0+ already established: the
 * application supplies the address of a peripheral register that the final
 * protection policy denies to the calling core's protection context, and the
 * self-test attempts an unauthorized access and checks that:
 * -# the expected peripheral-protection violation is captured in the supplied
 *    fault structure: source \ref stl_ppu_entry_t::expectedFault (a
 *    `CY_SYSFAULT_MS_PPU_*` value) and `DATA0` equal to the accessed address, and
 * -# the register content is unchanged. This is checked only when
 *    \ref stl_ppu_entry_t::checkIntegrity is \c true, which requires a
 *    read-allowed \c testReg; otherwise only the denial is verified.
 *
 * The calling core must run at a non-zero protection context: PC0 always has
 * access to PPU-protected peripherals, so a denial cannot be observed at PC0.
 * PPU structures hold separate user and privileged permissions, the policy
 * must deny the write at the privilege level the self-test runs at.
 *
 * The self-test performs a denied write. With `CPUSS_BUFF_CTL.WRITE_BUFF = 1`
 * (reset default) and \c testReg mapped as bufferable memory (see
 * \ref stl_ppu_entry_t::testReg), the bus bridge buffers the write, the CPU
 * receives no bus error, and the violation captured in the fault structure is
 * the bus error response. The self-test installs no BusFault handler; if either
 * condition is not met, the denied write causes a HardFault. See the XMC7000
 * architecture reference manual for details.
 *
 * Coverage of safety-related areas: one call verifies one access
 * attempt on one register. Call \ref SelfTest_PPU() once per safety-related
 * area, that is, once per PPU structure that enforces the safety-related
 * policy against the calling core: one call per fixed PPU structure
 * (`PERI_MS_PPU_FX_*`, for example \c PERI_MS_PPU_FX_GPIO_PRT2_PRT and
 * \c PERI_MS_PPU_FX_GPIO_PRT2_CFG are two areas) and one call per programmable
 * PPU region. For each area pick one representative register inside the
 * address range of that PPU structure that meets the
 * \ref stl_ppu_entry_t::testReg requirements; the PPU structure applies one
 * set of permissions to its whole range. The same fault structure can be used
 * for all calls.
 *
 * \note
 * This is a startup-only test. Run it during system initialization, after the
 * final application PPU policy has been applied and before any protection lock.
 *
 * \defgroup group_ppu_macros Macros
 * \defgroup group_ppu_data_structures Data Structures
 * \defgroup group_ppu_functions Functions
 */

#if !defined(SELFTEST_PPU_H)
    #define SELFTEST_PPU_H
#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(SELFTEST_XMC7X_FAMILY) || defined(CY_DOXYGEN)

/**
 * \addtogroup group_ppu_data_structures
 * \{
 */

/** Entry parameters for \ref SelfTest_PPU(), supplied by the application. */
typedef struct
{
    /** Address of a peripheral register that the CM0+-configured PPU
     *  policy denies to the protection context of the calling core. The
     *  self-test attempts to write this register and expects the access to be
     *  denied. Must not be a register the test path itself depends on (for
     *  example the logging UART or \ref stl_ppu_entry_t::faultStruct).
     *
     *  Because the write lands on the peripheral if the PPU is defective, the
     *  register must be harmless to write:
     *  - a plain read/write configuration register without write side effects
     *    (not write-1-to-clear, set/clear/invert, FIFO, command, or trigger
     *    registers), and
     *  - not a register that controls pins or a peripheral in use (for example
     *    \c GPIO_PRTx->OUT of a port driving external hardware). Prefer a
     *    register of an unused or disabled peripheral instance.
     *
     *  The register must be mapped for the calling core as shareable,
     *  bufferable Device memory: the default memory map (MPU disabled, or
     *  not covered by an enabled region with the default map as background),
     *  or an MPU region with the same attributes (`TEX=0, C=0, B=1`,
     *  \c ARM_MPU_ACCESS_DEVICE(1)). Do not map it Strongly-ordered
     *  (\c ARM_MPU_ACCESS_ORDERED): the denied write may then raise a BusFault,
     *  which escalates to HardFault. The MPU must also permit write access at
     *  the calling privilege level (and read access when
     *  \ref stl_ppu_entry_t::checkIntegrity is \c true); otherwise a MemManage
     *  fault occurs before the PPU evaluates the access. */
    volatile uint32_t* testReg;

    /** Value the self-test attempts to write to \ref stl_ppu_entry_t::testReg.
     *  The write is expected to be denied, so the register content must not
     *  change. Denial detection (fault source and address) does not depend on
     *  this value.
     *
     *  When \ref stl_ppu_entry_t::checkIntegrity is \c true, \c writeValue must
     *  differ from \ref stl_ppu_entry_t::expectedValue in the implemented
     *  writable bits of \c testReg, otherwise a write leaking through the PPU
     *  cannot be detected. Equal values are rejected with \ref ERROR_BAD_PARAM. */
    uint32_t writeValue;

    /** Fault structure that captures the peripheral-protection violation.
     *  The self-test reconfigures it and leaves it released on exit; the
     *  application must configure it again afterwards if it uses it (see
     *  \ref SelfTest_PPU). */
    FAULT_STRUCT_Type* faultStruct;

    /** Expected SysFault source for the denied access: the
     *  `CY_SYSFAULT_MS_PPU_*` value for the calling core's master interface
     *  (device specific, for example \c CY_SYSFAULT_MS_PPU_1 for CM7_0 on
     *  XMC7200). Must be below \c CY_SYSFAULT_NO_FAULT. */
    cy_en_SysFault_source_t expectedFault;

    /** Set \c true to verify data integrity: \ref stl_ppu_entry_t::testReg must
     *  stay readable at the calling core's protection context (write denied,
     *  read allowed). The self-test then reads the register before and after
     *  the denied write and requires both reads to equal
     *  \ref stl_ppu_entry_t::expectedValue.
     *
     *  Set \c false only for a read-denied \c testReg. The register content is
     *  then not verified by the self-test; data integrity for that register
     *  must be verified by the application from a protection context with read
     *  access, or addressed in the application safety analysis. */
    bool checkIntegrity;

    /** Value \ref stl_ppu_entry_t::testReg must hold before and after the
     *  denied write. Applicable only when \ref stl_ppu_entry_t::checkIntegrity is
     *  \c true. */
    uint32_t expectedValue;
} stl_ppu_entry_t;
/** \} group_ppu_data_structures */

/**
 * \addtogroup group_ppu_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_PPU
****************************************************************************//**
*
* Runs the functional self-test for the Peripheral Protection Unit. Attempts an
* unauthorized write to an application-provided, already-protected peripheral
* register, then verifies that the expected peripheral-protection violation was
* captured in the supplied fault structure (with the fault address matching the
* accessed register) and, optionally, that the register content is unchanged.
*
* \param entry
*  Pointer to \ref stl_ppu_entry_t describing the protected register under
*  test and the fault structure / expected fault source to observe.
*
* \return
*  \ref OK_STATUS       - The denied write was captured as
*                          \c entry->expectedFault with the fault address equal
*                          to \c entry->testReg, and (if \c entry->checkIntegrity
*                          is \c true) the register content is unchanged. <br>
*  \ref ERROR_STATUS    - A fault was already present when the function was
*                          called (captured in the fault structure, or a
*                          pending \c entry->expectedFault): the write is not
*                          attempted and the fault is left for the application.
*                          Otherwise: the expected violation was not observed,
*                          the fault address did not match, or the register
*                          content changed. <br>
*  \ref ERROR_BAD_PARAM - Invalid entry: null \c entry, \c testReg, or
*                          \c faultStruct pointer, \c entry->expectedFault not
*                          below \c CY_SYSFAULT_NO_FAULT, or
*                          \c entry->checkIntegrity is \c true and
*                          \c entry->writeValue equals \c entry->expectedValue.
*
* \note
* Preconditions: the final application PPU policy is applied (the calling core
* runs at a non-zero protection context, and at its privilege level, that is
* denied write access to \c entry->testReg),
* \c entry->faultStruct is accessible at that protection context, and the
* bus-error conditions in \ref group_ppu_more_information are met. Any fault
* captured in \c entry->faultStruct and any pending \c entry->expectedFault
* (for example from an earlier failed call) must be handled by the application
* first. The self-test does not discard faults it did not cause.
*
* \note
* Side effects:
*  - During the call, \c entry->faultStruct captures only
*    \c entry->expectedFault, with the fault interrupt (\c INTR_MASK) and the
*    reset request, IO output (\c fault_out) and trigger outputs (\c CTL)
*    disabled, so the deliberate fault raises no fault ISR, reset, or external
*    fault signal. Faults of other sources are not captured by this structure
*    and stay pending.
*  - On exit, \c entry->faultStruct is released: \c MASK0..2 and \c INTR_MASK
*    are cleared and \c CTL has all outputs disabled. \c STATUS.VALID and
*    \c INTR are cleared only if they hold the test's own violation
*    (\c entry->expectedFault at \c entry->testReg). Any other captured fault
*    is left for the application. If the induced violation was not captured
*    (for example a wrong \c entry->expectedFault), it stays pending.
*    If the application uses this fault structure, it must configure it again
*    after the self-test.
*
* \note
* This function is blocking; the poll for the violation is bounded by
* \ref PPU_TEST_POLL_TIMEOUT_US. It is not reentrant and must not be called
* from an ISR.
*
*******************************************************************************/
uint8_t SelfTest_PPU(const stl_ppu_entry_t* entry);

/** \} group_ppu_functions */

/**
 * \addtogroup group_ppu_macros
 * \{
 */
/** Timeout, in microseconds, that \ref SelfTest_PPU() waits for the
 *  peripheral-protection violation to be captured in the fault structure after
 *  the denied write. */
#define PPU_TEST_POLL_TIMEOUT_US        (1000u)
/** \} group_ppu_macros */

#endif /* defined(SELFTEST_XMC7X_FAMILY) || defined(CY_DOXYGEN) */

#endif /* SELFTEST_PPU_H */

/** \} group_ppu */

/* [] END OF FILE */

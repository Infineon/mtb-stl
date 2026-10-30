/*******************************************************************************
* File Name: SelfTest_CONTROL.h
*
* Description:
*  This file provides the function prototype and constants used for the CONTROL
*  register self-test.
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
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
 * \defgroup group_control CONTROL (CONTROL register STL module)
 * \{
 *
 * The CONTROL register test detects stuck-at faults in the CONTROL register bits
 * that are implemented and writable on the core. It also checks that the stack
 * pointer selection bit (SPSEL) really switches the active stack pointer.
 *
 * \section group_control_more_information More Information
 *
 * The test performs the following major tasks: <br>
 *
 *      1) Checks that it is called from privileged Thread mode and masks interrupts.
 *      2) Saves CONTROL, the active stack pointer, and the process stack pointer (PSP).
 *      3) Writes SPSEL = 0, reads CONTROL back, and checks that the main stack
 *         pointer (MSP) is the active stack pointer.
 *      4) Points PSP to a dedicated test stack inside the library, writes SPSEL = 1,
 *         reads CONTROL back, checks that the active stack pointer is the test
 *         stack, and pushes a probe word to it.
 *      5) Restores CONTROL and PSP, and checks the restored values before it
 *         returns. The caller's interrupt mask (PRIMASK) is restored too.
 *
 * The nPRIV bit is not tested, because unprivileged Thread mode can't set it
 * back to privileged. The test never writes it and leaves it at its original value.
 *
 * On cores that execute FPU instructions (the image is compiled with __ARM_FP), the test
 * also checks the FPCA bit. In a Secure image (__ARM_FEATURE_CMSE == 3) it checks the SFPA
 * bit too. Set \ref SELFTEST_CONTROL_SKIP_FP_BITS to 1 to exclude both bits, when the device
 * safety argument doesn't require them. The test then never writes FPCA and SFPA.
 *
 * \warning
 * The test clears FPCA for a short time while it runs. If an NMI or HardFault occurs in that
 * window while the application has a live FP context, the FP registers aren't stacked. If the
 * handler uses the FPU, it corrupts the application's FP state. With FPCA = 1 and SPSEL = 1,
 * the exception frame in this case is the extended one; the test stack is large enough for it.
 * Set \ref SELFTEST_CONTROL_SKIP_FP_BITS to 1 if this is not acceptable.
 *
 * On Armv8-M Mainline cores (for example, Cortex-M33 and Cortex-M55), the test also
 * saves PSPLIM, sets it to the bottom of the test stack while PSP points there,
 * and restores and checks it afterward.
 *
 * \defgroup group_control_macros Macros
 * \defgroup group_control_functions Functions
 */

#if !defined(SELFTEST_CONTROL_H)
#define SELFTEST_CONTROL_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

/**
 * \addtogroup group_control_macros
 * \{
 */

#if !defined(SELFTEST_CONTROL_SKIP_FP_BITS) || defined(CY_DOXYGEN)
/** Optional user macro, 0 by default. Set it to 1 (for example, in the application
 * Makefile DEFINES) to exclude the FPCA and SFPA bits from the test. Use it only when the
 * device safety argument doesn't require these bits to be tested. It has no effect when the
 * image isn't compiled with FPU code generation. It doesn't change the size of the test stack. */
#define SELFTEST_CONTROL_SKIP_FP_BITS   (0)
#endif

/** \} group_control_macros */

/** \cond INTERNAL */
/* Implemented in TOOLCHAIN_<toolchain>/SelfTest_CONTROL_<toolchain>.S */
uint8_t SelfTest_CONTROL_Asm(uint32_t stackTop, uint32_t stackLimit, uint32_t testMask);
/** \endcond */

/**
 * \addtogroup group_control_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_CONTROL
****************************************************************************//**
*
* This function tests the CONTROL register bits that are implemented and
* writable (SPSEL; also FPCA in FPU builds and SFPA in Secure FPU builds, unless
* \ref SELFTEST_CONTROL_SKIP_FP_BITS is 1), and checks that the SPSEL bit
* switches the active stack pointer.
*
* The function masks interrupts for the duration of the test and restores the
* caller's PRIMASK value before it returns. NMI and HardFault can't be masked;
* if one of them occurs while PSP is selected, its exception frame is stacked
* on the library's test stack.
*
* \note
* Call this function only from privileged Thread mode. Otherwise it returns
* \ref ERROR_INCORRECT_USAGE_STATUS without changing CONTROL.
*
* \note
* If the function can't confirm that CONTROL, the stack pointer, and PSP were
* restored, it never returns: it stays in an endless loop with interrupts
* disabled. A watchdog must be running to reset the device in that case.
*
* \note
* This test runs only in blocking mode.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS (4) - Called from Handler mode or from
*  unprivileged Thread mode <br>
*
*******************************************************************************/
uint8_t SelfTest_CONTROL(void);

/** \} group_control_functions */

/** \} group_control */

#endif /* SELFTEST_CONTROL_H */

/* [] END OF FILE */

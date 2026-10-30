/*******************************************************************************
* File Name: SelfTest_PRIMASK.h
*
* Description:
*  This file provides the function prototype used for the PRIMASK register
*  self-test.
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
 * \defgroup group_primask PRIMASK (PRIMASK register STL module)
 * \{
 *
 * The PRIMASK register test detects stuck-at faults in the PM bit (bit 0) of the PRIMASK
 * register, which masks all exceptions with configurable priority. PM is the only
 * implemented bit of PRIMASK, so the test checks both of its legal values.
 *
 * \section group_primask_more_information More Information
 *
 * The test performs the following major tasks: <br>
 *
 *      1) Checks that it is called from Handler mode or privileged Thread mode, and saves
 *         the PRIMASK value.
 *      2) Writes PM = 0, reads PRIMASK back, and checks that PM = 0.
 *      3) Writes PM = 1, reads PRIMASK back, and checks that PM = 1.
 *      4) Writes the saved PRIMASK value back, and checks it.
 *
 * The test is available on all Cortex-M cores.
 *
 * \warning
 * The test changes the interrupt mask while it runs. Maskable interrupts are enabled for the
 * time between the PM = 0 write and the next PM = 1 write. If PRIMASK was 1 when the function
 * was called, an interrupt can occur in this window. If PRIMASK was 0, interrupts are masked
 * between the PM = 1 write and the restore, so their latency can increase. NMI and HardFault
 * aren't affected by PRIMASK.
 *
 * \defgroup group_primask_functions Functions
 */

#if !defined(SELFTEST_PRIMASK_H)
#define SELFTEST_PRIMASK_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

/**
 * \addtogroup group_primask_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_PRIMASK
****************************************************************************//**
*
* This function tests the PM bit of the PRIMASK register with the values 0 and 1,
* and attempts to restore the original PRIMASK value.
*
* The function writes the original PRIMASK value back also when a check fails, and
* checks it. It returns \ref ERROR_STATUS if any check fails, including when a stuck PM
* prevents the original value from being restored.
*
* \note
* Call this function from Handler mode or from privileged Thread mode. A write to
* PRIMASK from unprivileged Thread mode is ignored, so in this case the function
* returns \ref ERROR_INCORRECT_USAGE_STATUS without changing PRIMASK.
*
* \note
* This test runs only in blocking mode.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed, or the original PRIMASK value wasn't restored <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS (4) - Called from unprivileged Thread mode <br>
*
*******************************************************************************/
uint8_t SelfTest_PRIMASK(void);

/** \} group_primask_functions */

/** \} group_primask */

#endif /* SELFTEST_PRIMASK_H */

/* [] END OF FILE */

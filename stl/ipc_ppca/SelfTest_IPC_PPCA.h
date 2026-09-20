/*******************************************************************************
* File Name: SelfTest_IPC_PPCA.h
*
* Description:
*  This file provides constants and parameter values used for the IPC self
*  tests.
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
 * \defgroup group_ipc_ppca PPCA IPC (PPCA IPC STL module)
 * \{
 *
 * This module tests the PPCA IPC block on PSOC Control C3 M8 in loopback mode
 * from the Main CM33. No PPCA CPU firmware is required.
 *
 *      Procedure: <br>
 *      1) Acquire the PPCA IPC channel. <br>
 *      2) Write a 64-bit message. <br>
 *      3) Send an acquire-notify event to a specified PPCA IPC interrupt line. <br>
 *      4) Read the written data back in the ISR. <br>
 *      5) Release the PPCA IPC channel.
 *
 * \section group_ipc_ppca_more_information More Information
 *
 *      For PSOC Control C3 M8, the PPCA IPC block exposes:
 *      Channels   : 0 - 3  (CY_PPCA_IPC_CHANNELS = 4)
 *      Interrupts : 0 - 3  (ppca_ipc_0_IRQn ... ppca_ipc_3_IRQn)
 *
 *      The test exercises all 16 (channel * interrupt-structure) pairs.
 *      On failure, use \ref SelfTest_IPC_PPCA_GetFailChannel(),
 *      \ref SelfTest_IPC_PPCA_GetFailIntr(), and
 *      \ref SelfTest_IPC_PPCA_GetFailSubtest() to identify the failing pair
 *      and sub-test.
 *
 * \section group_ipc_ppca_subtests Sub-test Reference
 *
 * | Sub-test | Feature tested                        | Failure condition                                              |
 * |----------|---------------------------------------|----------------------------------------------------------------|
 * |  1       | Acquire on free channel               | `LockAcquire()` did not return `CY_IPC_DRV_SUCCESS`           |
 * |  2       | Release - ACQUIRED bit clears         | `LockRelease()` failed or ACQUIRED bit remained set           |
 * |  3       | Lock-status ACQUIRED attribute        | `IsLockAcquired()` returned false immediately after acquire   |
 * |  4       | DATA0/DATA1 write/read integrity      | Read-back of `0xDEADBEEF/0xCAFEBABE` did not match write      |
 * |  5       | DATA0/DATA1 independence              | `0x55555555/0xAAAAAAAA` pattern shows cross-register leak     |
 * |  6       | Notify-to-self loopback               | ISR timed out or did not observe the correct channel bit      |
 * |  7       | INTR clear-on-write-1                 | Notify bit still set in INTR after `ClearInterrupt()` in ISR  |
 * |  8       | Interrupt masking                     | Raw INTR wrong, INTR_MASKED != 0 at mask=0, ISR fired masked  |
 * |  9       | INTR_MASKED algebraic consistency     | `INTR_MASKED != INTR & INTR_MASK` (two mask states checked)   |
 * | 10       | INTR_SET software-forced interrupt    | ISR did not fire after `SetInterrupt()` + unmask              |
 * | 11       | Release-event self-loopback           | ISR timed out or did not observe the release bit              |
 * | 12       | Silent release - no interrupt         | ISR fired on zero-mask release, or ACQUIRED did not clear     |
 * | 13       | DATA group: lock release              | `LockRelease()` returned error after DATA sub-tests 4-5       |
 * | 14       | NOTIFY group: lock release            | `LockRelease()` returned error after NOTIFY sub-tests 6-7     |
 *
 * \defgroup group_ipc_ppca_functions Functions
 */


#if !defined(SELFTEST_IPC_PPCA_H)
#define SELFTEST_IPC_PPCA_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_ipc_ppca_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_IPC_PPCA
****************************************************************************//**
*
* Tests selected PPCA IPC channel/interrupt-structure pairs in loopback mode.
*
* \param testMask
* Bitmask selecting which sub-test groups to execute. Use any combination of
* \ref SELFTEST_IPC_PPCA_LOCK, \ref SELFTEST_IPC_PPCA_DATA,
* \ref SELFTEST_IPC_PPCA_NOTIFY, \ref SELFTEST_IPC_PPCA_INTR,
* \ref SELFTEST_IPC_PPCA_RELEASE_EVT, \ref SELFTEST_IPC_PPCA_SILENT_REL,
* or \ref SELFTEST_IPC_PPCA_ALL to run all groups.
*
* \return
*  \ref OK_STATUS (0) - All selected groups passed <br>
*  \ref ERROR_STATUS (1) - No valid group selected, or a sub-test failed <br>
*
*******************************************************************************/
uint8_t SelfTest_IPC_PPCA(uint8_t testMask);

/**
 * \defgroup group_ipc_ppca_test_mask Test selection bitmask values
 * \{
 */
/** Acquire / lock-status / Release (sub-tests 1, 2, 3) */
#define SELFTEST_IPC_PPCA_LOCK          (0x01U)
/** DATA0/DATA1 write/read integrity and independence (sub-tests 4, 5, 13) */
#define SELFTEST_IPC_PPCA_DATA          (0x02U)
/** Notify-to-self loopback and INTR clear-on-write-1 (sub-tests 6, 7, 14) */
#define SELFTEST_IPC_PPCA_NOTIFY        (0x04U)
/** Interrupt masking, INTR_MASKED consistency, INTR_SET (sub-tests 8, 9, 10) */
#define SELFTEST_IPC_PPCA_INTR          (0x08U)
/** Release-event self-loopback (sub-test 11) */
#define SELFTEST_IPC_PPCA_RELEASE_EVT   (0x10U)
/** Silent release, no interrupt generated (sub-test 12) */
#define SELFTEST_IPC_PPCA_SILENT_REL    (0x20U)
/** All six groups combined */
#define SELFTEST_IPC_PPCA_ALL           (0x3FU)
/** \} group_ipc_ppca_test_mask */

/*******************************************************************************
* Function Name: SelfTest_IPC_PPCA_GetFailChannel
****************************************************************************//**
*
* Returns the PPCA IPC channel index (0-3) of the first failure detected in
* the last SelfTest_IPC_PPCA() run. Valid only when ERROR_STATUS was returned.
*
* \return Channel index (0-3) of the failing channel.
*
*******************************************************************************/
uint32_t SelfTest_IPC_PPCA_GetFailChannel(void);

/*******************************************************************************
* Function Name: SelfTest_IPC_PPCA_GetFailIntr
****************************************************************************//**
*
* Returns the PPCA IPC interrupt structure index (0-3) of the first failure
* detected in the last SelfTest_IPC_PPCA() run. Valid only when ERROR_STATUS
* was returned.
*
* \return Interrupt structure index (0-3) of the failing interrupt.
*
*******************************************************************************/
uint32_t SelfTest_IPC_PPCA_GetFailIntr(void);

/*******************************************************************************
* Function Name: SelfTest_IPC_PPCA_GetFailSubtest
****************************************************************************//**
*
* Returns the sub-test number (1-14) of the first failure detected in the last
* SelfTest_IPC_PPCA() run. Valid only when ERROR_STATUS was returned.
* See \ref group_ipc_ppca for the sub-test reference table.
*
* \return Sub-test number (1-14) of the failing sub-test.
*
*******************************************************************************/
uint8_t SelfTest_IPC_PPCA_GetFailSubtest(void);

/** \} group_ipc_ppca_functions */

#endif /* defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN) */

/** \} group_ipc_ppca */
#endif /* SELFTEST_IPC_PPCA_H */
/* [] END OF FILE */

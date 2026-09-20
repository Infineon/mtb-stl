/***************************************************************************//**
 * \file SelfTest_common.h
 * \version 1.0.0
 *
 * This file combines all return status codes of mtb-stl middleware.
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
 ******************************************************************************/


#ifndef SELFTEST_COMMON_H
#define SELFTEST_COMMON_H

/** \cond INTERNAL */
/***************************************
* Device Family Helper Macros
***************************************/

#if (defined(CY_DEVICE_SERIES_PSOC_4100S_MAX) || defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS) || \
    defined(CY_DEVICE_SERIES_PSOC_4500S) || defined(CY_DEVICE_SERIES_PSOC_4100S) || \
    defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS) || defined(CY_DEVICE_SERIES_PSOC_4000T) || \
    defined(CY_DEVICE_SERIES_PSOC_4000S) || defined(CY_DEVICE_SERIES_PSOC_4700S))
#define SELFTEST_PSOC4_FAMILY 1
#endif

#if (defined(CY_DEVICE_SERIES_PSOC_61) || defined(CY_DEVICE_SERIES_PSOC_62) || \
    defined(CY_DEVICE_SERIES_PSOC_63) || defined(CY_DEVICE_SERIES_PSOC_64))
#define SELFTEST_PSOC6_FAMILY 1
#endif

#if (defined(CY_DEVICE_SERIES_XMC7100) || defined(CY_DEVICE_SERIES_XMC7200))
#define SELFTEST_XMC7X_FAMILY 1
#endif

#if (defined(CY_DEVICE_SERIES_PSC3M3) || defined(CY_DEVICE_SERIES_PSC3M5) || \
    defined(CY_DEVICE_SERIES_PSC3P2) || defined(CY_DEVICE_SERIES_PSC3P5) || \
    defined(CY_DEVICE_SERIES_PSC3M8) || defined(CY_DEVICE_SERIES_PSC3P8) || \
    defined(CY_DEVICE_SERIES_PSC3M7) || defined(CY_DEVICE_SERIES_PSC3P7) || \
    defined(CY_DEVICE_SERIES_PSC3M6) || defined(CY_DEVICE_SERIES_PSC3P6))
#define SELFTEST_PSC3_FAMILY 1
#endif

#if (defined(CY_DEVICE_SERIES_XMC5100) || defined(CY_DEVICE_SERIES_XMC5200) || defined(CY_DEVICE_SERIES_XMC5300))
#define SELFTEST_XMC5X_FAMILY 1
#endif
/** \endcond */

/**
 * \defgroup group_stl_status STL return status codes
 * \{
 *
 * Return status codes used by the mtb-stl self-test APIs.
 *
 * Only \ref OK_STATUS and \ref ERROR_STATUS are global to the whole library.
 * Every other code is <b>module-scoped</b>:
 * several codes deliberately reuse the same numeric value because they
 * belong to different self-tests and are never returned by the same API. For
 * example, \ref SHORT_TO_VCC and \ref ERROR_STACK_OVERFLOW are both 1, and
 * \ref SHORT_TO_GND, \ref ERROR_STACK_UNDERFLOW, \ref CRC_SAVED_STATUS and
 * \ref PASS_STILL_TESTING_STATUS are all 2. Always compare a returned value
 * only against the constants documented for the specific API that produced it;
 * never compare status codes across modules.
 */

/** Test passed / operation completed successfully. Returned by every self-test. */
#define OK_STATUS                       (0u)
/** Test failed or a generic error occurred. Returned by every self-test. */
#define ERROR_STATUS                    (1u)

/** PWM init: initialization failed before the self-test could run. */
#define PWM_INIT_ERROR_STATUS           (255u)

/** UART slave respond: background response started successfully.
 *  Module-scoped: aliases \ref OK_STATUS for the UART slave respond API. */
#define UART_SLAVE_RESPOND_STARTED_STATUS      (0u)
/** UART slave respond: response was not started because the unit is idle or input is invalid.
 *  Module-scoped: shares the value 1 with \ref ERROR_STATUS and other module-specific failures. */
#define UART_SLAVE_RESPOND_NOT_STARTED_STATUS  (1u)

/** UART master transfer: background message process started successfully.
 *  Module-scoped: aliases \ref OK_STATUS for the UART master transfer API. */
#define UART_MASTER_MESSAGE_STARTED_STATUS      (0u)
/** UART master transfer: message process did not start because the unit is busy or input is invalid.
 *  Module-scoped: shares the value 1 with \ref ERROR_STATUS and other module-specific failures. */
#define UART_MASTER_MESSAGE_NOT_STARTED_STATUS  (1u)

/** UART self-test: the UART block is not enabled. */
#define ERROR_UART_NOT_ENABLE           (6u)
/** UART self-test: an unexpected or unclassified error occurred. */
#define UNKNOWN_ERROR                   (8u)

/** UART/SPI self-test: the transmit buffer did not drain as expected. */
#define ERROR_TX_NOT_EMPTY              (4u)
/** UART/SPI self-test: the receive buffer was not empty as expected. */
#define ERROR_RX_NOT_EMPTY              (5u)

/** Stack range bitmask flag: bottom guard corrupted (stack overflow).
 *  Module-scoped: shares the value 1 with \ref SHORT_TO_VCC. */
#define ERROR_STACK_OVERFLOW              (1u)
/** Stack range bitmask flag: top guard corrupted (stack underflow).
 *  Module-scoped: shares the value 2 with \ref SHORT_TO_GND, \ref CRC_SAVED_STATUS
 *  and \ref PASS_STILL_TESTING_STATUS. */
#define ERROR_STACK_UNDERFLOW             (2u)

/** I2C self-test: the I2C master is busy. */
#define I2C_MASTER_BUSY_STATUS          (4u)

/** Digital I/O self-test: the tested pin is shorted to VCC.
 *  Module-scoped: shares the value 1 with \ref ERROR_STACK_OVERFLOW. */
#define SHORT_TO_VCC                    (1u)
/** Digital I/O self-test: the tested pin is shorted to ground.
 *  Module-scoped: shares the value 2 with \ref ERROR_STACK_UNDERFLOW,
 *  \ref CRC_SAVED_STATUS and \ref PASS_STILL_TESTING_STATUS.
 */
#define SHORT_TO_GND                    (2u)

/** Clock self-test: the API was called in an incorrect sequence or state. */
#define ERROR_INCORRECT_USAGE_STATUS    (4u)

/** A bad input parameter was passed to a self-test API. */
#define ERROR_BAD_PARAM                 (9u)

/** Timeout return status - waiting for hardware event exceeded timeout */
#define MTB_STL_ERROR_TIMEOUT                   (10u)

/** Startup Configuration Registers self-test (CRC mode): the CRC baseline was
 *  just stored on the first run.
 *  Module-scoped: shares the value 2 with \ref ERROR_STACK_UNDERFLOW,
 *  \ref SHORT_TO_GND and \ref PASS_STILL_TESTING_STATUS. */
#define CRC_SAVED_STATUS                (2u)

/** Multi-call self-test (Flash, Clock, Configuration Registers, I2C, UART, SPI):
 *  the test is still running and has not yet produced a final result.
 *  Module-scoped: shares the value 2 with \ref ERROR_STACK_UNDERFLOW,
 *  \ref SHORT_TO_GND and \ref CRC_SAVED_STATUS. */
#define PASS_STILL_TESTING_STATUS       (2u)
/** Multi-call self-test: the test completed and the final result is available. */
#define PASS_COMPLETE_STATUS            (3u)

/** \} group_stl_status */

#endif /* SELFTEST_COMMON_H */

/* [] END OF FILE */

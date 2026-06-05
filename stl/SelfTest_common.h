/***************************************************************************//**
 * \file SelfTest_common.h
 * \version 1.0.0
 *
 * This file combines all return status codes of mtb-stl middleware.
 *
 *******************************************************************************
 * (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
    defined(CY_DEVICE_SERIES_PSC3P2) || defined(CY_DEVICE_SERIES_PSC3P5))
#define SELFTEST_PSC3_FAMILY 1
#endif

#if (defined(CY_DEVICE_SERIES_XMC5100) || defined(CY_DEVICE_SERIES_XMC5200) || defined(CY_DEVICE_SERIES_XMC5300))
#define SELFTEST_XMC5X_FAMILY 1
#endif
/** \endcond */

/* Common return status codes */
#define OK_STATUS                       (0u)
#define ERROR_STATUS                    (1u)

/* UART Self-Test return status codes */
#define ERROR_UART_NOT_ENABLE           (6u)
#define UNKNOWN_ERROR                   (8u)

/* UART and SPI Self-Test common return status codes */
#define ERROR_TX_NOT_EMPTY              (4u)
#define ERROR_RX_NOT_EMPTY              (5u)


/* STACK OVERFLOW and UNDERFLOW */
#define ERROR_STACK_OVERFLOW              (1u)
#define ERROR_STACK_UNDERFLOW             (2u)

/* I2C Self-Test return status codes */
#define I2C_MASTER_BUSY_STATUS          (4u)

/* Digital I/O Self-Test return status codes */
#define SHORT_TO_VCC                    (1u)
#define SHORT_TO_GND                    (2u)

/* Clock Self-Test return status */
#define ERROR_INCORRECT_USAGE_STATUS    (4u)

/* Startup Configuration Registers Self-Test return status */
/* If this status flag is true then CRC already saved */
#define CRC_SAVED_STATUS                (2u)

/* Additional return status codes for Flash, Clock, Configuration Registers,
   I2C, UART, SPI */
#define PASS_STILL_TESTING_STATUS       (2u)
#define PASS_COMPLETE_STATUS            (3u)

#endif /* SELFTEST_COMMON_H */

/* [] END OF FILE */

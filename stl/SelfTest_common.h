/***************************************************************************//**
* \file SelfTest_common.h
* \version 1.0.0
*
* This file combines all return status codes of mtb-stl middleware.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/* Common return status codes */
#define OK_STATUS                       (0u)
#define ERROR_STATUS                    (1u)

/* UART Self-Test return status codes */
#define ERROR_UART_NOT_ENABLE           (6u)
#define UNKNOWN_ERROR                   (8u)

/* UART and SPI Self-Test common return status codes
   (TO DO: MIDDLEWARE-11530 and MIDDLEWARE-11535) */
#define ERROR_TX_NOT_EMPTY              (4u)
#define ERROR_RX_NOT_EMPTY              (5u)


/* STACK OVERFLOW and UNDERFLOW */
#define ERROR_STACK_OVERFLOW              (1u)
#define ERROR_STACK_UNDERFLOW             (2u)

/* I2C Self-Test return status codes */
/* TO DO: Change Busy status value to 2u (MIDDLEWARE-11529) */
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
   I2C, UART, SPI (TO DO: Remove these codes in mtb-stl-v0.5) */
#define PASS_STILL_TESTING_STATUS       (2u)
#define PASS_COMPLETE_STATUS            (3u)

/* [] END OF FILE */

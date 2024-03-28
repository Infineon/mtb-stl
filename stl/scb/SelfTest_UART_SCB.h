/*******************************************************************************
* File Name: SelfTest_UART_SCB.h
* Version 1.0.0
*
* Description:
*  This file provides the function prototype, constants, and parameter values used
*  for the UART self tests according to the Class B library for CAT2(PSoC4), CAT1A, 
*  CAT1C devices.
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
/**
* \addtogroup group_uart
* \{
*
* This test implements the UART internal data loopback test. The test is a success
* if the transmitted byte is equal to the received byte and returns 2. Each function
* call increments the test byte. After 256 function calls, when the test finishes testing
* all 256 values and they are all a success, the function returns 0. 256 bytes are tested
* so that all of the cells in the 256 byte FIFO are used.
*
*
* \section group_uart_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_uart_macros Macros
* \defgroup group_uart_functions Functions
*/
#include "SelfTest_common.h"


#if !defined(SELFTEST_UART_SCB_H)
    #define SELFTEST_UART_SCB_H

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_uart_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_UART_SCB
****************************************************************************//**
*
* Test loopback of UART (transmitted byte must be equal to received)
*
*
* \param base 
*  Pointer to SCB hardware to configure 
*
* \return
*  1 - test failed <br>
*  2 - Still testing <br>
*  3 - Test completed OK  <br>
*  4 - Error, TX not empty  <br>
*  5 - Error, RX not empty <br>
*  6 - Error, TX not enabled <br>
*  7 - Error, RX not enabled
*
* \note 
*  During call, function transmits and receives bytes from 0x01 to 0xFF.
*  User is responsible for routing the loop back before the test.
*******************************************************************************/
uint8_t SelfTest_UART_SCB(CySCB_Type* base);

/** \} group_uart_functions */

/** \cond INTERNAL */
/***************************************
* Initial Parameter Constants
***************************************/

/* UART data transmit guard interval using guard interval > */
/* (uart_bitrate * uart_data_bits(start data parity stop) * uart_tx_buf_size) */
#define UART_TX_DATA_TIME               (400u)
#define UART_RX_DATA_TIME               (400u)

#define UART_SCB_TRANSMIT_BYTE_ERROR    (100u)
#define UART_TEST_RANGE                 (0xF0u)

/** \endcond */
/** \} group_uart */

#endif /* End __SelfTest_UART_SCB_h */


/* [] END OF FILE */

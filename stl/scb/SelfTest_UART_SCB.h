/*******************************************************************************
* File Name: SelfTest_UART_SCB.h
*
* Description:
*  This file provides the function prototype, constants, and parameter values used
*  for the UART self tests according to the Class B library.
*
* Related Document:
*  AN36847: PSOC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
*******************************************************************************
* (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
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
 * \defgroup group_uart_functions Functions
 */

#if !defined(SELFTEST_UART_SCB_H)
    #define SELFTEST_UART_SCB_H

#include "SelfTest_common.h"

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
* This function tests a loopback of UART (transmitted byte must be equal to received).
*
* This function should be called repeatedly while it returns the
* PASS_STILL_TESTING_STATUS status. The test is complete when
* PASS_COMPLETE_STATUS or any error status is returned.
*
* \param base
* The pointer to SCB hardware to configure.
*
* \return
*  - ERROR_STATUS - Test failed
*  - PASS_STILL_TESTING_STATUS - Still testing
*  - PASS_COMPLETE_STATUS - Test completed OK
*  - ERROR_TX_NOT_EMPTY - Error, TX buffer is not empty
*  - ERROR_RX_NOT_EMPTY - Error, RX buffer is not empty
*  - ERROR_UART_NOT_ENABLE - Error, UART block is not enabled
*
* \note
*  During a call, the function transmits and receives bytes from 0x00 to 0xFF.
*  The user is responsible for the routing the loop back before the test.
*******************************************************************************/
uint8_t SelfTest_UART_SCB(CySCB_Type* base);

/** \} group_uart_functions */

/** \cond INTERNAL */
/***************************************
* Initial Parameter Constants
***************************************/

/* UART data transmit guard interval using the guard interval > */
/* (uart_bitrate * uart_data_bits(start data parity stop) * uart_tx_buf_size) */
#define UART_TX_DATA_TIME               (400u)
#define UART_RX_DATA_TIME               (400u)

#define UART_SCB_TRANSMIT_BYTE_ERROR    (100u)
#define UART_TEST_RANGE                 (0xFFu)

/** \endcond */
/** \} group_uart */

#endif /* End __SelfTest_UART_SCB_h */


/* [] END OF FILE */

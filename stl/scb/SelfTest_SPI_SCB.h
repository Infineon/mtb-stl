/*******************************************************************************
* File Name: SelfTest_SPI_SCB.h
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the SPI self tests according to the Class B library.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
* \addtogroup group_spi
* \{
*
* This test implements the SPI internal data loopback test. The test is a success
* if the transmitted byte is equal to the received byte and returns 2. Each function 
* call increments the test byte. After 256 function calls, when the test finishes 
* testing all 256 values and they are all a success, the function returns 3. 
*
*
* \defgroup group_spi_functions Functions
*/

#if !defined(SELFTEST_SPI_SCB_H)
    #define SELFTEST_SPI_SCB_H

#include "SelfTest_common.h"

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_spi_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_SPI_SCB
****************************************************************************//**
*
*  Test internal loopback of SPI (transmitted byte must be equal to received)
*
*
* \param base 
* The pointer to SCB hardware to configure
*
*
* \note
*  SmartIO should be used for internal loopback.
*  During call the function transmits and receives bytes from 0x01 to 0xFF
*
*
* \return
*  1 - Test failed <br>
*  2 - Still testing <br>
*  3 - Test completed <br>
*  4 - TX Not empty <br>
*  5 - RX Not empty
*
*******************************************************************************/
uint8_t SelfTest_SPI_SCB(CySCB_Type* base);

/** \} group_spi_functions */

/** \cond INTERNAL */
/***************************************
* Initial Parameter Constants
***************************************/

/* SPI data transmit guard interval
 * using guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
#define SPI_TXRX_DATA_TIME              (32u)

#define SPI_SCB_TRANSMIT_BYTE_ERROR     (100u)

/** \endcond */

/** \} group_spi */

#endif /* SELFTEST_SPI_SCB_H */


/* [] END OF FILE */

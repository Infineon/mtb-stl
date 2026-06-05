/*******************************************************************************
* File Name: SelfTest_SPI_SCB.h
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the SPI self tests according to the Class B library.
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
 * \addtogroup group_spi
 * \{
 *
 * This test implements the SPI internal data loopback test. The test is a success
 * if the transmitted byte is equal to the received byte and returns 2. Each function
 * call increments the test byte. After 256 function calls, when the test completes
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
*  This function tests an internal loopback of SPI (transmitted byte must be equal to received).
*
*
* \param base
* The pointer to SCB hardware to configure.
*
*
* \note
*  Use SmartIO for internal loopback.
*  During a call, the function transmits and receives bytes from 0x01 to 0xFF.
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
 * using the guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
#define SPI_TXRX_DATA_TIME              (32u)

#define SPI_SCB_TRANSMIT_BYTE_ERROR     (100u)

/** \endcond */

/** \} group_spi */

#endif /* SELFTEST_SPI_SCB_H */


/* [] END OF FILE */

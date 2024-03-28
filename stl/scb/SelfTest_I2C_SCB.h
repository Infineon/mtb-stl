/*******************************************************************************
* File Name: SelfTest_I2C_SCB.h
* Version 1.0.0
*
* Description:
*  This file provides the function prototype, constants, and parameter values used
*  for the I2C self tests according to the Class B library for CAT2(PSoC4), CAT1A,
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
* \addtogroup group_i2c
* \{
*
* This test requires connecting the I2C Master and Slave externally.
* 1) Master periodically sends an I2C data (from 0x00 to 0xFF one by one) addressing slave [0x08]. <br>
* 2) Slave reads the data, complements, and writes to slave read buffer. <br>
* 3) Master reads the data from slave and compares it with the complement of data sent in last interaction.
*
* \section group_i2c_more_information More Information
*
*
*
// \verbatim
I2C Write API - Operation
1) Initiates I2C Master transaction to write data to the slave.
2) Waits for Write transfer completion
3) Reads the Slave Write buffer
4) Performs 1's complement on the read data
5) Writes the complemented data to Slave Read Buffer

I2C Read API - Operation
1) Initiates I2C Master transaction to read data from the slave .
2) Waits for Read transfer completion
3) Checks whether the data read is equal to the complement of data written
\endverbatim
*
*
*
* \section group_i2c_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_i2c_macros Macros
* \defgroup group_i2c_functions Functions
*/
#include "SelfTest_common.h"


#if !defined(SELFTEST_I2C_SCB_H)
    #define SELFTEST_I2C_SCB_H


/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_i2c_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_I2C_SCB
****************************************************************************//**
*
*  I2C master under test sends data to I2C slave for self-test.
*  Slave read the data and write the complement[1's] to the slave read buffer
*  Master read back the data from slave and check the data is corrupted.
*
*
* \param master_base
*  The pointer to the master I2C SCB instance.
* \param master_context
*  The pointer to the master I2C SCB context.
* \param slave_base
*  The pointer to the slave I2C SCB instance.
* \param slave_context
*  The pointer to the slave I2C SCB context.
* \param slave_read_buf 
*  The pointer to slave read buffer.
* \param slave_write_buf 
*  The pointer to slave write buffer.
*
*
* \note
*  During call, function transmits from 0x01 to 0xFF
*  Clear I2C Master status, read buffer and write buffer
*
*
* \return
*  1 - Test failed <br>
*  2 - Still testing <br>
*  3 - Test completed OK
*
*******************************************************************************/
uint8_t SelfTest_I2C_SCB(CySCB_Type* master_base, cy_stc_scb_i2c_context_t* master_context,
                         CySCB_Type* slave_base, cy_stc_scb_i2c_context_t* slave_context,
                         uint8_t* slave_read_buf, uint8_t* slave_write_buf);

/** \} group_i2c_functions */
/***************************************
 *            Constants
 ****************************************/
/** \addtogroup group_i2c_macros
* \{
*/
/** I2C Slave address to communicate with */
#define I2C_SLAVE_ADDR                  (0x08u)

/** No of data byte to be send*/
#define PACKET_SIZE                     (1u)

/** \} group_i2c_macros */

/***************************************
* Initial Parameter Constants
***************************************/
/** \cond INTERNAL */
/* I2C data transmit guard interval using guard interval > */
#define I2C_DATA_TIME                   (800u)

/* Raise error if I2C Master is busy for 100 re-tries */
#define I2C_MASTER_BUSY_RETRY           (100u)

#define I2C_SCB_TRANSMIT_BYTE_ERROR     (100u)
#define I2C_TEST_RANGE                  (0xFFu)

#if !defined(WRITE)
    #define WRITE                       (0u)
#endif /* End FALSE */
#if !defined(READ)
    #define READ                        (1u)
#endif /* End TRUE */

/** \endcond */

/** \} group_i2c */

#endif /* End __SelfTest_I2C_SCB_h */


/* [] END OF FILE */

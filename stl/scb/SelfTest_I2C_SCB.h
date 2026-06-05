/*******************************************************************************
* File Name: SelfTest_I2C_SCB.h
*
* Description:
*  This file provides the function prototype, constants, and parameter values used
*  for the I2C self tests according to the Class B library.
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
 * \addtogroup group_i2c
 * \{
 *
 * This test requires connecting the I2C Master and Slave externally.
 * 1) Master periodically sends I2C data (from 0x00 to 0xFF one by one) addressing the slave [0x08].
 * <br>
 * 2) Slave reads the data, complements, and writes to slave read buffer. <br>
 * 3) Master reads the data from slave and compares it with the complement of data sent in last
 *    interaction.
 *
 * \section group_i2c_more_information More Information
 *
 * I2C Write API - Operation:
 *
 *      1) Initiates I2C Master transaction to write data to the slave.
 *      2) Waits for Write transfer completion.
 *      3) Reads the Slave Write buffer.
 *      4) Performs 1's complement on the read data.
 *      5) Writes the complemented data to Slave Read Buffer.
 *
 * I2C Read API - Operation:
 *
 *      1) Initiates I2C Master transaction to read data from the slave.
 *      2) Waits for Read transfer completion.
 *      3) Checks whether the data read is equal to the complement of data written.
 *
 *
 * \defgroup group_i2c_macros Macros
 * \defgroup group_i2c_functions Functions
 */

#if !defined(SELFTEST_I2C_SCB_H)
    #define SELFTEST_I2C_SCB_H
#include "SelfTest_common.h"

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
*  I2C master under test sends data to I2C slave for self test.
*  Slave reads the data and writes the complement[1's] to the slave read buffer.
*  Master reads back the data from the slave and checks the dataforis corruption.
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
*  The pointer to slave read buffe.
* \param slave_write_buf
*  The pointer to slave write buffer.
*
*
* \note
*  During a call, the function transmits from 0x01 to 0xFF.
*  Clears I2C Master status, reads and writes the buffer
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

/** No data byte to send */
#define PACKET_SIZE                     (1u)

/** \} group_i2c_macros */

/***************************************
* Initial Parameter Constants
***************************************/
/** \cond INTERNAL */
/* I2C data transmits a guard interval using the guard interval > */
#define I2C_DATA_TIME                   (800u)

/* Raise an error if I2C Master is busy for 100 re-tries */
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

/*******************************************************************************
* File Name: SelfTest_I2C_SCB.h
* Version 2.0
*
* Description:
*  This file provides the function prototype, constants, and parameter values used
*  for the I2C self tests according to the Class B library for PSoC4
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "SelfTest_common.h"


#if !defined(SELFTEST_I2C_SCB_H)
    #define SELFTEST_I2C_SCB_H


/***************************************
* Function Prototypes
***************************************/
uint8_t SelfTest_I2C_SCB(CySCB_Type* master_base, cy_stc_scb_i2c_context_t* master_context,
                         CySCB_Type* slave_base, cy_stc_scb_i2c_context_t* slave_context,
                         uint8_t* slave_read_buf, uint8_t* slave_write_buf);


/***************************************
 *            Constants
 ****************************************/

/* I2C Slave address to communicate with */
#define I2C_SLAVE_ADDR                  (0x08u)
#define PACKET_SIZE                     (1u)

/***************************************
* Initial Parameter Constants
***************************************/

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

#endif /* End __SelfTest_I2C_SCB_h */


/* [] END OF FILE */

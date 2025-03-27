/*******************************************************************************
* File Name: SelfTest_CRC_calc.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the CRC16 implementation according to CCITT standards.
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
* \addtogroup group_crc
* \{
*
* The CRC test implements the CRC16 implementation according to CCITT standards.
*
* \section group_crc_more_information More Information
*
* This api's are used by the various modules for calculating CRC.
*
*
* \defgroup group_crc_macros Macros
* \defgroup group_crc_functions Functions
*/

#if !defined(SELFTEST_CRC_CALC_H)
    #define SELFTEST_CRC_CALC_H

#include "cy_pdl.h"

/** \addtogroup group_crc_macros
* \{
*/

/** CRC mask */
#define CRC_MASK    0x00FFu

/** Initial value for CRC-32*/
#define CRC32_INIT_VALUE        (0xFFFFFFFFUL)

/** Initial value for CRC-16 */
#define CRC16_CCITT_INIT_VALUE    (0x0000U)
/** \} group_crc_macros */

/**
* \addtogroup group_crc_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTests_CRC16_CCITT
****************************************************************************//**
*
* This function calculates CRC of memory area defined in parameters "BaseAdress"
* and "len"
*
* \param BaseAdress
* Address from which CRC needs to calculate
*
* \param len
* Length of area for CRC calculation
*
* \return
*  Calculated CRC value
*
*******************************************************************************/
uint16_t SelfTests_CRC16_CCITT(uint32_t BaseAdress, uint16_t len);


/*******************************************************************************
* Function Name: SelfTests_CRC16_CCITT_ACC
****************************************************************************//**
*
* This function calculates CRC of area defined in parameters "BaseAdress"
* and "len"
*
* \param crc
* Current CRC
*
* \param BaseAdress
* Address from which CRC needs to calculate
*
* \param len
* Length of area for CRC calculation
*
* \return
* Calculated CRC value
*
*
*******************************************************************************/
uint16_t SelfTests_CRC16_CCITT_ACC(uint16_t crc, uint32_t BaseAdress, uint16_t len);

/*******************************************************************************
* Function Name: SelfTests_CRC16_CCITT_Byte
****************************************************************************//**
*
* This function calculates CRC of one byte "val"
*
* \param crc
* Current CRC
*
* \param val
* Value for which CRC needs to calculate
*
* \return
* Calculated CRC value
*
*
*******************************************************************************/
uint16_t SelfTests_CRC16_CCITT_Byte(uint16_t crc, uint8_t val);

/*******************************************************************************
* Function Name: SelfTests_CRC32
****************************************************************************//**
*
* This function calculates CRC of memory area defined in parameters "BaseAdress"
* and "len"
*
* \param BaseAdress
* Address from which CRC needs to calculate
*
* \param len
* Length of area for CRC calculation
*
* \return
* Calculated CRC value
*
*
*******************************************************************************/
uint32_t SelfTests_CRC32(uint32_t BaseAdress, uint32_t len);

/*******************************************************************************
* Function Name: SelfTests_CRC32_ACC
****************************************************************************//**
*
* This function calculates CRC of memory area defined in parameters "BaseAdress"
* and "len"
*
* \param crc
* Current CRC
*
* \param BaseAdress
* Address from which CRC needs to calculate
*
* \param len
* Length of area for CRC calculation
*
* \return
* Calculated CRC value
*
*
*******************************************************************************/
uint32_t SelfTests_CRC32_ACC(uint32_t crc, uint32_t BaseAdress, uint32_t len);

/*******************************************************************************
* Function Name: SelfTests_CRC32_Byte
****************************************************************************//**
*
*  This function calculates CRC of one byte "val"
*
* \param crc
* Current CRC
*
* \param val
* Value to calculate CRC
*
* \return
* Calculated CRC value
*
*
*******************************************************************************/
uint32_t SelfTests_CRC32_Byte(uint32_t crc, uint8_t val);


/** \} group_crc_functions */

/** \} group_crc */

#endif /* End __SelfTest_CRC_calc_h */


/* [] END OF FILE */

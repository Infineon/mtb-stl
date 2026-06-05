/*******************************************************************************
* File Name: SelfTest_CRC_calc.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for the CRC16 implementation according to CCITT standards.
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
 * \addtogroup group_crc
 * \{
 *
 * The CRC test implements the CRC16 implementation according to CCITT standards.
 *
 * \section group_crc_more_information More Information
 *
 * These functions are used by the various modules for calculating the CRC.
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
* This function calculates the CRC of the memory area defined in the parameters "BaseAdress"
* and "len".
*
* \param BaseAdress
* The address from which to calculate the CRC.
*
* \param len
* The length of the area for CRC calculation.
*
* \return
*  The calculated CRC value.
*
*******************************************************************************/
uint16_t SelfTests_CRC16_CCITT(uint32_t BaseAdress, uint16_t len);


/*******************************************************************************
* Function Name: SelfTests_CRC16_CCITT_ACC
****************************************************************************//**
*
* This function calculates the CRC of the area defined in the parameters "BaseAdress"
* and "len".
*
* \param crc
* The current CRC.
*
* \param BaseAdress
* The address from which to calculate the CRC.
*
* \param len
* The length of the area for CRC calculation.
*
* \return
* The calculated CRC value.
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
* The current CRC.
*
* \param val
* The value to calculate the CRC for.
*
* \return
* The calculated CRC value.
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
* The address from which to calculate the CRC.
*
* \param len
* The length of the area for CRC calculation.
*
* \return
* The calculated CRC value.
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
* The current CRC.
*
* \param BaseAdress
* The address from which to calculate the CRC.
*
* \param len
* The length of the area for CRC calculation.
*
* \return
* The calculated CRC value.
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
* The current CRC.
*
* \param val
* The value to calculate the CRC for.
*
* \return
* The calculated CRC value.
*
*
*******************************************************************************/
uint32_t SelfTests_CRC32_Byte(uint32_t crc, uint8_t val);


/** \} group_crc_functions */

/** \} group_crc */

#endif /* End __SelfTest_CRC_calc_h */


/* [] END OF FILE */

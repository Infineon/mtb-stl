/*******************************************************************************
* File Name: SelfTest_ECC.h
* Version 1.0.0
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for Ecc self tests.
*
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
* \addtogroup group_ecc
* \{
*
* ECC self test to detect and correct the single
* bit error and reports fault for double bit error.
*
* \section group_ecc_more_information More Information
* ECC self test is supported on CAT1C devices.
*
* \section group_ecc_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_ecc_enums Enumerated Types 
* \defgroup group_ecc_functions Functions
*/
#if !defined(SELFTEST_ECC_H)
    #define SELFTEST_ECC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))

/** \cond INTERNAL */
#define CY_FLASH_SIZE_ROW                512U
#define CY_FLASH_ADDR ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY    0x14
#define CY_ECC_C_ERROR_PARITY     0x5D
/** \endcond */

/***************************************
*       Enumerations
***************************************/
/** \addtogroup group_ecc_enums
* \{
*/
/** Error injection mode */
typedef enum
{
    CY_ECC_NO_ERROR = 0, /**< No ECC error injection.*/
    CY_ECC_C_ERROR = 1,  /**< Correctable ECC error injection.*/
    CY_ECC_NC_ERROR = 2, /**< Non correctable error injection.*/
} cy_en_ecc_error_mode_t;
/** \}group_ecc_enums */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_ecc_functions
* \{
*/
/*******************************************************************************
* Function Name: SelfTest_ECC
****************************************************************************//**
*
* This function performs the ECC hardware self test to detect and correct the single
* bit error and reports fault for double bit error.
*
* \param eccErrorMode 
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode);

/** \}group_ecc_functions */

/** \}group_ecc */

#endif
#endif /* SELFTEST_ECC_H */

/* [] END OF FILE */

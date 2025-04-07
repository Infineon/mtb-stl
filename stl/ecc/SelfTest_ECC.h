/*******************************************************************************
* File Name: SelfTest_ECC.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for Ecc self tests.
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
* \addtogroup group_ecc
* \{
*
* ECC self test to detect and correct the single
* bit error and reports fault for double bit error.
*
* \section group_ecc_more_information More Information
* ECC self test is supported on CAT1B, CAT1C devices.
*
*
* \defgroup group_ecc_enums Enumerated Types 
* \defgroup group_ecc_functions Functions
*/
#if !defined(SELFTEST_ECC_H)
    #define SELFTEST_ECC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"
#include "SelfTest_ErrorInjection.h"

#if (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))  || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
#include "cy_sysfault.h"

/** \cond INTERNAL */
#define CY_FLASH_SIZE_ROW                512U
#if (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7))
#define CY_FLASH_ADDR ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY    0x14
#define CY_ECC_C_ERROR_PARITY     0x5D
#define CY_MEMORY_DATA            0x5A5A5A5A
#define CY_ECC_C_FAULT            CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT           CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))
#define CY_FLASH_ADDR (CY_FLASH_BASE + (CY_FLASH_SIZE/2))
#define CY_ECC_NC_ERROR_PARITY    0xE1
#define CY_ECC_C_ERROR_PARITY     0x16
#define CY_MEMORY_DATA            0x12341234
#define CY_ECC_C_FAULT            CPUSS_FLASHC_MAIN_C_ECC_MMIO
#define CY_ECC_NC_FAULT           CPUSS_FLASHC_MAIN_NC_ECC_MMIO
#define CY_ECC_C_RAM_FAULT        CPUSS_RAMC0_C_ECC_MMIO
#define CY_ECC_NC_RAM_FAULT       CPUSS_RAMC0_NC_ECC_MMIO
#define CY_RAM_ADDR               0x34009000
#define CY_RAM_ECC_NC_ERROR_PARITY 0x039
#define CY_RAM_ECC_C_ERROR_PARITY  0x070
#define CY_RAM_MEMORY_DATA         0x5A5A5A5A
#endif
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
    CY_ECC_NC_ERROR = 1, /**< Non correctable error injection.*/
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
* This function performs the ECC hardware self test for Flash memory
* to report fault for double bit error.
*
* \param eccErrorMode 
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* This API is applicable to CAT1C devices
*
*******************************************************************************/
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash
****************************************************************************//**
*
* This function performs the ECC hardware self test to detect and correct the single
* bit error and reports fault for double bit error.
*
* \param addr 
* Aligned flash row address.
*
* \param eccErrorMode 
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* This API is applicable to CAT1B devices
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);


/*******************************************************************************
* Function Name: SelfTest_ECC_Ram
****************************************************************************//**
*
* This function performs the ECC hardware self test for Ram memory
* to report fault for double bit error.
*
* \param addr 
* Ram address.
*
* \param eccErrorMode 
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* This API is applicable to CAT1B devices
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);
/** \}group_ecc_functions */

/** \}group_ecc */

#endif
#endif /* SELFTEST_ECC_H */

/* [] END OF FILE */

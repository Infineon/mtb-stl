/*******************************************************************************
* File Name: SelfTest_ECC.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
 * \addtogroup group_ecc
 * \{
 *
 * The ECC self test detects and corrects the single-
 * bit error and reports a fault for the double-bit error.
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

#if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT) || defined (CY_DOXYGEN))
#include "cy_sysfault.h"

/** \cond INTERNAL */
#define CY_FLASH_SIZE_ROW                512U
#if defined(CY_IP_M7CPUSS)
#define CY_FLASH_ADDR ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY    0x14
#define CY_ECC_C_ERROR_PARITY     0x5D
#define CY_MEMORY_DATA            0x5A5A5A5A
#define CY_ECC_C_FAULT            CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT           CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif

#if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT)
#define CY_FLASH_ADDR ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY    0x14
#define CY_ECC_C_ERROR_PARITY     0x5D
#define CY_MEMORY_DATA            0x5A5A5A5A
#define CY_ECC_C_FAULT            CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT           CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif /* if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT) */

#if defined(CY_IP_M33SYSCPUSS)
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
#endif /* defined(CY_IP_M33SYSCPUSS) */
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
    CY_ECC_NC_ERROR = 1 /**< Non correctable error injection.*/
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
* to report a fault for the double-bit error.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* This API is applicable only to CAT1C devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash
****************************************************************************//**
*
* This function performs the ECC hardware self test to detect and correct the single
* bit error and report a fault for the double-bit error.
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
* This API is applicable only to CAT1B devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);


/*******************************************************************************
* Function Name: SelfTest_ECC_Ram
****************************************************************************//**
*
* This function performs the ECC hardware self test for Ram memory
* to report a fault for the double-bit error.
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
* This API is applicable only to CAT1B devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);
/** \}group_ecc_functions */

/** \}group_ecc */

#endif /* if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT) || defined (CY_DOXYGEN)) */

#endif /* SELFTEST_ECC_H */

/* [] END OF FILE */

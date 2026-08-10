/*******************************************************************************
* File Name: SelfTest_ECC.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
*
*******************************************************************************
* (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
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
 * \defgroup group_ecc ECC (ECC STL module)
 * \{
 *
 * ECC hardware can correct single-bit (correctable) memory errors and report
 * double-bit (non-correctable) memory errors through SysFault.
 *
 * The STL ECC self-test APIs validate the non-correctable fault-reporting path
 * by injecting ECC corruption and verifying the expected SysFault source.
 *
 * Test sequence used by the APIs:
 * - Configure SysFault mask/interrupt for ECC sources.
 * - Inject ECC corruption into the target memory.
 * - Read corrupted location to trigger ECC check.
 * - Poll fault source and compare with expected non-correctable source.
 *
 * \section group_ecc_more_information More Information
 *
 * API applicability:
 *
 * <table>
 *   <tr><th>API</th><th>Device Family</th><th>Memory under test</th><th>Address requirement</th></tr>
 *   <tr><td>\ref SelfTest_ECC</td><td>XMC</td><td>Flash</td><td>Internal test address is used</td></tr>
 *   <tr><td>\ref SelfTest_ECC_Flash</td><td>PSOC Control</td><td>Flash</td><td>Flash row-aligned address</td></tr>
 *   <tr><td>\ref SelfTest_ECC_Ram</td><td>PSOC Control</td><td>RAM</td><td>Valid SRAM address</td></tr>
 * </table>
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
#define CY_FLASH_SIZE_ROW          512U
#if defined(CY_IP_M7CPUSS)
#define CY_FLASH_ADDR              ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY     0x14
#define CY_ECC_C_ERROR_PARITY      0x5D
#define CY_MEMORY_DATA             0x5A5A5A5A
#define CY_ECC_C_FAULT             CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT            CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif

#if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT)
#define CY_FLASH_ADDR              ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY     0x14
#define CY_ECC_C_ERROR_PARITY      0x5D
#define CY_MEMORY_DATA             0x5A5A5A5A
#define CY_ECC_C_FAULT             CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT            CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif /* if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT) */

#if defined(CY_IP_M33SYSCPUSS)
#define CY_FLASH_ADDR              (CY_FLASH_BASE + (CY_FLASH_SIZE/2))
#define CY_ECC_NC_ERROR_PARITY     0xE1
#define CY_ECC_C_ERROR_PARITY      0x16
#define CY_MEMORY_DATA             0x12341234
#define CY_ECC_C_FAULT             CPUSS_FLASHC_MAIN_C_ECC_MMIO
#define CY_ECC_NC_FAULT            CPUSS_FLASHC_MAIN_NC_ECC_MMIO
#define CY_ECC_C_RAM_FAULT         CPUSS_RAMC0_C_ECC_MMIO
#define CY_ECC_NC_RAM_FAULT        CPUSS_RAMC0_NC_ECC_MMIO
#if (CY_IP_MXSRAMC_INSTANCES == 2U)
#define CY_ECC_C_RAM1_FAULT         CPUSS_RAMC1_C_ECC_MMIO
#define CY_ECC_NC_RAM1_FAULT        CPUSS_RAMC1_NC_ECC_MMIO
#endif
#define CY_RAM_ADDR                (CY_SRAM_BASE + (CY_SRAM_SIZE/2))
#define CY_RAM_ECC_NC_ERROR_PARITY 0x039
#define CY_RAM_ECC_C_ERROR_PARITY  0x070
#define CY_RAM_MEMORY_DATA         0x5A5A5A5A
#define CY_RAM_CORRUPT_DATA        0x7A7A7A7A
#endif /* if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)) */
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
* Performs ECC hardware self-test for Flash memory on XMC devices.
*
* The self-test configures SysFault for both correctable and
* non-correctable Flash ECC sources, then injects ECC corruption
* into the internal flash test location and reads that location to
* validate that the non-correctable source is reported.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable Flash ECC source.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it erases and programs the internal flash
* test row. Any previous content at the test address is permanently overwritten.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API is applicable only to XMC devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash
****************************************************************************//**
*
* Performs ECC hardware self-test for Flash memory on PSOC Control devices.
*
* The self-test configures SysFault for Flash ECC sources, programs
* conflicting flash data/ECC information to inject corruption, re-enables ECC check,
* and reads the provided flash row to trigger ECC fault reporting.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable Flash ECC source.
*
* \param addr
* Aligned flash row address.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it erases and reprograms the flash row at
* \p addr. Any previous content at that address is permanently overwritten.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API is applicable only to PSOC Control devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);


/*******************************************************************************
* Function Name: SelfTest_ECC_Ram
****************************************************************************//**
*
* Performs ECC hardware self-test for RAM memory on PSOC Control devices.
*
* The self-test configures SysFault for RAM ECC sources, writes baseline
* data with ECC enabled, corrupts data with ECC disabled, re-enables ECC check,
* and reads memory to trigger ECC fault reporting.
*
* On devices with multiple RAMC instances, the API selects the RAMC instance
* based on \p addr and accepts the non-correctable source from the corresponding
* RAMC block.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable RAM ECC source.
*
* \param addr
* RAM address.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it overwrites the RAM location at \p addr
* with test data. The caller must ensure \p addr does not hold live application
* data during the test.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API assumes that RAM address range is continuous and does not have gaps
* between address ranges of different RAM controllers
*
* \note
* This API is applicable only to devices that have RAM ECC.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);
/** \}group_ecc_functions */

/** \}group_ecc */

#endif /* if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT) || defined (CY_DOXYGEN)) */
#endif /* SELFTEST_ECC_H */

/* [] END OF FILE */

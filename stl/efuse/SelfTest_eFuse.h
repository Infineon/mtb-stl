/*******************************************************************************
* File Name: SelfTest_eFuse.h
*
* Description:
*  Provides the function prototype and usage contract for the XMC7x eFuse
*  customer-data integrity self-test.
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
 * \defgroup group_stl_efuse eFuse Test (eFuse STL module)
 * \{
 *
 * Provides a read-only CRC32 check for safety-relevant bits in the eFuse
 * customer region on XMC7000 devices. The caller selects a
 * contiguous byte range and supplies an independently established reference
 * CRC. This library neither programs eFuse nor creates the reference.
 *
 * The application may call \ref SelfTest_EFuse_CheckSum during initialization and periodically;
 * scheduling, reference storage, and the reaction to a failed test belong to
 * the application. The test covers only the range selected by the caller.
 *
 * \defgroup group_stl_efuse_functions Functions
 */

#if !defined(SELFTEST_EFUSE_H)
#define SELFTEST_EFUSE_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXEFUSE) && (CY_IP_MXEFUSE_VERSION <= 2)) || defined(CY_DOXYGEN)
/** \addtogroup group_stl_efuse_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_EFuse_CheckSum
****************************************************************************//**
*
* Checks an accessible byte range within the eFuse customer region against a
* caller-provided reference CRC32. Every byte is read through the PDL
* eFuse API. Any failed read or CRC mismatch causes the test to fail.
*
* The IP header supplies the start of CUSTOMER_DATA. No absolute OTP
* address is accepted from the caller. The CRC uses \ref SelfTests_CRC32_Byte
* over bytes in ascending offset order, starting with \ref CRC32_INIT_VALUE
* (0xFFFFFFFF), without a final XOR. Standard CRC-32/IEEE yields a different
* reference and must not be substituted.
*
* \param startOffset
* First byte relative to the customer region (not the physical eFuse address).
* Valid together with \p length only if the complete range fits within the
* customer region.
*
* \param length
* Number of customer bytes to test. Must be greater than zero. The region is
* 24 bytes on XMC7100 and XMC7200.
*
* \param expectedCrc
* Expected CRC for exactly the selected bytes. The integrator must establish
* this value independently, for example during provisioning, and store it
* outside the tested region.
*
* \return
*  \ref OK_STATUS - All reads succeeded and the CRC matches. <br>
*  \ref ERROR_STATUS - Failed read or CRC mismatch. <br>
*  \ref ERROR_BAD_PARAM - Invalid customer-relative range. <br>
*
*******************************************************************************/
uint8_t SelfTest_EFuse_CheckSum(uint32_t startOffset, uint32_t length,
                                uint32_t expectedCrc);
/** \} group_stl_efuse_functions */
#endif /* (defined(CY_IP_MXEFUSE) && (CY_IP_MXEFUSE_VERSION <= 2)) || defined(CY_DOXYGEN) */

#endif /* SELFTEST_EFUSE_H */

/** \} group_stl_efuse */

/* [] END OF FILE */

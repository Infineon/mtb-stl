/*******************************************************************************
* File Name: SelfTest_eFuse.c
*
* Description:
*  Provides the XMC7x eFuse customer-data integrity self-test. Reads the
*  selected customer bytes through the PDL and compares their STL CRC32 with
*  the caller's reference value.
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

#include "cy_pdl.h"
#include "SelfTest_eFuse.h"
#include "SelfTest_CRC_calc.h"
#include <stddef.h>

#if (defined(CY_IP_MXEFUSE) && (CY_IP_MXEFUSE_VERSION <= 2))
/*******************************************************************************
* Function Name: SelfTest_EFuse_CalculateCrc
*******************************************************************************/
static uint8_t SelfTest_EFuse_CalculateCrc(uint32_t startOffset, uint32_t length,
                                           uint32_t* calculatedCrc)
{
    uint32_t crc = CRC32_INIT_VALUE;
    uint8_t ret = OK_STATUS;

    for (uint32_t index = 0u; index < length; ++index)
    {
        uint8_t value = 0u;

        if (Cy_EFUSE_GetEfuseByte(startOffset + index, &value) != CY_EFUSE_SUCCESS)
        {
            ret = ERROR_STATUS;
            break;
        }

        crc = SelfTests_CRC32_Byte(crc, value);
    }

    *calculatedCrc = crc;
    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_EFuse_CheckSum
*******************************************************************************/
uint8_t SelfTest_EFuse_CheckSum(uint32_t startOffset, uint32_t length,
                                uint32_t expectedCrc)
{
    uint32_t customerDataStart = (uint32_t)offsetof(cy_stc_efuse_data_t, CUSTOMER_DATA) /
                                 CY_EFUSE_BITS_PER_BYTE;
    uint32_t customerDataSize = (uint32_t)sizeof(((cy_stc_efuse_data_t*)0)->CUSTOMER_DATA) /
                                CY_EFUSE_BITS_PER_BYTE;
    uint32_t calculatedCrc = 0u;

    uint8_t ret = ERROR_BAD_PARAM;

    if (!(length == 0u) && !(startOffset >= customerDataSize) &&
        !(length > (customerDataSize - startOffset)))
    {
        ret = SelfTest_EFuse_CalculateCrc(customerDataStart + startOffset, length, &calculatedCrc);
        if (ret == OK_STATUS)
        {
            ret = (calculatedCrc == expectedCrc) ? OK_STATUS : ERROR_STATUS;
        }
    }
    return ret;
}


#endif /* (defined(CY_IP_MXEFUSE) && (CY_IP_MXEFUSE_VERSION <= 2)) */

/* [] END OF FILE */

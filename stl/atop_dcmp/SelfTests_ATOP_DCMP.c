/*******************************************************************************
* File Name: SelfTests_ATOP_DCMP.c
*
* Description:
*  This file provides source code for the APIs to perform ATOP DCMP
*  self tests according to the Class B library.
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
#include "SelfTests_ATOP_DCMP.h"

#if defined(CY_IP_MXS40PPSS)

static const uint32_t stlAtopDcmp_testValues[] =
{
    0xFFFUL, /* Top of the ADC range, greater than threshold */
    0x800UL, /* Middle of the ADC range, same as threshold (corner case) */
    0x00UL  /* Bottom of the ADC range, less than threshold)*/
};

static const bool stlAtopDcmp_expectedResults[] = { (bool)1, (bool)0, (bool)0 };

#define DCMP_TEST_NUM_VALUES  (sizeof(stlAtopDcmp_testValues) / sizeof(stlAtopDcmp_testValues[0]))

/* DCMP test threshold for unsigned ADC channel (middle of the ADC range) */
#define STL_DCMP_TH_UNS (0x800U)

/* DCMP test threshold for signed ADC channel (middle of the ADC range) */
#define STL_DCMP_TH_SGN (0U)

#define STL_DCMP_ADC_GAIN_CORRECTION (1024U)
#define STL_DCMP_ADC_OFFET (0U)
#define STL_DCMP_ADC_TIMEOUT_US (100U)
/*******************************************************************************
* Function Name: SelfTests_ATOP_DCMP
********************************************************************************
*
* Performs ATOP DCMP self-test using ADC test-data injection.
*
* The function sets an initial DCMP threshold, adapts it for signed mode if
* needed, and then checks DCMP status for three injected ADC values.
*
* \param adcBase
* Pointer to ADC base used for test-data injection and conversion trigger.
*
* \param dcmpBase
* Pointer to DCMP base used for threshold setup and status read.
*

* \return
* \ref OK_STATUS (0) if all comparator results match expected values.
* \ref ERROR_STATUS (1) if any comparator result mismatches.
* \ref MTB_STL_ERROR_TIMEOUT (10) if ADC conversion times out.
*
* \note This test changes selected DCMP threshold so user need to restore it manually.
*
*******************************************************************************/
uint8_t SelfTests_ATOP_DCMP(ATOPSS_ADC_TYPE* adcBase, ATOPSS_DCMP_TYPE* dcmpBase)
{
    uint8_t status = OK_STATUS;
    uint8_t timeout = STL_DCMP_ADC_TIMEOUT_US;

    /* Test Setup Stage */
    uint8_t channelNum = Cy_PPCA_DCMP_Get_Source_Channel(dcmpBase);
    uint16_t channelMask = (uint16_t)((uint16_t)1U << channelNum);

    /* Storing previous configuration*/
    bool prevTestMode = Cy_PPCA_ADC_Is_Test_Mode_Enabled(adcBase);
    uint16_t prevGainCorrection = Cy_PPCA_ADC_Get_Gain_Correction(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL);
    uint8_t prevOffset = Cy_PPCA_ADC_Get_Offset(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL);

    /* Writing test configuration*/
    Cy_PPCA_ADC_Set_Test_mode(adcBase, true);
    Cy_PPCA_ADC_Set_Gain_Correction(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL, STL_DCMP_ADC_GAIN_CORRECTION);
    Cy_PPCA_ADC_Set_Offset(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL, STL_DCMP_ADC_OFFET);

    if (Cy_PPCA_ADC_Is_Channel_Signed(adcBase, channelNum))
    {
        Cy_PPCA_DCMP_Set_Threshold(dcmpBase, STL_DCMP_TH_SGN);
    }
    else
    {
        Cy_PPCA_DCMP_Set_Threshold(dcmpBase, STL_DCMP_TH_UNS);
    }

    /* Test Run Stage */

    /* Wait until ADC stops being busy */
    while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
    {
        Cy_SysLib_DelayUs(1U);
        timeout--;
    }

    if (Cy_PPCA_ADC_Is_ADC_Busy(adcBase))
    {
        status = MTB_STL_ERROR_TIMEOUT;
    }
    else
    {
        /* Reset timeout */
        timeout = STL_DCMP_ADC_TIMEOUT_US;
    }

    for (uint8_t testIndex = 0U; testIndex < DCMP_TEST_NUM_VALUES; testIndex++)
    {
        if (status != OK_STATUS)
        {
            break;
        }
        Cy_PPCA_ADC_Write_Test_data(adcBase, stlAtopDcmp_testValues[testIndex]);
        Cy_PPCA_ADC_Trigger(adcBase, channelMask);

        while (Cy_PPCA_ADC_Is_ADC_Busy(adcBase) && (timeout > 0U))
        {
            Cy_SysLib_DelayUs(1U);
            timeout--;
        }

        if (Cy_PPCA_ADC_Is_ADC_Busy(adcBase))
        {
            status = MTB_STL_ERROR_TIMEOUT;
        }

        if (status == OK_STATUS)
        {
            Cy_PPCA_DCMP_Trigger(dcmpBase);
            Cy_SysLib_DelayUs(1U);
            bool compRes = Cy_PPCA_DCMP_Get_Status(dcmpBase);
            if (compRes != stlAtopDcmp_expectedResults[testIndex])
            {
                status = ERROR_STATUS;
            }
        }
    }

    /* Restoring previous configuration*/
    Cy_PPCA_ADC_Set_Test_mode(adcBase, prevTestMode);
    Cy_PPCA_ADC_Set_Offset(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL, prevOffset);
    Cy_PPCA_ADC_Set_Gain_Correction(adcBase, channelNum, CY_ADC_SINGLE_ENDED_CHANNEL, prevGainCorrection);
    return status;
}


#endif // if defined(CY_IP_MXS40PPSS)

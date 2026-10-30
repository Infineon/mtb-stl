/*******************************************************************************
* File Name: SelfTest_ADC_Config.c
*
* Description:
*  This file provides the source code for the ADC Configuration Protection
*  self tests.
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
#include "SelfTest_ADC_Config.h"

#if defined(CY_IP_MXS40EPASS_ESAR)

#include <string.h>
#include <stddef.h>

/*******************************************************************************
* Function Name: SelfTest_ADC_Config_Save
*******************************************************************************/
uint8_t SelfTest_ADC_Config_Save(PASS_SAR_Type* base,
                                 const stl_adc_config_mask_t* userMask,
                                 stl_adc_config_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((NULL != base) && (NULL != userMask) && (NULL != snapshot))
    {
        uint32_t chanNum = CY_SAR2_CHAN_NUM(base);
        uint32_t i;

        (void)memset(snapshot, 0, sizeof(*snapshot));

        /* Per-ADC instance registers - driver-known-fixed bits are masked inline */
        snapshot->ctl        = base->CTL & ~PASS_SAR_CTL_ENABLED_Msk;
        snapshot->diagCtl    = base->DIAG_CTL & ~PASS_SAR_DIAG_CTL_DIAG_EN_Msk;
        snapshot->precondCtl = base->PRECOND_CTL;
        snapshot->anaCal     = userMask->skipAnaCal ? 0UL : base->ANA_CAL;
        snapshot->digCal     = userMask->skipDigCal ? 0UL : base->DIG_CAL;
        snapshot->anaCalAlt  = userMask->skipAnaCalAlt ? 0UL : base->ANA_CAL_ALT;
        snapshot->digCalAlt  = userMask->skipDigCalAlt ? 0UL : base->DIG_CAL_ALT;

        /* Per-channel registers; channels covered by channelSkipMask stay zero-filled */
        for (i = 0u; i < chanNum; i++)
        {
            if (0UL == (userMask->channelSkipMask & (1UL << i)))
            {
                snapshot->chTrCtl[i]     = base->CH[i].TR_CTL     & ~userMask->chTrCtlMask[i];
                snapshot->chSampleCtl[i] = base->CH[i].SAMPLE_CTL & ~userMask->chSampleCtlMask[i];
                snapshot->chPostCtl[i]   = base->CH[i].POST_CTL   & ~userMask->chPostCtlMask[i];
                snapshot->chRangeCtl[i]  = userMask->chSkipRangeCtl[i] ? 0UL : base->CH[i].RANGE_CTL;
                snapshot->chIntrMask[i]  = base->CH[i].INTR_MASK  & ~userMask->chIntrMaskMask[i];
                snapshot->chEnable[i]    = base->CH[i].ENABLE     & ~PASS_SAR_CH_ENABLE_CHAN_EN_Msk;
            }
        }

        snapshot->userMask = *userMask;

        snapshot->crc = SelfTests_CRC32((uint32_t)snapshot,
                                        (uint32_t)offsetof(stl_adc_config_snapshot_t, crc));

        ret = OK_STATUS;
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_ADC_MMIO_Config_Save
*******************************************************************************/
uint8_t SelfTest_ADC_MMIO_Config_Save(PASS_EPASS_MMIO_Type* mmioBase,
                                      const stl_adc_mmio_config_mask_t* userMask,
                                      stl_adc_mmio_config_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((NULL != mmioBase) && (NULL != userMask) && (NULL != snapshot))
    {
        uint32_t i;

        (void)memset(snapshot, 0, sizeof(*snapshot));

        snapshot->passCtl = mmioBase->PASS_CTL & ~PASS_EPASS_MMIO_PASS_CTL_DBG_FREEZE_EN_Msk &
                            ~userMask->passCtlMask;

        for (i = 0u; i < STL_ADC_CONFIG_TR_SEL_NUM; i++)
        {
            snapshot->sarTrInSel[i]  = mmioBase->SAR_TR_IN_SEL[i]  & ~userMask->sarTrInSelMask[i];
            snapshot->sarTrOutSel[i] = mmioBase->SAR_TR_OUT_SEL[i] & ~userMask->sarTrOutSelMask[i];
        }

        snapshot->userMask = *userMask;

        snapshot->crc = SelfTests_CRC32((uint32_t)snapshot,
                                        (uint32_t)offsetof(stl_adc_mmio_config_snapshot_t, crc));

        ret = OK_STATUS;
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_ADC_Config_Verify
*******************************************************************************/
uint8_t SelfTest_ADC_Config_Verify(PASS_SAR_Type* base,
                                   const stl_adc_config_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((NULL != base) && (NULL != snapshot))
    {
        uint32_t crc = SelfTests_CRC32((uint32_t)snapshot,
                                       (uint32_t)offsetof(stl_adc_config_snapshot_t, crc));

        if (crc != snapshot->crc)
        {
            /* The snapshot cannot be trusted */
            ret = ERROR_ADC_CONFIG_SNAPSHOT_INVALID;
        }
        else
        {
            const stl_adc_config_mask_t* mask = &snapshot->userMask;
            uint32_t chanNum = CY_SAR2_CHAN_NUM(base);
            uint32_t i;
            bool mismatch = (((base->CTL & ~PASS_SAR_CTL_ENABLED_Msk) != snapshot->ctl) ||
                             ((base->DIAG_CTL & ~PASS_SAR_DIAG_CTL_DIAG_EN_Msk) != snapshot->diagCtl) ||
                             (base->PRECOND_CTL != snapshot->precondCtl) ||
                             (!mask->skipAnaCal && (base->ANA_CAL != snapshot->anaCal)) ||
                             (!mask->skipDigCal && (base->DIG_CAL != snapshot->digCal)) ||
                             (!mask->skipAnaCalAlt && (base->ANA_CAL_ALT != snapshot->anaCalAlt)) ||
                             (!mask->skipDigCalAlt && (base->DIG_CAL_ALT != snapshot->digCalAlt)));

            if (!mismatch)
            {
                for (i = 0u; i < chanNum; i++)
                {
                    if (0UL == (mask->channelSkipMask & (1UL << i)))
                    {
                        if (((base->CH[i].TR_CTL & ~mask->chTrCtlMask[i]) != snapshot->chTrCtl[i]) ||
                            ((base->CH[i].SAMPLE_CTL & ~mask->chSampleCtlMask[i]) != snapshot->chSampleCtl[i]) ||
                            ((base->CH[i].POST_CTL & ~mask->chPostCtlMask[i]) != snapshot->chPostCtl[i]) ||
                            ((base->CH[i].INTR_MASK & ~mask->chIntrMaskMask[i]) != snapshot->chIntrMask[i]) ||
                            ((base->CH[i].ENABLE & ~PASS_SAR_CH_ENABLE_CHAN_EN_Msk) != snapshot->chEnable[i]) ||
                            (!mask->chSkipRangeCtl[i] && (base->CH[i].RANGE_CTL != snapshot->chRangeCtl[i])))
                        {
                            mismatch = true;
                            break;
                        }
                    }
                }
            }

            ret = mismatch ? ERROR_STATUS : OK_STATUS;
        }
    }

    return ret;
}


/*******************************************************************************
* Function Name: SelfTest_ADC_MMIO_Config_Verify
*******************************************************************************/
uint8_t SelfTest_ADC_MMIO_Config_Verify(PASS_EPASS_MMIO_Type* mmioBase,
                                        const stl_adc_mmio_config_snapshot_t* snapshot)
{
    uint8_t ret = ERROR_BAD_PARAM;

    if ((NULL != mmioBase) && (NULL != snapshot))
    {
        uint32_t crc = SelfTests_CRC32((uint32_t)snapshot,
                                       (uint32_t)offsetof(stl_adc_mmio_config_snapshot_t, crc));

        if (crc != snapshot->crc)
        {
            /* The snapshot cannot be trusted */
            ret = ERROR_ADC_CONFIG_SNAPSHOT_INVALID;
        }
        else
        {
            const stl_adc_mmio_config_mask_t* mask = &snapshot->userMask;
            uint32_t i;
            bool mismatch = ((mmioBase->PASS_CTL & ~PASS_EPASS_MMIO_PASS_CTL_DBG_FREEZE_EN_Msk & ~mask->passCtlMask) !=
                             snapshot->passCtl);

            if (!mismatch)
            {
                for (i = 0u; i < STL_ADC_CONFIG_TR_SEL_NUM; i++)
                {
                    if (((mmioBase->SAR_TR_IN_SEL[i] & ~mask->sarTrInSelMask[i]) != snapshot->sarTrInSel[i]) ||
                        ((mmioBase->SAR_TR_OUT_SEL[i] & ~mask->sarTrOutSelMask[i]) != snapshot->sarTrOutSel[i]))
                    {
                        mismatch = true;
                        break;
                    }
                }
            }

            ret = mismatch ? ERROR_STATUS : OK_STATUS;
        }
    }

    return ret;
}


#endif /* defined(CY_IP_MXS40EPASS_ESAR) */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_HWFILT3P3Z.c
*
*
* Description:
*  This file provides the source code for the HWFILT3P3Z (Hardware
*  3-Pole/3-Zero Filter) self tests.
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
#include "SelfTest_HWFILT3P3Z.h"
#include "SelfTest_ErrorInjection.h"


#if defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)

#define HWFILT3P3Z_HPPASS_BUSY_TIMEOUT      (1000U)

/* Samples fed to the filter before the measurement window (allows the
 * IIR state to reach steady-state at the new input frequency). */
#define HWFILT3P3Z_HPPASS_SETTLE_SAMPLES    (50U)

/* Samples over which the peak |DATA_OUT| value is measured.
 * Low-frequency (period = 40 samples): 40 samples = 1 full period, needed
 * to capture the peak of the in-band waveform.
 * High-frequency (period = 5 samples): 10 samples = 2 full periods; the
 * attenuated high-freq output reaches its (tiny) peak within 2 samples,
 * so 10 provides ample margin while cutting the measurement loop by 75 %. */
#define HWFILT3P3Z_HPPASS_LOW_MEAS_SAMPLES   (40U)
#define HWFILT3P3Z_HPPASS_HIGH_MEAS_SAMPLES  (10U)

/* Quarter-period length of the low-frequency sine table (numSamples/4 = 40/4 = 10).
 * The full 40-sample period is reconstructed on-the-fly using sine symmetry. */
#define HWFILT3P3Z_HPPASS_LOW_QUARTER_LEN   (10U)

/* Fundamental-period length of the high-frequency sine table (period = 5 samples).
 * The waveform is reconstructed by periodic wrap: sample[n] = table[n % 5]. */
#define HWFILT3P3Z_HPPASS_HIGH_PERIOD_LEN   (5U)

/* Minimum acceptable |DATA_OUT| peak in the in-band sub-test.
 * Guards against a completely broken filter that outputs zero even for
 * an in-band signal. */
#define HWFILT3P3Z_HPPASS_INBAND_MIN        (4U)


/* The dataOutOffset is set to zero so that the
 * signed output can be measured symmetrically around zero;
 *
 * The IIR has poles at magnitude ~0.940 with angle ~0.050 rad/sample,
 * giving very high gain at low frequencies and heavy attenuation above
 * omega ~0.5 rad/sample. */
static const cy_stc_hppass_hwfilt3p3z_config_t stl_lpfCfg =
{
    .srcSel        = CY_AHB_IF,
    .enTrig0       = true,
    .enTrig1       = false,
    .cx0           = 0x000003CDU,
    .cx1           = 0x0000079AU,
    .cx2           = 0x000003CDU,
    .cx3           = 0x00000000U,
    .cy1           = 0x007833D6U,
    .cy2           = 0xFFC78F61U,
    .cy3           = 0x00000000U,
    .dataOutOffset = 0x00000000U,   /* zero offset - measure signed output directly */
    .limMax        = 0x007FFFFFU,   /* full Q0.23 range, no saturation */
    .limMin        = 0xFF800000U,
    .scaleCX       = (uint8_t)CY_SCALE_FACTOR_0,
    .scaleCY       = (uint8_t)CY_SCALE_FACTOR_1,
    .gIn           = 0U,
    .gOut          = 1U,
    .awMax         = 0x007FFFFFU,
    .awMin         = 0xFF800000U,
    .awGain        = 0x00000000U,
    .channelSel    = 0U,            /* no SAR channel - AHB mode */
};


/* Low-frequency quarter-period table: HWFILT3P3Z_HPPASS_LOW_QUARTER_LEN = 10 entries.
 * Stores the first quarter of sin(2*pi*n/40)*2048 for n = 0..9 (ascending from 0
 * to near-peak).  The full 40-sample period is reconstructed on-the-fly using sine
 * symmetry inside stl_sineSample(). */
static const int16_t stl_inBandSine[HWFILT3P3Z_HPPASS_LOW_QUARTER_LEN] =
{
    0,    320,  633,  930,  1204,
    1448, 1657, 1825, 1948, 2023
};


/* High-frequency tone: one fundamental period of sin(2*pi*n/5)*2048 for n = 0..4.
 * HWFILT3P3Z_HPPASS_HIGH_PERIOD_LEN = 5 entries.
 * The full waveform is reconstructed by periodic wrap: sample[n] = table[n % 5]. */
static const int16_t stl_outOfBandSine[HWFILT3P3Z_HPPASS_HIGH_PERIOD_LEN] =
{
    0, 1948, 1204, -1204, -1948
};


/*******************************************************************************
* Function Name: stl_sineSample (internal helper)
*
* Summary:
*  Returns the n-th sample of a sine waveform reconstructed on-the-fly from a
*  compact table, avoiding storage of the full period.
*
* Parameters:
*  q          - Pointer to the reduced sine table.
*  numEntries - Number of entries in q[].
*               Equals numSamples/4 in quarter-period mode,
*               or numSamples in periodic mode.
*  numSamples - Full reconstruction period
*               (total number of samples per waveform cycle).
*               When numSamples == numEntries the function operates in periodic
*               mode (direct lookup).
*               When numSamples == 4*numEntries it operates in quarter-period
*               mode (sine-symmetry reconstruction: mirror + negate).
*  n          - Sample index in [0 .. numSamples-1].
*
* Return:
*  int16_t - The reconstructed sine sample at position n.
*******************************************************************************/
static inline int16_t stl_sineSample(const int16_t* q, uint32_t numEntries,
                                     uint32_t numSamples, uint32_t n)
{
    if (numSamples == numEntries)
    {
        return q[n];
    }

    /* Quarter-period reconstruction (numSamples == 4*numEntries). */
    uint32_t pos = n;
    bool neg = (pos >= (2U * numEntries));
    if (neg)
    {
        pos -= (2U * numEntries);
    }

    uint32_t idx;
    if (pos < numEntries)
    {
        idx = pos;
    }
    else if (pos == numEntries)
    {
        idx = numEntries - 1U;        /* approximate peak as last quarter entry */
    }
    else
    {
        idx = (2U * numEntries) - pos; /* mirror descent: idx in [1..numEntries-1] */
    }
    return neg ? (int16_t)(-(int32_t)q[idx]) : q[idx];
}


/*******************************************************************************
* Function Name: stl_subTest (internal helper)
*
* Summary:
*  Drives a HPPASS HWFILT3P3Z filter instance with a tone waveform for
*  settleLen + measLen samples, using a running wrap counter to avoid
*  per-sample division.  The settle phase discards output; the measure phase
*  records the peak absolute DATA_OUT value.
*
* Parameters:
*  filtIdx    - Filter instance index (0..CY_HPPASS_HWFILT3P3Z_NUM_INSTANCE-1).
*  q          - Pointer to the sine table (numEntries entries).
*  numEntries - Number of entries in q[].  See stl_sineSample().
*  numSamples - Full reconstruction period.
*               numSamples == numEntries selects periodic mode;
*               numSamples == 4*numEntries selects quarter-period mode.
*  settleLen  - Number of samples to feed before the measurement window.
*  measLen    - Number of samples over which to record the peak |DATA_OUT|.
*               Should cover at least one full waveform period to guarantee
*               the peak is observed (e.g. 40 for low-freq, 10 for high-freq).
*
* Return:
*  uint32_t  - Peak absolute DATA_OUT value over the measLen window.
*  UINT32_MAX  if FILTER_BUSY did not clear within HWFILT3P3Z_HPPASS_BUSY_TIMEOUT
*              iterations on any sample (indicates a hardware fault).
*******************************************************************************/
static uint32_t stl_subTest(uint8_t filtIdx,
                            const int16_t* q,
                            uint32_t numEntries,
                            uint32_t numSamples,
                            uint32_t settleLen,
                            uint32_t measLen)
{
    uint32_t waveIdx = 0U;
    uint32_t n;
    uint32_t poll;

    /* ---- Settle phase: drive IIR to steady state, discard output ---- */
    for (n = 0U; n < settleLen; n++)
    {
        poll = HWFILT3P3Z_HPPASS_BUSY_TIMEOUT;
        Cy_HPPASS_HWFILT3P3Z_Write_DATA_IN0(
            filtIdx, (int32_t)stl_sineSample(q, numEntries, numSamples, waveIdx));
        while ((CY_FILTER_IS_BUSY == Cy_HPPASS_HWFILT3P3Z_ReadFilterStatus(filtIdx)) &&
               (poll > 0U))
        {
            poll--;
        }
        if (0U == poll)
        {
            return UINT32_MAX;
        }
        if (++waveIdx == numSamples)
        {
            waveIdx = 0U;
        }
    }

    /* ---- Measure phase: record peak |DATA_OUT| ---- */
    uint32_t peakAbs = 0U;
    for (n = 0U; n < measLen; n++)
    {
        poll = HWFILT3P3Z_HPPASS_BUSY_TIMEOUT;
        Cy_HPPASS_HWFILT3P3Z_Write_DATA_IN0(
            filtIdx, (int32_t)stl_sineSample(q, numEntries, numSamples, waveIdx));
        while ((CY_FILTER_IS_BUSY == Cy_HPPASS_HWFILT3P3Z_ReadFilterStatus(filtIdx)) &&
               (poll > 0U))
        {
            poll--;
        }
        if (0U == poll)
        {
            return UINT32_MAX;
        }
        int32_t  out    = Cy_HPPASS_HWFILT3P3Z_ReadFilterDataOutput(filtIdx);
        uint32_t absOut = (out >= 0) ? (uint32_t)out : (uint32_t)(-(int32_t)out);
        if (absOut > peakAbs)
        {
            peakAbs = absOut;
        }
        if (++waveIdx == numSamples)
        {
            waveIdx = 0U;
        }
    }

    return peakAbs;
}


/*******************************************************************************
* Function Name: SelfTest_HWFILT3P3Z_HPPASS
*******************************************************************************/
uint8_t SelfTest_HWFILT3P3Z_HPPASS(uint8_t filtIdx)
{
    if (filtIdx >= CY_HPPASS_HWFILT3P3Z_NUM_INSTANCE)
    {
        return ERROR_BAD_PARAM;
    }

    /* ---- Save current TOP and per-filter state ---- */
    bool savedTopEn  = ((HPPASS_HWFILT3P3Z_TOP_CTRL(HPPASS_BASE) &
                         HPPASS_MX3P3ZFILT_HWFILT3P3Z_TOP_CTRL_HWFILT3P3Z_EN_Msk) != 0U);
    bool savedFiltEn = ((HPPASS_HWFILT3P3Z_CTRL(HPPASS_BASE, filtIdx) &
                         HPPASS_MX3P3ZFILT_HWFILT3P3Z_CTRL_FILTER_EN_Msk) != 0U);

    /* Read all configuration registers back into a saved config struct so
     * that Cy_HPPASS_HWFILT3P3Z_InitFilterConfig() can restore them exactly. */
    cy_stc_hppass_hwfilt3p3z_config_t savedCfg;
    Cy_HPPASS_HWFILT3P3Z_GetFilterConfig(filtIdx, &savedCfg);

    /* ---- Configure the filter for the frequency-response test ---- */
    Cy_HPPASS_HWFILT3P3Z_TOP_PeripheralEnable();
    Cy_HPPASS_HWFILT3P3Z_FilterEnable(filtIdx, false);
    Cy_HPPASS_HWFILT3P3Z_InitFilterConfig(filtIdx, &stl_lpfCfg);
    Cy_HPPASS_HWFILT3P3Z_FilterEnable(filtIdx, true);

    /* ---- Sub-test 1: out-of-band high-frequency tone (run first so the IIR
     * state is flushed to near-zero before the in-band measurement, giving the
     * low-frequency sub-test a well-conditioned settling baseline) ---- */
    uint32_t highPeak = stl_subTest(
        filtIdx,
        stl_outOfBandSine,
        HWFILT3P3Z_HPPASS_HIGH_PERIOD_LEN,
        HWFILT3P3Z_HPPASS_HIGH_PERIOD_LEN,         /* numSamples == numEntries:
                                                      periodic wrap */
        HWFILT3P3Z_HPPASS_SETTLE_SAMPLES,
        HWFILT3P3Z_HPPASS_HIGH_MEAS_SAMPLES);

    /* ---- Sub-test 2: in-band low-frequency tone ---- */
    uint32_t lowPeak = stl_subTest(
        filtIdx,
        stl_inBandSine,
        HWFILT3P3Z_HPPASS_LOW_QUARTER_LEN,
        4U * HWFILT3P3Z_HPPASS_LOW_QUARTER_LEN,    /* full period = 4 * 10 = 40 */
        HWFILT3P3Z_HPPASS_SETTLE_SAMPLES,
        HWFILT3P3Z_HPPASS_LOW_MEAS_SAMPLES);

    /* ---- Evaluate frequency response ---- */
    uint8_t status;
    if ((UINT32_MAX == lowPeak) || (UINT32_MAX == highPeak))
    {
        /* FILTER_BUSY timeout on at least one sample. */
        status = ERROR_STATUS;
    }
    else if ((lowPeak < HWFILT3P3Z_HPPASS_INBAND_MIN) ||
             ((uint32_t)(HWFILT3P3Z_ATT_RATIO * highPeak) >= lowPeak))
    {
        /* In-band output too small, or insufficient out-of-band attenuation. */
        status = ERROR_STATUS;
    }
    else
    {
        status = OK_STATUS;
    }

    #if (ERROR_IN_HWFILT3P3Z)
    /* Fault injection: flip the status on an otherwise healthy device. */
    status = (OK_STATUS == status) ? ERROR_STATUS : OK_STATUS;
    #endif

    /* ---- Restore the original filter configuration ---- */
    Cy_HPPASS_HWFILT3P3Z_FilterEnable(filtIdx, false);
    Cy_HPPASS_HWFILT3P3Z_InitFilterConfig(filtIdx, &savedCfg);
    Cy_HPPASS_HWFILT3P3Z_FilterEnable(filtIdx, savedFiltEn);
    if (!savedTopEn)
    {
        Cy_HPPASS_HWFILT3P3Z_TOP_PeripheralDisable();
    }

    return status;
}


#endif /* defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u) */

/* [] END OF FILE */

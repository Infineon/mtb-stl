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

#if (defined(CY_IP_MXS40PPSS)) || \
    (defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u))

/*******************************************************************************
* Shared frequency-response test core.
*
* Both the PSOC Control C3 M8 PPCA (CY_IP_MXS40PPSS) and the PSOC Control C3 M6 HPPASS
* (CY_IP_MXS40MCPASS) sub-systems embed the same HW3P3Z (3-pole / 3-zero) IIR
* filter. The self-test configures it as a 2nd-order low-pass filter and
* verifies its magnitude response by driving two tones through the AHB
* interface: an in-band low-frequency tone that the LPF passes and an
* out-of-band high-frequency tone that it heavily attenuates. The ratio of the
* measured output peaks confirms that the multiplier / accumulator datapath and
* the IIR feedback operate correctly - coverage a static offset check cannot
* provide. Only one IP is compiled in a given build; the algorithm, stimulus
* tables and pass/fail criteria below are shared, while the register access is
* provided by a thin per-IP accessor layer.
*******************************************************************************/

/* Filter coefficients (24-bit signed Q0.23) forming a 2nd-order IIR low-pass
 * filter: a numerator with a double zero at z = -1 (cx1 = 2*cx0) and a
 * denominator with a conjugate pole pair of magnitude ~0.94 close to DC
 * (cy1, cy2 stored sign-swapped, compensated by scaleCY = 1). See
 * \ref group_hwfilt3p3z_hppass_theory for the full transfer-function analysis. */
#define HWFILT3P3Z_CX0                  (0x000003CDU)
#define HWFILT3P3Z_CX1                  (0x0000079AU)
#define HWFILT3P3Z_CX2                  (0x000003CDU)
#define HWFILT3P3Z_CX3                  (0x00000000U)
#define HWFILT3P3Z_CY1                  (0x007833D6U)
#define HWFILT3P3Z_CY2                  (0xFFC78F61U)
#define HWFILT3P3Z_CY3                  (0x00000000U)

/* Output saturation limits opened to the full Q0.23 signed range. */
#define HWFILT3P3Z_LIM_MAX              (0x007FFFFFU)
#define HWFILT3P3Z_LIM_MIN              (0xFF800000U)

/* 24-bit DATA_OUT payload mask and sign bit. The PDL read does not guarantee
 * sign-extension of the reserved upper byte, so the output magnitude is
 * derived from the masked value explicitly (see stl_absDataOut). */
#define HWFILT3P3Z_OUT_MASK             (0x00FFFFFFU)
#define HWFILT3P3Z_OUT_SIGN_BIT         (0x00800000U)

/* FILTER_BUSY poll bound. The computation completes in ~10 CPU cycles;
 * 1000 register reads is a defensive upper bound against a stuck status bit. */
#define HWFILT3P3Z_BUSY_TIMEOUT         (1000U)

/* Samples fed before the measurement window so the IIR state reaches steady
 * state at the new input frequency. */
#define HWFILT3P3Z_SETTLE_SAMPLES       (50U)

/* Measurement-window lengths. Each covers at least one full waveform period so
 * the peak is observed. The low tone period is 40 samples; the attenuated high
 * tone output peaks within a few samples, so 10 provides ample margin. */
#define HWFILT3P3Z_LOW_MEAS_SAMPLES     (40U)
#define HWFILT3P3Z_HIGH_MEAS_SAMPLES    (10U)

/* Low-frequency tone: 10-entry quarter-period table, full 40-sample period
 * reconstructed via sine symmetry. High-frequency tone: 5-entry full period,
 * reconstructed by periodic wrap (sample[n] = table[n % 5]). */
#define HWFILT3P3Z_LOW_QUARTER_LEN      (10U)
#define HWFILT3P3Z_HIGH_PERIOD_LEN      (5U)

/* Minimum acceptable in-band |DATA_OUT| peak; guards against a filter that
 * outputs zero even for an in-band signal. */
#define HWFILT3P3Z_INBAND_MIN           (4U)


/* Low-frequency quarter-period table: sin(2*pi*n/40)*2048 for n = 0..9. */
static const int16_t stl_inBandSine[HWFILT3P3Z_LOW_QUARTER_LEN] =
{
    0,    320,  633,  930,  1204,
    1448, 1657, 1825, 1948, 2023
};

/* High-frequency tone: one fundamental period of sin(2*pi*n/5)*2048, n = 0..4. */
static const int16_t stl_outOfBandSine[HWFILT3P3Z_HIGH_PERIOD_LEN] =
{
    0, 1948, 1204, -1204, -1948
};


/* Per-IP filter-instance handle and the primitive AHB accessors that the
 * shared measurement loop needs. Only the branch for the compiled IP is used. */
#if defined(CY_IP_MXS40PPSS)

typedef PPCA_HWFILT3P3Z_SS_HWFILT3P3Z_Type* stl_hwfilt_handle_t;

static inline void stl_writeDataIn0(stl_hwfilt_handle_t h, int32_t v)
{
    Cy_PPCA_HWFILT3P3Z_Write_DATA_IN0(h, v);
}


static inline bool stl_filterBusy(stl_hwfilt_handle_t h)
{
    return (CY_FILTER_IS_BUSY == Cy_PPCA_HWFILT3P3Z_ReadFilterStatus(h));
}


static inline int32_t stl_readDataOut(stl_hwfilt_handle_t h)
{
    return Cy_PPCA_HWFILT3P3Z_ReadFilterDataOutput(h);
}


#else /* CY_IP_MXS40MCPASS */

typedef uint8_t stl_hwfilt_handle_t;

static inline void stl_writeDataIn0(stl_hwfilt_handle_t h, int32_t v)
{
    Cy_HPPASS_HWFILT3P3Z_Write_DATA_IN0(h, v);
}


static inline bool stl_filterBusy(stl_hwfilt_handle_t h)
{
    return (CY_FILTER_IS_BUSY == Cy_HPPASS_HWFILT3P3Z_ReadFilterStatus(h));
}


static inline int32_t stl_readDataOut(stl_hwfilt_handle_t h)
{
    return Cy_HPPASS_HWFILT3P3Z_ReadFilterDataOutput(h);
}


#endif // if defined(CY_IP_MXS40PPSS)


/*******************************************************************************
* Function Name: stl_absDataOut (internal helper)
*
* Summary:
*  Returns the magnitude of a 24-bit signed DATA_OUT read. The 24-bit payload
*  is masked out and, if its sign bit is set, its two's-complement magnitude is
*  computed with unsigned arithmetic. This does not rely on the PDL read
*  sign-extending the reserved upper byte.
*******************************************************************************/
static uint32_t stl_absDataOut(int32_t raw)
{
    uint32_t u = (uint32_t)raw & HWFILT3P3Z_OUT_MASK;
    uint32_t mag;

    if (0U != (u & HWFILT3P3Z_OUT_SIGN_BIT))
    {
        mag = (HWFILT3P3Z_OUT_MASK + 1U) - u;   /* |negative 24-bit value| */
    }
    else
    {
        mag = u;
    }

    return mag;
}


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
*  Drives a HW3P3Z filter instance with a tone waveform for settleLen + measLen
*  samples, using a running wrap counter to avoid per-sample division. The
*  settle phase discards output; the measure phase records the peak absolute
*  DATA_OUT value.
*
* Parameters:
*  h          - Filter-instance handle (see stl_hwfilt_handle_t).
*  q          - Pointer to the sine table (numEntries entries).
*  numEntries - Number of entries in q[].  See stl_sineSample().
*  numSamples - Full reconstruction period.
*               numSamples == numEntries selects periodic mode;
*               numSamples == 4*numEntries selects quarter-period mode.
*  settleLen  - Number of samples to feed before the measurement window.
*  measLen    - Number of samples over which to record the peak |DATA_OUT|.
*
* Return:
*  uint32_t  - Peak absolute DATA_OUT value over the measLen window.
*  UINT32_MAX  if FILTER_BUSY did not clear within HWFILT3P3Z_BUSY_TIMEOUT
*              iterations on any sample (indicates a hardware fault).
*******************************************************************************/
static uint32_t stl_subTest(stl_hwfilt_handle_t h,
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
        poll = HWFILT3P3Z_BUSY_TIMEOUT;
        stl_writeDataIn0(h, (int32_t)stl_sineSample(q, numEntries, numSamples, waveIdx));
        while (stl_filterBusy(h) && (poll > 0U))
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
        poll = HWFILT3P3Z_BUSY_TIMEOUT;
        stl_writeDataIn0(h, (int32_t)stl_sineSample(q, numEntries, numSamples, waveIdx));
        while (stl_filterBusy(h) && (poll > 0U))
        {
            poll--;
        }
        if (0U == poll)
        {
            return UINT32_MAX;
        }
        uint32_t absOut = stl_absDataOut(stl_readDataOut(h));
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
* Function Name: stl_evalResponse (internal helper)
*
* Summary:
*  Evaluates the two measured output peaks against the LPF pass / attenuation
*  criteria: the in-band peak must be at least HWFILT3P3Z_INBAND_MIN and must
*  exceed HWFILT3P3Z_ATT_RATIO times the out-of-band peak.
*******************************************************************************/
static uint8_t stl_evalResponse(uint32_t lowPeak, uint32_t highPeak)
{
    uint8_t status;

    if ((UINT32_MAX == lowPeak) || (UINT32_MAX == highPeak))
    {
        /* FILTER_BUSY timeout on at least one sample. */
        status = ERROR_STATUS;
    }
    else if ((lowPeak < HWFILT3P3Z_INBAND_MIN) ||
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

    return status;
}


/*******************************************************************************
* Function Name: stl_runResponseTest (internal helper)
*
* Summary:
*  Runs the out-of-band (high-frequency) then in-band (low-frequency) sub-tests
*  on an already-configured filter instance and returns the evaluated result.
*  The high-frequency tone is run first so the IIR state is flushed to near
*  zero before the in-band measurement.
*******************************************************************************/
static uint8_t stl_runResponseTest(stl_hwfilt_handle_t h)
{
    uint32_t highPeak = stl_subTest(h, stl_outOfBandSine,
                                    HWFILT3P3Z_HIGH_PERIOD_LEN,
                                    HWFILT3P3Z_HIGH_PERIOD_LEN,
                                    HWFILT3P3Z_SETTLE_SAMPLES,
                                    HWFILT3P3Z_HIGH_MEAS_SAMPLES);

    uint32_t lowPeak = stl_subTest(h, stl_inBandSine,
                                   HWFILT3P3Z_LOW_QUARTER_LEN,
                                   4U * HWFILT3P3Z_LOW_QUARTER_LEN,
                                   HWFILT3P3Z_SETTLE_SAMPLES,
                                   HWFILT3P3Z_LOW_MEAS_SAMPLES);

    return stl_evalResponse(lowPeak, highPeak);
}


#endif /* shared frequency-response core */


#if defined(CY_IP_MXS40PPSS)


/*******************************************************************************
* Function Name: SelfTest_HWFILT3P3Z
*******************************************************************************/
uint8_t SelfTest_HWFILT3P3Z(PPCA_HWFILT3P3Z_SS_Type* base, uint8_t channel)
{
    if (NULL == base)
    {
        return ERROR_BAD_PARAM;
    }

    /* Validate channel against the subsystem's HW-defined channel count.
     * Compare base addresses through uintptr_t because the two SS instances
     * have distinct PDL struct types. */
    uint8_t maxChannel;
    const uintptr_t baseAddr = (uintptr_t)base;
    if (baseAddr == (uintptr_t)PPCA_HWFILT3P3Z_SS_0)
    {
        maxChannel = HWFILT3P3Z_SS0_MAX_CHANNEL_INDEX;
    }
    else if (baseAddr == (uintptr_t)PPCA_HWFILT3P3Z_SS_1)
    {
        maxChannel = HWFILT3P3Z_SS1_MAX_CHANNEL_INDEX;
    }
    else
    {
        return ERROR_BAD_PARAM;
    }

    if (channel > maxChannel)
    {
        return ERROR_BAD_PARAM;
    }

    PPCA_HWFILT3P3Z_SS_HWFILT3P3Z_Type* ch =
        (PPCA_HWFILT3P3Z_SS_HWFILT3P3Z_Type*)&base->HWFILT3P3Z[channel];

    /* Save original SS-level and per-channel state via PDL getter APIs
     * for non-destructive restore on return. */
    bool savedSsEnable     = Cy_PPCA_HWFILT3P3Z_SS_GetPeripheralEnable(base);
    bool savedFilterEnable = Cy_PPCA_HWFILT3P3Z_GetFilterEnable(ch);
    cy_stc_ppca_hwfilt3p3z_config_t savedCfg;
    Cy_PPCA_HWFILT3P3Z_GetFilterConfig(ch, &savedCfg);

    /* LPF test configuration (shared filter coefficients; the PPCA config has
     * no anti-windup / channel-select fields). */
    static const cy_stc_ppca_hwfilt3p3z_config_t testCfg =
    {
        .srcSel        = CY_AHB_IF,
        .enTrig0       = true,
        .enTrig1       = false,
        .cx0           = HWFILT3P3Z_CX0,
        .cx1           = HWFILT3P3Z_CX1,
        .cx2           = HWFILT3P3Z_CX2,
        .cx3           = HWFILT3P3Z_CX3,
        .cy1           = HWFILT3P3Z_CY1,
        .cy2           = HWFILT3P3Z_CY2,
        .cy3           = HWFILT3P3Z_CY3,
        .dataOutOffset = 0U,
        .limMax        = HWFILT3P3Z_LIM_MAX,
        .limMin        = HWFILT3P3Z_LIM_MIN,
        .scaleCX       = (uint8_t)CY_SCALE_FACTOR_0,
        .scaleCY       = (uint8_t)CY_SCALE_FACTOR_1,
        .gIn           = 0U,
        .gOut          = 1U,
    };

    /* Stop any in-progress computation and program the test configuration. */
    Cy_PPCA_HWFILT3P3Z_SS_PeripheralEnable(base);
    Cy_PPCA_HWFILT3P3Z_FilterEnable(ch, false);
    Cy_PPCA_HWFILT3P3Z_InitFilterConfig(ch, &testCfg);
    Cy_PPCA_HWFILT3P3Z_FilterEnable(ch, true);

    /* Drive the LPF frequency-response test on this channel. */
    uint8_t status = stl_runResponseTest(ch);

    /* Restore. Disable the filter first to avoid spurious computation while
     * the configuration registers are being written back. */
    Cy_PPCA_HWFILT3P3Z_FilterEnable(ch, false);
    Cy_PPCA_HWFILT3P3Z_InitFilterConfig(ch, &savedCfg);
    Cy_PPCA_HWFILT3P3Z_FilterEnable(ch, savedFilterEnable);
    if (!savedSsEnable)
    {
        Cy_PPCA_HWFILT3P3Z_SS_PeripheralDisable(base);
    }

    return status;
}


#endif /* defined(CY_IP_MXS40PPSS) */


#if defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)

/* LPF test configuration for the PSOC Control C3 M6 HPPASS HW3P3Z. Shares the filter
 * coefficients with the PSOC Control C3 M8 variant and adds the anti-windup and SAR
 * channel-select fields that only the HPPASS config exposes. */
static const cy_stc_hppass_hwfilt3p3z_config_t stl_lpfCfg =
{
    .srcSel        = CY_AHB_IF,
    .enTrig0       = true,
    .enTrig1       = false,
    .cx0           = HWFILT3P3Z_CX0,
    .cx1           = HWFILT3P3Z_CX1,
    .cx2           = HWFILT3P3Z_CX2,
    .cx3           = HWFILT3P3Z_CX3,
    .cy1           = HWFILT3P3Z_CY1,
    .cy2           = HWFILT3P3Z_CY2,
    .cy3           = HWFILT3P3Z_CY3,
    .dataOutOffset = 0U,
    .limMax        = HWFILT3P3Z_LIM_MAX,
    .limMin        = HWFILT3P3Z_LIM_MIN,
    .scaleCX       = (uint8_t)CY_SCALE_FACTOR_0,
    .scaleCY       = (uint8_t)CY_SCALE_FACTOR_1,
    .gIn           = 0U,
    .gOut          = 1U,
    .awMax         = HWFILT3P3Z_LIM_MAX,
    .awMin         = HWFILT3P3Z_LIM_MIN,
    .awGain        = 0x00000000U,
    .channelSel    = 0U,
};


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

    /* Drive the LPF frequency-response test on this filter instance. */
    uint8_t status = stl_runResponseTest(filtIdx);

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

/*******************************************************************************
* File Name: SelfTest_HWFILT3P3Z.h
*
* Description:
*  This file provides the constants and parameter values for the HWFILT3P3Z
*  (Hardware 3-Pole/3-Zero Filter) self tests.
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
 * \defgroup group_hwfilt3p3z HWFILT3P3Z (HW3P3Z filter STL module)
 * \{
 *
 * The self-test exercises the HPPASS 3P3Z filter signal path
 * by applying in-band and out-of-band tones and verifying the resulting LPF
 * magnitude response without requiring external analog stimulus routing.
 *
 * \section group_hwfilt3p3z_hppass_theory Filter Theory
 *
 * The the self-test exercises the full IIR signal path by
 * applying two frequency-selective stimulus tones and verifying the LPF
 * magnitude response.  The filter under test is a 2nd-order IIR low-pass
 * filter (3P3Z topology, 3rd pole and zero at the origin, cx3 = cy3 = 0)
 * with a double zero at z = -1 (Nyquist) and a conjugate pole pair close to DC.
 *
 * \par Filter Transfer Function
 * The Z-domain transfer function is:
 * \code
 *          K * (1 + z^-1)^2
 *  H(z) =  -----------------------
 *          1 - b1*z^-1 + b2*z^-^2
 * \endcode
 * Coefficient mapping (scaleCY = CY_SCALE_FACTOR_1, gOut = 1):
 * \code
 *   cx0 = cx2 = 0x3CD  (~= 9.78 * 10^-4 in Q0.23 after scaleCX=0 shift)
 *   cx1        = 0x79A  (= 2 * cx0, double-zero structure)
 *   cy1        = 0x007833D6  (stored value; actual b1 ~= +1.87816 after scaleCY=1 shift)
 *   cy2        = 0xFFC78F61  (stored value; actual b2 ~= +0.88187 after scaleCY=1 shift)
 * \endcode
 * Pole locations (magnitude r, angle theta):
 * \code
 *   r ~= 0.9403  (sqrt(b2))
 *   theta ~= 0.051 rad/sample  (arccos(b1 / (2r)))
 * \endcode
 *
 * \par Cutoff Frequency
 * The -3 dB normalized frequency computed from H(z) is:
 * \code
 *   wc ~= 0.0404 rad/sample
 *   fc = wc / (2*pi) * fs  ~=  0.00643 * fs
 * \endcode
 * Because the filter is driven by AHB register writes rather than a
 * fixed hardware clock, the real-world cutoff frequency scales linearly
 * with the AHB sample rate (fs).  There is no absolute Hz cutoff baked
 * into the coefficients; all frequency information is normalized to
 * samples.  The test tones are also defined in normalized frequency, so
 * the pass/fail criterion is independent of the actual write rate.
 *
 * \par Test Tone Gains
 * \code
 *   omega_low  = 2*pi/40   ->  gain ~= -17.2 dB (in-band, passes the LPF)
 *   omega_high = 2*pi/5    ->  gain ~= -54.6 dB (out-of-band, heavily attenuated)
 *   attenuation ratio ~= 74:1  (threshold: HWFILT3P3Z_ATT_RATIO = 8:1)
 * \endcode
 *
 * \par Compressed Sine Tables
 * To minimise ROM usage, neither tone is stored as a full 40-sample array:
 * \code
 *   Low-frequency  (period  40): 10-entry quarter-period table.
 *     Reconstruction: sine symmetry -- Q2 mirrors Q1, Q3/Q4 negate Q1/Q2.
 *     Storage: 10 * 2 Bytes = 20 Bytes.
 *
 *   High-frequency (period   5): 5-entry fundamental-period table.
 *     Reconstruction: periodic wrap -- sample[n] = table[n % 5].
 *     Storage:  5 * 2 Bytes = 10 Bytes.
 * \endcode
 *
 * \defgroup group_hwfilt3p3z_macros Macros
 * \defgroup group_hwfilt3p3z_functions Functions
 */

#if !defined(SELFTEST_HWFILT3P3Z_H)
    #define SELFTEST_HWFILT3P3Z_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)) || defined(CY_DOXYGEN)

/**
 * \addtogroup group_hwfilt3p3z_macros
 * \{
 */

/** Maximum HWFILT3P3Z filter instance index for HPPASS (3 instances: 0..2). */
#define HWFILT3P3Z_HPPASS_MAX_IDX   (2U)

/** Required attenuation ratio for the out-of-band sub-test:
 *  low_peak must be strictly greater than HWFILT3P3Z_ATT_RATIO * high_peak.
 *  The theoretical LPF attenuation ratio at the test frequencies is ~74:1,
 *  so a threshold of 8 provides a large safety margin. */
#define HWFILT3P3Z_ATT_RATIO                    (8U)

/** \} group_hwfilt3p3z_macros */

/**
 * \addtogroup group_hwfilt3p3z_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_HWFILT3P3Z_HPPASS
****************************************************************************//**
*
* Performs a self-test of one HWFILT3P3Z filter instance inside the HPPASS
* (High Performance Programmable Analog Sub-System) on devices that provide
* CY_IP_MXS40MCPASS v3.
*
* The test validates the LPF frequency-response of the selected 3P3Z filter
* instance using two sub-tests driven entirely through the AHB interface:
*
*   1. Low-frequency sub-test (omega = 2pi/40 rad/sample, amplitude = 2048).
*      The waveform is stored as a 10-entry quarter-period table and
*      reconstructed on-the-fly using sine symmetry (first quarter ascending,
*      second quarter mirrored descending, second half negated first half).
*      The LPF passes this in-band signal; the measured DATA_OUT peak must
*      be >= HWFILT3P3Z_HPPASS_INBAND_MIN.
*
*   2. High-frequency sub-test (omega = 2pi/5 rad/sample, same amplitude).
*      The waveform is stored as a 5-entry fundamental-period table and
*      reconstructed by periodic wrap (sample[n] = table[n % 5]).
*      The LPF heavily attenuates this out-of-band signal; the measured peak
*      must satisfy HWFILT3P3Z_ATT_RATIO * high_peak < low_peak.
*
* Stages exercised: AHB write path (DATA_IN0 trigger), FILTER_BUSY
* handshake, AHB read of DATA_OUT, forward multipliers (cx0..cx3),
* feedback multipliers (cy1..cy3), 44-bit accumulator, coefficient scale
* factors (scaleCX, scaleCY), output gain (gOut), output limits (limMax,
* limMin) and the frequency-selective magnitude response of the IIR
* section. No DAC, ADC or Autonomous Controller hardware is required.
*
* For filter theory (transfer function, cutoff frequency, test-tone gains
* and compressed sine tables) see \ref group_hwfilt3p3z_hppass_theory.
*
* \param filtIdx
* Filter instance index. Valid range: 0..\ref HWFILT3P3Z_HPPASS_MAX_IDX (0..2).
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Frequency response outside tolerance <br>
*  \ref ERROR_BAD_PARAM (9) - filtIdx out of range <br>
*
*******************************************************************************/
uint8_t SelfTest_HWFILT3P3Z_HPPASS(uint8_t filtIdx);

/** \} group_hwfilt3p3z_functions */

#endif /* (defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)) || defined(CY_DOXYGEN) */

#endif /* !defined(SELFTEST_HWFILT3P3Z_H) */

/** \} group_hwfilt3p3z */
/* [] END OF FILE */

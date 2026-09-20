/*******************************************************************************
* File Name: SelfTest_Analog.h
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the analog component self tests according to Class B library.
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
 * \defgroup group_analog Analog (Analog STL module)
 * \{
 *
 * This module carries out 4 tests: <br>
 * 1) SAR ADC <br>
 * 2) OPAMP <br>
 * 3) LPCOMP <br>
 * 4) DAC
 *
 * \section group_analog_more_information More Information
 *
 * SAR ADC Test:
 * This test is to check the SAR ADC analog functions. Each ADC enabled by
 * the test is connected to an reference voltage using the Programmable
 * Analog block. If the measured value falls within the expected range,
 * the test passes.
 *
 * OPAMP Test:
 * The opamp is tested by generating the same output as the input
 * (Unity Gain Buffer) and the result is read by the SAR ADC.
 * If it is as per expected the test passes.
 *
 * LPCOMP:
 * The inputs to the comparators can be connected to the internal reference voltage,
 * one side of each comparator is connected to Some Voltage and the other side is
 * connected to ground. The results are checked. The inputs are switched and the
 * result is checked again.
 *
 * DAC Test:
 * This test is to check the DAC analog functions. Performs DAC test and
 * verifies that input of DAC and output from ADC are same.
 * If it is as per expected then test passes.
 *
 *
 * \defgroup group_analog_macros Macros
 * \defgroup group_analog_functions Functions
 */

#if !defined(SELFTEST_ANALOG_H)
    #define SELFTEST_ANALOG_H

#include "cy_pdl.h"
#include "cybsp.h"
#include "SelfTest_common.h"

/* Supports three self-test modes: */
/* ANALOG_TEST_VREF_EXTERNAL  - Depends on an external three-series resistor
 * voltage divider from VDDA to GND to supply a reference voltage for the self test.
 *
 * ANALOG_TEST_VREF_DUAL_MSC - Uses the two internal MSCv3 blocks to route VREF(1.2V) and VDDA/2 to
 * the AMUXBUS which the tests can use.
 *
 * ANALOG_TEST_VREF_CSD_IDAC - Uses a internal CSD IDAC to generate a voltage from a external pull
 * down resistor.
 * */

/** \addtogroup group_analog_macros
 * \{
 */
/** The test is done by using Vref from External*/
#define ANALOG_TEST_VREF                       (ANALOG_TEST_VREF_EXTERNAL)
/** Depends on an external three-series resistor voltage divider from VDDA to GND to supply a
 *  reference voltage for the self test. */
#define ANALOG_TEST_VREF_EXTERNAL              (0u)
/** Uses the two internal MSCv3 blocks to route VREF(1.2V) and VDDA/2 to the AMUXBUS, which the tests
 *  can use. Only for PSOC 4 devices. */
#define ANALOG_TEST_VREF_DUAL_MSC              (1u)
/** Uses an internal CSD IDAC to generate a voltage from an external pull-down resistor. Only for
 *  PSOC 4 devices. */
#define ANALOG_TEST_VREF_CSD_IDAC              (2u)


/** \cond INTERNAL */
#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC) && !defined(CY_IP_M0S8MSCV3) && \
    (CY_IP_M0S8MSCV3_INSTANCES < 2u)
#error "Hardware does not support ANALOG_TEST_VREF_DUAL_MSC mode"
#endif

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC) && !defined(CY_IP_M0S8CSDV2)
#error "Hardware does not support ANALOG_TEST_VREF_CSD_IDAC mode"
#endif

#ifdef CY_IP_MXS40PASS_CTDAC
    #define DAC_INPUT 0x7D0U /* Maps to 1.5 V */
    #define DAC_ADC_OUTPUT_DEVIATION_PCT 1U /* Deviation of 1% between DAC input and ADC output */
#endif

/** \endcond */

/***************************************
* Initial Parameter Constants
***************************************/
/** ADC accuracy: 12% */
#define ADC_TEST_ACC                    12

/** OPAMP accuracy - the same as ADC accuracy */
#define OPAMP_TEST_ACCURACY             12
/** \cond INTERNAL */
#define OPAMP_TEST_ACURACCY             OPAMP_TEST_ACCURACY  /* deprecated: use OPAMP_TEST_ACCURACY */
/** \endcond */

/* Definition of the analog components included in class B */
#if defined (CY_IP_MXLPCOMP) || defined (CY_IP_MXS22LPCOMP) || \
    defined (CY_IP_MXS40LPCOMP) || defined (CY_IP_M0S8LPCOMP)
    #define CLASSB_SELF_TEST_COMP           1u
#endif

#if defined(CY_IP_M0S8PASS4A_SAR) || defined(CY_IP_MXS40PASS_SAR) || \
    defined (CY_IP_MXS40EPASS_ESAR) || defined(CY_IP_MXS40MCPASS)
    #define CLASSB_SELF_TEST_ADC            1u
#endif

#if defined(CLASSB_SELF_TEST_ADC) && (defined(CY_IP_MXS40PASS_CTB) || defined(CY_IP_M0S8PASS4A_CTB))
    #define CLASSB_SELF_TEST_OPAMP          1u
#endif

#if defined(CLASSB_SELF_TEST_ADC) && defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)
    #define CLASSB_SELF_TEST_OPAMP          1u
#endif

#if defined(CY_IP_MXS40MCPASS)
    #define CLASSB_SELF_TEST_DAC            1u
#endif


/** \cond INTERNAL */
/*****************************************************************************
* Defined parameters for Voltage References
*****************************************************************************/
#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC)
    #define INTERNAL_BANDGAP_VREF_MV        1200

    #define ANALOG_TEST_VREF1_MV            (INTERNAL_BANDGAP_VREF_MV)
    #define ANALOG_TEST_VREF2_MV            (CY_CFG_PWR_VDDA_MV/2)

/*****************************************************************************
* Defined parameters for MSC block as Voltage References
*****************************************************************************/
/* SW config to route the output to AMUXBUS A */
    #define MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_A      ((1UL << MSC_MODE_SW_SEL_TOP_AYA_CTL_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYA_EN_Pos) |\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYB_CTL_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYB_EN_Pos))

/* SW config to route the output to AMUXBUS B */
    #define MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_B      ((3UL << MSC_MODE_SW_SEL_TOP_BYB_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_MBCC_Pos))

    #define MSC_SW_SEL_SH_SO_TO_AMUXBUS_B           (1UL << MSC_MODE_SW_SEL_SH_SOMB_Pos)

/* SW config to use the internal bandgap as vref to MSC block */
    #define MSC_SW_SEL_TOP_BANDGAP_VREF_OUT         (1UL << MSC_MODE_SW_SEL_TOP_BGRF_Pos)

/* SW config to use vdda/2 as vref to MSC block */
    #define MSC_SW_SEL_TOP_VDDA_DIV2_OUT            (1UL << MSC_MODE_SW_SEL_TOP_RMF_Pos)


/* SW config to setup MSC as the voltage output */
    #define MSC_SW_SEL_TOP_VOLTAGE_OUT_SW_CONFIG    ((1UL << MSC_MODE_SW_SEL_TOP_CACB_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_CACC_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_CBCC_Pos))

    #define MSC_SW_SEL_SH_VOLTAGE_OUT_SW_CONFIG        ((1UL << MSC_MODE_SW_SEL_SH_CBSO_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_SH_FSP_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_SH_CCSO_Pos)|\
                                                    (7UL << MSC_MODE_SW_SEL_SH_BUF_SEL_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_SH_BUF_EN_Pos))
    #endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC */

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)

/* Define the external resistors values in Ohms */
    #define IDAC_RES1_VAL            4700u
    #define IDAC_RES2_VAL            4700u
/* 2.4uA IDAC resolution */
    #define IDAC_RANGE_2400_NA        2uL

    #define ANALOG_CSD_IDAC_VALUE1            42u
    #define ANALOG_CSD_IDAC_CURENT1_UA        101u

    #define ANALOG_CSD_IDAC_VALUE2            85u
    #define ANALOG_CSD_IDAC_CURENT2_UA        204u

    #define ANALOG_TEST_VREF1_MV            ((ANALOG_CSD_IDAC_CURENT1_UA * IDAC_RES1_VAL) / 1000)
    #define ANALOG_TEST_VREF2_MV            ((ANALOG_CSD_IDAC_CURENT2_UA * IDAC_RES1_VAL) / 1000)

/*****************************************************************************
* Defined parameters for CSD IDAC
*****************************************************************************/

    #define CSDV2_SENSE_PERIOD                  CSD_SENSE_PERIOD_LFSR_BITS_Msk

/* Routing IDAC B to AMUXB config */
    #define CSDV2_SW_BYP_SEL_IDACB_TO_AMUXB     CSD_SW_BYP_SEL_SW_BYB_Msk
    #define CSDV2_REF_GEN_SEL_IDACB_TO_AMUXB    CSD_SW_REFGEN_SEL_SW_IBCB_Msk
    #define CSDV2_IDACB_CONFIG                  \
        (IDAC_RANGE_2400_NA << CSD_IDACB_RANGE_Pos) | CSD_IDACB_LEG1_EN_Msk

/* Routing IDAC A to AMUXA config */
    #define CSDV2_SW_BYP_SEL_IDACA_TO_AMUXA     CSD_SW_BYP_SEL_SW_BYA_Msk
    #define CSDV2_IDACA_CONFIG                  \
        (IDAC_RANGE_2400_NA << CSD_IDACA_RANGE_Pos) | CSD_IDACA_LEG1_EN_Msk
#endif /* CY_IP_M0S8CSDV2 == 1u */
/** \endcond */

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_EXTERNAL)
/** External reference voltage 1 (VDDA / 3) using a three-resistor voltage divider. */
#define EXTERNAL_VREF1_MV               ((CY_CFG_PWR_VDDA_MV*1)/3)
/** External reference voltage 2 (2 * VDDA / 3) using a three-resistor voltage divider. */
#define EXTERNAL_VREF2_MV               ((CY_CFG_PWR_VDDA_MV*2)/3)
/** Selected test reference voltage 1. */
#define ANALOG_TEST_VREF1_MV            (EXTERNAL_VREF1_MV)
/** Selected test reference voltage 2. */
#define ANALOG_TEST_VREF2_MV            (EXTERNAL_VREF2_MV)
#endif /*(ANALOG_TEST_VREF == ANALOG_TEST_VREF_EXTERNAL )*/

/*****************************************************************************
* Defined parameters for Opamp self test
*****************************************************************************/
/** ADC channel to read OPAMP output. Only for PSOC 4 devices. */
#define ANALOG_ADC_CHNL_OPAMP           0x00u

/** OPAMP Accuracy in counts */
#define ANALOG_OPAMP_ACCURACY           ((CY_CFG_PWR_VDDA_MV * OPAMP_TEST_ACCURACY) / 100)
/** \cond INTERNAL */
#define ANALOG_OPAMP_ACURACCY           ANALOG_OPAMP_ACCURACY  /* deprecated: use ANALOG_OPAMP_ACCURACY */
/** \endcond */

/** Expected OPAMP SAR result for ANALOG_TEST_VREF1_MV. */
#define ANALOG_OPAMP_SAR_RESULT1        (ANALOG_TEST_VREF1_MV)
/** Expected OPAMP SAR result for ANALOG_TEST_VREF2_MV. */
#define ANALOG_OPAMP_SAR_RESULT2        (ANALOG_TEST_VREF2_MV)

/** \cond INTERNAL */
/*****************************************************************************
* Defined parameters for ADC self test
*****************************************************************************/
#define ANALOG_ADC_CHNL_VREF1           0x00u


#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
/* IDAC does not need a AMUXBUS per channel*/
    #define ANALOG_ADC_CHNL_VREF2           ANALOG_ADC_CHNL_VREF1
    #else
    #define ANALOG_ADC_CHNL_VREF2           0x01u
#endif

/** \endcond */

/** ADC Accuracy in counts */
#define ANALOG_ADC_ACCURACY             ((CY_CFG_PWR_VDDA_MV * ADC_TEST_ACC) / 100)
/** \cond INTERNAL */
#define ANALOG_ADC_ACURACCY             ANALOG_ADC_ACCURACY  /* deprecated: use ANALOG_ADC_ACCURACY */
/** \endcond */
/** Expected SAR result for ANALOG_TEST_VREF1_MV. */
#define ANALOG_ADC_SAR_RESULT1          (ANALOG_TEST_VREF1_MV)
/** Expected SAR result for ANALOG_TEST_VREF2_MV. */
#define ANALOG_ADC_SAR_RESULT2          (ANALOG_TEST_VREF2_MV)

/** ADC conversion time in test mode, uS */
#define ADC_TEST_CON_TIME_uS            (1000u)


/*****************************************************************************
* Defined parameters for LPCOMP self test
*****************************************************************************/
/** Expected COMP result when +ve terminal voltage > -ve terminal Voltage*/
#define ANALOG_COMP_RESULT1                (1u)
/** Expected COMP result when +ve terminal voltage < -ve terminal Voltage*/
#define ANALOG_COMP_RESULT2                (0u)

/** \} group_analog_macros */

/** \cond INTERNAL */
/*****************************************************************************
* Defined parameters for Analog Calibration
*****************************************************************************/
#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
    #define ANALOG_ADC_CHNL_IDACA           0x00u
    #define ANALOG_ADC_CHNL_IDACB           0x01u

    #define IDAC_SETTLE_TIME                (10u)
#endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */


/***************************************
* Function Prototypes
***************************************/
#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC)
void SelfTest_Init_MSCv3_Vdda_Div2_Amux_B(MSC_Type* base);
void SelfTest_Init_MSCv3_Vref_Amux_A(MSC_Type* base);
#endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC */

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
void SelfTest_Init_CSDv2_IDACA_Amux_A(CSD_Type* base);
void SelfTest_Init_CSDv2_IDACB_Amux_B(CSD_Type* base);
void SelfTest_Init_CSDv2_Dual_IDAC_Out(CSD_Type* base);
void SelfTest_IDACA_SetValue(CSD_Type* base, uint8_t value);
void SelfTest_IDACB_SetValue(CSD_Type* base, uint8_t value);
uint8_t SelfTests_IDACB_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base);
uint8_t SelfTests_IDACA_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base);
#endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */

/** \endcond */


/**
 * \addtogroup group_analog_functions
 * \{
 */

#if (defined(CLASSB_SELF_TEST_ADC) && (defined(CY_IP_MXS40PASS_CTB) || defined(CY_IP_M0S8PASS4A_CTB))) || \
    defined (CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTests_Opamp
****************************************************************************//**
*
* Performs OPAMP test and verifies if the measured voltage is in the accuracy range.
*
*
* \param sar_base
* The pointer to a SAR ADC instance.
* \param expected_res
* If count_to_mV = 1 => Expected result in mV, else Expected result in counts.
* \param accuracy
* Accuracy in count ANALOG_OPAMP_ACCURACY
* \param opamp_in_channel
* Channel number where the OPAMP output is read.
* \param count_to_mV
* 1 = convert the count to mV.(take more time)
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTests_Opamp(SAR_Type* sar_base, int16_t expected_res, int16_t accuracy,
                        uint32_t opamp_in_channel, bool count_to_mV);
#endif /* End Testing Opamp */


#if defined(CY_IP_MXS40MCPASS) || defined (CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTests_ADC_TrigIn
****************************************************************************//**
*
* Performs an ADC self-test by issuing a firmware SAR trigger and verifying
* that the measured result falls within the expected range.
*
* <b>HPPASS subsystem configuration and signal routing:</b><br>
* The test uses the CSG VDAC (pass[0].csg[0].slice[0]) as an
* internally generated stimulus so that no external reference voltage is
* required on any device pin. The complete analog signal path through the
* HPPASS block is:
*
*   CSG Group 0, Slice 0 (VDAC, buffered, 10-bit)
*   → CSG analog router (dacOutSelLoc = 1)
*   → SAR muxed sampler input
*   → SAR channel CSG_VDAC_OUT_CHAN_IDX (defined in cycfg_peripherals.h)
*
* The following HPPASS configurations are required:
*   - CSG[0]: Group 0 power enabled (grpPwrEn0 = true),
*     CSG AROUTE mux not used (arouteMuxSel0 = CY_HPPASS_CSG_AROUTE_NONE),
*     DAC output location 1 (dacOutSelLoc = 1, routes the VDAC output to the
*     HPPASS analog router so the SAR muxed sampler can reach it).
*   - CSG[0] Slice 0: mode = CY_HPPASS_DAC_MODE_BUFFERED (10-bit VDAC with
*     output buffer).
*   - HPPASS input trigger 2 (pass[0].input_trigger[2]): CY_HPPASS_TR_FW_PULSE
*     — used to fire the SAR sequencer group that samples the DAC output
*     (passed as \p trig_in = CY_HPPASS_TRIG_2_MSK).
*   - SAR[0]: Vref = CY_HPPASS_SAR_VREF_VDDA, 12-bit result, right-aligned,
*     unsigned. Muxed sampler 0 (pass[0].sar[0].muxed_sampler[0]) connected to
*     the CSG VDAC analog router output.
*   - SAR[0] sequencer group (pass[0].sar[0].seq[0].grp[N]): muxed sampler 0
*     enabled (muxSampEn0 = true), mux input select pointing to the CSG VDAC
*     output, trigger = CY_HPPASS_SAR_TRIG_2 (corresponding to \p trig_in).
*     The result is stored in SAR channel CSG_VDAC_OUT_CHAN_IDX.
*   - Autonomous Controller (pass[0].ac[0]): the HPPASS AC must be started
*     (Cy_HPPASS_AC_Start()) before any SAR conversion can be triggered. The AC
*     Finite State Automaton (FSA) must be configured with at least one state
*     (pass[0].ac[0].stt[0].vstate[0]) that keeps the subsystem running, e.g.
*     action = CY_HPPASS_ACTION_STOP with count = 1. The AC holds the HPPASS
*     clock and power rails active; without it the SAR trigger is ignored.
*
* The caller must pre-stage the DAC stimulus before calling this function:
* program the desired 10-bit code with Cy_HPPASS_DAC_SetValue(), start the DAC
* slice with Cy_HPPASS_DAC_Start() in HW mode, and wait for the DAC to settle
* before invoking SelfTests_ADC_TrigIn(). This function then issues the
* SAR trigger specified by \p trig_in and reads the conversion result from
* \p channel.
*
* \param group
* SAR group instance (0 for SAR[0]).
* \param channel
* SAR channel index where the DAC stimulus is sampled. Use the macro
* CSG_VDAC_OUT_CHAN_IDX generated in cycfg_peripherals.h for the target BSP.
* \param expected_res
* Expected ADC result in counts (12-bit, Vref = VDDA).
* \param accuracy
* Acceptable deviation from expected_res in counts (ANALOG_ADC_ACCURACY).
* \param trig_in
* HPPASS input trigger mask that fires the SAR sequencer group containing
* the muxed sampler channel (e.g. CY_HPPASS_TRIG_2_MSK).
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTests_ADC_TrigIn(uint32_t group, uint32_t channel, int32_t expected_res, int32_t accuracy,
                             uint32_t trig_in);


#if defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)
/** \addtogroup group_analog_macros
 * \{
 */
/**
 * SAR channel index for DAC0 output readback (AN_A5 via HPPASS AROUTE direct sampler 5).
 *
 * Topology: DAC buffer 0 output → AN_A5 → HPPASS AROUTE → SAR direct sampler 5
 */
#define SELFTEST_HPPASS_DAC0_R2R_SAR_CHAN_IDX   (5U)
/**
 * SAR channel index for DAC1 output readback (AN_B5 via HPPASS AROUTE muxed sampler 13).
 *
 * Topology: DAC buffer 1 output → AN_B5 → HPPASS AROUTE → SAR muxed sampler 13 (MUX1_SEL=0 → AIO_B_5)
 *
 */
#define SELFTEST_HPPASS_DAC1_R2R_SAR_CHAN_IDX  (18U)
/** \} group_analog_macros */

/*******************************************************************************
* Function Name: SelfTests_DAC_TrigIn
****************************************************************************//**
*
* Performs DAC R2R self-test: sets the DAC output to the specified value and
* verifies the SAR ADC readback is within the expected range.
*
* \param dac_idx
* DAC index: 0 = DAC0 (AN_A5, SAR ch 5, trigger 0),
*            1 = DAC1 (AN_B5, SAR ch 18, trigger 1).
* \param dac_val
* DAC output value to write (12-bit unsigned).
* \param expected_res
* Expected SAR result in counts.
* \param accuracy
* Accuracy tolerance in counts (ANALOG_ADC_ACCURACY).
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTests_DAC_TrigIn(uint8_t dac_idx, uint32_t dac_val,
                             int32_t expected_res, int32_t accuracy);


/*******************************************************************************
* Function Name: SelfTests_AFE_TrigIn
****************************************************************************//**
*
* The Opamp test is performed to verify that the Opamp output voltage is aligned
* with the Opamp input voltage and gain.
*
* \param group
* The SAR group instance.
* \param channel
* The SAR channel number where the Opamp output is read.
* \param expected_res
* Expected SAR result in counts.
* \param accuracy
* The SAR accuracy in count ANALOG_ADC_ACCURACY
* \param trig_in
* The SAR trigger input.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTests_AFE_TrigIn(uint32_t group, uint32_t channel, int32_t expected_res, int32_t accuracy,
                             uint32_t trig_in);
#endif /* defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u) */
#endif /* defined(CY_IP_MXS40MCPASS) || defined (CY_DOXYGEN) */

/*******************************************************************************
* Function Name: SelfTests_ADC
****************************************************************************//**
*
* Performs ADC test and verifies if the measured voltage is in the accuracy range.
*
*
* \param base
* The pointer to a SAR ADC instance. For PSOC Control C3 devices, the base is the group instance.
* \param channel
* The channel number where the input voltage needs to be read.
* \param expected_res
* If count_to_mV = 1, then the expected result in mV, else expected result in counts
* \param accuracy
* Accuracy in count ANALOG_ADC_ACCURACY
* \param vbg_channel
* The channel number where the VBG voltage is connected. Only for XMC7000 and XMC5000 devices.
* \param count_to_mV
* 1 = convert the count to mV.(take more time). Not applicable for PSOC Control C3 devices.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note
* For CA1B devices, the user needs to configure the trigger input the same as the channel number.
*
*******************************************************************************/
#if defined(CY_IP_M0S8PASS4A_SAR) || defined(CY_IP_MXS40PASS_SAR) || defined (CY_DOXYGEN)
uint8_t SelfTests_ADC(SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV);
#elif defined (CY_IP_MXS40EPASS_ESAR)
uint8_t SelfTests_ADC(PASS_SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV);
#elif defined(CY_IP_MXS40MCPASS)
uint8_t SelfTests_ADC(uint32_t group, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV);
#endif /* defined(CY_IP_M0S8PASS4A_SAR) || defined(CY_IP_MXS40PASS_SAR) || defined (CY_DOXYGEN) */


#if defined(CLASSB_SELF_TEST_COMP) || defined (CY_DOXYGEN)

/*******************************************************************************
* Function Name: SelfTests_Comparator
****************************************************************************//**
*
* Performs Comparator test and verifies if the higher voltage on +ve terminal
* compared to -ve terminal generates a high output and vice verse.
*
*
* \param lpcomp_base
* The pointer to a Comparator instance
* \param lpcomp_channel
* Configured channel number
* \param expected_res
* Expected result (1 or 0)
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res);
#endif /* defined(CLASSB_SELF_TEST_COMP) || defined (CY_DOXYGEN) */


/*******************************************************************************
* Function Name: SelfTests_DAC
****************************************************************************//**
*
* Performs DAC test and verifies that the input of DAC and output from ADC are the same.
* The digital input value to DAC maps to 1.5V
*
* \param dacBase
* The pointer to a DAC instance.
* \param adcBase
* The pointer to a SAR ADC instance.
* \param adcChannel
* The channel number of SAR ADC instance.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note
* Applicable only to PSOC 61 Programmable Line and PSOC 62 Performance Line devices.
*
*******************************************************************************/
#if defined(CY_IP_MXS40PASS_CTDAC) || defined (CY_DOXYGEN)
uint8_t SelfTests_DAC(CTDAC_Type* dacBase, SAR_Type* adcBase, uint32_t adcChannel);
#endif

#if (defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION < 3u)) || defined (CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTests_DAC_TrigIn
****************************************************************************//**
*
* Performs DAC test and verifies that the input of DAC and output from ADC are the same.
*
* \param adc_channel
* The pointer to the ADC channel.
* \param dac_slice
* The pointer to DAC slice.
* \param dac_val
* The value to be loaded into DAC register.
* \param expected_res
* Channel expected result in ADC.
* \param accuracy
* Error tolerance.
* \param adc_trig_in
* ADC trigger input.
* \param dac_trig_in
* DAC trigger input.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note
* Applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTests_DAC_TrigIn(uint32_t adc_channel, uint32_t dac_slice, uint32_t dac_val,
                             int16_t expected_res, int16_t accuracy, uint32_t adc_trig_in, uint32_t dac_trig_in);
#endif /* defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION < 3u) */
/** \} group_analog_functions */

/** \} group_analog */

#endif /* End __SelfTest_Analog_h */

/* [] END OF FILE */

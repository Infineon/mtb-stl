/*******************************************************************************
* File Name: SelfTest_Analog.h
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the analog component self tests according to Class B library.
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
 * \addtogroup group_analog
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
 *  can use. Only for CAT2 device */
#define ANALOG_TEST_VREF_DUAL_MSC              (1u)
/** Uses an internal CSD IDAC to generate a voltage from an external pull-down resistor. Only for CAT2
 *  device. */
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
#define OPAMP_TEST_ACURACCY             12

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
/** External voltage references using 3 equivalent resistor voltage divider*/
#define EXTERNAL_VREF1_MV               ((CY_CFG_PWR_VDDA_MV*1)/3)
/** External voltage references using 3 equivalent resistor voltage divider*/
#define EXTERNAL_VREF2_MV               ((CY_CFG_PWR_VDDA_MV*2)/3)
/** External voltage references using 3 equivalent resistor voltage divider*/
#define ANALOG_TEST_VREF1_MV            (EXTERNAL_VREF1_MV)
/** External voltage references using 3 equivalent resistor voltage divider*/
#define ANALOG_TEST_VREF2_MV            (EXTERNAL_VREF2_MV)
#endif /*(ANALOG_TEST_VREF == ANALOG_TEST_VREF_EXTERNAL )*/

/*****************************************************************************
* Defined parameters for Opamp self test
*****************************************************************************/
/** ADC channel to read OPAMP output. Only for CAT2 device */
#define ANALOG_ADC_CHNL_OPAMP           0x00u

/** OPAMP Accuracy in counts */
#define ANALOG_OPAMP_ACURACCY           ((CY_CFG_PWR_VDDA_MV * OPAMP_TEST_ACURACCY) / 100)

/** Expected OPAMP result 1*/
#define ANALOG_OPAMP_SAR_RESULT1        (ANALOG_TEST_VREF1_MV)
/** Expected OPAMP result 2*/
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
#define ANALOG_ADC_ACURACCY             ((CY_CFG_PWR_VDDA_MV * ADC_TEST_ACC) / 100)
/** Expected ADC result 1*/
#define ANALOG_ADC_SAR_RESULT1          (ANALOG_TEST_VREF1_MV)
/** Expected ADC result 2*/
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

#if defined(CLASSB_SELF_TEST_OPAMP) || defined (CY_DOXYGEN)
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
* Accuracy in count ANALOG_OPAMP_ACURACCY
* \param opamp_in_channel
* Channel number where the OPAMP output is read.
* \param count_to_mV
* 1 = convert the count to mV.(take more time)
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
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
* Performs ADC test and verifies if the measured voltage is in the accuracy range.
*
* \param group
* Group instance.
* \param channel
* The channel number where the input voltage needs to be read.
* \param expected_res
* if count_to_mV = 1 => Expected result in mV, else Expected result in counts.
* \param accuracy
* Accuracy in count ANALOG_ADC_ACURACCY
* \param trig_in
* Trigger input.
*
* \return
*  0 - Test Passed <br>
*  1 - Test Failed
*
*******************************************************************************/
uint8_t SelfTests_ADC_TrigIn(uint32_t group, uint32_t channel, int16_t expected_res, int16_t accuracy,
                             uint32_t trig_in);
#endif /* End Testing ADC */

/*******************************************************************************
* Function Name: SelfTests_ADC
****************************************************************************//**
*
* Performs ADC test and verifies if the measured voltage is in the accuracy range.
*
*
* \param base
* The pointer to a SAR ADC instance. For CAT1B, the base is the group instance.
* \param channel
* The channel number where the input voltage needs to be read.
* \param expected_res
* If count_to_mV = 1, then the expected result in mV, else expected result in counts
* \param accuracy
* Accuracy in count ANALOG_ADC_ACURACCY
* \param vbg_channel
* The channel number where the VBG voltage is connected. Only for CAT1C device.
* \param count_to_mV
* 1 = convert the count to mV.(take more time). Not applicable for CAT1B device.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
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
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res);
#endif /* defined(CLASSB_SELF_TEST_COMP) || defined (CY_DOXYGEN) */


#if defined(CY_IP_MXS40PASS_CTDAC) || defined (CY_DOXYGEN)
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
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* Applicable only to CAT1A devices
*
*******************************************************************************/
uint8_t SelfTests_DAC(CTDAC_Type* dacBase, SAR_Type* adcBase, uint32_t adcChannel);
#endif /* defined(CY_IP_MXS40PASS_CTDAC) || defined (CY_DOXYGEN) */

#if defined(CLASSB_SELF_TEST_DAC) || defined (CY_DOXYGEN)
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
*  0 - Test passed <br>
*  1 - Test failed
*
* \note
* Applicable only to CAT1B devices.
*
*******************************************************************************/
uint8_t SelfTests_DAC_TrigIn(uint32_t adc_channel, uint32_t dac_slice, uint32_t dac_val,
                             int16_t expected_res, int16_t accuracy, uint32_t adc_trig_in, uint32_t dac_trig_in);
#endif
/** \} group_analog_functions */

/** \} group_analog */

#endif /* End __SelfTest_Analog_h */

/* [] END OF FILE */

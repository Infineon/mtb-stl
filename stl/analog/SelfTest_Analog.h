/*******************************************************************************
* File Name: SelfTest_Analog.h
* Version 1.0.0
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the analog component self tests according to Class B library.
*
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
/**
* \addtogroup group_analog
* \{
*
* This module carries out 3 tests: <br>
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
* Analog block. If the measured value is more than the expected value,
* an error is returned.
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
* \section group_analog_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_analog_macros Macros
* \defgroup group_analog_functions Functions
*/

#if !defined(SELFTEST_ANALOG_H)
    #define SELFTEST_ANALOG_H

#include "cy_pdl.h"
#include "cybsp.h"
#include "SelfTest_common.h"

/* Supports three self test modes: */
/* ANALOG_TEST_VREF_EXTERNAL  - depends on a external three series resistor
 * voltage divider from VDDA to GND to supply a reference voltage for the self test.
 *
 * ANALOG_TEST_VREF_DUAL_MSC - uses the two internal MSCv3 blocks to route VREF(1.2V) and VDDA/2 to
 * the
 * AMUXBUS which the tests can use.
 *
 * ANALOG_TEST_VREF_CSD_IDAC - Uses a internal CSD IDAC to generate a voltage from a external pull
 * down resistor.
 * */

/** \addtogroup group_analog_macros
* \{
*/
#if (CY_CPU_CORTEX_M0P)
    /** Test is carried out by using Vref from External*/
    #define ANALOG_TEST_VREF                       (ANALOG_TEST_VREF_EXTERNAL)
    /** Depends on a external three series resistor voltage divider from VDDA to GND to supply a reference voltage for the self test. */
    #define ANALOG_TEST_VREF_EXTERNAL              (0u)
    /** Uses the two internal MSCv3 blocks to route VREF(1.2V) and VDDA/2 to the AMUXBUS which the tests can use. Only for CAT2 device */
    #define ANALOG_TEST_VREF_DUAL_MSC              (1u)
    /** Uses a internal CSD IDAC to generate a voltage from a external pull down resistor. Only for CAT2 device */
    #define ANALOG_TEST_VREF_CSD_IDAC              (2u)
#else
    /** Test is carried out by using Vref from External*/
    #define ANALOG_TEST_VREF                       (ANALOG_TEST_VREF_EXTERNAL)
    /** Depends on a external three series resistor voltage divider from VDDA to GND to supply a reference voltage for the self test. */
    #define ANALOG_TEST_VREF_EXTERNAL              (0u)
#endif


/** \cond INTERNAL */
#if (CY_CPU_CORTEX_M0P)
    #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC) && !defined(CY_IP_M0S8MSCV3) && \
        (CY_IP_M0S8MSCV3_INSTANCES < 2u)
    #error "Hardware does not support ANALOG_TEST_VREF_DUAL_MSC mode"
    #endif

    #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC) && !defined(CY_IP_M0S8CSDV2)
    #error "Hardware does not support ANALOG_TEST_VREF_CSD_IDAC mode"
    #endif
#endif

#ifdef CY_IP_MXS40PASS_CTDAC
    #define DAC_INPUT 0x7D0 /* Maps to 1.5 V */
    #define DAC_ADC_OUTPUT_DEVIATION 0.01 /* Devation of 1% between DAC input and ADC output */
#endif

/** \endcond */

/***************************************
* Initial Parameter Constants
***************************************/
/** ADC accuracy: 12% */
#define ADC_TEST_ACC                    12

/* Definition of the analog components included in class B */
#if (CY_CPU_CORTEX_M0P)
    /** OP-AMP accuracy: 12% */
    #define OPAMP_TEST_ACURACCY             12
    /** Whether ADC is present or not*/
    #define CLASSB_SELF_TEST_ADC            1u
    /** Whether OPAMP is present or not */
    #define CLASSB_SELF_TEST_OPAMP          1u
    /** Whether Comparator is present or not */
    #define CLASSB_SELF_TEST_COMP           1u

#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M33 )
    /** Whether ADC is present or not */
    #define CLASSB_SELF_TEST_ADC            1u
    /** Whether Comparator is present or not */
    #define CLASSB_SELF_TEST_COMP           1u
    #if CY_CPU_CORTEX_M33
    #define CLASSB_SELF_TEST_DAC            1u
    #endif
    #if (defined(CY_DEVICE_PSOC6ABLE2) || defined(CY_DEVICE_PSOC6A256K))
        /** Whether OP-AMP is present or not */
        #define OPAMP_TEST_ACURACCY             12
        /** OP-AMP accuracy: 12% */
        #define CLASSB_SELF_TEST_OPAMP          1u
    #endif

#elif (CY_CPU_CORTEX_M7)
    /** Whether ADC is present or not */
    #define CLASSB_SELF_TEST_ADC            1u
#endif

/** \cond INTERNAL */
#if (CY_CPU_CORTEX_M0P)
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
    /* SW config to route output to AMUXBUS A */
    #define MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_A      ((1UL << MSC_MODE_SW_SEL_TOP_AYA_CTL_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYA_EN_Pos) |\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYB_CTL_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_AYB_EN_Pos))

    /* SW config to route output to AMUXBUS B */
    #define MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_B      ((3UL << MSC_MODE_SW_SEL_TOP_BYB_Pos)|\
                                                    (1UL << MSC_MODE_SW_SEL_TOP_MBCC_Pos))

    #define MSC_SW_SEL_SH_SO_TO_AMUXBUS_B           (1UL << MSC_MODE_SW_SEL_SH_SOMB_Pos)

    /* SW config to use internal bandgap as vref to MSC block */
    #define MSC_SW_SEL_TOP_BANDGAP_VREF_OUT         (1UL << MSC_MODE_SW_SEL_TOP_BGRF_Pos)

    /* SW config to use vdda/2 as vref to MSC block */
    #define MSC_SW_SEL_TOP_VDDA_DIV2_OUT            (1UL << MSC_MODE_SW_SEL_TOP_RMF_Pos)


    /* SW config to setup MSC as a voltage output */
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

    /* Define external resistors values in Ohms */
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

    /* routing IDAC B to AMUXB config */
    #define CSDV2_SW_BYP_SEL_IDACB_TO_AMUXB     CSD_SW_BYP_SEL_SW_BYB_Msk
    #define CSDV2_REF_GEN_SEL_IDACB_TO_AMUXB    CSD_SW_REFGEN_SEL_SW_IBCB_Msk
    #define CSDV2_IDACB_CONFIG                  \
        (IDAC_RANGE_2400_NA << CSD_IDACB_RANGE_Pos) | CSD_IDACB_LEG1_EN_Msk

    /* routing IDAC A to AMUXA config */
    #define CSDV2_SW_BYP_SEL_IDACA_TO_AMUXA     CSD_SW_BYP_SEL_SW_BYA_Msk
    #define CSDV2_IDACA_CONFIG                  \
        (IDAC_RANGE_2400_NA << CSD_IDACA_RANGE_Pos) | CSD_IDACA_LEG1_EN_Msk
    #endif /* CY_IP_M0S8CSDV2 == 1u */
#endif
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

#if (CY_CPU_CORTEX_M0P)
    #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
    /* IDAC does not need a AMUXBUS per channel*/
    #define ANALOG_ADC_CHNL_VREF2            ANALOG_ADC_CHNL_VREF1
    #else
    #define ANALOG_ADC_CHNL_VREF2           0x01u
    #endif
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

/** \cond INTERNAL */
#if (CY_CPU_CORTEX_M0P)
    /*****************************************************************************
    * Defined parameters for Analog Calibration
    *****************************************************************************/
    #define ANALOG_ADC_CHNL_IDACA           0x00u
    #define ANALOG_ADC_CHNL_IDACB           0x01u

    #define IDAC_SETTLE_TIME                (10u)


    /***************************************
    * Function Prototypes
    ***************************************/
    #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC)
    void SelfTest_Init_MSCv3_Vdda_Div2_Amux_B(MSC_Type* base);
    void SelfTest_Init_MSCv3_Vref_Amux_A(MSC_Type* base);
    #endif /* CY_IP_M0S8MSCV3 == 1u */
    #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
    void SelfTest_Init_CSDv2_IDACA_Amux_A(CSD_Type* base);
    void SelfTest_Init_CSDv2_IDACB_Amux_B(CSD_Type* base);
    void SelfTest_Init_CSDv2_Dual_IDAC_Out(CSD_Type* base);
    void SelfTest_IDACA_SetValue(CSD_Type* base, uint8_t value);
    void SelfTest_IDACB_SetValue(CSD_Type* base, uint8_t value);
    uint8_t SelfTests_IDACB_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base);
    uint8_t SelfTests_IDACA_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base);

    #endif /* CY_IP_M0S8CSDV2 == 1u */
#endif
/** \endcond */
/** \} group_analog_macros */


/**
* \addtogroup group_analog_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTests_Opamp
****************************************************************************//**
*
* Performs OPAMP test and verify measured voltage is in accuracy range.
*
*
* \param sar_base 
* The pointer to a SAR ADC instance
* \param expected_res 
* if count_to_mV = 1 => Expected result in mV, else Expected result in counts.
* \param accuracy 
* Accuracy in count ANALOG_OPAMP_ACURACCY
* \param opamp_in_channel 
* channel no. where the OPAMP output is been read
* \param count_to_mV 
* 1 = convert the count to mV.(take more time)
*
* \return
*  0 - Test Passed <br>
*  1 - Test Failed
*
*******************************************************************************/
#if defined(CLASSB_SELF_TEST_OPAMP)
    uint8_t SelfTests_Opamp(SAR_Type* sar_base, int16_t expected_res, int16_t accuracy, uint32_t opamp_in_channel, bool count_to_mV);
#endif /* End Testing Opamp */

/*******************************************************************************
* Function Name: SelfTests_ADC
****************************************************************************//**
*
* Performs ADC test and verify measured voltage is in accuracy range.
*
*
* \param base 
* The pointer to a SAR ADC instance. For CAT1B, base is the group instance.
* \param channel
* channel no. where the input voltage needs to be read
* \param expected_res 
* if count_to_mV = 1 => Expected result in mV, else Expected result in counts.
* \param accuracy 
* Accuracy in count ANALOG_ADC_ACURACCY
* \param vbg_channel
* Channel no. where the VBG voltage is connected. Only for CAT1C device.
* \param count_to_mV 
* 1 = convert the count to mV.(take more time). Not applicable for CAT1B device
*
* \return
*  0 - Test Passed <br>
*  1 - Test Failed
*
* \note
* For CA1B devices, User needs to configure the trigger input same as channel number.
*
*******************************************************************************/
#if defined(CLASSB_SELF_TEST_ADC)
    #if CY_CPU_CORTEX_M0P
        uint8_t SelfTests_ADC(SAR_Type * base, uint32_t channel, int16_t expected_res, int16_t accuracy, uint32_t vbg_channel, bool count_to_mV);
    #elif CY_CPU_CORTEX_M4
        uint8_t SelfTests_ADC(SAR_Type * base, uint32_t channel, int16_t expected_res, int16_t accuracy, uint32_t vbg_channel, bool count_to_mV);
    #elif CY_CPU_CORTEX_M7
       uint8_t SelfTests_ADC(PASS_SAR_Type * base, uint32_t channel, int16_t expected_res, int16_t accuracy, uint32_t vbg_channel, bool count_to_mV);
    #elif CY_CPU_CORTEX_M33
        uint8_t SelfTests_ADC(uint32_t base, uint32_t channel, int16_t expected_res, int16_t accuracy, uint32_t vbg_channel, bool count_to_mV);
    #endif
#endif /* End Testing ADC */

/*******************************************************************************
* Function Name: SelfTests_Comparator
****************************************************************************//**
*
* Performs Comparator test and verify that the higher voltage on +ve terminal 
* compared to -ve terminal generates high output and vice verse.
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
*  0 - Test Passed <br>
*  1 - Test Failed
*
*******************************************************************************/
#if defined(CLASSB_SELF_TEST_COMP)
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res);
#endif /* End  Testing AN cmp */


/*******************************************************************************
* Function Name: SelfTests_DAC
****************************************************************************//**
*
* Performs DAC test and verifies that input of DAC and output from ADC are same.
* The digital input value to DAC maps to 1.5V
*
* \param dacBase 
* The pointer to a DAC instance
* \param adcBase 
* The pointer to a SAR ADC instance
* \param adcChannel
* channel number of SAR ADC instance
*
* \return
*  0 - Test Passed <br>
*  1 - Test Failed
*
* \note
* Applicable only to CAT1A devices
*
*******************************************************************************/
#if defined(CY_IP_MXS40PASS_CTDAC) || defined (CY_DOXYGEN)
uint8_t SelfTests_DAC(CTDAC_Type* dacBase, SAR_Type* adcBase, uint32_t adcChannel);
#endif

#if defined(CLASSB_SELF_TEST_DAC) || defined (CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTests_DAC
****************************************************************************//**
*
* Performs DAC test and verifies that input of DAC and output from ADC are same.
*
* \param adc_channel 
* Pointer to the ADC channel
* \param dac_slice 
* Pointer to DAC slice
* \param dac_val
* Value to be loaded in DAC register
* \param expected_res
* channel Expected result in ADC
* \param accuracy
* Error tolerance
* \return
*  0 - Test Passed <br>
*  1 - Test Failed
*
* \note
* Applicable only to CAT1B devices
*
*******************************************************************************/
uint8_t SelfTests_DAC(uint32_t adc_channel, uint32_t dac_slice, uint32_t dac_val, int16_t expected_res, int16_t accuracy);
#endif
/** \} group_analog_functions */

/** \} group_analog */

#endif /* End __SelfTest_Analog_h */

/* [] END OF FILE */

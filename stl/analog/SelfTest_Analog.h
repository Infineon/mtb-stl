/*******************************************************************************
* File Name: SelfTest_Analog.h
* Version 1.0.0
*
* Description:
*  This file provides the function prototypes, constants, and parameter values used
*  for the analog component self tests according to Class B library
*  for CAT2(PSoC4) devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
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

#include "SelfTest_common.h"

#if !defined(SELFTEST_ANALOG_H)
    #define SELFTEST_ANALOG_H

#include "cy_pdl.h"
#include "cybsp.h"

#if CY_CPU_CORTEX_M0P

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
#define ANALOG_TEST_VREF            (ANALOG_TEST_VREF_EXTERNAL)

#define ANALOG_TEST_VREF_EXTERNAL           (0u)
#define ANALOG_TEST_VREF_DUAL_MSC           (1u)
#define ANALOG_TEST_VREF_CSD_IDAC              (2u)

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC) && !defined(CY_IP_M0S8MSCV3) && \
    (CY_IP_M0S8MSCV3_INSTANCES < 2u)
#error "Hardware does not support ANALOG_TEST_VREF_DUAL_MSC mode"
#endif

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC) && !defined(CY_IP_M0S8CSDV2)
#error "Hardware does not support ANALOG_TEST_VREF_CSD_IDAC mode"
#endif
/***************************************
* Initial Parameter Constants
***************************************/
/* ADC accuracy: 12% */
#define ADC_TEST_ACC                    12
#define OPAMP_TEST_ACURACCY                12
/* Definition of the analog components included in class B */
#define CLASSB_SELF_TEST_ADC            1u
#define CLASSB_SELF_TEST_OPAMP          1u
#define CLASSB_SELF_TEST_COMP           1u

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

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_EXTERNAL)
/* External voltage references using 3 equivalent resistor voltage divider*/
#define EXTERNAL_VREF1_MV               ((CY_CFG_PWR_VDDA_MV*1)/3)
#define EXTERNAL_VREF2_MV               ((CY_CFG_PWR_VDDA_MV*2)/3)

#define ANALOG_TEST_VREF1_MV            (EXTERNAL_VREF1_MV)
#define ANALOG_TEST_VREF2_MV            (EXTERNAL_VREF2_MV)
#endif /*(ANALOG_TEST_VREF == ANALOG_TEST_VREF_EXTERNAL )*/

/*****************************************************************************
* Defined parameters for Opamp self test
*****************************************************************************/
#define ANALOG_ADC_CHNL_OPAMP           0x00u

#define ANALOG_OPAMP_ACURACCY           ((CY_CFG_PWR_VDDA_MV * OPAMP_TEST_ACURACCY) / 100)

#define ANALOG_OPAMP_SAR_RESULT1        (ANALOG_TEST_VREF1_MV)
#define ANALOG_OPAMP_SAR_RESULT2        (ANALOG_TEST_VREF2_MV)


/*****************************************************************************
* Defined parameters for ADC self test
*****************************************************************************/
#define ANALOG_ADC_CHNL_VREF1           0x00u
#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
/* IDAC does not need a AMUXBUS per channel*/
#define ANALOG_ADC_CHNL_VREF2            ANALOG_ADC_CHNL_VREF1
#else
#define ANALOG_ADC_CHNL_VREF2           0x01u
#endif

#define ANALOG_ADC_ACURACCY             ((CY_CFG_PWR_VDDA_MV * ADC_TEST_ACC) / 100)

#define ANALOG_ADC_SAR_RESULT1          (ANALOG_TEST_VREF1_MV)
#define ANALOG_ADC_SAR_RESULT2          (ANALOG_TEST_VREF2_MV)

/* ADC conversion time in test mode, uS */
#define ADC_TEST_CON_TIME_uS            (1000u)


/*****************************************************************************
* Defined parameters for LPCOMP self test
*****************************************************************************/
#define ANALOG_COMP_RESULT1                (1u)
#define ANALOG_COMP_RESULT2                (0u)

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

/* Testing Opamp */
#if defined(CLASSB_SELF_TEST_OPAMP)
uint8_t SelfTests_Opamp(SAR_Type* sar_base, int16_t expected_res, int16_t accuracy);
#endif /* End Testing Opamp */

/* Testing ADC */
#if defined(CLASSB_SELF_TEST_ADC)
uint8_t SelfTests_ADC(SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy);
#endif /* End Testing ADC */

/* Testing Analog comparator */
#if defined(CLASSB_SELF_TEST_COMP)
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res);
#endif /* End  Testing AN cmp */

#endif /* End __SelfTest_Analog_h */

#endif

/* [] END OF FILE */

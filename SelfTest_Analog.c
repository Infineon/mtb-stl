/*******************************************************************************
* File Name: SelfTest_Analog.c
* Version 2.0
*
* Description:
*  This file provides source code for the APIs to perform analog component
*  self tests according to the Class B library for PSoC 4.
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
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "SelfTest_Analog.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"



#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC)
static void SelfTest_Clear_MSCv3_Regs(MSC_Type* base);

/*****************************************************************************
* Function Name: SelfTest_Clear_MSCv3_Regs
******************************************************************************
*
* Summary:
*  Clears all the registers of the MSCv3 block.
*
* Parameters:
*  MSC_Type* base - The pointer to a MSC HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
static void SelfTest_Clear_MSCv3_Regs(MSC_Type* base)
{
    uint16_t reg_size =  (uint16_t)sizeof(uint32_t);
    /* Clear general MSCv3 registers */
    for (uint16_t offset = 0; offset <= 0x220u; offset += reg_size)
    {
        Cy_MSC_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    /* Clear SW_SEL_CSW array */
    for (uint16_t offset = 0x300; offset <= 0x37Cu; offset += reg_size)
    {
        Cy_MSC_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    /* Clear SW_SEL_CSW_FUNC array */
    for (uint16_t offset = 0x400; offset <= 0x41Cu; offset += reg_size)
    {
        Cy_MSC_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    /* Clear MODE Structures */
    for (uint16_t offset = 0x500; offset <= 0x550u; offset += reg_size)
    {
        Cy_MSC_ClrBits(base, offset, 0xFFFFFFFFuL);
    }
}


/*****************************************************************************
* Function Name: SelfTest_Init_MSCv3_Vref_Amux_A
******************************************************************************
*
* Summary:
*  Attempts to acquire the MSC block and configure it to output the internal
*  bandgap voltage reference to AMUXBUS A
*
* Parameters:
*  MSC_Type* base - The pointer to a MSC HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
void SelfTest_Init_MSCv3_Vref_Amux_A(MSC_Type* base)
{
    SelfTest_Clear_MSCv3_Regs(base);
    base->MODE[0].SW_SEL_SH = (uint32_t)(MSC_SW_SEL_SH_VOLTAGE_OUT_SW_CONFIG);
    base->MODE[0].SW_SEL_TOP =
        (MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_A|  MSC_SW_SEL_TOP_BANDGAP_VREF_OUT|
         MSC_SW_SEL_TOP_VOLTAGE_OUT_SW_CONFIG);
    base->CTL =
        (1UL << MSC_CTL_BUF_MODE_Pos) | (1UL << MSC_CTL_ENABLED_Pos)| (1UL << MSC_CTL_SENSE_EN_Pos);
}


/*****************************************************************************
* Function Name: SelfTest_Init_MSCv3_Vdda_Div2_Amux_B
******************************************************************************
*
* Summary:
*  Attempts to acquire the MSC block and configure it to output the internal
*  vdda/2 reference to AMUXBUS B
*
* Parameters:
*  MSC_Type* base - The pointer to a MSC HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
void SelfTest_Init_MSCv3_Vdda_Div2_Amux_B(MSC_Type* base)
{
    SelfTest_Clear_MSCv3_Regs(base);
    base->MODE[0].SW_SEL_SH =
        (uint32_t)(MSC_SW_SEL_SH_SO_TO_AMUXBUS_B| MSC_SW_SEL_SH_VOLTAGE_OUT_SW_CONFIG);
    base->MODE[0].SW_SEL_TOP =
        (MSC_SW_SEL_TOP_CSDBUS_TO_AMUXBUS_B | MSC_SW_SEL_TOP_VDDA_DIV2_OUT |
         MSC_SW_SEL_TOP_VOLTAGE_OUT_SW_CONFIG);
    base->CTL =
        (1UL <<
         MSC_CTL_BUF_MODE_Pos) | (1UL << MSC_CTL_ENABLED_Pos) | (1UL << MSC_CTL_SENSE_EN_Pos);
}


#endif /* (ANALOG_TEST_VREF == ANALOG_TEST_VREF_DUAL_MSC) */

#if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
static int16_t dacOffset = 0;
static void SelfTest_Clear_CSDv2_Regs(CSD_Type* base);
static uint8_t SelfTests_Set_IDAC_Offset(SAR_Type* sar_base, uint32_t channel);
/*****************************************************************************
* Function Name: SelfTest_Clear_CSDv2_Regs
******************************************************************************
*
* Summary:
*  Clears all the registers of the CSDv2 block.
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
static void SelfTest_Clear_CSDv2_Regs(CSD_Type* base)
{
    uint16_t reg_size =  (uint16_t)sizeof(uint32_t);

    for (uint16_t offset = 0; offset <= 0x4u; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x80; offset <= 0x8Cu; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0xD0; offset <= 0xFCu; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x180; offset <= 0x18Cu; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x180; offset <= 0x18Cu; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x1C0; offset <= 0x1C4u; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x1F0; offset <= 0x204u; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x280; offset <= 0x2C0u; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }

    for (uint16_t offset = 0x300; offset <= 0x340u; offset += reg_size)
    {
        Cy_CSD_ClrBits(base, offset, 0xFFFFFFFFuL);
    }
}


/*****************************************************************************
* Function Name: SelfTest_Init_CSDv2_IDACA_Amux_A
******************************************************************************
*
* Summary:
*  Configures the CSDv2 block to output IDAC A to AMUX A
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
void SelfTest_Init_CSDv2_IDACA_Amux_A(CSD_Type* base)
{
    SelfTest_Clear_CSDv2_Regs(base);
    base->IDACA = CSDV2_IDACA_CONFIG;
    base->SW_BYP_SEL = CSDV2_SW_BYP_SEL_IDACA_TO_AMUXA;
    base->CONFIG = CSD_CONFIG_ENABLE_Msk | CSD_CONFIG_SENSE_EN_Msk;
    SelfTest_IDACA_SetValue(base, ANALOG_CSD_IDAC_VALUE1);
}


/*****************************************************************************
* Function Name: SelfTest_Init_CSDv2_IDACB_Amux_B
******************************************************************************
*
* Summary:
*  Configures the CSDv2 block to output IDAC B to AMUX B
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
void SelfTest_Init_CSDv2_IDACB_Amux_B(CSD_Type* base)
{
    SelfTest_Clear_CSDv2_Regs(base);
    base->IDACB = CSDV2_IDACB_CONFIG;
    base->SW_REFGEN_SEL = CSDV2_REF_GEN_SEL_IDACB_TO_AMUXB;
    base->SW_BYP_SEL = CSDV2_SW_BYP_SEL_IDACB_TO_AMUXB;
    base->CONFIG = CSD_CONFIG_ENABLE_Msk | CSD_CONFIG_SENSE_EN_Msk;
    SelfTest_IDACB_SetValue(base, ANALOG_CSD_IDAC_VALUE2);
}


/*****************************************************************************
* Function Name: SelfTest_Init_CSDv2_Dual_IDAC_Out
******************************************************************************
*
* Summary:
*  Configures the CSDv2 block to output IDAC A to Amux A and IDAC B to Amux B
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*
* return:
*  void
*
* Note:
*
*****************************************************************************/
void SelfTest_Init_CSDv2_Dual_IDAC_Out(CSD_Type* base)
{
    SelfTest_Clear_CSDv2_Regs(base);
    base->IDACB |= CSDV2_IDACB_CONFIG;
    base->SW_REFGEN_SEL |= CSDV2_REF_GEN_SEL_IDACB_TO_AMUXB;

    base->SW_BYP_SEL |= CSDV2_SW_BYP_SEL_IDACB_TO_AMUXB;
    base->IDACA |= CSDV2_IDACA_CONFIG;
    base->SW_BYP_SEL |= CSDV2_SW_BYP_SEL_IDACA_TO_AMUXA;
    SelfTest_IDACA_SetValue(base, ANALOG_CSD_IDAC_VALUE1);
    SelfTest_IDACB_SetValue(base, ANALOG_CSD_IDAC_VALUE2);
    base->CONFIG |= CSD_CONFIG_ENABLE_Msk | CSD_CONFIG_SENSE_EN_Msk;
}


/*****************************************************************************
* Function Name: SelfTest_IDACA_SetValue
******************************************************************************
*
* Summary:
*  Sets the IDACB register value to configure the current.
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*  uint8_t value - value to place in the IDACA register field
*
* Return:
*   NONE
*
* Note:
*
*****************************************************************************/
void SelfTest_IDACA_SetValue(CSD_Type* base, uint8_t value)
{
    base->IDACA &=  ~(uint32_t)CSD_IDACA_VAL_Msk;
    base->IDACA |= ((((uint32_t)value) << CSD_IDACA_VAL_Pos) & CSD_IDACA_VAL_Msk);
}


/*****************************************************************************
* Function Name: SelfTest_IDACB_SetValue
******************************************************************************
*
* Summary:
*  Sets the IDACA register value to configure the current.
*
* Parameters:
*  CSD_Type* base - The pointer to a CSD HW block base address.
*  uint8_t value - value to place in the IDACB register field
*
* Return:
*   NONE
*
* Note:
*
*****************************************************************************/
void SelfTest_IDACB_SetValue(CSD_Type* base, uint8_t value)
{
    base->IDACB &=  ~(uint32_t)CSD_IDACB_VAL_Msk;
    base->IDACB |= ((((uint32_t)value) << CSD_IDACB_VAL_Pos) & CSD_IDACB_VAL_Msk);
}


/*****************************************************************************
* Function Name: SelfTests_IDACA_Analog_Calibration
******************************************************************************
* Summary:
*  Determines IDACA measurement system offset to be later used in ADC and
*  Opamp self tests.
*
* Parameters:
*  CSD_Type* csd_base - The pointer to a CSD HW block base address.
*  SAR_Type* sar_base - The pointer to a SAR ADC HW block base address.
*
* Return:
*  0 - calibration pass
*  1 - calibration failed
*
* Theory:
*  This function stores the offset measured by the ADC and stores the IDAC offset
*  for future calculations.
*
* Note:
*   This function assumes that the ADC and CSD blocks are already configured such
*   that the CSD block is configured as an IDAC attached to an external pull
*   down resistor, and the SAR ADC is reading the voltage generated by the IDAC.
*
*   This function is only needed for the Opamp and ADC Self-Test for
*   ANALOG_TEST_VREF_CSD_IDAC mode.
*
*   This function and Self-Test_IDACB_Analog_Calibration write to the same global
*   offset variable since only one IDAC is needed for ADC and Opamp Self-Test.
*
*****************************************************************************/
uint8_t SelfTests_IDACA_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base)
{
    uint8_t ret = ERROR_STATUS;

    /* Set test value for IDACA */
    SelfTest_IDACA_SetValue(csd_base, 0x00);

    /* Delay to stabilize IDAC output */
    Cy_SysLib_DelayUs(IDAC_SETTLE_TIME);

    ret = SelfTests_Set_IDAC_Offset(sar_base, ANALOG_ADC_CHNL_IDACA);

    return ret;
}


/*****************************************************************************
* Function Name: SelfTests_IDACB_Analog_Calibration
******************************************************************************
* Summary:
*  Determines IDACB measurement system offset to be later used in ADC and
*  Opamp self tests.
*
* Parameters:
*  CSD_Type* csd_base - The pointer to a CSD HW block base address.
*  SAR_Type* sar_base - The pointer to a SAR ADC HW block base address.
*
* Return:
*  0 - calibration pass
*  1 - calibration failed
*
* Theory:
*  This function stores the offset measured by the ADC and stores the IDAC offset
*  for future calculations.
*
* Note:
*   This function assumes that the ADC and CSD blocks are already configured such
*   that the CSD block is configured as an IDAC attached to an external pull
*   down resistor, and the SAR ADC is reading the voltage generated by the IDAC.
*
*   This function is only needed for the Opamp and ADC Self-Test for
*   ANALOG_TEST_VREF_CSD_IDAC mode.
*
*   This function and SelfTest_IDACA_Analog_Calibration write to the same global
*   offset variable since only one IDAC is needed for ADC and Opamp Self-Test.
*
*****************************************************************************/
uint8_t SelfTests_IDACB_Analog_Calibration(CSD_Type* csd_base, SAR_Type* sar_base)
{
    uint8_t ret = ERROR_STATUS;

    /* Set test value for IDACB */
    SelfTest_IDACB_SetValue(csd_base, 0x00);

    /* Delay to stabilize IDAC output */
    Cy_SysLib_DelayUs(IDAC_SETTLE_TIME);

    ret = SelfTests_Set_IDAC_Offset(sar_base, ANALOG_ADC_CHNL_IDACB);

    return ret;
}


/*****************************************************************************
* Function Name: SelfTests_Set_IDAC_Offset
******************************************************************************
* Summary:
*  Determines DAC measurement system offset to be later used in DAC/ADC self tests.
*
* Parameters:
*  SAR_Type* sar_base - The pointer to a SAR ADC HW block base address.
*  uint32_t channel - The channel the voltage needs to be read from.
*
* Return:
*  0 - calibration pass
*  1 - calibration failed
*
* Theory:
*  This function stores the offset measured by the ADC and stores the IDAC offset
*  for future calculations.
*
* Note:
*   This function assumes that the ADC and CSD blocks are already configured such
*   that the CSD block is configured as an IDAC attached to an external pull
*   down resistor, and the SAR ADC is reading the voltage generated by the IDAC.
*
*   This function is only needed for the Opamp and ADC Self-Test for
*   ANALOG_TEST_VREF_CSD_IDAC mode.
*
*****************************************************************************/
static uint8_t SelfTests_Set_IDAC_Offset(SAR_Type* sar_base, uint32_t channel)
{
    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Start ADC conversion */
    Cy_SAR_StartConvert(sar_base, CY_SAR_START_CONVERT_SINGLE_SHOT);

    /* Wait for end of conversion using guard interval > ADC conversion time */
    guardCnt = 0u;
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while((Cy_SAR_IsEndConversion(sar_base,
                                    CY_SAR_RETURN_STATUS) != CY_SAR_SUCCESS) &&
            (guardCnt < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if (guardCnt < ADC_TEST_CON_TIME_uS)
    {
        dacOffset = Cy_SAR_GetResult16(sar_base, channel);
        dacOffset = Cy_SAR_CountsTo_mVolts(sar_base, channel, dacOffset);
        ret = OK_STATUS;
    }
    return ret;
}


#endif /* ANLAOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */

/*****************************************************************************
* Function Name: SelfTests_ADC
******************************************************************************
*
* Summary:
*  Test ADC using internal or external voltage references.
*
* Parameters:
*  SAR_Type* base - Pointer to structure describing SAR instance registers
*  uint32_t channel - number of the channel that will be reading the voltage
*  int16_t expected_res - the expected result in mV
*  int16_t accuracy - threshold above and below the expected value that is
*   acceptable.
*
* return:
*  0 - test pass
*  1 - test failed
*
* Theory:
*  This function uses the SAR ADC and reads "known" voltage references
*  to test the ADC functionality. User must configure the ADC and
*  the input voltage before the self test.
*
*****************************************************************************/
#ifdef CLASSB_SELF_TEST_ADC
uint8_t SelfTests_ADC(SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy)
{
    int16_t adc_res = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Start ADC conversion */
    Cy_SAR_StartConvert(base, CY_SAR_START_CONVERT_SINGLE_SHOT);

    /* Wait for end of conversion using guard interval > ADC conversion time */
    guardCnt = 0u;
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while((Cy_SAR_IsEndConversion(base,
                                    CY_SAR_RETURN_STATUS) != CY_SAR_SUCCESS) &&
            (guardCnt < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if (guardCnt < ADC_TEST_CON_TIME_uS)
    {
        /* Read value from ADC */
        adc_res = Cy_SAR_GetResult16(base, channel);
        adc_res = Cy_SAR_CountsTo_mVolts(base, channel, adc_res);
        #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
        if (adc_res > dacOffset)
        {
            adc_res = adc_res - dacOffset;
        }
        #endif
        #if ERROR_IN_ADC
        adc_res += 2*ANALOG_ADC_ACURACCY;
        #endif
        ret = OK_STATUS;
    }


    /* Check that measured results are in range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#endif /* CLASSB_SELF_TEST_ADC defined */


/*****************************************************************************
* Function Name: SelfTests_Opamp
******************************************************************************
*
* Summary:
*  Test Opamp using ADC and GPIO as a mux to connect to voltage references on
*  AMUXBUS A, AMUXBUS B.
*
* Parameters:
*  SAR_Type* sar_base - Pointer to structure describing SAR instance registers
*  int16_t expected_res - expected voltage in mV
*  int16_t accuracy - threshold above and below the expected value that is
*          acceptable.
*
* return:
*  0 - test pass
*  1 - test failed
*
* Theory:
*  This function uses the GPIO as a mux to feed AMUXBUS A then AMUXBUSB into
*  into the Opamp for testing. User must configure the ADC, Opamp, and
*  input voltage, and routing before the self test.
*
*****************************************************************************/
#ifdef CLASSB_SELF_TEST_OPAMP
uint8_t SelfTests_Opamp(SAR_Type* sar_base, int16_t expected_res, int16_t accuracy)
{
    int16_t adc_res = 0;
    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;


    /* Start ADC conversion */
    Cy_SAR_StartConvert(sar_base, CY_SAR_START_CONVERT_SINGLE_SHOT);

    /* Wait for end of conversion using guard interval > ADC conversion time */
    guardCnt = 0u;
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while((Cy_SAR_IsEndConversion(sar_base,
                                    CY_SAR_RETURN_STATUS) != CY_SAR_SUCCESS) &&
            (guardCnt < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if (guardCnt < ADC_TEST_CON_TIME_uS)
    {
        /* Read value from ADC */
        adc_res = Cy_SAR_GetResult16(sar_base, ANALOG_ADC_CHNL_OPAMP);
        adc_res = Cy_SAR_CountsTo_mVolts(sar_base, ANALOG_ADC_CHNL_OPAMP, adc_res);
        #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
        if (adc_res > dacOffset)
        {
            adc_res = adc_res - dacOffset;
        }
        #endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */
        #if (ERROR_IN_OPAMP == 1)
        adc_res = 0xFFFF;
        #endif /* ERROR_IN_OPAMP == 1 */

        ret = OK_STATUS;
    }

    /* Check that measured results are in range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#endif /* end CLASSB_SELF_TEST_OPAMP defined */



/*****************************************************************************
* Function Name: SelfTests_Comparator
******************************************************************************
* Summary:
*  Test Analog comparator using GPIO to connect to two voltage references on
*  AMUXBUS A and AMUXBUS B.
*
* Parameters:
*  LPCOMP_Type const* lpcomp_base - The low-power comparator registers
*     structure-pointer.
*  cy_en_lpcomp_channel_t lpcomp_channel - The low-power comparator channel number.
*  uint8_t expected_res - expected output of the Comparator. non-zero value when the
*   it is expected that the positive input is greater than the negative input voltage.
* return:
*  0 - test pass
*  1 - test failed
*
* Theory:
*  This function uses the GPIO as a mux to feed AMUXBUS A then AMUXBUSB
*  into the comparator for testing.  User must configure the comparator
*  input voltage before the self test.
*
*****************************************************************************/
#ifdef CLASSB_SELF_TEST_COMP
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res)
{
    uint32_t compOut;
    uint8_t ret = OK_STATUS;

    /* Get comparator result */
    compOut = Cy_LPComp_GetCompare(lpcomp_base, lpcomp_channel);

    #if ERROR_IN_COMPARATOR
    compOut = (compOut == 0) ? 1u : 0u;
    #endif

    /* Check result */
    if (((expected_res == 0u) && (compOut != 0u)) ||
        ((expected_res != 0u) && (compOut == 0u)))
    {
        ret = ERROR_STATUS;
    }

    return ret;
}


#endif /* CLASSB_SELF_TEST_COMP defined */


/* [] END OF FILE */

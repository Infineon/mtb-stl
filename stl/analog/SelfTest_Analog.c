/*******************************************************************************
* File Name: SelfTest_Analog.c
*
* Description:
*  This file provides source code for the APIs to perform analog component
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
#include "SelfTest_Analog.h"
#include "SelfTest_ErrorInjection.h"


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
*  Attempts to acquire and configure the MSC block in order to output the internal
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
*  Attempts to acquire and configure the MSC block in order to output the internal
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
static int16_t stlAnalog_dacOffset = 0;
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
*  uint8_t value - The value to be placed in the IDACA register field.
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
*  uint8_t value - The value to be placed in the IDACB register field.
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
*  Determines the IDACA measurement system-offset to use it in ADC and
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

    /* Set the test value for IDACA */
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

    /* Set the test value for IDACB */
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
*  Determines the DAC measurement system-offset to use it in DAC/ADC self tests.
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

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
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
        stlAnalog_dacOffset = Cy_SAR_GetResult16(sar_base, channel);
        stlAnalog_dacOffset = Cy_SAR_CountsTo_mVolts(sar_base, channel, stlAnalog_dacOffset);
        ret = OK_STATUS;
    }
    return ret;
}


#endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */


/*****************************************************************************
* Function Name: SelfTests_ADC
******************************************************************************
*
* Summary:
*  Test ADC using internal or external voltage references.
*
* Parameters:
*  SAR_Type* base - The pointer to a structure that describes SAR instance registers.
*  uint32_t channel - The number of the channel to read the voltage.
*  int16_t expected_res - The expected result in mV
*  int16_t accuracy - The threshold above and below the expected acceptable value.
*
* return:
*  0 - test pass
*  1 - test failed
*
* Theory:
*  This function uses the SAR ADC and reads "known" voltage references
*  to test the ADC functionality. The user must configure the ADC and
*  the input voltage before the self test.
*
*****************************************************************************/
#if defined(CY_IP_M0S8PASS4A_SAR) || defined(CY_IP_MXS40PASS_SAR)
uint8_t SelfTests_ADC(SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV)
{
    (void)vbg_channel;
    int16_t adc_res = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;
    /* Start ADC conversion */
    Cy_SAR_StartConvert(base, CY_SAR_START_CONVERT_SINGLE_SHOT);

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
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
        /* Read the value from ADC */
        adc_res = Cy_SAR_GetResult16(base, channel);
        if (count_to_mV)
        {
            adc_res = Cy_SAR_CountsTo_mVolts(base, channel, adc_res);
        }

        #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
        if (adc_res > stlAnalog_dacOffset)
        {
            adc_res = adc_res - stlAnalog_dacOffset;
        }
        #endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */

        #if ERROR_IN_ADC
        adc_res += 2*ANALOG_ADC_ACCURACY;
        #endif
        ret = OK_STATUS;
    }


    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#elif defined (CY_IP_MXS40EPASS_ESAR)
uint8_t SelfTests_ADC(PASS_SAR_Type* base, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV)
{
    float adc_res = 0.0f;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Obtaining conversion results in counts */
    uint16_t resultVBG;
    uint16_t resultAN0;

    /* Trigger a software start */
    Cy_SAR2_Channel_SoftwareTrigger(base, channel);

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
    guardCnt = 0u;
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while((CY_SAR2_INT_GRP_DONE != Cy_SAR2_Channel_GetInterruptStatus(base, vbg_channel)) &&
            (guardCnt < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if (guardCnt < ADC_TEST_CON_TIME_uS)
    {
        /* Read the value from ADC */
        if (count_to_mV)
        {
            resultVBG = Cy_SAR2_Channel_GetResult(base, vbg_channel, NULL);
        }
        resultAN0 = Cy_SAR2_Channel_GetResult(base, channel, NULL);
        /* Clear the interrupt source */
        Cy_SAR2_Channel_ClearInterrupt(base, channel, CY_SAR2_INT_GRP_DONE);

        if (count_to_mV)
        {
            /* Calculate the conversion results in volts */
            adc_res = ((float)resultAN0 * 900.0f) / (float)resultVBG;
        }
        else
        {
            adc_res = (float)resultAN0;
        }

        #if ERROR_IN_ADC
        adc_res += 2*ANALOG_ADC_ACCURACY;
        #endif
        ret = OK_STATUS;
    }

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        float exp_min_acc = (float)expected_res - (float)accuracy;
        float exp_pl_acc = (float)expected_res + (float)accuracy;
        if ((adc_res < exp_min_acc) || (adc_res > exp_pl_acc))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#elif defined(CY_IP_MXS40MCPASS)
uint8_t SelfTests_ADC(uint32_t group, uint32_t channel, int16_t expected_res, int16_t accuracy,
                      uint32_t vbg_channel, bool count_to_mV)
{
    (void)vbg_channel;
    (void)count_to_mV;
    (void)group;
    int16_t adc_res = 0;
    uint32_t result_status = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Start the HPPASS autonomous controller (AC) from state 0, do not wait for HPPASS block to be ready */
    if (CY_HPPASS_SUCCESS != Cy_HPPASS_AC_Start(0U, 0U))
    {
        CY_ASSERT(0);
    }

    /* Check SAR ADC busy status */
    while (Cy_HPPASS_SAR_IsBusy())
    {
    }
    Cy_SysLib_Delay(100u);
    /* Start ADC conversion */
    /* Trigger SAR ADC */
    if (CY_HPPASS_SUCCESS != Cy_HPPASS_SetFwTrigger((uint8_t)channel))
    {
        CY_ASSERT(0);
    }

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
    guardCnt = 0u;

    do
    {
        guardCnt++;
        result_status = Cy_HPPASS_SAR_Result_GetStatus();
    } while(((result_status & (1UL << channel)) == 0U) && (guardCnt < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if (guardCnt < ADC_TEST_CON_TIME_uS)
    {
        /* Get the channel data */
        adc_res = (int16_t)Cy_HPPASS_SAR_Result_ChannelRead((uint8_t)channel);

        #if ERROR_IN_ADC
        adc_res += 2*ANALOG_ADC_ACCURACY;
        #endif
        ret = OK_STATUS;
    }

    /* Clear the result status */
    Cy_HPPASS_SAR_Result_ClearStatus(1UL << channel);

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


uint8_t SelfTests_ADC_TrigIn(uint32_t group, uint32_t channel, int32_t expected_res, int32_t accuracy, uint32_t trig_in)
{
    (void)group;
    int32_t adc_res = 0;
    uint32_t result_status = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Start the HPPASS autonomous controller (AC) from state 0, do not wait for HPPASS block to be ready */
    if (CY_HPPASS_SUCCESS == Cy_HPPASS_AC_Start(0U, 0U))
    {
        /* Time for system readiness */
        Cy_SysLib_Delay(40u);

        /* Check SAR ADC busy status */
        guardCnt = 0u;
        while (Cy_HPPASS_SAR_IsBusy() && (guardCnt < ADC_TEST_CON_TIME_uS))
        {
            Cy_SysLib_DelayUs(1u);
            guardCnt++;
        }

        if (guardCnt < ADC_TEST_CON_TIME_uS)
        {
            /* Start ADC conversion */
            /* Trigger SAR ADC */
            if (CY_HPPASS_SUCCESS == Cy_HPPASS_SetFwTrigger((uint8_t)trig_in))
            {
                /* Wait for the end of conversion using a guard interval > ADC conversion time */
                guardCnt = 0u;
                do
                {
                    Cy_SysLib_DelayUs(1u);
                    result_status = Cy_HPPASS_SAR_Result_GetStatus();
                    guardCnt++;
                } while(((result_status & (1UL << channel)) == 0U) && (guardCnt < ADC_TEST_CON_TIME_uS));

                if (guardCnt < ADC_TEST_CON_TIME_uS)
                {
                    /* Get the channel data */
                    adc_res = Cy_HPPASS_SAR_Result_ChannelRead((uint8_t)channel);

                    #if ERROR_IN_ADC
                    adc_res += 2*ANALOG_ADC_ACCURACY;
                    #endif
                    ret = OK_STATUS;
                }

                /* Clear the result status */
                Cy_HPPASS_SAR_Result_ClearStatus(1UL << channel);
            }
        }
    }

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#if defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u)
/*******************************************************************************
* Static HPPASS Configuration for DAC R2R Self-Test
*
* These structures replace the configuration generated from design.modus so
* that SelfTests_DAC_TrigIn() is independent of cycfg_peripherals.c.
*
* Hardware topology:
*   DAC 0 output → AN_A5 pin → HPPASS AROUTE → SAR direct sampler 5 → SAR channel 5
*                             SAR group 0, firmware trigger 0 (CY_HPPASS_TRIG_0_MSK)
*   DAC 1 output → AN_B5 pin → HPPASS AROUTE → SAR muxed sampler 13 (AIO_B_5, MUX1_SEL=0) → SAR channel 18
*                             SAR group 1, firmware trigger 1 (CY_HPPASS_TRIG_1_MSK)
*******************************************************************************/
/** HPPASS startup clock divider for CLK_HF = 180 MHz (CLK_INFRA = 90 MHz).
 *  Chosen so that each startup count tick is ~200 ns: 18 * (1/90 MHz) = 200 ns. */
#define STL_HPPASS_CLK_HF_DIV           (18U)
/** CLK_INFRA = CLK_HF / 2 in MHz. Used to derive the startup tick period. */
#define STL_HPPASS_CLK_INFRA_MHZ        (90U)
/** Startup clock tick period in nanoseconds (= STL_HPPASS_CLK_HF_DIV / CLK_INFRA). */
#define STL_HPPASS_STARTUP_TICK_NS      ((STL_HPPASS_CLK_HF_DIV * 1000U) / STL_HPPASS_CLK_INFRA_MHZ)
/** Desired SAR power-up delay in microseconds. */
#define STL_HPPASS_SAR_STARTUP_US       (40U)
/** startup[].count value: hardware adds 1, so actual delay =
 *  (STL_HPPASS_SAR_STARTUP_COUNT + 1) * STL_HPPASS_STARTUP_TICK_NS ≈ STL_HPPASS_SAR_STARTUP_US µs. */
#define STL_HPPASS_SAR_STARTUP_COUNT    ((STL_HPPASS_SAR_STARTUP_US * 1000U) / STL_HPPASS_STARTUP_TICK_NS)
#define STL_DAC0_OUT_SAMP_MSK           ((uint16_t)(1U << SELFTEST_HPPASS_DAC0_R2R_SAR_CHAN_IDX))
/** DAC1: muxed sampler 13 (bit 1 of muxSampEnMsk/muxSampMsk), MUX1_SEL=0 → AIO_B_5 → AN_B5.
 *  DAC buffer 1 output (bufferOutputSelect=0) routes to AIO_B_5 input of sampler 13. */
#define STL_DAC1_OUT_MUX_SAMP_MSK       (0x2U)
/** MUX1_SEL value 0 = AIO_B_5 (AN_B5); result channel for sampler 13 = 18 + 0 = 18. */
#define STL_DAC1_OUT_MUX_CHAN_IDX       (0U)
/** DAC load mode */
#define STL_DAC_BUF_PWR_ULTRA           (3U)
/** DAC settling time */
#define DAC_R2R_SETTLE_TIME_uS          (1000U)

static const cy_stc_hppass_ac_stt_t stlAnalog_dacR2rStt[] =
{
    {
        .condition      = (cy_en_hppass_condition_t)CY_HPPASS_CONDITION_FALSE,
        .action         = CY_HPPASS_ACTION_STOP,
        .branchStateIdx = 0U,
        .interrupt      = false,
        .count          = 1U,
        .gpioOutUnlock  = false,
        .gpioOutMsk     = 0U,
        .csgUnlock      =
        {
            false, false, false,
            false, false, false,
            false, false, false,
        },
        .csgEnable      =
        {
            false, false, false,
            false, false, false,
            false, false, false,
        },
        .csgDacTrig     =
        {
            false, false, false,
            false, false, false,
            false, false, false,
        },
        .sarUnlock      = true,
        .sarEnable      = true,
        .sarGrpMsk      = 0U,
        .sarMux         =
        {
            { false, 0U },
            { false, 0U },
            { false, 0U },
            { false, 0U },
        },
    },
};

/* SAR channel 5: DAC0 output readback (AN_A5, direct sampler 5) */
static const cy_stc_hppass_sar_chan_t stlAnalog_dacR2rSarChan0 =
{
    .diff       = false,
    .sign       = false,
    .rightAlign = true,
    .avg        = CY_HPPASS_SAR_AVG_DISABLED,
    .limit      = CY_HPPASS_SAR_LIMIT_DISABLED,
    .result     = true,
    .fifo       = CY_HPPASS_FIFO_DISABLED,
};

/* SAR group 0: DAC0 — direct sampler 5 (AN_A5), triggered by FW trigger 0 */
static const cy_stc_hppass_sar_grp_t stlAnalog_dacR2rSarGrp0 =
{
    .dirSampMsk = STL_DAC0_OUT_SAMP_MSK,
    .muxSampMsk = 0x0U,
    .muxChanIdx =
    {
        0U,
        0U,
        0U,
        0U,
    },
    .trig       = CY_HPPASS_SAR_TRIG_0,
    .sampTime   = CY_HPPASS_SAR_SAMP_TIME_0,
    .priority   = true,
    .continuous = false,
};

/* SAR channel 18: DAC1 output readback (AN_B5, muxed sampler 13, MUX1_SEL=0 → AIO_B_5) */
static const cy_stc_hppass_sar_chan_t stlAnalog_dacR2rSarChan1 =
{
    .diff       = false,
    .sign       = false,
    .rightAlign = true,
    .avg        = CY_HPPASS_SAR_AVG_DISABLED,
    .limit      = CY_HPPASS_SAR_LIMIT_DISABLED,
    .result     = true,
    .fifo       = CY_HPPASS_FIFO_DISABLED,
};

/* SAR group 1: DAC1 — muxed sampler 13 (AIO_B_5 / AN_B5), triggered by FW trigger 1.
 * MUX1_SEL=0 selects AIO_B_5; muxChanIdx[1]=0; result channel = 18 + 0 = 18. */
static const cy_stc_hppass_sar_grp_t stlAnalog_dacR2rSarGrp1 =
{
    .dirSampMsk = 0x0U,
    .muxSampMsk = STL_DAC1_OUT_MUX_SAMP_MSK,
    .muxChanIdx =
    {
        0U,
        STL_DAC1_OUT_MUX_CHAN_IDX,
        0U,
        0U,
    },
    .trig       = CY_HPPASS_SAR_TRIG_1,
    .sampTime   = CY_HPPASS_SAR_SAMP_TIME_0,
    .priority   = true,
    .continuous = false,
};

static const cy_stc_hppass_sar_t stlAnalog_dacR2rSar =
{
    .vref         = CY_HPPASS_SAR_VREF_VDDA,
    .lowSupply    = false,
    .offsetCal    = false,
    .linearCal    = false,
    .gainCal      = false,
    .chanId       = false,
    .aroute       = true,
    .dirSampEnMsk = STL_DAC0_OUT_SAMP_MSK,
    .muxSampEnMsk = STL_DAC1_OUT_MUX_SAMP_MSK,
    .holdCount    = 0xFFU,
    .dirSampGain  =
    {
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
    },
    .muxSampGain  =
    {
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
        CY_HPPASS_SAR_SAMP_GAIN_1, CY_HPPASS_SAR_SAMP_GAIN_1,
    },
    .sampTime     =
    {
        512U,
        32U,
        32U,
    },
    .chan         =
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &stlAnalog_dacR2rSarChan0, /* ch  5 = DAC0 (AN_A5, direct sampler 5) */
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &stlAnalog_dacR2rSarChan1, /* ch 18 = DAC1 (AN_B5, muxed sampler 13, MUX1_SEL=0) */
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
    .grp          =
    {
        &stlAnalog_dacR2rSarGrp0, /* group 0 - DAC0 (AN_A5), trigger 0 */
        &stlAnalog_dacR2rSarGrp1, /* group 1 - DAC1 (AN_B5), trigger 1 */
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
    .limit        =
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
    .muxMode      =
    {
        CY_HPPASS_SAR_MUX_SEQ, CY_HPPASS_SAR_MUX_SEQ,
        CY_HPPASS_SAR_MUX_SEQ, CY_HPPASS_SAR_MUX_SEQ,
    },
    .fir          =
    {
        NULL,
        NULL,
    },
    .fifo         = NULL,
};

static const cy_stc_hppass_cfg_t stlAnalog_dacR2rHppassCfg =
{
    .ac                               =
    {
        .sttEntriesNum                = 1U,
        .stt                          = stlAnalog_dacR2rStt,
        .gpioOutEnMsk                 = 0U,
        .startupClkDiv                = STL_HPPASS_CLK_HF_DIV,
        .startup                      =
        {
            {
                .count                = STL_HPPASS_SAR_STARTUP_COUNT,
                .sar                  = true,
                .csgChan              = false,
                .csgSlice             = false,
                .csgReady             = false,
            },
            {
                .count                = 0U,
                .sar                  = false,
                .csgChan              = false,
                .csgSlice             = false,
                .csgReady             = false,
            },
            {
                .count                = 0U,
                .sar                  = false,
                .csgChan              = false,
                .csgSlice             = false,
                .csgReady             = false,
            },
            {
                .count                = 0U,
                .sar                  = false,
                .csgChan              = false,
                .csgSlice             = false,
                .csgReady             = false,
            },
        },
    },
    .csg                              = NULL,
    .sar                              = &stlAnalog_dacR2rSar,
    .trigIn                           =
    {
        /* [0] DAC0 */
        {
            .type                     = CY_HPPASS_TR_FW_LEVEL,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        /* [1] DAC1 */
        {
            .type                     = CY_HPPASS_TR_FW_LEVEL,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
        {
            .type                     = CY_HPPASS_TR_DISABLED,
            .hwMode                   = CY_HPPASS_PULSE_ON_POS_DOUBLE_SYNC,
        },
    },
    .trigPulse                        =
    {
        CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED,
        CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED,
        CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED,
        CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED, CY_HPPASS_DISABLED,
    },
    .trigLevel                        =
    {
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
        {
            .syncBypass               = true,
            .compMsk                  = 0U,
            .limitMsk                 = 0U,
            .compRemapMsk             = 0U,
            .dacFifoBelowThresholdMsk = 0U,
        },
    },
    .vrefSel                          = 1U,
};

static const cy_stc_hppass_dac_buf_config_t stlAnalog_dacR2rBufCfg =
{
    .startTriggerSelect         = CY_HPPASS_DAC_TRIG_DISABLED,
    .updateTriggerSelect        = CY_HPPASS_DAC_TRIG_DISABLED,
    .mode                       = CY_HPPASS_DAC_BUF_MODE_BUFFERED,
    .continuousMode             = false,
    .skipTriggerEnable          = false,
    .paramSyncEnable            = false,
    .stepSize                   = 0U,
    .fifoAccessMode             = 0U,
    .paramSyncReady             = false,
    .valueA                     = 0U,
    .valueB                     = 0U,
    .updatePeriodInteger        = 0U,
    .updatePeriodFractional     = 0U,
    .powerEnable                = true,
    .comparatorEnable           = false,
    .compDirection              = CY_HPPASS_DAC_COMP_DIR_RISING,
    .comparatorHysteresisEnable = false,
    .enableGainBoost            = false,
    .referenceSelect            = 0,
    .bufferOutputSelect         = 0,
    .powerMode                  = STL_DAC_BUF_PWR_ULTRA,
    .chargePumpEnable           = true,
    .adcLoopbackChannel         = 0U,
};

uint8_t SelfTests_DAC_TrigIn(uint8_t dac_idx, uint32_t dac_val,
                             int32_t expected_res, int32_t accuracy)
{
    /* Derive SAR channel and FW trigger mask from the DAC index.
     *   DAC 0 → direct sampler 5 (AN_A5),              SAR ch  5, trigger 0
     *   DAC 1 → muxed sampler 13 MUX1_SEL=0 (AN_B5),  SAR ch 18, trigger 1 */
    uint32_t channel = (dac_idx == 0U) ?
                       SELFTEST_HPPASS_DAC0_R2R_SAR_CHAN_IDX :
                       SELFTEST_HPPASS_DAC1_R2R_SAR_CHAN_IDX;
    uint32_t trig_in = (dac_idx == 0U) ?
                       CY_HPPASS_TRIG_0_MSK :
                       CY_HPPASS_TRIG_1_MSK;

    int32_t adc_res = 0;
    uint32_t result_status = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* De-initialize HPPASS first: Cy_HPPASS_Init fails if AC is already running
     * (e.g. started by the application via init_cycfg_all). */
    Cy_HPPASS_DeInit();

    if (CY_RSLT_SUCCESS == Cy_HPPASS_Init(&stlAnalog_dacR2rHppassCfg))
    {
        /* Initialize and enable the requested DAC buffer */
        (void)Cy_HPPASS_DAC_Init(dac_idx, &stlAnalog_dacR2rBufCfg);
        Cy_HPPASS_DAC_Enable(dac_idx);

        /* Start the HPPASS autonomous controller (AC) from state 0 */
        if (CY_HPPASS_SUCCESS == Cy_HPPASS_AC_Start(0U, 0U))
        {
            /* Time for system readiness */
            Cy_SysLib_Delay(40u);

            /* Check DAC busy status. */
            guardCnt = 0u;
            while (!Cy_HPPASS_DAC_Is_Ready(dac_idx) && (guardCnt < DAC_R2R_SETTLE_TIME_uS))
            {
                Cy_SysLib_DelayUs(1u);
                guardCnt++;
            }

            if (guardCnt < DAC_R2R_SETTLE_TIME_uS)
            {
                /* Set DAC mode */
                Cy_HPPASS_DAC_ModeSet(dac_idx, CY_HPPASS_DAC_BUF_MODE_BUFFERED);

                /* Write DAC Buffer data directly. */
                Cy_HPPASS_DAC_WriteValue(dac_idx, dac_val);

                /* Wait for the data to finish loading. */
                guardCnt = 0u;
                while ((Cy_HPPASS_DAC_Buf_IsBusy(dac_idx) == true) && (guardCnt < DAC_R2R_SETTLE_TIME_uS))
                {
                    Cy_SysLib_DelayUs(1u);
                    guardCnt++;
                }

                if (guardCnt < DAC_R2R_SETTLE_TIME_uS)
                {
                    /* Check SAR ADC busy status */
                    guardCnt = 0u;
                    while (Cy_HPPASS_SAR_IsBusy() && (guardCnt < ADC_TEST_CON_TIME_uS))
                    {
                        Cy_SysLib_DelayUs(1u);
                        guardCnt++;
                    }

                    if (guardCnt < ADC_TEST_CON_TIME_uS)
                    {
                        /* Start ADC conversion */
                        /* Trigger SAR ADC */
                        if (CY_HPPASS_SUCCESS == Cy_HPPASS_SetFwTrigger((uint8_t)trig_in))
                        {
                            /* Wait for the end of conversion using a guard interval > ADC conversion time */
                            guardCnt = 0u;
                            do
                            {
                                Cy_SysLib_DelayUs(1u);
                                result_status = Cy_HPPASS_SAR_Result_GetStatus();
                                guardCnt++;
                            } while(((result_status & (1UL << channel)) == 0U) && (guardCnt < ADC_TEST_CON_TIME_uS));

                            if (guardCnt < ADC_TEST_CON_TIME_uS)
                            {
                                /* Get the channel data */
                                adc_res = Cy_HPPASS_SAR_Result_ChannelRead((uint8_t)channel);

                                #if ERROR_IN_DAC
                                adc_res += 2*ANALOG_ADC_ACCURACY;
                                #endif
                                ret = OK_STATUS;
                            }

                            /* Clear the result status */
                            Cy_HPPASS_SAR_Result_ClearStatus(1UL << channel);
                        }
                    }
                }
            }
        }
    }

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


uint8_t SelfTests_AFE_TrigIn(uint32_t group, uint32_t channel, int32_t expected_res, int32_t accuracy, uint32_t trig_in)
{
    (void)group;
    int32_t adc_res = 0;
    uint32_t result_status = 0;

    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;

    /* Start the HPPASS autonomous controller (AC) from state 0, do not wait for HPPASS block to be ready */
    if (CY_HPPASS_SUCCESS == Cy_HPPASS_AC_Start(0U, 0U))
    {
        /* Time for system readiness */
        Cy_SysLib_Delay(40u);

        /* Check SAR ADC busy status */
        guardCnt = 0u;
        while (Cy_HPPASS_SAR_IsBusy() && (guardCnt < ADC_TEST_CON_TIME_uS))
        {
            Cy_SysLib_DelayUs(1u);
            guardCnt++;
        }

        if (guardCnt < ADC_TEST_CON_TIME_uS)
        {
            /* Start ADC conversion */
            /* Trigger SAR ADC */
            if (CY_HPPASS_SUCCESS == Cy_HPPASS_SetFwTrigger((uint8_t)trig_in))
            {
                /* Wait for the end of conversion using a guard interval > ADC conversion time */
                guardCnt = 0u;
                do
                {
                    Cy_SysLib_DelayUs(1u);
                    result_status = Cy_HPPASS_SAR_Result_GetStatus();
                    guardCnt++;
                } while(((result_status & (1UL << channel)) == 0U) && (guardCnt < ADC_TEST_CON_TIME_uS));

                if (guardCnt < ADC_TEST_CON_TIME_uS)
                {
                    /* Get the channel data */
                    adc_res = Cy_HPPASS_SAR_Result_ChannelRead((uint8_t)channel);

                    #if ERROR_IN_ADC
                    adc_res += 2*ANALOG_ADC_ACCURACY;
                    #endif
                    ret = OK_STATUS;
                }

                /* Clear the result status */
                Cy_HPPASS_SAR_Result_ClearStatus(1UL << channel);
            }
        }
    }

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#endif /* defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION >= 3u) */
#endif /* defined(CY_IP_M0S8PASS4A_SAR) || defined(CY_IP_MXS40PASS_SAR) */


#if defined(CLASSB_SELF_TEST_ADC) && (defined(CY_IP_MXS40PASS_CTB) || defined(CY_IP_M0S8PASS4A_CTB))
/*****************************************************************************
* Function Name: SelfTests_Opamp
******************************************************************************
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
*  0 - Test passed <br>
*  1 - Test failed
*
*****************************************************************************/
uint8_t SelfTests_Opamp(SAR_Type* sar_base, int16_t expected_res, int16_t accuracy,
                        uint32_t opamp_in_channel, bool count_to_mV)
{
    int16_t adc_res = 0;
    uint16_t guardCnt;
    uint8_t ret = ERROR_STATUS;


    /* Start ADC conversion */
    Cy_SAR_StartConvert(sar_base, CY_SAR_START_CONVERT_SINGLE_SHOT);

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
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
        /* Read the value from ADC */
        adc_res = Cy_SAR_GetResult16(sar_base, opamp_in_channel);

        if (count_to_mV)
        {
            adc_res = Cy_SAR_CountsTo_mVolts(sar_base, opamp_in_channel, adc_res);
        }

        #if (ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC)
        if (adc_res > stlAnalog_dacOffset)
        {
            adc_res = adc_res - stlAnalog_dacOffset;
        }
        #endif /* ANALOG_TEST_VREF == ANALOG_TEST_VREF_CSD_IDAC */
        #if (ERROR_IN_OPAMP == 1)
        adc_res = 0xFFFF;
        #endif /* ERROR_IN_OPAMP == 1 */

        ret = OK_STATUS;
    }

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#endif /* defined(CLASSB_SELF_TEST_OPAMP) */


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
*  uint8_t expected_res - expected output of the Comparator. Use non-zero value when
*  expected that the positive input is greater than the negative input voltage.
* return:
*  0 - test pass
*  1 - test failed
*
* Theory:
*  This function uses the GPIO as a mux to feed AMUXBUS A then AMUXBUSB
*  into the comparator for testing.  The user must configure the comparator
*  input voltage before the self test.
*
*****************************************************************************/
#ifdef CLASSB_SELF_TEST_COMP
uint8_t SelfTests_Comparator(LPCOMP_Type const* lpcomp_base, cy_en_lpcomp_channel_t lpcomp_channel,
                             uint8_t expected_res)
{
    uint32_t compOut;
    uint8_t ret = OK_STATUS;

    /* Get the comparator result */
    compOut = Cy_LPComp_GetCompare(lpcomp_base, lpcomp_channel);

    #if ERROR_IN_COMPARATOR
    compOut = (compOut == 0) ? 1u : 0u;
    #endif

    /* Check the result */
    if (((expected_res == 0u) && (compOut != 0u)) ||
        ((expected_res != 0u) && (compOut == 0u)))
    {
        ret = ERROR_STATUS;
    }

    return ret;
}


#endif /* CLASSB_SELF_TEST_COMP defined */

#if defined(CY_IP_MXS40PASS_CTDAC)
/*****************************************************************************
* Function Name: SelfTests_DAC
******************************************************************************
*
* Summary:
*  Test DAC veriifies that the input to DAC and output from ADC are the same.
*  The digital input value to DAC maps to 1.5V
*
* Parameters:
*  CTDAC_Type* dacBase - The pointer to a structure that describes the DAC instance registers
*  SAR_Type* adcBase - The pointer to a structure that describes SAR ADC instance registers
*  uint32_t adcChannel - The SAR ADC channel number.
*
* return:
*  0 - test pass
*  1 - test failed
*
*****************************************************************************/
uint8_t SelfTests_DAC(CTDAC_Type* dacBase, SAR_Type* adcBase, uint32_t adcChannel)
{
    uint8_t  ret       = ERROR_STATUS;
    uint32_t adcResult = 0U;
    uint32_t dacInput  = DAC_INPUT;
    uint32_t deviation = (dacInput * DAC_ADC_OUTPUT_DEVIATION_PCT) / 100U;

    /* Set the DAC value  */
    Cy_CTDAC_SetValue(dacBase, (int32_t)dacInput);

    /* Start ADC conversion */
    Cy_SAR_StartConvert(adcBase, CY_SAR_START_CONVERT_CONTINUOUS);

    /* Wait for the end of conversion */
    do
    {
        Cy_SysLib_DelayUs(1U);
    } while((Cy_SAR_IsEndConversion(adcBase, CY_SAR_RETURN_STATUS) != CY_SAR_SUCCESS));
    /* Get the ADC output value */
    adcResult = (uint32_t)Cy_SAR_GetResult32(adcBase, adcChannel);
    #if (ERROR_IN_DAC == 1)
    adcResult = 0x1U;
    #endif /* ERROR_IN_DAC == 1 */

    /* Compare the result */
    if ((adcResult < (dacInput - deviation)) ||
        (adcResult > (dacInput + deviation)))
    {
        ret = ERROR_STATUS;
    }
    else
    {
        ret = OK_STATUS;
    }

    return ret;
}


#endif /* CY_IP_MXS40PASS_CTDAC defined */


#if defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION < 3u)
/*******************************************************************************
* Function Name: SelfTests_DAC_TrigIn
********************************************************************************
*
* Performs DAC test and verifies that input of DAC and output from ADC are same.
*
* Parameters :
* adc_channel -  The pointer to the ADC channel.
* dac_slice -  The pointer to DAC slice.
* dac_val -  The value to be loaded to the DAC register.
* expected_res - The channel expected result in ADC.
* accuracy -  Error tolerance
*
* Return :
*  0 - Test Passed
*  1 - Test Failed
*
*******************************************************************************/
/** DAC conversion time in test mode, uS */
#define DAC_TEST_CON_TIME_uS            (900u)
uint8_t SelfTests_DAC_TrigIn(uint32_t adc_channel, uint32_t dac_slice, uint32_t dac_val,
                             int16_t expected_res, int16_t accuracy, uint32_t adc_trig_in, uint32_t dac_trig_in)
{
    int16_t adc_res = 0;
    uint32_t result_status = 0;

    uint16_t guardCnt_ADC;
    uint16_t guardCnt_DAC;
    uint8_t ret = ERROR_STATUS;

    /* Start the HPPASS autonomous controller (AC) from state 0, do not wait for HPPASS block to be ready */
    if (CY_HPPASS_SUCCESS != Cy_HPPASS_AC_Start(0U, 0U))
    {
        CY_ASSERT(0);
    }

    /* Check the SAR ADC busy status */
    while (Cy_HPPASS_SAR_IsBusy())
    {
    }
    Cy_SysLib_Delay(100u);

    /* Set the DAC value */
    Cy_HPPASS_DAC_SetValue((uint8_t)dac_slice, (uint16_t)dac_val);

    /* Start DAC*/
    Cy_HPPASS_DAC_Start((uint8_t)dac_slice, CY_HPPASS_DAC_HW);

    /* Start DAC conversion */
    if (CY_HPPASS_SUCCESS != Cy_HPPASS_SetFwTrigger((uint8_t)dac_trig_in))
    {
        CY_ASSERT(0);
    }

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
    guardCnt_DAC = 0u;
    do
    {
        guardCnt_DAC++;
        Cy_SysLib_DelayUs(1u);
    } while((Cy_HPPASS_DAC_IsBusy(0) == true) && (guardCnt_DAC < DAC_TEST_CON_TIME_uS));

    Cy_HPPASS_DAC_Stop((uint8_t)dac_slice);

    /* Start ADC conversion */
    if (CY_HPPASS_SUCCESS != Cy_HPPASS_SetFwTrigger((uint8_t)adc_trig_in))
    {
        CY_ASSERT(0);
    }

    /* Wait for the end of conversion using a guard interval > ADC conversion time */
    guardCnt_ADC = 0u;

    do
    {
        guardCnt_ADC++;
        result_status = Cy_HPPASS_SAR_Result_GetStatus();
    } while(((result_status & (1UL << adc_channel)) == 0U) && (guardCnt_ADC < ADC_TEST_CON_TIME_uS));

    /* Check if timeout */
    if ((guardCnt_ADC < ADC_TEST_CON_TIME_uS) && (guardCnt_ADC < DAC_TEST_CON_TIME_uS))
    {
        /* Get the channel data */
        adc_res = (int16_t)Cy_HPPASS_SAR_Result_ChannelRead((uint8_t)adc_channel);

        #if ERROR_IN_ADC
        adc_res += 2*ANALOG_ADC_ACCURACY;
        #endif
        ret = OK_STATUS;
    }

    /* Clear the result status */
    Cy_HPPASS_SAR_Result_ClearStatus(1UL << adc_channel);

    /* Check that the measured results are in the range */
    if (ret == OK_STATUS)
    {
        if ((adc_res < (expected_res - accuracy)) || (adc_res > (expected_res + accuracy)))
        {
            ret = ERROR_STATUS;
        }
    }

    return ret;
}


#endif /* defined(CY_IP_MXS40MCPASS) && (CY_IP_MXS40MCPASS_VERSION < 3u) */

/* [] END OF FILE */

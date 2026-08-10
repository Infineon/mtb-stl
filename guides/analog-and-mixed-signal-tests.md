# Analog and Mixed-Signal Tests

This guide covers ADC, DAC, OPAMP, LPCOMP, and mixed-signal processing self-tests.

## ADC tests

### ADC TrigIn test

For MCPASS v3 devices, the supported ADC API in the device support matrix is `SelfTests_ADC_TrigIn()`.

The HPPASS internal CSG DAC drives the SAR input so no external stimulus is needed. The test exercises two voltage points (VDDA/3 and 2×VDDA/3) against fixed 12-bit expected results; both must fall within `ANALOG_ADC_ACURACCY` counts (default 100) to pass.

The following is an example of an ADC TrigIn self-test:

```c
uint8_t  status;
uint16_t guard;

/* --- Stimulus: VDDA/3 --- */
(void)Cy_HPPASS_AC_Start(0U, 0U);
while (Cy_HPPASS_SAR_IsBusy())
{
}
Cy_SysLib_Delay(100u);
Cy_HPPASS_DAC_SetValue((uint8_t)CYBSP_DAC_SLICE_IDX, ADC_TRIGIN_DAC_VDDA_3);
Cy_HPPASS_DAC_Start((uint8_t)CYBSP_DAC_SLICE_IDX, CY_HPPASS_DAC_HW);
(void)Cy_HPPASS_SetFwTrigger((uint8_t)CY_HPPASS_TRIG_1_MSK);
for (guard = 0u; Cy_HPPASS_DAC_IsBusy(0) && (guard < ADC_TRIGIN_DAC_SETTLE_US); guard++)
{
    Cy_SysLib_DelayUs(1u);
}

status = SelfTests_ADC_TrigIn(0u, CSG_VDAC_OUT_CHAN_IDX,
                              ADC_TRIGIN_EXP_VDDA_3, ANALOG_ADC_ACURACCY,
                              CY_HPPASS_TRIG_2_MSK);
Cy_HPPASS_DAC_Stop((uint8_t)CYBSP_DAC_SLICE_IDX);

if (OK_STATUS != status)
{
    /* Handle ADC TrigIn VDDA/3 test failure */
}

/* --- Stimulus: 2*VDDA/3 --- */
if (OK_STATUS == status)
{
    while (Cy_HPPASS_SAR_IsBusy())
    {
    }
    Cy_HPPASS_DAC_SetValue((uint8_t)CYBSP_DAC_SLICE_IDX, ADC_TRIGIN_DAC_2VDDA_3);
    Cy_HPPASS_DAC_Start((uint8_t)CYBSP_DAC_SLICE_IDX, CY_HPPASS_DAC_HW);
    (void)Cy_HPPASS_SetFwTrigger((uint8_t)CY_HPPASS_TRIG_1_MSK);
    for (guard = 0u; Cy_HPPASS_DAC_IsBusy(0) && (guard < ADC_TRIGIN_DAC_SETTLE_US); guard++)
    {
        Cy_SysLib_DelayUs(1u);
    }

    status = SelfTests_ADC_TrigIn(0u, CSG_VDAC_OUT_CHAN_IDX,
                                  ADC_TRIGIN_EXP_2VDDA_3, ANALOG_ADC_ACURACCY,
                                  CY_HPPASS_TRIG_2_MSK);
    Cy_HPPASS_DAC_Stop((uint8_t)CYBSP_DAC_SLICE_IDX);

    if (OK_STATUS != status)
    {
        /* Handle ADC TrigIn 2*VDDA/3 test failure */
    }
}
```

## DAC test

The DAC self-test sets the R2R DAC output to a known code and verifies the SAR ADC readback is within the acceptance window. The function starts the SAR conversion internally via the DAC's built-in trigger; no external settling delay is required in application code.

**Applies to:** MCPASS v3.

Both DAC instances are tested at VDD/3 and 2×VDD/3. The 12-bit DAC code and 12-bit SAR expected result are equal at each point (1365 and 2730 respectively). Both must fall within `ANALOG_ADC_ACURACCY` counts (default 100) to pass.

- DAC 0 output → AN_A5 → SAR channel 5, trigger 0
- DAC 1 output → AN_B5 → SAR channel 18, trigger 1

The following is an example of a DAC TrigIn self-test:

```c
uint8_t status;

/* --- DAC 0 (AN_A5, SAR ch 5, trigger 0): VDD/3 --- */
status = SelfTests_DAC_TrigIn(0U, DAC_TRIGIN_DAC_VDDA_3,
                              DAC_TRIGIN_EXP_VDDA_3, ANALOG_ADC_ACURACCY);
if (OK_STATUS != status)
{
    /* Handle DAC 0 VDD/3 test failure */
}

/* --- DAC 0: 2*VDD/3 --- */
if (OK_STATUS == status)
{
    status = SelfTests_DAC_TrigIn(0U, DAC_TRIGIN_DAC_2VDDA_3,
                                  DAC_TRIGIN_EXP_2VDDA_3, ANALOG_ADC_ACURACCY);
    if (OK_STATUS != status)
    {
        /* Handle DAC 0 2*VDD/3 test failure */
    }
}

/* --- DAC 1 (AN_B5, SAR ch 18, trigger 1): VDD/3 --- */
if (OK_STATUS == status)
{
    status = SelfTests_DAC_TrigIn(1U, DAC_TRIGIN_DAC_VDDA_3,
                                  DAC_TRIGIN_EXP_VDDA_3, ANALOG_ADC_ACURACCY);
    if (OK_STATUS != status)
    {
        /* Handle DAC 1 VDD/3 test failure */
    }
}

/* --- DAC 1: 2*VDD/3 --- */
if (OK_STATUS == status)
{
    status = SelfTests_DAC_TrigIn(1U, DAC_TRIGIN_DAC_2VDDA_3,
                                  DAC_TRIGIN_EXP_2VDDA_3, ANALOG_ADC_ACURACCY);
    if (OK_STATUS != status)
    {
        /* Handle DAC 1 2*VDD/3 test failure */
    }
}
```

### Generic ADC example

Analog tests verify ADC and comparator functionality by comparing measured values against known reference voltages.

The ADC test measures reference voltages such as VDDA/3 and VDDA/2 using a voltage divider and verifies the ADC reading is within tolerance. The test requires external resistor dividers to generate the reference voltages.

The following is an example of a self-test for analog tests:

```c
uint8_t status = SelfTests_ADC(0, 4, ANALOG_ADC_SAR_RESULT1, ANALOG_ADC_ACCURACY, 0, 0);

/* ADC Test - Test with VDDA/3 voltage */
/* Connect VDDA/3 signal to ADC channel 4 using a voltage divider */
if (OK_STATUS != status)
{
    /* Handle ADC test failure for VDDA/3 */
}

/* ADC Test - Test with VDDA/2 voltage */
/* Connect VDDA/2 signal to ADC channel 4 using a voltage divider */
if (OK_STATUS == status)
{
    status = SelfTests_ADC(0, 4, ANALOG_ADC_SAR_RESULT2, ANALOG_ADC_ACCURACY, 0, 0);
    if (OK_STATUS != status)
    {
        /* Handle ADC test failure for VDDA/2 */
    }
}

if (OK_STATUS == status)
{
    /* Initialize LPCOMP with Device Configurator generated structure */
    static cy_stc_lpcomp_context_t context;
    Cy_LPComp_Init_Ext(CYBSP_DUT_LPCOMP_HW, CYBSP_DUT_LPCOMP_CHANNEL,
                       &CYBSP_DUT_LPCOMP_config, &context);
    Cy_LPComp_Enable_Ext(CYBSP_DUT_LPCOMP_HW, CYBSP_DUT_LPCOMP_CHANNEL, &context);

    /* Configure AMUXBUS routing for LPCOMP inputs */
    if (CY_LPCOMP_CHANNEL_0 == CYBSP_DUT_LPCOMP_CHANNEL)
    {
        LPCOMP->CMP0_SW = LPCOMP_CMP0_SW_CMP0_AP0_Msk | LPCOMP_CMP0_SW_CMP0_BN0_Msk;
    }
    else if (CY_LPCOMP_CHANNEL_1 == CYBSP_DUT_LPCOMP_CHANNEL)
    {
        LPCOMP->CMP1_SW = LPCOMP_CMP1_SW_CMP1_AP1_Msk | LPCOMP_CMP1_SW_CMP1_BN1_Msk;
    }
    LPCOMP->CONFIG |= LPCOMP_CONFIG_ENABLED_Msk;

    /* Test with lower voltage on positive input */
    Cy_GPIO_Pin_FastInit(CYBSP_DUT_LPCOMP_VPLUS_PORT, CYBSP_DUT_LPCOMP_VPLUS_PIN,
                         CY_GPIO_DM_ANALOG, 0u, HSIOM_SEL_AMUXB);
    Cy_GPIO_Pin_FastInit(CYBSP_DUT_LPCOMP_VMINUS_PORT, CYBSP_DUT_LPCOMP_VMINUS_PIN,
                         CY_GPIO_DM_ANALOG, 0u, HSIOM_SEL_AMUXA);
    Cy_SysLib_Delay(1u);

    status = SelfTests_Comparator(CYBSP_DUT_LPCOMP_HW, CYBSP_DUT_LPCOMP_CHANNEL,
                                  ANALOG_COMP_RESULT2);
    if (OK_STATUS != status)
    {
        /* Handle LPCOMP lower voltage test failure */
    }
}

if (OK_STATUS == status)
{
    /* Test with higher voltage on positive input */
    Cy_GPIO_Pin_FastInit(CYBSP_DUT_LPCOMP_VPLUS_PORT, CYBSP_DUT_LPCOMP_VPLUS_PIN,
                         CY_GPIO_DM_ANALOG, 0u, HSIOM_SEL_AMUXA);
    Cy_GPIO_Pin_FastInit(CYBSP_DUT_LPCOMP_VMINUS_PORT, CYBSP_DUT_LPCOMP_VMINUS_PIN,
                         CY_GPIO_DM_ANALOG, 0u, HSIOM_SEL_AMUXB);
    Cy_SysLib_Delay(1u);

    status = SelfTests_Comparator(CYBSP_DUT_LPCOMP_HW, CYBSP_DUT_LPCOMP_CHANNEL,
                                  ANALOG_COMP_RESULT1);
    if (OK_STATUS != status)
    {
        /* Handle LPCOMP higher voltage test failure */
    }
}
```

## LPCOMP test

The low-power comparator test applies different voltages to the positive and negative inputs through AMUX and verifies that the comparator output matches the expected result.

Configure the LPCOMP in Device Configurator with the following settings:

- Set **Output Configuration** to `Direct`
- Set **Power/Speed** to `Low Power/Low`
- Enable **Hysteresis**
- Set **Pulse/Interrupt Configuration** to `Rising Edge`
- Leave **Local VREF input** disabled
- Set **Positive Input (inp)** to an analog-capable pin connected to AMUXBUS A
- Set **Negative Input (inn)** to an analog-capable pin connected to AMUXBUS B
- Leave **Comparator Output** unassigned

Both pins must be configured as analog drive mode. The VPLUS pin must be held at a higher voltage than the VMINUS pin. The test routes these two voltages through the AMUX buses in both directions: first applying the lower voltage to the LPCOMP positive input and then applying the higher voltage to verify that the comparator changes state correctly.

![Device Configurator LPCOMP settings showing Direct output, Low Power/Low speed, Hysteresis enabled, Rising Edge interrupt, and analog input pins routed through AMUXBUS A and AMUXBUS B](../images/analog_lpcomp_config.png)

## OPAMP test

The OPAMP test verifies the configured AFE/OPAMP signal path and gain. An internal DAC generates a controlled stimulus, the AFE/OPAMP amplifies it using the configured topology and gain, and the HPPASS SAR ADC measures the output. `SelfTests_AFE_TrigIn()` compares the conversion result with the expected SAR count within the supplied tolerance. No external connection is required on devices that support an equivalent internal DAC-to-AFE-to-SAR route.

### Device Configurator setup

Configure the HPPASS analog subsystem, including its autonomous controller (AC), AFE, DAC, SAR ADC, samplers, and input trigger. The test API starts AC state 0 and issues the firmware trigger; the generated configuration must make that state and trigger start the intended SAR measurement path.

Create an internal signal chain equivalent to:

`internal DAC stimulus -> AFE/OPAMP positive input -> amplified AFE output -> SAR muxed sampler -> SAR channel`

The instance and channel numbers are examples only; another device with `HPPASS v3` can use different indices when the same logical route is preserved.

- HPPASS instance `pass[0]` is enabled and initialized, with AC instance `pass[0].ac[0]` containing state 0.
- DAC `pass[0].dac[1]` is enabled in buffered mode with buffer output select 1. Device Configurator generates its configuration object as `cy_dac_1_config` in this example.
- AFE `pass[0].afe[0]` is enabled with configuration 0 selected. Configuration 0 uses `CY_HPPASS_AFE_INPUT_SRC_DAC` for the positive input, `CY_HPPASS_AFE_INPUT_SRC_GND` for the negative input, and `CY_HPPASS_AFE_INT_SEIN_SEOUT_PGA` output mode with `CY_HPPASS_AFE_GAIN_2`.
- SAR `pass[0].sar[0]` uses VDDA as its reference. Channel `pass[0].sar[0].ch[21]` is the AFE output result channel.
- Muxed sampler `pass[0].sar[0].muxed_sampler[1]` is enabled by SAR sequence group `pass[0].sar[0].seq[0].grp[0]`, which selects mux input 3 and uses `CY_HPPASS_SAR_TRIG_0`.
- HPPASS input trigger 0 is configured for `CY_HPPASS_TR_FW_PULSE`, matching the `CY_HPPASS_TRIG_0_MSK` firmware-trigger mask passed to the test API.

The names generated by Device Configurator depend on the aliases chosen in the design. Record the DAC configuration object, AFE output SAR channel, and firmware trigger mask used by the selected route, then use those same resources in the application code.

### Application-owned resources

The application owns the DAC instance and its generated configuration object, the AFE output/SAR result-channel index, trigger mask, DAC input codes, expected SAR results, conversion tolerance, and bounded DAC ready/update timeouts. The application also owns any resource restoration required after the test.

The reference values in the following setup are specific to a 12-bit DAC, a 12-bit SAR with VDDA reference, and the configured AFE topology and gain. Recalculate the DAC codes, expected SAR counts, and tolerance for a different DAC resolution, SAR resolution, gain, topology, or reference voltage.

```c
/* These values match the configured DAC-to-AFE-to-SAR route. */
#define STL_OPAMP_DAC_INDEX                 (1U)
#define STL_OPAMP_SAR_GROUP                 (0U)
#define STL_OPAMP_SAR_CHANNEL               (21U)
#define STL_OPAMP_TRIGGER_MASK              (CY_HPPASS_TRIG_0_MSK)
#define STL_OPAMP_FIRST_DAC_CODE            (1024U)
#define STL_OPAMP_SECOND_DAC_CODE           (455U)
#define STL_OPAMP_FIRST_EXPECTED_RESULT     (3071)
#define STL_OPAMP_SECOND_EXPECTED_RESULT    (1365)
#define STL_OPAMP_CONVERSION_TOLERANCE      (ANALOG_ADC_ACCURACY)
#define STL_OPAMP_DAC_READY_TIMEOUT_US      (1000U)
#define STL_OPAMP_DAC_UPDATE_TIMEOUT_US     (1000U)
#define STL_OPAMP_DAC_CONFIG                (&cy_dac_1_config)

static uint8_t stlOpampWriteDacStimulus(uint8_t dacIndex, uint32_t dacCode)
{
    uint32_t timeout = 0U;

    while ((!Cy_HPPASS_DAC_Is_Ready(dacIndex)) && (timeout < STL_OPAMP_DAC_READY_TIMEOUT_US))
    {
        Cy_SysLib_DelayUs(1U);
        timeout++;
    }

    if (!Cy_HPPASS_DAC_Is_Ready(dacIndex))
    {
        return ERROR_STATUS;
    }

    Cy_HPPASS_DAC_ModeSet(dacIndex, CY_HPPASS_DAC_BUF_MODE_BUFFERED);
    Cy_HPPASS_DAC_WriteValue(dacIndex, dacCode);

    timeout = 0U;
    while ((Cy_HPPASS_DAC_Buf_IsBusy(dacIndex)) && (timeout < STL_OPAMP_DAC_UPDATE_TIMEOUT_US))
    {
        Cy_SysLib_DelayUs(1U);
        timeout++;
    }

    return Cy_HPPASS_DAC_Buf_IsBusy(dacIndex) ? ERROR_STATUS : OK_STATUS;
}
```

`SelfTests_AFE_TrigIn()` does not initialize the DAC, select its mode, write its value, or create the AFE-to-SAR route. It starts HPPASS AC state 0, waits for the SAR conversion, issues the supplied firmware trigger, reads and clears the selected SAR result status, and checks the result against `expected_res +/- accuracy`.

Pass the configured SAR group as `group`, the AFE output result-channel index as `channel`, the calculated count as `expected_res`, the allowed count deviation as `accuracy`, and the firmware-trigger bit mask that starts the sequence group as `trig_in`.

### API sequence

Initialize and enable the selected DAC, start AC state 0, and wait for the DAC to become ready. Select buffered mode, write a stimulus code, and wait for the DAC buffer update using a bounded timeout. Call `SelfTests_AFE_TrigIn()` with the matching SAR group, result channel, expected result, tolerance, and trigger mask. Check its returned status, repeat with a second stimulus level where appropriate, then deinitialize or restore the DAC.

The following application-side sequence uses two levels and always deinitializes the DAC before returning. The DAC-ready and buffer-update loops are bounded; handle `ERROR_STATUS` as a failed preparation or self-test result.

```c
uint8_t status = OK_STATUS;

Cy_HPPASS_DAC_Init(STL_OPAMP_DAC_INDEX, STL_OPAMP_DAC_CONFIG);
Cy_HPPASS_DAC_Enable(STL_OPAMP_DAC_INDEX);

if (CY_HPPASS_SUCCESS != Cy_HPPASS_AC_Start(0U, 0U))
{
    status = ERROR_STATUS;
}

if (OK_STATUS == status)
{
    status = stlOpampWriteDacStimulus(STL_OPAMP_DAC_INDEX, STL_OPAMP_FIRST_DAC_CODE);
    if (OK_STATUS != status)
    {
        /* Handle first DAC stimulus preparation failure */
    }
}

if (OK_STATUS == status)
{
    status = SelfTests_AFE_TrigIn(STL_OPAMP_SAR_GROUP, STL_OPAMP_SAR_CHANNEL,
                                  STL_OPAMP_FIRST_EXPECTED_RESULT,
                                  STL_OPAMP_CONVERSION_TOLERANCE,
                                  STL_OPAMP_TRIGGER_MASK);
    if (OK_STATUS != status)
    {
        /* Handle first OPAMP self-test failure */
    }
}

if (OK_STATUS == status)
{
    status = stlOpampWriteDacStimulus(STL_OPAMP_DAC_INDEX, STL_OPAMP_SECOND_DAC_CODE);
    if (OK_STATUS != status)
    {
        /* Handle second DAC stimulus preparation failure */
    }
}

if (OK_STATUS == status)
{
    status = SelfTests_AFE_TrigIn(STL_OPAMP_SAR_GROUP, STL_OPAMP_SAR_CHANNEL,
                                  STL_OPAMP_SECOND_EXPECTED_RESULT,
                                  STL_OPAMP_CONVERSION_TOLERANCE,
                                  STL_OPAMP_TRIGGER_MASK);
    if (OK_STATUS != status)
    {
        /* Handle second OPAMP self-test failure */
    }
}

Cy_HPPASS_DAC_DeInit(STL_OPAMP_DAC_INDEX);
```

## HWFILT3P3Z test

The HWFILT3P3Z self-test validates supported mixed-signal filter paths.

The following example runs the `SelfTest_HWFILT3P3Z_HPPASS()` self-test across all
supported HPPASS filter instances for the active hardware family:

```c
uint8_t status = OK_STATUS;

for (uint8_t filter = 0U; filter <= HWFILT3P3Z_HPPASS_MAX_IDX; filter++)
{
    status = SelfTest_HWFILT3P3Z_HPPASS(filter);
    if (OK_STATUS != status)
    {
        /* Handle HWFILT3P3Z HPPASS filter test failure */
        break;
    }
}
```

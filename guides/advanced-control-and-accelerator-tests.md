# Advanced Hardware Block Self-Tests

This guide covers IPC communication, DMA DW, CORDIC math accelerator, and MOTIF quadrature-decoder self-tests.

## IPC test

The IPC self-test validates the device's internal inter-processor or inter-core communication resources.

The following is an example of an IPC self-test:

```c
uint8_t status = SelfTest_IPC();

if (OK_STATUS != status)
{
    /* Handle IPC self-test failure */
}
```

## DMA DW test

The DMA DW self-test validates the configured DataWire DMA path and descriptor flow. The library re-initializes both descriptors at runtime from the config structs you supply, then overwrites the source and destination addresses with its own internal static buffers. The transfer geometry you configure in Device Configurator must therefore match those internal buffers exactly, or the destination-vs-source comparison will always fail.

Add a DataWire DMA channel in Device Configurator and give it the alias `DMA_DW_TEST`. Configure it with **two descriptors** as follows.

**Descriptor 0** — clears the destination (Word-to-Word, 16 elements):

- Data transfer width: Word to Word
- X count: 16 (transfers 16 × 4 = 64 bytes, matching the 16-word internal source buffer)
- X source and destination increment: 1
- Y count: 1
- Trigger in type: Descriptor (`CY_DMA_DESCR_CHAIN`)
- Trigger out type: 1 Element (`CY_DMA_1ELEMENT`)
- Interrupt out: 1 Element
- Channel state on completion: Enabled
- Enable chaining: Yes, chain to Descriptor 1

**Descriptor 1** — transfers the test pattern (Byte-to-Byte, 3 × 22 elements):

- Data transfer width: Byte to Byte
- X count: 3 (bytes per row)
- X source and destination increment: 1
- Y count: 22 (rows; 22 × 3 = 66 bytes, matching the 66-byte internal pattern buffer)
- Y source and destination increment: 3
- Trigger in type: Descriptor (`CY_DMA_DESCR_CHAIN`)
- Trigger out type: 1 Element (`CY_DMA_1ELEMENT`)
- Interrupt out: 1 Element
- Channel state on completion: Enabled
- Enable chaining: No

**Channel** — set priority to 3, preemptable and bufferable both off.

The `trigLine` argument passed to `SelfTest_DMA_DW()` must be the trigger-mux input line that maps to the chosen DW channel. For DW0 channel 0 this is `TRIG_OUT_MUX_0_PDMA0_TR_IN0`; select the matching constant for the DW instance and channel number you use.

If a correctly wired DMA path still returns `ERROR_STATUS`, the most likely cause is a descriptor geometry mismatch (wrong X/Y count or wrong data width) or the trigger line constant not matching the configured DW channel.

The following is an example of a DMA DW self-test:

```c
uint8_t status = SelfTest_DMA_DW(DMA_DW_TEST_HW, DMA_DW_TEST_CHANNEL,
                                 &DMA_DW_TEST_Descriptor_0, &DMA_DW_TEST_Descriptor_1,
                                 &DMA_DW_TEST_Descriptor_0_config, &DMA_DW_TEST_Descriptor_1_config,
                                 &DMA_DW_TEST_channelConfig, TRIG_OUT_MUX_0_PDMA0_TR_IN0);

if (OK_STATUS != status)
{
    /* Handle DMA DW self-test failure */
}
```

## CORDIC tests

### Wrapper API

The wrapper API runs the self-test against the default CORDIC instance.

The following is an example of a self-test using the wrapper API:

```c
Cy_CORDIC_Enable(MXCORDIC);

uint8_t status = SelfTest_Cordic();

if (OK_STATUS != status)
{
    /* Handle default CORDIC self-test failure */
}

Cy_CORDIC_Disable(MXCORDIC);
```

### Explicit-instance API

The explicit-instance API runs the self-test against a caller-provided CORDIC base address.

The following is an example of a self-test using an explicit CORDIC instance:

```c
Cy_CORDIC_Enable(MXCORDIC);

uint8_t status = SelfTest_Cordic_Ext(MXCORDIC);

if (OK_STATUS != status)
{
    /* Handle explicit-instance CORDIC self-test failure */
}

Cy_CORDIC_Disable(MXCORDIC);
```

## MOTIF test

The MOTIF self-test validates the MOTIF quadrature decoder by driving it with internally generated emulated Phase A, Phase B, and Index signals and then verifying that the Q-CLK period captured by a dedicated timer counter matches the expected value.

The test requires five TCPWM resources in the same TCPWM group and a MOTIF instance in the same group:

| Alias | TCPWM type | Role |
|---|---|---|
| `EMU_PH_A` | PWM | Generates the emulated Phase A quadrature signal |
| `EMU_PH_B` | PWM | Generates the emulated Phase B quadrature signal, 90° offset from Phase A |
| `EMU_PH_I` | PWM | Generates the emulated Index (revolution marker) signal |
| `Tick_Time` | Timer-Counter | Captures the Q-CLK period output from MOTIF |
| `MOTIF0` | MOTIF | Quadrature decoder under test |

All five resources must be clocked from the same peripheral clock divider (alias: `MOTIF0_CLK`). This shared clock is critical: `ref_count` and `margin_count` in `stl_motif_cfg_handle_t` are expressed in units of that clock.

### MOTIF0_CLK (shared peripheral clock divider)

Add a peripheral clock divider of type `8-bit` and give it the alias `MOTIF0_CLK`. Set the integer divider to any value that produces a stable, known frequency from your system clock. In the reference design the counter clock producing a 50 MHz. Connect this divider to `EMU_PH_A`, `EMU_PH_B`, `EMU_PH_I`, `Tick_Time`, and `MOTIF0` clock inputs.

### EMU_PH_A (Phase A emulator)

Add a TCPWM in **PWM** mode in the same group as `MOTIF0` and give it the alias `EMU_PH_A`.

- **Mode:** PWM, Center-Asymmetric CC0/CC1 alignment, continuous run
- **Period0:** choose a value that produces the desired Q-CLK frequency. In the reference design, `Period0 = 600` at 50 MHz gives one Q-CLK edge every 300 clock cycles (see formula below)
- **Compare0:** `Period0 / 2` (for 50% duty cycle; reference design: 300)
- **Trigger Out 0:** CC0 match (`CY_TCPWM_CNT_TRIGGER_ON_CC0_MATCH`) — routed to EMU_PH_B `Reload` input to create the 90° phase offset
- **Trigger Out 1:** Line-out (`CY_TCPWM_CNT_TRIGGER_ON_LINE_OUT`) — routed to the MOTIF Phase A input
- **Reload Input:** Rising edge — driven by EMU_PH_I Trigger Out 0 to synchronize the phase signals with the index revolution

### EMU_PH_B (Phase B emulator)

Add a second TCPWM in **PWM** mode in the same group and give it the alias `EMU_PH_B`.

- **Mode:** PWM, Center-Asymmetric CC0/CC1 alignment, continuous run
- **Prescaler:** Divide-by-1
- **Period0 and Compare0:** same as `EMU_PH_A` (reference: 600 / 300)
- **Trigger Out 1:** Line-out (`CY_TCPWM_CNT_TRIGGER_ON_LINE_OUT`) — routed to the MOTIF Phase B input
- **Reload Input:** Rising edge — driven by EMU_PH_A Trigger Out 0 (CC0 match). This loads Phase B in the middle of Phase A's period, creating the required 90° quadrature offset

### EMU_PH_I (Index emulator)

Add a third TCPWM in **PWM** mode in the same group and give it the alias `EMU_PH_I`.

- **Mode:** PWM, Center-Asymmetric CC0/CC1 alignment, continuous run
- **Prescaler:** Divide-by-1
- **Period0:** set to a multiple of `EMU_PH_A Period0`, representing one encoder revolution. Reference design: `Period0 = 60000` (100 × EMU_PH_A period of 600)
- **Compare0:** `Period0 − 1` (narrow pulse; reference design: 59701, producing a short index pulse near the end of each revolution)
- **Trigger Out 0:** Terminal count (`CY_TCPWM_CNT_TRIGGER_ON_TC`) — routed to EMU_PH_A `Reload` input to synchronize the quadrature phase signals at each revolution
- **Trigger Out 1:** Line-out — routed to the MOTIF Index input
- **Reload Input:** Disabled (EMU_PH_I is the free-running master; EMU_PH_A and EMU_PH_B synchronize to it)

### Tick_Time (Q-CLK period capture counter)

Add a TCPWM in **Timer-Counter** mode in the same group and give it the alias `Tick_Time`.

- **Mode:** Timer-Counter, capture mode (`CY_TCPWM_COUNTER_MODE_CAPTURE`), continuous run, up-counting
- **Prescaler:** Divide-by-1
- **Period:** 65535 (maximum; the counter must not overflow between Q-CLK events)
- **Capture Input:** Rising edge (`CY_TCPWM_INPUT_RISINGEDGE`) — driven by the MOTIF period-clock output. Each Q-CLK event both captures the current count and reloads the counter to zero
- **Reload Input:** Rising edge — same MOTIF period-clock output. The simultaneous capture and reload means the captured value directly equals the number of clock cycles between two consecutive Q-CLK edges

### MOTIF0 (quadrature decoder)

Add a MOTIF in **Quadrature Decoder** mode in the same group as the PWM emulators.

- **Mode:** Quadrature Decoder (`CY_MOTIF_PD_QUAD_MODE`)
- **Phase A signal:** `CY_QUADDEC_POSI0`
- **Phase B signal:** `CY_QUADDEC_POSI1`
- **Index signal:** `CY_QUADDEC_POSI2`
- **Phase A edge mode:** Both edges (`CY_TCPWM_DETECT_ON_BOTH_EDGE`). Both rising and falling edges of Phase A produce a Q-CLK pulse
- **Low-pass filter:** 2 clock cycles (`CY_LOWPASS_OF_2CLOCK_CYCLE`)
- **Period-clock event:** Forward to OUT2 (`CY_FWD_QUADDEC_EVENT_TO_OUT2`) — routes the Q-CLK period pulse to `Tick_Time` capture0 and reload inputs
- **Clock and direction events:** Forward to OUT0 and OUT1 respectively (unused by the self-test but should be set to forward, not ignore)

### Deriving ref_count and margin_count

`ref_count` and `margin_count` are the only application-controlled thresholds in `stl_motif_cfg_handle_t`. All other values are set by the library.

With both-edge Phase A detection, each Q-CLK event is separated by half the Phase A PWM period:

`ref_count = EMU_PH_A Period0 / 2`

For the reference design: ref_count = 600 / 2 = 300.

Set `margin_count` to about 10% of `ref_count` to allow for measurement jitter without false failures. The test passes when the captured `Tick_Time` value falls in `[ref_count − margin_count, ref_count + margin_count]`.

Set the `delay` field in `stl_motif_cfg_handle_t` to the number of milliseconds needed for at least one complete Q-CLK period to elapse after calling `SelfTest_Motif_Init()`. One millisecond (`delay = 1u`) is sufficient for the reference design.

If a correctly wired MOTIF self-test still fails, verify that `EMU_PH_A Period0` and `MOTIF0_CLK` divider match the values used to compute `ref_count`.

The following is an example of a MOTIF self-test:

```c
static stl_motif_cfg_handle_t motifCfg =
{
    .motif_base   = MOTIF0_HW,
    .motif_config = &MOTIF0_quaddec_config,
    .qclk_base    = Tick_Time_HW,
    .qclk         =
    {
        .idx      = Tick_Time_NUM,
        .cfg      = &Tick_Time_config,
    },
    .sgen_base    = { EMU_PH_A_HW, EMU_PH_B_HW, EMU_PH_I_HW },
    .sgen         =
    {
        {
            .idx  = EMU_PH_A_NUM,
            .cfg  = &EMU_PH_A_config,
        },
        {
            .idx  = EMU_PH_B_NUM,
            .cfg  = &EMU_PH_B_config,
        },
        {
            .idx  = EMU_PH_I_NUM,
            .cfg  = &EMU_PH_I_config,
        },
    },
    .ref_count    = 300u,
    .margin_count = 30u,
    .delay        = 1u,
};

SelfTest_Motif_Init(&motifCfg);

uint8_t status = SelfTest_Motif_Start(&motifCfg);

if (OK_STATUS != status)
{
    /* Handle MOTIF self-test failure */
}

SelfTest_Motif_DeInit(&motifCfg);
```

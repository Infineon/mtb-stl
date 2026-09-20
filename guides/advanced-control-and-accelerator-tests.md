# Advanced Hardware Block Self-Tests

This guide covers CLB, EPU, and IPC communication, DMA DW, CORDIC math accelerator, and MOTIF quadrature-decoder self-tests.

## CLB test

The CLB (Custom Logic Block) self-test is available on PSOC Control C3 M8 devices with the PPCA subsystem. It exercises the DICO (Digital Current Observer) and MACO (Matrix Commutation) CLB sub-blocks. The DICO tests use internal timer-driven switching and, where needed, internal ADC test-data injection; the MACO test supplies its own minimal sector patterns. No external trigger source, routing, pin connection, power converter, or analog stimulus is required.

`SelfTest_CLB(uint8_t testMask)` accepts one or more of the following OR-combined selection bits. `SELFTEST_CLB_ALL` is the OR of all six bits. The function evaluates selected subtests in table order and stops at the first failure. Run the masks in separate calls when the returned status must identify the failing path.

| Test mask | Tested path |
| --- | --- |
| `SELFTEST_CLB_SAFE_STATE` | DICO kill-path transition: autonomous Phase 1 switching leaves safe state, a software kill forces `KILL_STATE` and `SAFE_STATE`, and kill exit clears `KILL_STATE`. |
| `SELFTEST_CLB_INTR` | DICO interrupt-register path: `INTR_MASK` write/readback, software `INTR_SET` assertion and write-one-to-clear, and `INTR_MASKED` behavior. |
| `SELFTEST_CLB_DICO_PATH` | DICO autonomous Phase 1 switching and hardware-generated DICO interrupt status. |
| `SELFTEST_CLB_DICO_IL` | Internal ADC Group 0 Slice 0 test-data injection through AFLT0 median pass-through (`FILT_OUT0`) to the DICO Phase 1 `IL_ACCU` observer. |
| `SELFTEST_CLB_DICO_LIMIT` | DICO limit detection for an illegal `IMINL > IMAXL` pair and the `MAX_ON_REACHED` on-time limiter. |
| `SELFTEST_CLB_MACO_SEQ` | MACO sequencer start, next-sector transition, and software-kill safe-state behavior using temporary sector patterns. |
| `SELFTEST_CLB_ALL` | All six paths above. |

Bits outside `SELFTEST_CLB_ALL` are ignored when at least one valid bit is present. A zero mask, or a mask with no valid bit, returns `ERROR_STATUS` without running a subtest. A selected hardware failure or any internal timeout also returns `ERROR_STATUS`; there is no separate timeout status. The DICO autonomous-switch and limit checks are bounded to 50 ms, the ADC conversions in the IL path are bounded to 50 ms each, and the MACO start and sector-transition checks are bounded to 1 ms and 50 ms respectively.

### Device Configurator setup

In the PPCA Configuration personality, allocate `clb` to the CPU that calls the self-test. The M8 reference configuration uses `CY_ALLOCATE_PERIPHERAL_TO_CPU0`. `SELFTEST_CLB_DICO_IL` also accesses ADC Group 0, so allocate `adcGrp0` to the same CPU; the reference configuration likewise uses `CY_ALLOCATE_PERIPHERAL_TO_CPU0`.

No CLB, DICO, or MACO personality, generated configuration object, application ISR, or external signal route is required by this API. The normal system and PPCA peripheral clocks must be initialized before the call. The API enables the CLB and its selected DICO or MACO sub-block itself; the IL subtest additionally enables PPCA and AREF while it runs.

### Resource ownership and restoration

Reserve the PPCA CLB exclusively while the self-test runs. The DICO tests change CLB output selection and DICO control, configuration, current limits, polarity, safe/on-time settings, interrupt masks, and interrupt/status flags. `SELFTEST_CLB_DICO_IL` additionally changes PPCA enable state, AREF enable state, ADC Group 0 Slice 0 configuration and test/calibration mode, and AFLT0 configuration. The MACO test changes CLB selection, MACO control/event state, all three sector row/safe registers, and their pattern tables.

Each selected subtest completes its cleanup before `SelfTest_CLB()` can return a failure. Configuration registers saved by the test are restored before the next selected subtest or return. Pending DICO and MACO interrupt/status flags are cleared rather than preserved. The IL cleanup disables AREF before restoring the saved PPCA, ADC, and AFLT registers; it does not preserve a pre-existing AREF enabled state, so an application that owned that state must re-enable AREF after the call. Do not run the API while application code, an ISR, or another CPU uses DICO, MACO, ADC Group 0/AFLT0, or observes the CLB outputs. In particular, an application DICO interrupt handler must not consume the temporary interrupt state created by the interrupt-path test.

### API sequence

Call `SelfTest_CLB()` only after the required PPCA ownership allocation and clock initialization. Use `SELFTEST_CLB_ALL` when one aggregate result is sufficient. To isolate a fault, call one mask at a time and stop after the first non-`OK_STATUS` result; the API has no failure-detail getter. The following example deliberately uses individual masks for that reason:

```c
static const uint8_t clbTests[] =
{
    SELFTEST_CLB_SAFE_STATE,
    SELFTEST_CLB_INTR,
    SELFTEST_CLB_DICO_PATH,
    SELFTEST_CLB_DICO_IL,
    SELFTEST_CLB_DICO_LIMIT,
    SELFTEST_CLB_MACO_SEQ,
};
uint8_t status = OK_STATUS;
uint8_t failedTestMask = 0U;

for (uint32_t testIndex = 0U;
     (testIndex < (sizeof(clbTests) / sizeof(clbTests[0]))) && (OK_STATUS == status);
     testIndex++)
{
    status = SelfTest_CLB(clbTests[testIndex]);
    if (OK_STATUS != status)
    {
        failedTestMask = clbTests[testIndex];
    }
}

if (OK_STATUS != status)
{
    /* Handle the first failing CLB path identified by failedTestMask. */
    (void)failedTestMask;
}
```

## EPU test

The EPU (Event Processing Unit) self-test is available on PSOC Control C3 M8 devices with PPCA. For every requested processing unit (PU), it configures asynchronous bypass mode, generates an internal software event, routes the PU output through COMBO0 to EPU IRQ0, and checks the EPU interrupt status. The test therefore covers the requested PU signal path plus COMBO and EPU IRQ routing without an external event source, pin, or application ISR.

`EPU_PU_TYPE_T1` selects the full-featured T1 PU implementation, which includes masking, blanking, debouncing, and delay capabilities. `EPU_PU_TYPE_T2` selects the simplified T2 implementation, which provides bypass, polarity, and edge capabilities. The self-test exercises both types in asynchronous bypass mode; it does not infer their behavior from the type name.

The `puInitial` and `puEnd` arguments form an inclusive interval:

| Type | Valid inclusive indices | Public maximum-index macro |
| --- | --- | --- |
| `EPU_PU_TYPE_T1` | `0` through `31` | `EPU_PU_T1_MAX_INDEX` |
| `EPU_PU_TYPE_T2` | `0` through `15` | `EPU_PU_T2_MAX_INDEX` |

An unknown type, an end index beyond the selected type's maximum, or `puInitial > puEnd` returns `ERROR_INCORRECT_USAGE_STATUS` without hardware access. There is no empty valid interval. A failed PU path returns `ERROR_STATUS` and the call stops at that PU; the API has no public failing-PU getter. For each PU it waits a fixed 100 us propagation interval before reading interrupt status, so there is no configurable polling timeout.

### Device Configurator setup

Add the PPCA EPU personality and allocate every PU that will be tested to the calling CPU. The verified M8 configuration assigns `puty1_0` through `puty1_31` and `puty2_0` through `puty2_15` to `CY_ALLOCATE_PERIPHERAL_TO_CPU0`, which permits the full public ranges above. A different M8 application may test only the PU indices allocated to its caller.

Before calling `SelfTest_EPU_PU()`, enable the EPU block with `Cy_PPCA_EPU_Enable(PPCA_EPU)`. The STL API deliberately does not toggle that enable state, so that power and clock policy remains application-owned. The API uses raw PPCA EPU registers and needs no generated EPU configuration object in customer code. A CPU interrupt allocation is not required for the self-test: it reads EPU IRQ0 status directly. In the verified configuration, `epuIrq0` through `epuIrq7` are unassigned (`-1`).

### Resource ownership and restoration

Reserve every requested PU, COMBO0, and EPU IRQ0 exclusively during the call. For each PU, the API temporarily rewrites that PU configuration, COMBO0 configuration, and EPU IRQ0 source selection; it generates and clears a software event and clears EPU IRQ0 status. Those saved configuration registers are restored before the next PU or before a failing call returns, but pending interrupt state is cleared rather than preserved. Other PU configuration registers are not changed.

Do not use COMBO0, EPU IRQ0, or a requested PU concurrently from an ISR, another CPU, or application routing. Choose an unused COMBO0/IRQ0 topology or pause the application users first. The reference M8 configuration reserves other COMBO and T2 resources for application routing; those resources remain outside the self-test except when their PU index is deliberately included in the requested range.

### API sequence

Enable the application-owned EPU block, call `SelfTest_EPU_PU()` once for each PU type and inclusive range, preserve the first failure, then disable the block only when the application owns that enable state. The example tests the complete ranges exposed by the public macros:

```c
uint8_t status;

/* This example owns the PPCA EPU enable state. */
Cy_PPCA_EPU_Enable(PPCA_EPU);

status = SelfTest_EPU_PU(EPU_PU_TYPE_T1, 0U, EPU_PU_T1_MAX_INDEX);
if (OK_STATUS == status)
{
    status = SelfTest_EPU_PU(EPU_PU_TYPE_T2, 0U, EPU_PU_T2_MAX_INDEX);
}

if (OK_STATUS != status)
{
    /* Handle the first failing EPU PU range. */
}

Cy_PPCA_EPU_Disable(PPCA_EPU);
```

## IPC test

### Conventional IPC implementation

`SelfTest_IPC()` validates the device's conventional internal inter-processor or inter-core communication resources.

```c
uint8_t status = SelfTest_IPC();

if (OK_STATUS != status)
{
    /* Handle IPC self-test failure */
}
```

### PPCA implementation

`SelfTest_IPC_PPCA(uint8_t testMask)` is available on PSOC Control C3 M8 devices with `CY_IP_MXS40PPSS`. It runs on the Main CM33 in loopback mode, so no PPCA CPU firmware or external IPC endpoint is required. The PPCA IPC block has channels 0 through 3 and interrupt structures 0 through 3. Every invocation tests all 16 channel/interrupt-structure pairs for the selected groups, and each selected group runs for every pair until the first failure.

`testMask` selects one or more OR-combined groups. `SELFTEST_IPC_PPCA_ALL` is the OR of all six public group bits.

| Test mask | Tested path | Subtests |
| --- | --- | --- |
| `SELFTEST_IPC_PPCA_LOCK` | Acquire, acquired-state verification, and release. | 1, 2, 3 |
| `SELFTEST_IPC_PPCA_DATA` | DATA0/DATA1 integrity, independence, and final lock release. | 4, 5, 13 |
| `SELFTEST_IPC_PPCA_NOTIFY` | Notify-to-self loopback, interrupt clear-on-write-one, and final lock release. | 6, 7, 14 |
| `SELFTEST_IPC_PPCA_INTR` | Interrupt masking, `INTR_MASKED` consistency, and software-forced interrupt. | 8, 9, 10 |
| `SELFTEST_IPC_PPCA_RELEASE_EVT` | Release-event self-loopback. | 11 |
| `SELFTEST_IPC_PPCA_SILENT_REL` | Release without interrupt generation. | 12 |
| `SELFTEST_IPC_PPCA_ALL` | All six groups. | 1 through 14 |

At least one valid mask bit is required. A zero mask, or a mask containing no valid bit, returns `ERROR_STATUS` without running hardware tests. Bits outside `SELFTEST_IPC_PPCA_ALL` are ignored when at least one valid bit is present. A selected subtest timeout or path failure also returns `ERROR_STATUS`; there is no separate timeout status. Positive ISR waits are bounded to 1000 us, while checks that an interrupt must not fire use a 50 us observation interval.

#### Failure diagnostics

After a failed channel/interrupt-pair test, `SelfTest_IPC_PPCA_GetFailChannel()`, `SelfTest_IPC_PPCA_GetFailIntr()`, and `SelfTest_IPC_PPCA_GetFailSubtest()` identify the first failed channel, interrupt structure, and numbered subtest. Do not treat those coordinates as fresh for every `ERROR_STATUS`: the API does not reset or populate them when the mask contains no valid selection or when temporary ISR configuration fails before pair execution.

#### Setup and ownership

Initialize the normal system and PPCA clocks, then enable the PPCA block before calling the API using the applicable PPCA enable flow, such as `Cy_PPCA_Enable(PPCA_CNFG)`. `SelfTest_IPC_PPCA()` does not own the top-level PPCA enable/disable policy; disable PPCA afterward only when the calling application owns that enable state.

The API installs temporary handlers for `ppca_ipc_0_IRQn` through `ppca_ipc_3_IRQn`; no application ISR setup is required. It temporarily uses all four IPC channels and all four interrupt structures. Reserve the entire PPCA IPC block exclusively during the call, and do not allow application code, another CPU, or an ISR to use these IPC resources concurrently.

#### Restoration behavior

The API saves and restores interrupt masks, ISR vectors, NVIC priorities, and the NVIC enabled state. Test-generated pending IPC interrupt bits are cleared rather than preserved, and cleanup releases any channel locks remaining from the test. DATA0 and DATA1 values written by the test are not restored. Therefore, pre-existing locks, pending events, and channel data must not be treated as preserved application state.

#### API sequence

The following example owns the PPCA enable state, runs all PPCA IPC groups, and makes first-failure details available to application failure handling:

```c
uint8_t status;

/* This example owns the PPCA enable state. */
Cy_PPCA_Enable(PPCA_CNFG);

status = SelfTest_IPC_PPCA(SELFTEST_IPC_PPCA_ALL);
if (OK_STATUS != status)
{
    uint32_t failedChannel = SelfTest_IPC_PPCA_GetFailChannel();
    uint32_t failedIntr = SelfTest_IPC_PPCA_GetFailIntr();
    uint8_t failedSubtest = SelfTest_IPC_PPCA_GetFailSubtest();

    /* Make first-failure details available to application failure handling. */
    (void)failedChannel;
    (void)failedIntr;
    (void)failedSubtest;
}

Cy_PPCA_Disable(PPCA_CNFG);
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

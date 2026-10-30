# Core Self-Tests

This guide covers CPU, CONTROL register, APSR register, startup configuration register, FPU register, watchdog, and Digital I/O self-tests.

## CPU tests

The following is an example of a self-test for CPU tests (program counter, program flow, and register checkerboard):

```c
uint8_t status = SelfTest_PC();

if (OK_STATUS != status)
{
    /* Handle program counter test failure */
}

if (OK_STATUS == status)
{
    status = SelfTest_PROGRAM_FLOW();
    if (OK_STATUS != status)
    {
        /* Handle program flow test failure */
    }
}

if (OK_STATUS == status)
{
    status = SelfTest_CPU_Registers();
    if (OK_STATUS != status)
    {
        /* Handle CPU register test failure */
    }
}
```

### Linker script requirement for `SelfTest_PC`

The program counter self-test calls helper functions placed at Flash addresses that most closely match the `0x5555` and `0xAAAA` patterns. These sections must be placed explicitly in the linker script so the linker emits the `PC5555` and `PCAAAA` functions at the required addresses; otherwise the PC test can fail:

```ld
NV_CONFIG2 0x12015554 :
{
    . = 0x00;
    KEEP (*(PC5555))
} >flash = ORIGIN(flash)

NV_CONFIG3 0x1201AAA8 :
{
    . = 0x00;
    KEEP (*(PCAAAA))
} >flash = ORIGIN(flash)
```

The exact addresses and section names are device-specific. Refer to the device reference manual for the correct values for your target.

## Fast CPU register test

The fast CPU register self-test `SelfTest_CPU_Registers_Fast()` splits the CPU register check into short steps, so it can run periodically with a short execution time per call. Each call tests one step. The general-purpose register steps write the checkerboard patterns `0xAAAAAAAA` and `0x55555555`, read them back, and restore the register. The PRIMASK, CONTROL, and APSR steps write and check the tested bits. Each step must also return its unique signature before the test reports a pass.

The steps are tested in this order:

| Step | Tested register |
|------|-----------------|
| 0 | SP (bits [31:2], patterns `0xAAAAAAA8` and `0x55555554`) |
| 1 to 5 | R8 to R12 |
| 6 | LR |
| 7 to 14 | R0 to R7 |
| 15 | PRIMASK |
| 16 | CONTROL (SPSEL) |
| 17 | APSR (N, Z, C, V, Q, and GE[3:0]) |

After the last step, the next call starts again with step 0.

Usage requirements:

* Call `SelfTest_CPU_Registers_Fast()` only from privileged Thread mode, not from an interrupt handler. In unprivileged Thread mode, the PRIMASK step fails. In Handler mode, the CONTROL step fails. Both cases return `ERROR_STATUS`, the same value as a detected register fault.
* Before the first call, set PSP (for example, with `__set_PSP()`) to the top of a writable, 8-byte aligned RAM area that the application doesn't use for other data. The startup code doesn't initialize PSP. The CONTROL step selects PSP for a short time. If PSP is 0, this step returns `ERROR_STATUS`. A non-zero PSP isn't checked further. The test itself doesn't write to this area, but an NMI or HardFault in this window stacks its exception frame below PSP: 32 bytes, or 104 bytes with an active FP context. On Armv8-M Mainline cores, PSPLIM must also allow this frame. The test doesn't change PSP or PSPLIM.
* Initialize the step variable to `0xFF` before the first call and pass the same variable to every call. The test stores the attempted step in it on pass and on fail. If the pointer is `NULL` or the stored step is invalid, the test returns `ERROR_STATUS` without changing it.
* If the test returns `ERROR_STATUS`, stop calling it and enter a safe state. Another call would continue with the next step.
* The test masks interrupts while it runs and restores the caller's PRIMASK value before it returns. The PRIMASK step enables maskable interrupts for a short time, also when they were disabled by the caller.
* The test is available for the GCC_ARM, ARM, and IAR toolchains.
* To run the test from RAM, define `STL_CPU_FAST_RUN_FROM_RAM` in the assembler build. The test is then placed in the `.cy_ramfunc` section. On cores with ITCM, define `STL_CPU_FAST_RUN_FROM_ITCM` instead to place it in the `.cy_itcm` section. Don't define both. Check that the linker script copies the selected section to RAM or ITCM; otherwise the test runs from Flash.
* The PRIMASK, CONTROL, and APSR steps are separate from the `SelfTest_PRIMASK()`, `SelfTest_CONTROL()`, and `SelfTest_APSR()` tests. `SelfTest_CONTROL()` also checks that SPSEL really switches the active stack pointer, and uses its own test stack.

The following is an example of a fast CPU register self-test. Call it periodically, for example, from the main loop:

```c
/* Process stack for the CONTROL step. 128 bytes hold one exception frame
 * (NMI or HardFault), also the 104-byte frame with an active FP context. */
static uint64_t cpuFastPspStack[16u];
/* Last attempted step: 0xFF before the first call */
static uint8_t lastTestedStep = 0xFFu;

if (0xFFu == lastTestedStep)
{
    /* Bare-metal application that doesn't use PSP: set PSP once,
     * without switching the running code away from MSP */
    __set_PSP((uint32_t)(uintptr_t)&cpuFastPspStack[16u]);
}

/* Call from privileged Thread mode, not from an interrupt handler.
 * Each call tests the next step. */
uint8_t status = SelfTest_CPU_Registers_Fast(&lastTestedStep);

if (OK_STATUS != status)
{
    /* Handle CPU register test failure: lastTestedStep holds the failed
     * step. Don't call the test again, enter a safe state. */
}
```

## CPU instruction test

The CPU instruction self-test validates execution of selected instruction sequences used by the library to detect CPU datapath faults.

The following is an example of a self-test for CPU instructions:

```c
uint8_t status = SelfTest_CPU_Instructions();

if (OK_STATUS != status)
{
    /* Handle CPU instruction test failure */
}
```

## CONTROL register test

The CONTROL register self-test detects stuck-at faults in the stack pointer selection bit (SPSEL) and, in FPU builds, in the floating-point bits (FPCA, SFPA). It writes each tested bit to 0 and 1, reads CONTROL back, and checks that the active stack pointer really switches between MSP and a dedicated test stack inside the library. CONTROL, the stack pointers, and the caller's interrupt mask are restored and checked before the test returns.

The test is available on all Cortex-M cores. SPSEL is always tested. On cores where the image is compiled with FPU code generation (`__ARM_FP`), the floating-point bit FPCA is tested too, and in a Secure image (`__ARM_FEATURE_CMSE == 3`) also SFPA. The nPRIV bit is never tested and keeps its original value. On Armv8-M Mainline cores (for example, Cortex-M33 and Cortex-M55), the test also sets PSPLIM for the test stack and restores it.

Usage requirements:

* Call `SelfTest_CONTROL()` from privileged Thread mode. From an interrupt handler or from unprivileged code, it returns `ERROR_INCORRECT_USAGE_STATUS` without changing CONTROL.
* The test masks interrupts while it runs. NMI and HardFault can't be masked; if one of them occurs while PSP is selected, its exception frame is stacked on the library's test stack.
* Keep a watchdog running. If the test can't confirm that CONTROL and the stack pointers were restored, it stays in an endless loop with interrupts disabled instead of returning.
* In FPU builds, FPCA is cleared for a short time during the test. If an NMI or HardFault occurs then while the application has live FP registers, those registers aren't stacked, and a handler that uses the FPU corrupts them. If the device safety argument doesn't require FPCA and SFPA to be tested, or this risk isn't acceptable, define `SELFTEST_CONTROL_SKIP_FP_BITS=1` (for example, in the application Makefile `DEFINES`). The test then tests SPSEL only and never writes FPCA and SFPA. The size of the test stack doesn't change.

The following is an example of a self-test for the CONTROL register:

```c
/* Call from privileged Thread mode. A watchdog must be running: if the
 * test can't confirm that CONTROL and the stack pointers were restored,
 * it doesn't return. */
uint8_t status = SelfTest_CONTROL();

if (ERROR_INCORRECT_USAGE_STATUS == status)
{
    /* Handle incorrect usage: called from an interrupt handler or from
     * unprivileged code */
}
else if (OK_STATUS != status)
{
    /* Handle CONTROL register test failure */
}
else
{
    /* Test passed */
}
```

## APSR register test

The APSR register self-test detects stuck-at faults in the Application Program Status Register (APSR) bits that are implemented and writable on the core. It writes a known pattern to these bits, checks the N, Z, C, and V flags with conditional branches, reads APSR back, masks the result to the implemented bits, and compares it with the pattern. It then repeats the check with the complementary pattern, so that every tested bit is checked at 0 and at 1. APSR is restored to its original value and the restored value is checked.

The tested bits depend on the core and are selected at compile time: N, Z, C, and V on Armv6-M and Armv8-M Baseline cores (for example, Cortex-M0+ and Cortex-M23), N, Z, C, V, and Q on Armv7-M cores (Cortex-M3), and N, Z, C, V, Q, and GE[3:0] on cores with the DSP extension (for example, Cortex-M4, Cortex-M7, and Cortex-M33).

Usage requirements:

* The test doesn't need privileged access and doesn't mask interrupts: the APSR value is saved and restored on every exception entry and return.
* On return, Q and GE (where implemented) hold their original values. N, Z, C, and V aren't preserved, because the Procedure Call Standard for the Arm Architecture doesn't require a called function to preserve them.
* The test returns `ERROR_STATUS` if a pattern comparison fails, or if the original APSR value isn't restored.
* This test is separate from the APSR step of `SelfTest_CPU_Registers_Fast`.

The following is an example of a self-test for the APSR register:

```c
uint8_t status = SelfTest_APSR();

if (OK_STATUS != status)
{
    /* Handle APSR register test failure */
}
```

## PRIMASK register test

The PRIMASK register self-test detects stuck-at faults in the PM bit (bit 0) of the PRIMASK register, which masks all exceptions with configurable priority. PM is the only implemented bit of PRIMASK, so the test checks both of its legal values: it writes PM = 0 and PM = 1, reads PRIMASK back after each write, and checks the value. The original PRIMASK value is written back and checked.

The test is available on all Cortex-M cores.

Usage requirements:

* Call `SelfTest_PRIMASK()` from Handler mode or from privileged Thread mode. A write to PRIMASK from unprivileged Thread mode is ignored, so in this case the test returns `ERROR_INCORRECT_USAGE_STATUS` without changing PRIMASK.
* The test changes the interrupt mask while it runs. Maskable interrupts are enabled between the PM = 0 write and the next PM = 1 write. If PRIMASK was 1 when the function was called, an interrupt can occur in this window. If PRIMASK was 0, interrupts are masked between the PM = 1 write and the restore, so their latency can increase. NMI and HardFault aren't affected by PRIMASK.
* The test writes the original PRIMASK value back also when a check fails, and checks it. It returns `ERROR_STATUS` if any check fails, including when a stuck PM prevents the original value from being restored.
* This test is separate from the PRIMASK step of `SelfTest_CPU_Registers_Fast`.

The following is an example of a self-test for the PRIMASK register:

```c
/* Call from Handler mode or privileged Thread mode */
uint8_t status = SelfTest_PRIMASK();

if (ERROR_INCORRECT_USAGE_STATUS == status)
{
    /* Handle incorrect usage: called from unprivileged code */
}
else if (OK_STATUS != status)
{
    /* Handle PRIMASK register test failure */
}
else
{
    /* Test passed */
}
```

## Startup Configuration Registers test

This test checks critical startup configuration registers against values saved in nonvolatile memory.

The test supports two modes controlled by `STARTUP_CFG_REGS_MODE` in `SelfTest_ConfigRegisters.h`. By default, the library uses `CFG_REGS_TO_FLASH_MODE`.

To switch modes from build settings, define `STARTUP_CFG_REGS_MODE` in your project Makefile defines:

```makefile
DEFINES+=STARTUP_CFG_REGS_MODE=CFG_REGS_CRC_MODE
```

### Mode 1: `CFG_REGS_TO_FLASH_MODE` (default)

Store register copies once, then compare on later checks.

For PSOC™ 6 devices, call `SelfTests_Init_StartUp_ConfigReg()` once before running startup-register checks.

PSOC™ 4, PSOC™ Control C3, XMC7000, and XMC5000 devices do not require a call to `SelfTests_Init_StartUp_ConfigReg()`.

```c
/* Initialize Startup Configuration Register self-test */
SelfTests_Init_StartUp_ConfigReg();
```

Call `SelfTests_Save_StartUp_ConfigReg()` once after startup initialization, then call `SelfTests_StartUp_ConfigReg()` at startup checkpoints or periodically.

On XMC7000 and XMC5000 devices, the register copies are stored in the last two 512-byte rows of Code Flash. Because this Flash cannot erase a single row, `SelfTests_Save_StartUp_ConfigReg()` erases the entire last 8 KB small sector of Code Flash, starting at `CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE - 0x2000`, on every call. The function preserves only the `.flash_checksum` area at the end of the sector. Reserve the whole sector in the linker script and do not place code, constants, or other nonvolatile data in it.

```c
uint8_t status = OK_STATUS;

/* Save startup register baseline once */
cy_en_flashdrv_status_t startupCfgSaveStatus = SelfTests_Save_StartUp_ConfigReg();
if (CY_FLASH_DRV_SUCCESS != startupCfgSaveStatus)
{
    /* Handle nonvolatile save failure */
    status = ERROR_STATUS;
}

if (OK_STATUS == status)
{
    /* Run startup register integrity check */
    status = SelfTests_StartUp_ConfigReg();
    if (OK_STATUS != status)
    {
        /* Handle startup register integrity failure */
    }
}
```

### Mode 2: `CFG_REGS_CRC_MODE`

For PSOC™ 6 devices, call `SelfTests_Init_StartUp_ConfigReg()` once before running startup-register checks.

```c
/* Initialize Startup Configuration Register self-test */
SelfTests_Init_StartUp_ConfigReg();
```

In CRC mode, do not call `SelfTests_Save_StartUp_ConfigReg()`. The first call to `SelfTests_StartUp_ConfigReg()` calculates CRC, stores it to nonvolatile memory, and sets the CRC semaphore. Subsequent calls recalculate CRC and compare it with the stored value.

On XMC7000 and XMC5000, the CRC is stored in the last row of Code Flash. When the CRC is stored, the entire last 8 KB small sector of Code Flash is erased, the same as in Mode 1. Reserve that whole sector.

The following is an example of a self-test for Startup Configuration Registers in CRC mode:

```c
uint8_t status = OK_STATUS;
uint8_t startupCfgStatus = SelfTests_StartUp_ConfigReg();

if (CRC_SAVED_STATUS == startupCfgStatus)
{
    /* CRC baseline was stored successfully on first run */
}
else
{
    /* Handle CRC save failure */
    status = startupCfgStatus;
}

if (OK_STATUS == status)
{
    startupCfgStatus = SelfTests_StartUp_ConfigReg();
    if (PASS_COMPLETE_STATUS == startupCfgStatus)
    {
        /* Startup register CRC matches the stored value */
    }
    else
    {
        /* Handle startup register integrity failure */
        status = startupCfgStatus;
    }
}
```

In `CFG_REGS_CRC_MODE`, `SelfTests_StartUp_ConfigReg()` returns:

- `CRC_SAVED_STATUS` on first successful CRC save
- `PASS_COMPLETE_STATUS` when the calculated CRC matches stored CRC
- `ERROR_STATUS` if check or storage access fails

## FPU register test

The floating-point register self-test validates that the FPU register file can be written and read back correctly.

The following is an example of a self-test for FPU registers:

```c
uint8_t status = SelfTest_FPU_Registers();

if (OK_STATUS != status)
{
    /* Handle FPU register test failure */
}
```

## Watchdog supervision tests

### WDT test

The watchdog self-test verifies reset-driven supervision behavior using the hardware watchdog.

The STL API handles the required internal watchdog-test flow. Depending on the current phase of the test, the function can trigger a reset before later returning `OK_STATUS`.

The following is an example of a watchdog self-test call from application code:

```c
uint8_t status = SelfTest_WDT();

if (OK_STATUS != status)
{
    /* Handle watchdog self-test failure */
}
```

### Windowed MCWDT test

The windowed watchdog self-test verifies that the application services the watchdog only inside the allowed service window.

The STL API handles the internal phase management of the windowed watchdog self-test.

The following is an example of a windowed MCWDT self-test call from application code:

```c
uint8_t status = SelfTest_Windowed_MCWDT(MCWDT_STRUCT0, CY_MCWDT_COUNTER0);

if (OK_STATUS != status)
{
    /* Handle windowed MCWDT self-test failure */
}
```

## Digital I/O test

The Digital I/O test detects pin shorts to ground or VCC using internal pull-up and pull-down resistors. Before calling the test, configure which pins to test using a pin mask.

Do not test pins that are actively driven by external circuits, connected to special functions such as USB, debug port, or crystal oscillator, or tied directly to power rails, as these will cause false failures.

The following is an example of a self-test for Digital I/O:

```c
/* Pin mask: one element per GPIO port (index 0 = PORT0 … index 13 = PORT13).
 * Each bit corresponds to a pin on that port (bit 2 = pin 2, etc.).
 * Here P13.2 is tested — a free GPIO on KIT_PSC3M6_EVAL.
 * Adapt the array size and bit position for your target board. */
static uint8_t pinToTest[14] = {[13] = 0x04u };  /* P13.2 */

/* Set the custom pin mask */
SelfTest_IO_SetPinMask(pinToTest);

/* Run the I/O test */
uint8_t status = SelfTest_IO();
if (OK_STATUS == status)
{
    /* All tested pins are functional */
}
else if (SHORT_TO_GND == status)
{
    /* Handle short to ground failure */
    uint8_t errorPort = SelfTest_IO_GetPortError();
    uint8_t errorPin = SelfTest_IO_GetPinError();
    /* Pin P{errorPort}.{errorPin} is shorted to ground */
    (void)errorPort;
    (void)errorPin;
}
else if (SHORT_TO_VCC == status)
{
    /* Handle short to VCC failure */
    uint8_t errorPort = SelfTest_IO_GetPortError();
    uint8_t errorPin = SelfTest_IO_GetPinError();
    /* Pin P{errorPort}.{errorPin} is shorted to VCC */
    (void)errorPort;
    (void)errorPin;
}
else
{
    /* Unknown error */
}
```

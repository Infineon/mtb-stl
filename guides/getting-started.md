# Core Self-Tests

This guide covers CPU, startup configuration register, FPU register, watchdog, and Digital I/O self-tests.

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

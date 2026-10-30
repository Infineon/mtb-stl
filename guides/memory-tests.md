# Memory and Storage Integrity Tests

This guide covers SRAM, Flash, stack, ECC integrity, MPU, SMPU and PPU self-tests.

## SRAM test

The SRAM March or GALPAT test overwrites the memory block passed to it, so pass a region that does not contain live data, such as a dedicated reserved buffer.

In the example below, `SRAM_TEST_START_ADDR` is the start of a reserved SRAM test window and `SRAM_RESTORE_BUFFER_ADDR` points to a non-overlapping buffer used to preserve overwritten data.

The following is an example of a self-test for SRAM:

```c
/* Disable interrupts during destructive RAM test */
__disable_irq();

uint8_t status = SelfTest_SRAM(SRAM_MARCH_TEST_MODE,
                               SRAM_TEST_START_ADDR,
                               TEST_RAM_SIZE,
                               SRAM_RESTORE_BUFFER_ADDR,
                               SRAM_RESTORE_BUFFER_SIZE);
__enable_irq();

if (ERROR_STATUS == status)
{
    /* Handle SRAM test failure */
}
```

## SRAM March test (stack-free)

These March C- or March X tests use assembly-based stack-free execution, enabling safe testing of both variable RAM and stack regions. The tests are non-destructive: original data is backed up and restored automatically.

| Algorithm | Complexity | Use Case |
|-----------|------------|----------|
| March C- | 10n | Recommended for startup; detects stuck-at, transition, and all 2-cell coupling faults (CFid, CFin, CFst) |
| March X | 6n | Recommended for runtime; detects stuck-at, transition, and inversion coupling faults (CFin). Faster with reduced coverage |

The following is an example of a full SRAM test at startup using `SelfTest_SRAM_March_Full`:

```c
/* Disable interrupts during test to prevent ISRs from accessing tested memory */
__disable_irq();

/* Run March C- test on entire memory region */
uint8_t status = SelfTest_SRAM_March_Full(
    RAM_START,
    RAM_END,
    BLOCK_SIZE,
    BACKUP_BUFFER,
    STL_SRAM_MARCH_C_MINUS  /* or STL_SRAM_MARCH_X for faster test */
    );

__enable_irq();

if (OK_STATUS != status)
{
    /* Handle SRAM test failure - enter safe state */
}
```

The following is an example of incremental runtime testing using `SelfTest_SRAM_March_Runtime`. This function tests one block per call. Interrupts must be disabled for each call to prevent ISRs from accessing the block currently under test, but the critical section is bounded by `BLOCK_SIZE`. Full RAM coverage is achieved incrementally over many calls rather than in a single pass — size `BLOCK_SIZE` to keep each call within your interrupt-latency budget:

```c
/* State variable — must be initialized to RAM_START before first call and kept across calls */
static uint32_t ramTestPosition = 0u;
if (0u == ramTestPosition)
{
    ramTestPosition = RAM_START;
}

/* Disable interrupts for the duration of one block test to prevent ISRs from
 * accessing the block currently being overwritten. The critical section is
 * bounded by BLOCK_SIZE, so keeping BLOCK_SIZE small limits interrupt latency.
 * Call this function periodically; full RAM coverage is achieved over many
 * calls rather than in a single blocking pass. */
__disable_irq();

/* Call periodically (e.g., in main loop) */
uint8_t status = SelfTest_SRAM_March_Runtime(
    RAM_START,
    RAM_END,
    &ramTestPosition,  /* Function updates and wraps automatically */
    BLOCK_SIZE,
    BACKUP_BUFFER,
    STL_SRAM_MARCH_X     /* Fast algorithm for runtime */
    );

__enable_irq();

if (OK_STATUS != status)
{
    /* Handle SRAM test failure - enter safe state */
}
/* ramTestPosition automatically advances; wraps to RAM_START after last block */
```

The backup buffer must be located outside the test region and must be at least `BLOCK_SIZE` bytes. On the first call to `SelfTest_SRAM_March_Runtime`, the backup area itself is tested for integrity before testing the main memory region.

## SRAM stack test

The SRAM stack self-test validates the RAM currently used as stack storage.

In the example below, `STACK_TEST_TOP_ADDR` identifies the active stack top and `STACK_ALT_BASE` points to a non-overlapping alternate stack area reserved for backup and restore during the destructive test.

`STACK_ALT_BASE` must satisfy three requirements: the region must be reserved exclusively for this test and not overlap any live stack or variable data; it must be at least `STACK_TEST_SIZE` bytes; and the address must be naturally aligned to the stack-pointer alignment required by the target ABI (typically 8 bytes on Cortex-M). The test runs with interrupts disabled, so call it from a startup or maintenance context where the bounded interrupt-off period is acceptable.

On ARMv8-M Mainline devices (Cortex-M33, including PSOC™ Control C3), the toolchain startup sets `MSPLIM` to `__StackLimit`. `SelfTest_SRAM_Stack()` switches `SP` to `altStackBase`, which is outside the stack region, so the CPU would fault unless `MSPLIM` is cleared first. The example wraps these calls in `#if defined(__ARM_ARCH_8M_MAIN__)`, which evaluates to false on PSoC™ 4, PSoC™ 6, XMC5000, and XMC7000 where the register does not exist.

The following is an example of a self-test for the active stack region:

```c
/* ARMv8-M Mainline only: clear MSPLIM so SP can switch to altStackBase. */
#if defined(__ARM_ARCH_8M_MAIN__)
uint32_t savedMsplim = __get_MSPLIM();
__set_MSPLIM(0u);
#endif /* defined(__ARM_ARCH_8M_MAIN__) */
__disable_irq();

uint8_t status = SelfTest_SRAM_Stack(STACK_TEST_TOP_ADDR,
                                     STACK_TEST_SIZE,
                                     STACK_ALT_BASE);

/* Restore MSPLIM before re-enabling interrupts. */
#if defined(__ARM_ARCH_8M_MAIN__)
__set_MSPLIM(savedMsplim);
#endif /* defined(__ARM_ARCH_8M_MAIN__) */
__enable_irq();

if (OK_STATUS != status)
{
    /* Handle SRAM stack test failure */
}
```

## Stack range test

The stack range self-test verifies that the initialized stack guard pattern remains intact at the monitored boundaries.

In the example below, `STACK_RANGE_BASE_ADDR`, `STACK_RANGE_SIZE`, and `STACK_RANGE_PATTERN_SIZE` represent the application's monitored stack range and guard-pattern size.

The following example initializes stack sentinels and checks that they remain intact:

```c
SelfTests_Init_Stack_Range(STACK_RANGE_BASE_ADDR,
                           STACK_RANGE_SIZE,
                           STACK_RANGE_PATTERN_SIZE);

uint8_t status = SelfTests_Stack_Check_Range(STACK_RANGE_BASE_ADDR,
                                             STACK_RANGE_SIZE);

/* The returned value is a bitmask, so both flags can be set simultaneously. */
if ((status & ERROR_STACK_OVERFLOW) != 0u)
{
    /* Handle stack overflow detection */
}
if ((status & ERROR_STACK_UNDERFLOW) != 0u)
{
    /* Handle stack underflow detection */
}

/* Restore the guard pattern for subsequent checks. */
SelfTests_Init_Stack_Range(STACK_RANGE_BASE_ADDR,
                           STACK_RANGE_SIZE,
                           STACK_RANGE_PATTERN_SIZE);
```

## Flash integrity test

For CRC32 or Fletcher64 Flash integrity checks, call `SelfTest_Flash_init()` once before entering the test loop.

### Linker script requirement

The test reads a pre-computed reference checksum from a fixed location at the very end of Flash and compares it against the computed value. A dedicated section must be reserved in the linker script so this location is always allocated and never overwritten by application code:

```ld
CHECKSUM_SIZE = 0x00000008; /* 8 bytes for Fletcher64, or 4 bytes for CRC32 */

.flash_checksum ORIGIN(flash) + LENGTH(flash) - CHECKSUM_SIZE :
{
    KEEP(*(.flash_checksum))
} > flash
```

Without this section the end address overlaps application code, producing a different checksum on every build. The reference checksum value must be computed over `[FLASH_BASE, FLASH_END_ADDR)` as a post-build step and written into the `.flash_checksum` section.

The following is an example of a self-test for Flash integrity:

```c
/* One-time initialization with base address, end address, and pre-computed checksum */
SelfTest_Flash_init(CY_FLASH_BASE, FLASH_END_ADDR, flash_StoredCheckSum);

/* Periodic test — processes FLASH_DOUBLE_WORDS_TO_TEST words per call */
uint8_t status = SelfTest_FlashCheckSum(FLASH_DOUBLE_WORDS_TO_TEST);
if (ERROR_STATUS == status)
{
    /* Handle Flash integrity failure */
}
```

## ECC tests

### Flash ECC test

The Flash ECC self-test validates Flash ECC fault detection on a reserved, row-aligned Flash address.

**This test rewrites the Flash row containing `FLASH_ECC_TEST_ADDR`.** The application must ensure the chosen row does not hold any data that must survive the test — for example by reserving a dedicated row in the linker script, placing a scratch variable there with the appropriate section attribute, or using any blank row that is provably not reached by application code.

Define `FLASH_ECC_TEST_ADDR` in the application so that it points into a reserved, row-aligned Flash row.

On XMC7000 and XMC5000 devices, `SelfTest_ECC()` does not use `FLASH_ECC_TEST_ADDR`. It uses a fixed STL-internal row: the last 512-byte row of the second-to-last 8 KB small sector of Code Flash, at `CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE - 0x2000 - 0x200`. Because this Flash cannot erase a single row, the test erases the **entire 8 KB sector** that contains the row before programming it. Reserve that whole sector in the linker script and do not place code, constants, or other nonvolatile data in it.

The following is an example of a Flash ECC self-test:

```c
uint8_t status = SelfTest_ECC_Flash(FLASH_ECC_TEST_ADDR, CY_ECC_NC_ERROR);

if (OK_STATUS != status)
{
    /* Handle Flash ECC test failure */
}
```

### Runtime Flash ECC test

Available when built with `SELFTEST_ECC_MODE=STL_ECC_MODE_RUNTIME`. In this
mode the application owns the ECC register configuration, SysFault ISR, NVIC,
and CPU fault hook. STL verifies configuration and decodes captured evidence.

Configure and verify the Flash ECC registers once at startup:

```c
const stl_ecc_flash_cfg_t cfg =
{
    .eccEnable              = true,
    .rbusErrSilent          = false,
    .checkFlashMacroEncoder = true
};

FLASHC_FLASH_CTL |= FLASHC_FLASH_CTL_ECC_EN_Msk;
FLASHC_FLASH_CTL &= ~FLASHC_FLASH_CTL_RBUS_ERR_SILENT_Msk;
__DSB();
__ISB();
return SelfTest_ECC_Flash_ConfigVerify(&cfg);
```

Then route both Flash ECC fault sources to your own SysFault ISR:

```c
static const cy_stc_SysFault_t faultCfg =
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false
};
static const cy_stc_sysint_t faultIrqCfg =
{
    .intrSrc      = cpuss_interrupts_fault_0_IRQn,
    .intrPriority = 3U
};
uint8_t status = stl_example_ecc_flash_app_managed_config();

Cy_SysFault_ClearStatus(FAULT_STRUCT0);
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_FAULT);
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_FAULT);
Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
(void)Cy_SysFault_Init(FAULT_STRUCT0, &faultCfg);
(void)Cy_SysInt_Init(&faultIrqCfg, stl_example_ecc_flash_app_managed_isr);
NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);
```

**This provokes a real non-correctable error and rewrites the reserved row
(`CY_FLASH_ADDR`, the last Flash row).** It is destructive, one-shot, and
only used to validate the fault path — call it once, not on every boot:

```c
/* Destructive: rewrites the reserved row (CY_FLASH_ADDR) and forces a
 * non-correctable error that Cy_SysLib_ProcessingFault() must catch. */
ecc_app_managed_armed = true;

if (SelfTest_ECC_Flash_ProvokeNc(CY_FLASH_ADDR) == OK_STATUS)
{
    /* Reached only if the fault hook resumed execution instead of
     * resetting; restore the row and configuration afterwards. */
    (void)SelfTest_ECC_Flash_RestoreRow(CY_FLASH_ADDR);
}

ecc_app_managed_armed = false;
```

A non-correctable Flash read raises a precise BusFault *before* the SysFault
interrupt is taken, so `Cy_SysLib_ProcessingFault()` — the PDL `__WEAK` CPU
fault hook — is where non-correctable evidence is actually captured. STL
never defines this symbol in application-managed mode; your override always
resolves:

```c
stl_ecc_flash_snapshot_t snapshot;
stl_ecc_flash_event_t    event;

(void)SelfTest_ECC_Flash_SnapshotCapture(&snapshot);
(void)SelfTest_ECC_Flash_Decode(&snapshot, &event); /* store into application evidence, e.g. a CY_NOINIT struct */

if (ecc_app_managed_armed)
{
    ecc_app_managed_armed = false;

    /* The bus error is precise, so returning re-executes the faulting load.
     * ECC off is what makes that retry complete instead of faulting again. */
    Cy_Flashc_ECCDisable();
    __DSB();
    __ISB();
    return; /* fault record stays latched for the SysFault ISR to drain */
}

NVIC_SystemReset(); /* any other fault: no safe resume */
```

Once the fault hook resumes execution, the pending record is drained and
decoded from the SysFault ISR:

```c
stl_ecc_flash_snapshot_t snapshot;
stl_ecc_flash_event_t    event;

/* Cleared first: a record captured during the drain re-raises it and the
 * handler is entered again, instead of being dropped here. */
Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);

while (Cy_SysFault_GetErrorSource(FAULT_STRUCT0) != CY_SYSFAULT_NO_FAULT)
{
    (void)SelfTest_ECC_Flash_SnapshotCapture(&snapshot);
    (void)SelfTest_ECC_Flash_Decode(&snapshot, &event);
    SelfTest_ECC_Flash_Accumulate(&ecc_app_managed_counters, &event); /* application counters/log/policy */
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
}
```

**Threshold/counter semantics**: `ECCTHRESHOLD = 0` is not supported (reset
default `0xFF`); `main_c` fires on the (N+1)-th corrected error; `ECC1CNT`
saturates and only the application may clear it.

### RAM ECC test

The RAM ECC self-test validates SRAM ECC fault detection on a valid RAM target address.

Define `RAM_ECC_TEST_ADDR` in the application so that it points to an SRAM location reserved for ECC injection during the test.

The following is an example of a RAM ECC self-test:

```c
uint8_t status = SelfTest_ECC_Ram(RAM_ECC_TEST_ADDR, CY_ECC_NC_ERROR);

if (OK_STATUS != status)
{
    /* Handle RAM ECC test failure */
}
```

### Runtime RAM ECC test

Available when built with `SELFTEST_ECC_MODE=STL_ECC_MODE_RUNTIME`.
The application owns the RAMC and SysFault configuration, NVIC, ISR,
fault-record acknowledgement, and monitoring policy. STL verifies RAMC
configuration, captures and decodes evidence, updates caller-owned counters,
and provides the destructive stimulus.

Before enabling RAM checking at boot, configure each RAMC with ECC generation
enabled and checking disabled, then initialize every ECC-protected RAM region
that the startup code did not write. This includes unused initial stack space;
do not overwrite frames at or above the current MSP. Enable checking only after
that initialization is complete. Repeat the startup pattern for each RAMC
instance used by the application.

For a runtime configuration, first keep checking disabled while initializing
application-owned RAM ranges that may be read before their first write. This is
an example for the unused initial stack space; the application must identify
any other applicable ranges for each RAMC instance. Enable checking only after
that initialization, then verify the final ECC_CTL state including AUTO_CORRECT:

```c
const stl_ecc_ram_cfg_t cfgInit =
{
    .eccEnable    = true,
    .autoCorrect  = true,
    .checkEnable  = false
};
const stl_ecc_ram_cfg_t cfgRun =
{
    .eccEnable    = true,
    .autoCorrect  = true,
    .checkEnable  = true
};
uint8_t status = OK_STATUS;

/* Phase 1: enable generation with checking disabled on every RAMC. */
RAMC0->ECC_CTL = RAMC_ECC_CTL_EN_Msk |
                 RAMC_ECC_CTL_AUTO_CORRECT_Msk;
__DSB();
__ISB();
if (SelfTest_ECC_Ram_ConfigVerify(RAMC0, &cfgInit) != OK_STATUS)
{
    status = ERROR_STATUS;
}
#if (CY_IP_MXSRAMC_INSTANCES == 2U)
RAMC1->ECC_CTL = RAMC_ECC_CTL_EN_Msk |
                 RAMC_ECC_CTL_AUTO_CORRECT_Msk;
__DSB();
__ISB();
if (SelfTest_ECC_Ram_ConfigVerify(RAMC1, &cfgInit) != OK_STATUS)
{
    status = ERROR_STATUS;
}
#endif

/* Phase 2: initialize application-owned, uninitialized ranges while CHECK_EN
 * is off. Startup initializes the BSS/data objects above; this stack write
 * is an example. The application must initialize any other ranges that may
 * be read before their first write, including ranges protected by RAMC1. */
volatile uint32_t* word = (volatile uint32_t*)STL_STACK_LIMIT_ADDR;
uint32_t stackPointer = __get_MSP();
while ((uint32_t)word < stackPointer)
{
    *word = 0UL;
    word++;
}
__DSB();
__ISB();

/* Phase 3: enable checking and verify runtime configuration. */
RAMC0->ECC_CTL = RAMC_ECC_CTL_EN_Msk |
                 RAMC_ECC_CTL_AUTO_CORRECT_Msk |
                 RAMC_ECC_CTL_CHECK_EN_Msk;
__DSB();
__ISB();
if (SelfTest_ECC_Ram_ConfigVerify(RAMC0, &cfgRun) != OK_STATUS)
{
    status = ERROR_STATUS;
}
#if (CY_IP_MXSRAMC_INSTANCES == 2U)
RAMC1->ECC_CTL = RAMC_ECC_CTL_EN_Msk |
                 RAMC_ECC_CTL_AUTO_CORRECT_Msk |
                 RAMC_ECC_CTL_CHECK_EN_Msk;
__DSB();
__ISB();
if (SelfTest_ECC_Ram_ConfigVerify(RAMC1, &cfgRun) != OK_STATUS)
{
    status = ERROR_STATUS;
}
#endif
return status;
```

Then route both correctable and non-correctable sources to an application-owned
SysFault ISR:

```c
static cy_stc_SysFault_t faultCfg =
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false
};
static const cy_stc_sysint_t faultIrqCfg =
{
    .intrSrc      = cpuss_interrupts_fault_0_IRQn,
    .intrPriority = 3U
};
uint8_t status = stl_example_ecc_ram_app_managed_config();

Cy_SysFault_ClearStatus(FAULT_STRUCT0);
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_RAM_FAULT);
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_RAM_FAULT);
#if (CY_IP_MXSRAMC_INSTANCES == 2U)
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_C_RAM1_FAULT);
Cy_SysFault_SetMaskByIdx(FAULT_STRUCT0, CY_ECC_NC_RAM1_FAULT);
#endif
Cy_SysFault_SetInterruptMask(FAULT_STRUCT0);
(void)Cy_SysFault_Init(FAULT_STRUCT0, &faultCfg);
(void)Cy_SysInt_Init(&faultIrqCfg, stl_example_ecc_ram_app_managed_isr);
NVIC_EnableIRQ((IRQn_Type)cpuss_interrupts_fault_0_IRQn);
return status;
```

The ISR captures, decodes, accounts for, and acknowledges every record. A
decoded event of kind `STL_ECC_RAM_EVENT_NONE` increments `other`, so this loop
also safely drains fault sources shared with RAM ECC:

```c
stl_ecc_ram_snapshot_t snapshot;
stl_ecc_ram_event_t    event;

Cy_SysFault_ClearInterrupt(FAULT_STRUCT0);

while (Cy_SysFault_GetErrorSource(FAULT_STRUCT0) != CY_SYSFAULT_NO_FAULT)
{
    (void)SelfTest_ECC_Ram_SnapshotCapture(&snapshot);
    (void)SelfTest_ECC_Ram_Decode(&snapshot, &event);
    SelfTest_ECC_Ram_Accumulate(&ecc_ram_app_managed_counters, &event);
    Cy_SysFault_ClearStatus(FAULT_STRUCT0);
}
```

To validate both fault paths, reserve one application-owned, 32-bit-aligned RAM
word outside live data and stack storage. `SelfTest_ECC_Ram_Provoke()` overwrites
that word, triggers the selected error, and restores it with valid ECC data.
The sequence is destructive and intended only for a controlled diagnostic:

```c
/* Destructive: use only a dedicated, 32-bit-aligned RAM word. */
(void)SelfTest_ECC_Ram_Provoke(RAMC0,
                               (uint32_t)&ecc_ram_app_managed_test_word,
                               STL_ECC_RAM_INJECT_CORRECTABLE);
(void)SelfTest_ECC_Ram_Provoke(RAMC0,
                               (uint32_t)&ecc_ram_app_managed_test_word,
                               STL_ECC_RAM_INJECT_NON_CORRECTABLE);
```

## ARM core MPU test

The ARM Cortex-M core MPU self-test verifies that the MPU blocks a write to a protected memory location. It is available on devices with an ARM core MPU, such as XMC7000.

The test temporarily configures a spare MPU region as read-only over an application-provided 32-byte scratch block, and then attempts a privileged write to its first word. The test passes if the write raises a MemManage fault with `CFSR.DACCVIOL` at the scratch address and the scratch word is unchanged. The STL recovers the fault in place, so the device does not reset or halt.

### MPU test entry and scratch word

Define a dedicated 32-byte block (`MPU_TEST_BLOCK_BYTES`), aligned to 32 bytes, and describe its first word with `stl_mpu_entry_t`. The whole block is read-only during the test, so it must not hold any other data or the stack. `writeValue` must differ from `expectedValue`.

The `regionNumber` must be an unused MPU region from 1 to `MPU->TYPE.DREGION - 1` (15 on XMC7000 CM7). Region 0 is excluded because BSPs for cache-enabled cores usually reserve it for a non-cacheable memory window. When enabled regions overlap, the highest-numbered one applies: a higher-numbered region that allows writes to the scratch block overrides the test region, and `SelfTest_MPU()` returns `ERROR_STATUS`. Use the highest free region number, or make sure no higher-numbered region overlaps the scratch block.

```c
/* Dedicated 32-byte block covered by the STL's temporary read-only MPU region.
 * It must be 32-byte aligned and hold no other data; element 0 is the scratch word. */
CY_ALIGN(32) static volatile uint32_t mpuScratch[MPU_TEST_BLOCK_BYTES / sizeof(uint32_t)];

static const stl_mpu_entry_t mpuEntry =
{
    .testAddress   = &mpuScratch[0],
    .expectedValue = 0x5A5A5A5AU,          /* value the scratch word holds on entry */
    .writeValue    = 0xA5A5A5A5U,          /* must differ from expectedValue */
    .regionNumber  = 1U,                   /* spare MPU region, 1..(MPU->TYPE.DREGION - 1)
                                            * (region 0 is reserved by the BSP) */
};
```

### Run the MPU test

`SelfTest_MPU()` is a startup test. Call it once during initialization, before an RTOS scheduler starts (if one is used) and before the application arms its final MPU configuration. Before each call:

- Enable the ARM core MPU. Otherwise, `SelfTest_MPU()` returns `ERROR_INCORRECT_USAGE_STATUS` without touching the scratch word.
- Prime the scratch word with `expectedValue`. A failed test can leave `writeValue` in it.

Call it from thread mode with MemManage unmasked: do not call from an ISR, inside a `PRIMASK` or `FAULTMASK` critical section such as `__disable_irq()`, and with `BASEPRI` at or above the MemManage priority. Otherwise, the deliberate fault escalates to HardFault. For the return values, see the `SelfTest_MPU()` API documentation.

```c
/* The ARM core MPU must be enabled before SelfTest_MPU() runs */
if (0U == (MPU->CTRL & MPU_CTRL_ENABLE_Msk))
{
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

/* Prime the scratch word with the expected value */
mpuScratch[0] = mpuEntry.expectedValue;
__DSB();
__ISB();

uint8_t status = SelfTest_MPU(&mpuEntry);

if (OK_STATUS != status)
{
    /* Handle MPU self-test failure (ERROR_STATUS, ERROR_INCORRECT_USAGE_STATUS or ERROR_BAD_PARAM) */
}
```

### MemManage handler requirement

The application owns `MemManage_Handler`. The STL does not install it. Provide a small trampoline that selects the active stack (MSP or PSP) and forwards the exception frame to a C dispatcher. The dispatcher calls `SelfTest_MPU_HandleMemManage()`, which returns `true` only for the deliberate fault raised by `SelfTest_MPU()`. For any other MemManage fault, it returns `false` and leaves `CFSR` and `MMFAR` untouched, so the application can apply its own fault policy.

```c
/* Application-owned MemManage dispatch: forward the exception frame to the
 * STL, which recovers its deliberate denied write. */
#if defined(__ICCARM__)
#pragma inline=never
#else
__attribute__((noinline))
#endif
void Application_MemManage_Dispatch(uint32_t* stack)
{
    if (SelfTest_MPU_HandleMemManage(stack))
    {
        return;
    }

    /* Not an STL-owned fault: apply the application's fault policy */
    for (;;)
    {
    }
}

/* Naked MemManage_Handler trampoline: select MSP/PSP per EXC_RETURN and
 * branch to the C dispatcher. */
#if (defined(__GNUC__) && !defined(__clang__)) || defined(__ARMCC_VERSION)
__attribute__((naked, noinline, used)) void MemManage_Handler(void)
{
    __asm volatile
    (
        "tst   lr, #4                          \n"
        "ite   eq                              \n"
        "mrseq r0, msp                         \n"
        "mrsne r0, psp                         \n"
        "b     Application_MemManage_Dispatch  \n"
    );
}

#elif defined(__ICCARM__)
#pragma required=Application_MemManage_Dispatch
#pragma inline=never
__stackless __root void MemManage_Handler(void)
{
    asm volatile
    (
        "tst   lr, #4                          \n"
        "ite   eq                              \n"
        "mrseq r0, msp                         \n"
        "mrsne r0, psp                         \n"
        "b     Application_MemManage_Dispatch  \n"
    );
}

#endif /* if (defined(__GNUC__) && !defined(__clang__)) || defined(__ARMCC_VERSION) */
```

Whole-program optimization (LTO) can inline, reorder, or discard code around naked functions and silently break exception handling. Refer to your toolchain manual for details. The snippet above applies these safeguards:

- `noinline` (GCC/ARM) or `#pragma inline=never` (IAR) on both the trampoline and the C dispatcher.
- `used` (GCC/ARM) or `__root` (IAR) on the trampoline, so the linker keeps it even when the vector table references it only as data.
- `#pragma required=Application_MemManage_Dispatch` (IAR only), so the dispatcher is not removed when only the naked trampoline references it.

## SMPU test

The SMPU (shared memory protection unit) self-test is available on XMC7000 devices only. It verifies that the CPUSS SMPU detects and denies an access that violates its configuration.

The test borrows one low, disabled SMPU slave struct and programs it over an application-provided 256-byte scratch block so that only a protection context (PC) different from the one the calling bus master currently runs at is allowed. Then it performs a deliberate write to the first word of the block and checks that:

- the denied write raised the expected BusFault, which the STL recovered,
- the expected `CPUSS_MPU_VIO` SysFault source is latched in the supplied fault structure with `DATA0` equal to the scratch address, and
- the scratch word content is unchanged by the denied write.

The denied write raises a recoverable BusFault. `SelfTest_SMPU_HandleBusFault()` consumes this fault and resumes execution after the write.

Call `SelfTest_SMPU()` once during system initialization (before an RTOS scheduler starts, if one is used), before the final protection policy is locked. The call is blocking, not reentrant, and must not be called from an ISR. BusFault must not be masked when it is called: not inside a `PRIMASK` or `FAULTMASK` critical section such as `__disable_irq()`, and not with `BASEPRI` at or above the BusFault priority. Otherwise, the deliberate fault escalates to HardFault.

### Application requirements

- **Scratch block**: a dedicated 256-byte block (`SMPU_TEST_BLOCK_BYTES`) in non-cacheable memory, aligned to 256 bytes, holding no other data or stack. Pass its first word as `testAddress` and prime it with `expectedValue` before every call. On a cache-enabled CM7 a denied store to cacheable memory hits the D-cache and never reaches the bus, so the SMPU does not see it.
- **SMPU slave struct**: pass a low struct that is disabled, for example `PROT_SMPU_SMPU_STRUCT2`. An enabled struct is rejected with `ERROR_BAD_PARAM`. The high structs (13, 14, and 15) are boot-reserved and must not be used. The struct is left disabled after the test, the application should program its final policy afterwards. When enabled SMPU structs overlap, the highest-numbered one decides the access: a higher-numbered enabled struct that allows the active PC over the scratch block overrides the test struct, and `SelfTest_SMPU()` returns `ERROR_STATUS`. Make sure no higher-numbered enabled struct overlaps the scratch block.
- **Protection contexts**: the bus master must run at a non-zero active PC. `allowedPc` must be in the range 1 to 7 and different from the active PC.
- **Fault structure**: pass an application-provided fault structure, for example `FAULT_STRUCT0`, and the expected fault source for the tested memory area (`CPUSS_MPU_VIO_14` for SRAM on XMC7200). The test reinitializes the structure so that only the expected source is captured, which discards any earlier configuration of it, and leaves its masks cleared on return, so set up your own fault policy afterwards. It clears the status only when it holds the test's own violation. It reads the fault source by polling, so no fault-structure interrupt or NVIC configuration is needed.
- **Latched faults**: read and clear any fault already captured in the fault structure, or pending for the expected source, before the call (for example a violation at boot or one left by an earlier failed call). Otherwise `SelfTest_SMPU()` returns `ERROR_STATUS` without attempting the write and leaves that fault for the application.
- **BusFault handler**: the application owns `BusFault_Handler`. The STL does not install a handler. The application provides a trampoline that selects MSP or PSP from `EXC_RETURN` and forwards the exception frame to a C dispatcher.

The following is an example of the application-owned BusFault trampoline and dispatcher:

```c
/* Application-owned BusFault dispatch: forward the exception frame to the
 * STL, which recovers its deliberate denied write. */
#if defined(__ICCARM__)
#pragma inline=never
#else
__attribute__((noinline))
#endif
void Application_BusFault_Dispatch(uint32_t* stack)
{
    if (SelfTest_SMPU_HandleBusFault(stack))
    {
        return;
    }

    /* Not an STL-owned fault: apply the application's fault policy */
    for (;;)
    {
    }
}

/* Naked BusFault_Handler trampoline: select MSP/PSP per EXC_RETURN and
 * branch to the C dispatcher. */
#if (defined(__GNUC__) && !defined(__clang__)) || defined(__ARMCC_VERSION)
__attribute__((naked, noinline, used)) void BusFault_Handler(void)
{
    __asm volatile
    (
        "tst   lr, #4                         \n"
        "ite   eq                             \n"
        "mrseq r0, msp                        \n"
        "mrsne r0, psp                        \n"
        "b     Application_BusFault_Dispatch  \n"
    );
}

#elif defined(__ICCARM__)
#pragma required=Application_BusFault_Dispatch
#pragma inline=never
__stackless __root void BusFault_Handler(void)
{
    asm volatile
    (
        "tst   lr, #4                         \n"
        "ite   eq                             \n"
        "mrseq r0, msp                        \n"
        "mrsne r0, psp                        \n"
        "b     Application_BusFault_Dispatch  \n"
    );
}

#endif /* if (defined(__GNUC__) && !defined(__clang__)) || defined(__ARMCC_VERSION) */
```

The snippet applies the same LTO safeguards as the MemManage handler of the MPU test: `noinline` or `#pragma inline=never` on both functions, `used` or `__root` on the trampoline, and `#pragma required=Application_BusFault_Dispatch` for IAR.

`SelfTest_SMPU_HandleBusFault()` returns `true` only for the deliberate write driven by `SelfTest_SMPU()`. For every other BusFault it returns `false` and leaves `CFSR` and `BFAR` untouched.

### Application-side test sequence

Place the scratch block in non-cacheable SRAM, aligned to 256 bytes:

```c
#define SMPU_SCRATCH_INIT       (0x5A5A5A5Au)
#define SMPU_SCRATCH_WRITE      (0xA5A5A5A5u) /* must differ from the init value */
#define SMPU_ALLOWED_PC         (5u)          /* any PC in 1..7 other than the active PC of the bus master */

/* Dedicated block in non-cacheable SRAM, aligned to its size and holding no
 * other data; element 0 is the scratch word. */
CY_SECTION(".cy_sharedmem") CY_ALIGN(256)
static volatile uint32_t smpuScratch[SMPU_TEST_BLOCK_BYTES / sizeof(uint32_t)] = { SMPU_SCRATCH_INIT };
```

Then fill the entry, prime the scratch word, and run the test. `SMPU_ALLOWED_PC` must be a PC in the range 1 to 7 that differs from the active PC of the bus master:

```c
const stl_smpu_entry_t smpuEntry =
{
    .testAddress   = &smpuScratch[0],
    .expectedValue = SMPU_SCRATCH_INIT,
    .writeValue    = SMPU_SCRATCH_WRITE,
    .smpuStruct    = PROT_SMPU_SMPU_STRUCT2,  /* low struct, disabled */
    .allowedPc     = SMPU_ALLOWED_PC,
    .faultStruct   = FAULT_STRUCT0,
    .expectedFault = (cy_en_SysFault_source_t)CPUSS_MPU_VIO_14, /* SRAM on XMC7200 */
};

/* Prime the scratch word with the expected value */
smpuScratch[0] = SMPU_SCRATCH_INIT;
__DSB();
__ISB();

uint8_t status = SelfTest_SMPU(&smpuEntry);

if (OK_STATUS != status)
{
    /* Handle SMPU self-test failure (ERROR_STATUS, ERROR_INCORRECT_USAGE_STATUS or ERROR_BAD_PARAM) */
}
```

See `SelfTest_SMPU()` in the API reference for the return codes.

## PPU test

The PPU (peripheral protection unit) self-test is available on XMC7000 devices only. It verifies that the `PERI_MS` PPU denies an access that violates the protection policy already in place.

On XMC7000 the PPU is configured from the CM0+ core (PC0) during startup, before the CM7 cores are enabled. A CM7 application core cannot program a PPU at run time, so the test does not configure any PPU. Instead, you supply the address of a peripheral register that the final protection policy denies to the protection context (PC) of the calling core. The test attempts a write to it and checks that:

- the expected `CY_SYSFAULT_MS_PPU_*` violation is latched in the supplied fault structure with `DATA0` equal to the register address, and
- the register content is unchanged. This is checked only when `checkIntegrity` is `true`, which requires a register that is readable (write denied, read allowed) at the calling core's PC. Otherwise, only the denial is verified.

Call `SelfTest_PPU()` during system initialization, after the final application PPU policy is applied and before any protection lock. The call is blocking, not reentrant, and must not be called from an ISR.

One call verifies one access attempt on one register. Call it once per safety-related area, that is, once per PPU structure that enforces the safety-related policy against the calling core: once per fixed PPU structure (`PERI_MS_PPU_FX_*`; for example `PERI_MS_PPU_FX_GPIO_PRT2_PRT` and `PERI_MS_PPU_FX_GPIO_PRT2_CFG` are two areas) and once per programmable PPU region. For each area, pick one representative register inside the address range of that PPU structure. The same fault structure can be used for all calls.

### Application requirements

- **Protection policy**: the CM0+ core applies the final PPU policy and assigns the CM7 core its operational PC before the test runs. The calling core must run at a non-zero PC that is denied write access to the test register. PC0 always has access to PPU-protected peripherals, so a denial cannot be observed at PC0. PPU structures hold separate user and privileged permissions, so the policy must deny the write at the privilege level the test runs at.
- **Test register**: a register that the policy denies to the calling core's PC. Because the write lands on the peripheral if the PPU is defective, the register must be harmless to write: a plain read/write configuration register without write side effects (not write-1-to-clear, set/clear/invert, FIFO, command, or trigger registers), and not a register that controls pins or a peripheral in use. Prefer a register of an unused or disabled peripheral instance. It must not be a register the test path itself depends on, such as the logging UART or the fault structure.
- **Memory attributes**: map the register for the calling core as shareable, bufferable Device memory. This is the default memory map (MPU disabled, or not covered by an enabled region with the default map as background), or an MPU region with `TEX=0, C=0, B=1` (`ARM_MPU_ACCESS_DEVICE(1)`). Do not map it Strongly-ordered (`ARM_MPU_ACCESS_ORDERED`). The MPU must also permit write access at the calling privilege level, and read access when `checkIntegrity` is `true`. Otherwise, a MemManage fault occurs before the PPU evaluates the access.
- **Bus error handling**: keep `CPUSS_BUFF_CTL.WRITE_BUFF = 1` (reset default). With a bufferable register, the bus bridge buffers the write, the CPU receives no bus error, and the violation is captured in the fault structure. The test installs no BusFault handler. If these conditions are not met, the denied write causes a HardFault.
- **Fault structure**: pass an application-provided fault structure that is accessible at the calling core's PC, for example `FAULT_STRUCT0`, and the expected fault source for the calling core's master interface (`CY_SYSFAULT_MS_PPU_1` for CM7_0 on XMC7200). The test reconfigures the structure so that only the expected source is captured, with no fault interrupt, reset, or output. It reads the fault source by polling, so no fault-structure interrupt or NVIC configuration is needed. On return, the masks are cleared and the outputs are disabled, so configure the structure again if your application uses it. The status is cleared only when it holds the test's own violation.
- **Latched faults**: read and clear any fault already captured in the fault structure, or pending for the expected source, before the call (for example a violation from an earlier failed call). Otherwise `SelfTest_PPU()` returns `ERROR_STATUS` without attempting the write and leaves that fault for the application.
- **Integrity check**: when `checkIntegrity` is `true`, `expectedValue` must be the content the register holds before and after the write, and `writeValue` must differ from it in the implemented writable bits. Equal values are rejected with `ERROR_BAD_PARAM`.

### Application-side test sequence

The CM0+ core applies the PPU policy and assigns the CM7 core its operational PC. The following example protects two TCPWM counters that the application does not use: one is fully denied to PC5 and the other is read-only to PC5, while PC6 is allowed full access to both. A counter `PERIOD` register is a plain read/write register, so a write that leaks through a defective PPU has no effect outside the device:

```c
#define PPU_TEST_CM7_0_PC          (CY_PROT_PC5)
#define PPU_TEST_CM7_0_PCMASK      (CY_PROT_PCMASK5)
#define PPU_TEST_ALLOWED_PCMASK    (CY_PROT_PCMASK6)

static void apply_ppu_policy(void)
{
    /* Fully denied target: unused counter TCPWM1_GRP0_CNT1 denied to PC5, allowed to PC6. */
    (void)Cy_Prot_ConfigPpuFixedSlaveAtt(PERI_MS_PPU_FX_TCPWM1_GRP0_CNT1_CNT, (uint16_t)PPU_TEST_CM7_0_PCMASK,
                                         CY_PROT_PERM_DISABLED, CY_PROT_PERM_DISABLED, false);
    (void)Cy_Prot_ConfigPpuFixedSlaveAtt(PERI_MS_PPU_FX_TCPWM1_GRP0_CNT1_CNT, (uint16_t)PPU_TEST_ALLOWED_PCMASK,
                                         CY_PROT_PERM_RW, CY_PROT_PERM_RW, false);

    /* Write-denied / read-allowed target: unused counter TCPWM1_GRP0_CNT2 read-only to PC5. */
    (void)Cy_Prot_ConfigPpuFixedSlaveAtt(PERI_MS_PPU_FX_TCPWM1_GRP0_CNT2_CNT, (uint16_t)PPU_TEST_CM7_0_PCMASK,
                                         CY_PROT_PERM_R, CY_PROT_PERM_R, false);
    (void)Cy_Prot_ConfigPpuFixedSlaveAtt(PERI_MS_PPU_FX_TCPWM1_GRP0_CNT2_CNT, (uint16_t)PPU_TEST_ALLOWED_PCMASK,
                                         CY_PROT_PERM_RW, CY_PROT_PERM_RW, false);
}
```

After applying the policy, the CM0+ core assigns CM7_0 its operational PC and starts it:

```c
apply_ppu_policy();
(void)Cy_Prot_SetActivePC(CPUSS_MS_ID_CM7_0, PPU_TEST_CM7_0_PC);
Cy_SysEnableCM7(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
```

On the CM7 core, define the fault structure, the expected fault source, and the test registers:

```c
#define PPU_TEST_FAULT_STRUCT   (FAULT_STRUCT0)
#define PPU_TEST_EXPECTED_FAULT (CY_SYSFAULT_MS_PPU_1) /* CM7_0 master interface on XMC7200 */
#define PPU_TEST_WRITE_VALUE    (0xA5A5A5A5u)

/* PERIOD registers of counters unused by the design: plain read/write, no write side effects. */
#define PPU_TEST_TARGET_DENIED  (&TCPWM1_GRP0_CNT1->PERIOD) /* fully denied to the active PC */
#define PPU_TEST_TARGET_RDONLY  (&TCPWM1_GRP0_CNT2->PERIOD) /* write denied, read allowed */
```

Fill the entry for a fully denied register and run the test. The register is not readable, so the integrity check is not used:

```c
const stl_ppu_entry_t entry =
{
    .testReg        = PPU_TEST_TARGET_DENIED,
    .writeValue     = PPU_TEST_WRITE_VALUE,
    .faultStruct    = PPU_TEST_FAULT_STRUCT,
    .expectedFault  = PPU_TEST_EXPECTED_FAULT,
    .checkIntegrity = false,
    .expectedValue  = 0u,
};
uint8_t status = SelfTest_PPU(&entry);
```

For a register that is write-denied but read-allowed, set `expectedValue` to its current content and enable the integrity check:

```c
const stl_ppu_entry_t entry =
{
    .testReg        = PPU_TEST_TARGET_RDONLY,
    .writeValue     = PPU_TEST_WRITE_VALUE,
    .faultStruct    = PPU_TEST_FAULT_STRUCT,
    .expectedFault  = PPU_TEST_EXPECTED_FAULT,
    .checkIntegrity = true,
    .expectedValue  = *PPU_TEST_TARGET_RDONLY,
};
uint8_t status = SelfTest_PPU(&entry);
```

If `status` is not `OK_STATUS`, apply the application's failure handling. See `SelfTest_PPU()` in the API reference for the return codes.

# Memory and Storage Integrity Tests

This guide covers SRAM, Flash, stack, and ECC integrity checks.

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

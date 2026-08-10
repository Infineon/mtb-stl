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

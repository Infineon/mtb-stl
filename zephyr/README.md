# MTB-STL POST Integration for Zephyr RTOS

## Overview

This document describes the integration of Infineon MTB-STL (ModusToolbox Software Test Library) self-test functions into the Zephyr RTOS POST (Power-On Self Test) framework. This integration provides comprehensive hardware diagnostic testing that meets functional safety standards, including IEC 60730 Class B and IEC 61508 SIL requirements.

## Safety Standards Compliance

Based on Infineon Application Note AN236847, this implementation provides:

- IEC 60730 Class B compliance for household appliances
- IEC 61508 SIL (Safety Integrity Level) support for industrial systems
- Runtime and boot-time self-tests for critical hardware components
- Diagnostic coverage for various failure modes

## Test Categories and Implementation

### 1. CPU Tests
**Category:** `POST_CAT_CPU`

| Test | Kconfig Option | Purpose | Implementation |
|------|----------------|---------|----------------|
| CPU Register Checkerboard Test | `CONFIG_POST_MTB_STL_CPU_REGS` | Tests CPU register integrity using checkerboard pattern | `SelfTest_CPU_Registers()` wrapper |
| Program Counter Test | `CONFIG_POST_MTB_STL_CPU_PC` | Validates program counter functionality | `SelfTest_PC()` wrapper |
| Program Flow Test | `CONFIG_POST_MTB_STL_PROGRAM_FLOW` | Verifies program execution flow integrity | `SelfTest_PROGRAM_FLOW()` wrapper |

**Level:** `POST_KERNEL`

### 2. Startup Configuration Register Tests
**Category:** `POST_CAT_STARTUP_CFG_REG`

| Test | Kconfig Option | Purpose | Implementation |
|------|----------------|---------|----------------|
| Save Startup Configuration Register | `CONFIG_POST_MTB_STL_SAVE_STARTUP_CFGREG` | Saves initial configuration register values | `SelfTests_Save_StartUp_ConfigReg()` wrapper |
| Startup Configuration Register Test | `CONFIG_POST_MTB_STL_STARTUP_CFGREG` | Compares current register values with saved baseline | `SelfTests_StartUp_ConfigReg()` wrapper |

**Level:** `POST_KERNEL`

### 3. GPIO Test
**Category:** `POST_CAT_GPIO`

- **Kconfig:** `CONFIG_POST_MTB_STL_GPIO`
- **Purpose:** Validates GPIO functionality
- **Implementation:** `SelfTest_IO()` wrapper
- **Level:** `POST_KERNEL`

### 4. RAM Tests
**Category:** `POST_CAT_RAM`

| Test | Kconfig Option | Purpose | Implementation |
|------|----------------|---------|----------------|
| RAM March Test | `CONFIG_POST_MTB_STL_RAM_MARCH` | March algorithm memory test on a small buffer | Custom wrapper using `SelfTest_SRAM()` |
| Stack March Test | `CONFIG_POST_MTB_STL_STACK_MARCH` | March test on main stack region | Custom wrapper with safety checks |

**Special Notes:**
- **RAM March Test:** Tests 64-byte static buffer; for full RAM testing, configure appropriately
- **Stack March Test:** **WARNING: DESTRUCTIVE** - only runs when stack is unused
- **Key Features:** Dynamic safety buffer allocation, overlap detection, stack pointer verification, interrupt locking

### 5. Stack Overflow Test
**Category:** `POST_CAT_STACK`

- **Kconfig:** `CONFIG_POST_MTB_STL_STACK`
- **Purpose:** Detects stack overflow/underflow using pattern fill
- **Implementation:** Custom wrapper with pattern initialization
- **Level:** `POST_KERNEL`
- **Pattern Block Size:** 8 bytes
- **Checks:** Stack overflow and underflow conditions

### 6. Flash Checksum Test
**Category:** `POST_CAT_FLASH`

- **Kconfig:** `CONFIG_POST_MTB_STL_FLASH`
- **Purpose:** Verifies flash memory integrity using Fletcher64 checksum
- **Implementation:** Custom wrapper with address translation
- **Level:** `APPLICATION`
- **Key Features:** Excludes checksum storage region, handles PSoC4-specific address mapping
- **Checksum Location:** `.flash_checksum` section (end of flash)

### 7. Interrupt Test
**Category:** `POST_CAT_INTERRUPT`

- **Kconfig:** `CONFIG_POST_MTB_STL_INTERRUPT`
- **Purpose:** Validates interrupt handling mechanism
- **Implementation:** TCPWM timer-based interrupt test
- **Level:** `POST_KERNEL`
- **Hardware:** TCPWM Counter 1
- **IRQ:** `IFX_TCPWM_IRQ_NUM_CNT1`
- **Priority:** `IFX_TCPWM_CNT_PRIO_3`

### 8. Clock Frequency Test
**Category:** `POST_CAT_CLOCK`

- **Kconfig:** `CONFIG_POST_MTB_STL_CLOCK`
- **Purpose:** Verifies clock accuracy using WDT and TCPWM
- **Implementation:** Custom wrapper with timeout protection
- **Level:** `POST_KERNEL`
- **Hardware:** TCPWM Counter 0, Watchdog Timer
- **IRQ:** `IFX_TCPWM_IRQ_NUM_CNT0`
- **Timeout:** 100ms maximum

### 9. DMA Controller Test
**Category:** `POST_CAT_DMA`

- **Kconfig:** `CONFIG_POST_MTB_STL_DMA`
- **Purpose:** Validates DMA controller functionality
- **Implementation:** `SelfTest_DMAC()` wrapper
- **Level:** `APPLICATION`
- **Hardware:** DMAC instance with trigger input

### 10. Watchdog Test
**Category:** `POST_CAT_WATCHDOG`

- **Kconfig:** `CONFIG_POST_MTB_STL_WDT`
- **Purpose:** Tests watchdog timer reset functionality
- **Implementation:** `SelfTest_WDT()` wrapper
- **Level:** `APPLICATION`
- **WARNING:** Causes system reset

### 11. FPU Test
**Category:** `POST_CAT_CPU`

- **Kconfig:** `CONFIG_POST_MTB_STL_FPU`
- **Purpose:** Tests Floating Point Unit register integrity
- **Implementation:** `SelfTest_FPU_Registers()` wrapper
- **Level:** `POST_KERNEL`
- **Condition:** Only enabled with `CONFIG_FPU=y`

### 12. Analog Self-Test
**Category:** `POST_CAT_ADC`

- **Kconfig:** `CONFIG_POST_MTB_STL_ANALOG`
- **Purpose:** Tests SAR ADC accuracy using internal reference
- **Implementation:** Custom wrapper with SAR initialization
- **Level:** `APPLICATION`
- **Hardware:** SAR0, Channel 0
- **Accuracy:** ±12%
- **Pin:** P2.0 (VDDA reference)

### 13. UART Loopback Test
**Category:** `POST_CAT_UART_LOOPBACK`

- **Kconfig:** `CONFIG_POST_MTB_STL_UART_LOOPBACK`
- **Purpose:** Tests UART functionality using loopback
- **Implementation:** Generic wrapper that automatically selects a non-console UART device and saves/restores driver state
- **Level:** `APPLICATION`
- **Hardware:** Selects SCB1 based on Kconfig
- **Pins:** Configured via Kconfig
  - SCB1: TX: P1.5, RX: P1.4 (default)
- **Note:**
  - Requires jumper between TX and RX pins
  - The test automatically saves and restores the UART driver state, ensuring the driver continues to work correctly after the test
  - Console UART is automatically excluded from selection

### 14. TCPWM Counter Test
**Category:** `POST_CAT_COUNTER`

- **Kconfig:** `CONFIG_POST_MTB_STL_COUNTER`
- **Purpose:** Tests TCPWM counter functionality
- **Implementation:** Custom wrapper with timer initialization
- **Level:** `APPLICATION`
- **Hardware:** TCPWM Counter 2
- **IRQ:** `IFX_TCPWM_IRQ_NUM_CNT2`

### 15. PWM Self-Test
**Category:** `POST_CAT_PWM`

- **Kconfig:** `CONFIG_POST_MTB_STL_PWM`
- **Purpose:** Tests PWM generation and measurement
- **Implementation:** Custom wrapper with PWM initialization
- **Level:** `APPLICATION`
- **Hardware:** TCPWM Counter 3
- **Pins:** Output: P6.2, Input: P6.1
- **Note:** Requires jumper between P6.1 and P6.2

### 16. PWM Gatekill Test
**Category:** `POST_CAT_PWM_GATEKILL`

- **Kconfig:** `CONFIG_POST_MTB_STL_PWM_GATEKILL`
- **Purpose:** Tests PWM emergency stop (gatekill) functionality
- **Implementation:** Custom wrapper with PWM configuration
- **Level:** `APPLICATION`
- **Hardware:** TCPWM Counter 4

### 17. Communication Protocol Test
**Category:** `POST_CAT_COMMUNICATION`

- **Kconfig:** `CONFIG_POST_MTB_STL_COMM`
- **Purpose:** Tests UART-based communication protocol
- **Implementation:** Master-slave communication test
- **Level:** `APPLICATION`
- **Hardware:** Master UART: SCB4, Slave UART: SCB3
- **Timer:** TCPWM Counter 2 (timeout)
- **Pins:** Master TX (P4.0) → Slave RX (P2.5), Master RX (P4.1) → Slave TX (P2.4)
- **Timeout:** 100ms per transaction

## Pin Configuration Summary

| Test | Port/Pin | Direction | Description |
|------|----------|-----------|-------------|
| Analog Test | P2.0 | Input | VDDA reference voltage |
| UART Loopback | P1.4, P1.5 | TX/RX | Loopback connection required |
| PWM Test | P6.1, P6.2 | Input/Output | Requires jumper |
| Communication Test | P4.0, P4.1, P2.4, P2.5 | TX/RX | Master-slave connection |

## Safety Considerations

### Critical Tests
- **Stack March Test (`mtb_stl_stack_march`):** Runs at `EARLY` level, destructive
- **Watchdog Test (`mtb_stl_wdt`):** Causes system reset
- **Critical Flag Tests:** Tests marked with `POST_FLAG_CRITICAL` will halt system on failure if `CONFIG_POST_HALT_ON_FAILURE=y`

### Runtime Safety
- Tests marked with `POST_FLAG_RUNTIME_OK` can be executed during normal operation
- Tests marked with `POST_FLAG_DESTRUCTIVE` modify hardware state and should be used cautiously
- Tests marked with `POST_FLAG_BOOT_ONLY` should only run during system initialization

### Configuration Options
Each test is individually configurable via Kconfig options:
- `CONFIG_POST_MTB_STL_CPU_REGS`
- `CONFIG_POST_MTB_STL_RAM_MARCH`
- `CONFIG_POST_MTB_STL_FLASH`
- etc.

See `mtb_stl_post_cfg.h` for complete configuration options.

## Usage

### Shell Commands
The POST subsystem provides shell commands for test management:

```bash
# List all tests
post list

# Run specific test
post run 0x0001

# Run all runtime-safe tests
post run --all

# Run tests by category
post run --category cpu

# Show test status
post status

# View detailed results
post results

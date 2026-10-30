# ModusToolbox™ Safety Test Library 3.7.0

Refer to the [README.md](./README.md) for a complete description of the ModusToolbox™ Safety Test Library.

## What is Included?

### XMC7000

* Compilers Supported: GCC_ARM, ARM, IAR

* List of new Self-Tests:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | CSV |
    | CPU                | CPU_Fast |
    | Memory             | MPU, SMPU |
    | Peripheral         | PPU, eFuse, ADC_Config |

### PSOC™ 4

* Compilers Supported: GCC_ARM, ARM, IAR

* List of new Self-Tests:
    | Group              | Module |
    | ------             | ------ |
    | CPU                | APSR, CONTROL, PRIMASK |

## What Changed?

### v3.7.0
* Added new tests for XMC7000 devices: CSV, MPU, SMPU, PPU, eFuse, ADC_Config, CPU_Fast.
* Added new CPU tests for PSOC™ 4 devices: APSR, CONTROL and PRIMASK.
* Updated user documentation.
* Fixed issue with FLASH access in ECC and ConfigRegisters tests on XMC7000 devices.
* Fixed issue with missing settle time for ATOP DAC test on PSOC™ Control C3 M8 devices.

### v3.6.0
* Added support for the PSOC™ Control C3 M8 device family.
* Updated user documentation.
* Added support of runtime ECC self-tests for PSOC™ Control C3 devices.

### v3.5.0
* Added support for the PSOC™ Control C3 M6 device family.
* Updated user documentation.
* Fixed a failure in SelfTest_ECC() on XMC7000.

### v3.4.1
* Added new SelfTest_SRAM_March_Full and SelfTest_SRAM_March_Runtime tests.

### v3.4.0
* Updated user documentation.
* Added support of the XMC5000 device family.

### v3.3.1
* Fixed the return status for the SelfTest_UART_SCB() function for the case when the test is still in progress.
* Fixed build warnings for SAR ADC, ECC and IPC tests.

### v3.3.0
* Updated user documentation.
* Added support of the following device families:
    * PSOC™ 4000T
    * PSOC™ 4000S
    * PSOC™ 4100S
    * PSOC™ 4700S
* Added support of PSOC™ 4 devices for the following tests:
    * CAN
    * CANFD

### v3.2.0
* Updated user documentation.
* Added support for PSOC™ 4100T Plus and PSOC™ 4100S Plus device families.
* Added support of PSOC™ 4 devices for the following tests:
    * DMAC
    * Timer Counter
    * PWM
    * PWM Gatekill

---
(c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
Technologies AG.  All rights reserved.

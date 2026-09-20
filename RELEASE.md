# ModusToolbox™ Safety Test Library 3.6.0

Refer to the [README.md](./README.md) for a complete description of the ModusToolbox™ Safety Test Library.

## What is Included?

### PSOC™ Control C3 M8

* Compilers Supported: GCC_ARM, ARM, IAR

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Interrupt, Clock, IPC, Startup Configuration Registers, DMA/DW, WDT |
    | CPU                | CPU Registers, CPU Instructions, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | Digital I/O, UART, I2C, SPI, UART Data Transfer, ADC, ADC Filters, DAC, DCMP, DCSG, AFE, CLB, EPU, HWFILT3P3Z, Timer Counter, PWM, PWM Gatekill, CANFD, MOTIF, CORDIC, ECC |

## What Changed?

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

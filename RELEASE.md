# ModusToolbox Safety Test Library 3.2.0


Refer to the [README.md](./README.md) for a complete description of the ModusToolbox Safety Test Library.

## What is Included?

### CAT1A
* Devices Supported: Product families under PSOC™ 61 Programmable Line and Product families under PSOC™ 62 Performance Line

* Compilers Supported: GCC_ARM, ARM, IAR

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Interrupt, Clock, IPC, Startup Configuration Registers, DMAC, DMA/DW, WDT |
    | CPU                | CPU Registers, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | Digital I/O, UART, I2C, SPI, UART Data Transfer, ADC, DAC, Comparator, OP-AMP, Timer Counter, PWM, PWM Gatekill, CANFD |

### CAT1B
* Devices Supported: PSC3M5FDS2AFQ1

* Compilers Supported: GCC_ARM, ARM, IAR

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Interrupt, Clock, IPC, Startup Configuration Registers, DMA/DW, WDT |
    | CPU                | CPU Registers, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | Digital I/O, UART, I2C, SPI, UART Data Transfer, ADC, DAC, Comparator, Timer Counter, PWM, PWM Gatekill, CANFD, MOTIF, CORDIC, ECC |

### CAT1C
* Devices Supported: XMC7200D-E272K8384, XMC7100D-F176K4160

* Compilers Supported: GCC_ARM, ARM, IAR

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Interrupt, Clock, IPC, Startup Configuration Registers, DMAC, DMA/DW, WDT, Windowed Watchdog |
    | CPU                | CPU Registers, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | Digital I/O, UART, I2C, SPI, UART Data Transfer, ADC, Comparator, OP-AMP, Timer Counter, PWM, PWM Gatekill, CANFD |

### CAT2
* Device Families Supported: PSOC 4100S Max, PSOC 4500S, PSOC4100T Plus, PSOC4100S Plus

* Compilers Supported: GCC_ARM, ARM, IAR

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | Analog group       | SAR ADC, OPAMP, LPCOMP |
    | System platform    | Interrupt, Clock, Startup Configuration Registers, WDT, DMAC |
    | CPU                | CPU Registers, Program Counter, Program Flow |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | Digital I/O, UART, I2C, SPI, UART Data Transfer, Timer Counter, PWM, PWM Gatekill |

* Note: To perform SRAM tests, update CY_SRAM_BASE, CY_SRAM_SIZE, CY_STACK_SIZE according to the device being tested.

## What Changed?

### v3.2.0
* Updated user documentation
* Added support for PSOC4100T Plus and PSOC4100S Plus device families
* Added support of CAT2 devices for the following tests:
    * DMAC
    * Timer Counter
    * PWM
    * PWM Gatekill

## Supported Software and Tools

This version of the ModusToolbox Safety Test Library was validated for the compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 3.4.0   |
| CAT1 Peripheral Driver Library                          | 3.14.0  |
| CAT2 Peripheral Driver Library                          | 2.15.0  |
| GCC Compiler                                            | 11.3.1  |
| IAR Compiler                                            | 9.50.2  |
| ARM Compiler                                            | 6.22    |


## More information

For more information, refer to the following documents:

* [ModusToolbox Safety Test Library README.md](./README.md)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [Infineon Technologies AG](https://www.infineon.com)

---
© 2025, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.

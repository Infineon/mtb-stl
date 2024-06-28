# ModusToolbox Safety Test Library 2.0.0


Refer to the [README.md](./README.md) for a complete description of the ModusToolbox Safety Test Library.

## Implementation Details

### CAT1A
* Initial Implementation

* Devices Supported: CY8C624ABZI-S2D44, CY8C6245LQI-S3D72, CY8CKIT-062S4, CY8CKIT-062-WIFI-BT

* Compiler Supported: GCC_ARM Compiler

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Digital I/O, Interrupt, Clock, IPC, Startup Configuration Registers, DMAC, DMA/DW, WDT |
    | CPU                | CPU Registers, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | UART, I2C, SPI, UART data transfer, ADC, Comparator, OP-AMP, Timer counter, PWM, PWM Gatekill, CANFD |

### CAT1C
* Initial Implementation

* Devices Supported: XMC7200D-E272K8384, XMC7100D-F176K4160

* Compiler Supported: GCC_ARM Compiler

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | System platform    | Digital I/O, Interrupt, Clock, IPC, Startup Configuration Registers, DMAC, DMA/DW, WDT, Windowed Watchdog |
    | CPU                | CPU Registers, Program Counter, Program Flow, FPU Registers |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | UART, I2C, SPI, UART data transfer, ADC, Comparator, OP-AMP, Timer counter, PWM, PWM Gatekill, CANFD |

### CAT2
 - No changes have been made to the previous release: https://github.com/Infineon/mtb-stl/releases/tag/release-v1.0.0
 - For CAT2, this is a pre-production release. It is not recommended for production use unless the functionality is delivered in Infineon-provided applications.

* Devices Supported: CY8CKIT-041S-MAX, CY8CKIT-045S

* Compiler Supported: GCC_ARM Compiler, IAR Compiler

* List of Self-Tests supported:
    | Group              | Module |
    | ------             | ------ |
    | Analog group       | SAR ADC, OPAMP, LPCOMP |
    | System platform    | Digital I/O, Interrupt, Clock, Startup Configuration Registers, WDT |
    | CPU                | CPU Registers, Program Counter |
    | Memory             | Flash, RAM, Stack |
    | Peripheral         | UART, I2C, SPI |

* Note: To perform SRAM tests, update CY_SRAM_BASE, CY_SRAM_SIZE, CY_STACK_SIZE according to the device being tested.

## Defect Fixes

* Initial release

## Supported Software and Tools

This version of the ModusToolbox Safety Test Library was validated for the compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 3.2.0   |
| CAT1 Peripheral Driver Library                          | 3.11.0  |
| CAT2 Peripheral Driver Library                          | 2.5.0   |
| Core Library                                            | 1.4.0   |
| GCC Compiler                                            | 11.3.1  |
| IAR Compiler                                            | 9.30.1  |


## More information

For more information, refer to the following documents:

* [ModusToolbox Safety Test Library README.md](./README.md)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [Infineon Technologies AG](https://www.infineon.com)

---
© 2023, Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation.

# ModusToolbox™ Safety Test Library (mtb-stl) - IEC 60730 Class B Compliance for Industrial and Home Appliance Safety

# Overview

The ModusToolbox™ Safety Test Library (mtb-stl) provides functional safety APIs to implement overall safety of a system that depends on automatic protection suitable for use in industrial environments and home appliances. STL is created in compliance with IEC 60730 Class B and IEC 61508 SIL 2 standards and is scalable to different MCUs.

# Features

- IEC 60730 Class B and IEC 61508 SIL 2 compliant self-test routines
- CPU register and execution tests
- SRAM and Flash memory tests
- Clock frequency monitoring
- Interrupt functionality verification
- Scalable architecture for different MCU families
- Minimal runtime overhead
- Pre-certified library components

# When to use

Use mtb-stl when developing safety-critical applications that require IEC 60730 Class B or IEC 61508 SIL 2 compliance for industrial control systems, home appliances, or other safety-critical products. The library is provided as source code that integrates with your application — you control which self-test functions to call during startup, runtime, or both. The pre-certified STL library components and supported device certifications help enable your system-level functional safety certification. You are responsible for calling the appropriate test functions at the correct points in your application and defining error handling strategies when tests fail. Infineon provides the STL middleware library and code examples in source form to support your integration.


# How to use

This section assumes that the environment is configured to use the board support package (BSP) for your kit and the ModusToolbox™ software is installed on your machine.

## 1. Add mtb-stl library to the project

Add the mtb-stl library using the Library Manager.

## 2. Include the header

Include `SelfTest.h` in any source file that calls STL APIs:

```c
#include "SelfTest.h"
```

## 3. Call self-test routines

Invoke the relevant self-test functions at startup or periodically in your application. The functions return `OK_STATUS` on success or an error code on failure.

Detailed integration examples are split into dedicated guides:

- [Core self-tests](guides/getting-started.md) - Demonstrates CPU, startup configuration register, FPU register, watchdog, and Digital I/O modules.
- [Memory and storage integrity self-tests](guides/memory-tests.md) - Demonstrates SRAM, stack, Flash integrity, and ECC modules.
- [Clock, interrupt, timer, and PWM self-tests](guides/clock-and-timing-tests.md) - Demonstrates interrupt, clock, timer/counter, PWM, and PWM GateKill modules.
- [Communication interface self-tests](guides/serial-and-fieldbus-tests.md) - Demonstrates I2C, SPI, UART, and CAN FD modules.
- [Analog and mixed-signal self-tests](guides/analog-and-mixed-signal-tests.md) - Demonstrates ADC, DAC, OPAMP, LPCOMP, and HWFILT3P3Z modules.
- [Advanced hardware block self-tests](guides/advanced-control-and-accelerator-tests.md) - Demonstrates IPC, DMA DW, MOTIF, and CORDIC modules.

# Industry Standards and Compliance

## MISRA-C:2012 compliance

STL source is documented with MISRA-specific deviations at the relevant declaration or implementation site. Project-wide deviations that apply across the library are listed below.

### Verification environment

| Component | Name | Version |
|-----------|------|---------|
| Test Specification | MISRA-C:2012 Guidelines for the use of the C language in critical systems | March 2013 |
| MISRA Checking Tool | Coverity Static Analysis Tool | 2022.12.0 |

### Project deviations

| MISRA Rule/Directive | Deviation Class | Rule Class | Rule/Directive Description | Description of Deviation(s) |
|----------------------|-----------------|------------|----------------------------|-----------------------------|
| 4.3 | D | R | Assembly language shall be encapsulated and isolated. | SRAM STL drivers contain C syntax within assembly code. |
| 3.1 | R | R | The character sequences `/*` and `//` shall not be used within a comment. | Allow Doxygen-style comments. |
| 5.5 | R | R | Identifiers shall be distinct from macro names. | The CAT2 PDL header `cyip_can.h` declares a struct field named `ERROR_STATUS`, matching the STL's return-status macro of the same name. |
| 8.3 | R | R | All declarations of an object or function shall use the same names and type qualifiers. | During code analysis, the same source files are compiled multiple times with device-specific options. Identifiers remain unique for each specific run. |
| 8.5 | R | R | An external object or function shall be declared once in one and only one file. | During code analysis, the same source files are compiled multiple times with device-specific options. Identifiers remain unique for each specific run. |
| 8.6 | R | R | An identifier with external linkage shall have exactly one external definition. | During code analysis, the same source files are compiled multiple times with device-specific options. Identifiers remain unique for each specific run. |
| 13.2 | R | R | The value of an expression and its persistent side effects shall be the same under all permitted evaluation orders and shall be independent from thread interleaving. | Clock self-test code performs a read-modify-write on a volatile counter/status shared with an ISR. |

# Release Notes and Changelog

- [RELEASE.md](./RELEASE.md) - Detailed release notes for all versions

# License

This software is provided under the **Infineon End User License Agreement (EULA)**. Use, reproduction, and distribution are permitted solely as described in the accompanying license agreement. The software may not be used in life-critical applications without explicit written approval from Infineon.

- [LICENSE](./LICENSE) - Infineon End User License Agreement

---

# Copyright

(c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
Technologies AG. All rights reserved.

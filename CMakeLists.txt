# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2025 Infineon Technologies AG,
# or an affiliate of Infineon Technologies AG.

# Add include directories
# Use absolute paths based on module directory
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/export)

# Add include directories for all subdirectories containing headers
# SelfTest.h includes headers from these subdirectories
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/analog)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/can)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/canfd)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/clock)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/cordic)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/cpu)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/crc)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/dma_dw)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/dmac)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/ecc)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/flash)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/fpu)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/intr)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/io)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/ipc)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/motif)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/pwm)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/pwm_gatekill)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/ram)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/regs)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/scb)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/stack)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/timer_counter)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/wdt)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stl/wwdt)

# Add all C source files from stl subdirectories
zephyr_sources(stl/analog/SelfTest_Analog.c)
zephyr_sources(stl/can/SelfTest_CAN.c)
zephyr_sources(stl/canfd/SelfTest_CANFD.c)
zephyr_sources(stl/clock/SelfTest_Clock.c)
zephyr_sources(stl/cordic/SelfTest_Cordic.c)
zephyr_sources(stl/cpu/SelfTest_CPU.c)
zephyr_sources(stl/crc/SelfTest_CRC_calc.c)
zephyr_sources(stl/dma_dw/SelfTest_DMA_DW.c)
zephyr_sources(stl/dmac/SelfTest_DMAC.c)
zephyr_sources(stl/ecc/SelfTest_ECC.c)
zephyr_sources(stl/flash/SelfTest_Flash.c)
zephyr_sources(stl/fpu/SelfTest_FPU_Regs.c)
zephyr_sources(stl/intr/SelfTest_Interrupt.c)
zephyr_sources(stl/io/SelfTest_IO.c)
zephyr_sources(stl/ipc/SelfTest_IPC.c)
zephyr_sources(stl/motif/SelfTest_Motif.c)
zephyr_sources(stl/pwm/SelfTest_PWM.c)
zephyr_sources(stl/pwm_gatekill/SelfTest_PWM_GateKill.c)
zephyr_sources(stl/ram/SelfTest_RAM.c)
zephyr_sources(stl/regs/SelfTest_ConfigRegisters.c)
zephyr_sources(stl/scb/SelfTest_I2C_SCB.c)
zephyr_sources(stl/scb/SelfTest_SPI_SCB.c)
zephyr_sources(stl/scb/SelfTest_UART_SCB.c)
zephyr_sources(stl/scb/SelfTest_UART_master_message.c)
zephyr_sources(stl/scb/SelfTest_UART_slave_message.c)
zephyr_sources(stl/stack/SelfTest_Stack.c)
zephyr_sources(stl/timer_counter/SelfTest_Timer_Counter.c)
zephyr_sources(stl/wdt/SelfTest_WDT.c)
zephyr_sources(stl/wwdt/SelfTest_WWDT.c)

# Add toolchain-specific assembly files
# The preprocessor in the C code will select the correct function based on compiler
# Most Zephyr builds use GCC, so default to that
zephyr_sources(stl/TOOLCHAIN_GCC_ARM/SelfTest_CPU_Regs_GCC.S)

# FPU register tests - only for devices with FPU (CAT1A/B/C, not PSoC4)
# Include based on SOC family to avoid linker conflicts
zephyr_sources_ifdef(CONFIG_SOC_FAMILY_INFINEON_CAT1A
  stl/TOOLCHAIN_GCC_ARM/COMPONENT_CAT1A/SelfTest_FPU_Regs_GCC.S)
zephyr_sources_ifdef(CONFIG_SOC_FAMILY_INFINEON_CAT1B
  stl/TOOLCHAIN_GCC_ARM/COMPONENT_CAT1B/SelfTest_FPU_Regs_GCC.S)
zephyr_sources_ifdef(CONFIG_SOC_FAMILY_INFINEON_CAT1C
  stl/TOOLCHAIN_GCC_ARM/COMPONENT_CAT1C/SelfTest_FPU_Regs_GCC.S)

# IAR toolchain support (if needed)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_IAR
  stl/TOOLCHAIN_IAR/SelfTest_CPU_Regs_IAR.S)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_IAR
  stl/TOOLCHAIN_IAR/COMPONENT_CAT1A/SelfTest_FPU_Regs_IAR.S)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_IAR
  stl/TOOLCHAIN_IAR/COMPONENT_CAT1B/SelfTest_FPU_Regs_IAR.S)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_IAR
  stl/TOOLCHAIN_IAR/COMPONENT_CAT1C/SelfTest_FPU_Regs_IAR.S)

# ARM Compiler support (if needed)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_ARM
  stl/TOOLCHAIN_ARM/SelfTest_CPU_Regs_ARM.s)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_ARM
  stl/TOOLCHAIN_ARM/COMPONENT_CAT1A/SelfTest_FPU_Regs_ARM.s)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_ARM
  stl/TOOLCHAIN_ARM/COMPONENT_CAT1B/SelfTest_FPU_Regs_ARM.s)
zephyr_sources_ifdef(CONFIG_TOOLCHAIN_ARM
  stl/TOOLCHAIN_ARM/COMPONENT_CAT1C/SelfTest_FPU_Regs_ARM.s)

# Note: PSoC4 (psoc4100tp) uses PDL which should be available through SOC support
# The cy_pdl.h header is included by the STL source files and should be
# available through the SOC's include paths

# Define clock configuration constants that are typically generated by ModusToolbox
# For PSoC4, the default IMO frequency is 24MHz
# This is required by SelfTest_Clock for CAT2 devices
if(CONFIG_SOC_FAMILY_INFINEON_PSOC4)
  zephyr_compile_definitions(-DCY_CFG_SYSCLK_IMO_FREQ_HZ=24000000UL)
endif()


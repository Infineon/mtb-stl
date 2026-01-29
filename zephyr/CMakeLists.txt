# Copyright (c) 2026 Aerlync Labs Inc.
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_POST_VENDOR_MTB_STL)

zephyr_library()

zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../export)

# Include component directories
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/cpu)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/ram)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/stack)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/flash)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/clock)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/wdt)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/fpu)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/analog)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/scb)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/regs)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/crc)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/io)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/dmac)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/intr)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/pwm)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/pwm_gatekill)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../stl/timer_counter)

# Define linker sections for CPU tests
zephyr_linker_sources_ifdef(CONFIG_POST_MTB_STL_CPU_PC
  SECTIONS
  ${CMAKE_CURRENT_SOURCE_DIR}/cpu_test_sections.ld
)

# Define linker sections for flash checksum (place at end of flash)
zephyr_linker_sources_ifdef(CONFIG_POST_MTB_STL_FLASH
  SECTIONS
  ${CMAKE_CURRENT_SOURCE_DIR}/flash_checksum.ld
)

# POST wrapper sources
zephyr_library_sources(mtb_stl_post.c)
zephyr_library_sources(mtb_stl_post_cfg.c)
zephyr_library_sources(mtb_stl_post_pincfg.c)

# MTB-STL library sources
zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_CPU
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/cpu/SelfTest_CPU.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/TOOLCHAIN_GCC_ARM/SelfTest_CPU_Regs_GCC.S
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_CFGREG
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/regs/SelfTest_ConfigRegisters.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/crc/SelfTest_CRC_calc.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_RAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/ram/SelfTest_RAM.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_STACK
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/stack/SelfTest_Stack.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_FLASH
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/flash/SelfTest_Flash.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_CLOCK
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/clock/SelfTest_Clock.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_GPIO
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/io/SelfTest_IO.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_INTERRUPT
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/intr/SelfTest_Interrupt.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_WDT
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/wdt/SelfTest_WDT.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_DMA
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/dmac/SelfTest_DMAC.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_FPU
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/fpu/SelfTest_FPU_Regs.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_ANALOG
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/analog/SelfTest_Analog.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_UART_LOOPBACK
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/scb/SelfTest_UART_SCB.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/common/SelfTest_common.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_COUNTER
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/timer_counter/SelfTest_Timer_Counter.c
  ${CMAKE_CURRENT_SOURCE_DIR}/cy_sysint_zephyr.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_PWM
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/pwm/SelfTest_PWM.c
  ${CMAKE_CURRENT_SOURCE_DIR}/cy_sysint_zephyr.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_PWM_GATEKILL
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/pwm_gatekill/SelfTest_PWM_GateKill.c
)

zephyr_library_sources_ifdef(CONFIG_POST_MTB_STL_COMM
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/scb/SelfTest_UART_master_message.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/scb/SelfTest_UART_slave_message.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../stl/crc/SelfTest_CRC_calc.c
)


endif()

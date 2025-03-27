/***************************************************************************//**
* \file SelfTest.h
* \version 1.0.0
*
* Include this file in all of your source files that access mtb-stl middleware.
*
*******************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#if !defined(SELFTEST_H)
    #define SELFTEST_H

#include "cy_pdl.h"
#include "SelfTest_Analog.h"
#include "SelfTest_Clock.h"
#include "SelfTest_ConfigRegisters.h"
#include "SelfTest_CPU.h"
#include "SelfTest_CPU_Regs.h"
#include "SelfTest_CRC_calc.h"
#include "SelfTest_Flash.h"
#include "SelfTest_I2C_SCB.h"
#include "SelfTest_Interrupt.h"
#include "SelfTest_IO.h"
#include "SelfTest_RAM.h"
#include "SelfTest_SPI_SCB.h"
#include "SelfTest_Stack.h"
#include "SelfTest_UART_SCB.h"
#include "SelfTest_WDT.h"
#include "SelfTest_UART_master_message.h"
#include "SelfTest_UART_slave_message.h"
#include "SelfTest_DMAC.h"
#include "SelfTest_PWM.h"
#include "SelfTest_PWM_GateKill.h"
#include "SelfTest_Timer_Counter.h"

#if (defined(CY_CPU_CORTEX_M4) || defined(CY_CPU_CORTEX_M7))
#include "SelfTest_FPU_Regs.h"
#include "SelfTest_DMA_DW.h"
#include "SelfTest_IPC.h"
#endif

#if defined(CY_CPU_CORTEX_M33)
#include "SelfTest_Motif.h"
#include "SelfTest_FPU_Regs.h"
#include "SelfTest_DMA_DW.h"
#include "SelfTest_IPC.h"
#include "SelfTest_Cordic.h"
#include "SelfTest_ECC.h"
#endif

#if defined(CY_CPU_CORTEX_M7)
#include "SelfTest_WWDT.h"
#include "SelfTest_ECC.h"
#endif

#if defined(CY_CPU_CORTEX_M7) || defined(CY_CPU_CORTEX_M33) || defined(CY_DEVICE_PSOC6A256K) || defined(CY_DEVICE_PSOC6A512K)
#include "SelfTest_CANFD.h"
#endif

#include "UART_Debug.h"

#endif
/* [] END OF FILE */

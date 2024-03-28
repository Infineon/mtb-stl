/***************************************************************************//**
* \file SelfTest.h
* \version 1.0.0
*
* Include this file in all of your source files that access mtb-stl middleware.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#if CY_CPU_CORTEX_M4
#include "SelfTest_FPU_Regs.h"
#include "SelfTest_DMAC.h"
#include "SelfTest_DMA_DW.h"
#include "SelfTest_IPC.h"

#elif CY_CPU_CORTEX_M7
#include "SelfTest_FPU_Regs.h"
#include "SelfTest_DMAC.h"
#include "SelfTest_DMA_DW.h"
#include "SelfTest_IPC.h"
#include "SelfTest_WWDT.h"

#endif

/* TO DO: Remove UART_Debug.h in scope of MIDDLEWARE-11507 */
#include "UART_Debug.h"


/* [] END OF FILE */

/***************************************************************************//**
 * \file SelfTest.h
 * \version 1.0.0
 *
 * Include this file in all of your source files that access mtb-stl middleware.
 *
 *******************************************************************************
 * (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 ******************************************************************************/

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
#include "SelfTest_CAN.h"
#include "SelfTest_FPU_Regs.h"
#include "SelfTest_DMA_DW.h"
#include "SelfTest_IPC.h"
#include "SelfTest_Motif.h"
#include "SelfTest_Cordic.h"
#include "SelfTest_ECC.h"
#include "SelfTest_WWDT.h"
#include "SelfTest_CANFD.h"

#endif /* if !defined(SELFTEST_H) */
/* [] END OF FILE */

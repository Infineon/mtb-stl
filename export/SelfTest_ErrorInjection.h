/*******************************************************************************
* File Name: SelfTest_ErrorInjection.h
* Version 1.0.0
*
* Description:
*  This file provides defines to force artificial errors and to
*  provoke detection of defects by self tests for CAT2(PSoC4), CAT1A,
*  CAT1B(PSoC C3), CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#if !defined(SELFTESTERRORINJECTION_H)
    #define SELFTESTERRORINJECTION_H

#define ERROR_IN_ADC                                        0u
#define ERROR_IN_OPAMP                                      0u
#define ERROR_IN_COMPARATOR                                 0u
#define ERROR_IN_CLOCK                                      0u
#define ERROR_IN_PROGRAM_COUNTER                            0u
#define ERROR_IN_PROGRAM_FLOW                               0u
#define ERROR_IN_FLASH                                      0u
#define ERROR_IN_INTERRUPT_HANDLING                         0u
#define ERROR_IN_DIGITAL_IO                                 0u
#define ERROR_IN_STACK_OVERFLOW                             0u
#define ERROR_IN_STACK_UNDERFLOW                            0u
#define ERROR_IN_STARTUP_CONF_REG                           0u
#define ERROR_IN_SPI_SCB                                    0u
#define ERROR_IN_UART_SCB                                   0u
#define ERROR_IN_I2C_SCB                                    0u
#define ERROR_IN_WDT                                        0u
#define ERROR_IN_PROTOCOL                                   0u
#define ERROR_IN_IPC                                        0u
#define ERROR_IN_UART_DATA_XFER                             0u
#define ERROR_IN_WWDT_LOWER_LIMIT                           0u
#define ERROR_IN_WWDT_INTR                                  0u
#define ERROR_IN_TIMER_COUNTER                              0u
#define ERROR_IN_PWM                                        0u
#define ERROR_IN_CANFD                                      0u
#define ERROR_IN_PWM_GATEKILL                               0u
#define ERROR_IN_DMA_DW                                     0u
#define ERROR_IN_DMAC                                       0u
#define ERROR_IN_DAC                                        0u
#define ERROR_IN_SRAM_MARCH                                 0u
#define ERROR_IN_SRAM_GALPAT                                0u
#define ERROR_IN_MOTIF                                      0u
#define ERROR_IN_CORDIC                                     0u
#define ERROR_IN_ECC                                        0u
#endif /* SELFTESTERRORINJECTION_H */

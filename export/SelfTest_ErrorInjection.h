/*******************************************************************************
* File Name: SelfTest_ErrorInjection.h
*
* Description:
*  This file provides defines to force artificial errors and to
*  provoke detection of defects by self tests.
*
*******************************************************************************
* (c) 2024, Infineon Technologies AG, or an affiliate of Infineon
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
#define ERROR_IN_CAN                                        0u
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

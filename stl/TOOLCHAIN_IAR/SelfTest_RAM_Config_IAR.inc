/*------------------------------------------------------------------------------
* FILENAME: SelfTest_RAM_Config_IAR.inc
* Version 2.0
*
* DESCRIPTION:
*  This file contains functions to config the SRAM self test using March method.
*  Values in this file should match values found in the application's linker
*  script.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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

#define CY_SRAM_BASE    (0x20000000UL)

/* 1KB Default Stack Size */
#define CY_STACK_SIZE   (0x00000400UL)

/******************************************
 * 32KB SRAM Size
 * Compatible with 32KB SRAM versions of the
 * following parts
 *   4100S Max
 *   4500S
 *   4100S Plus 256K
 ******************************************/
#define CY_SRAM_SIZE    (0x00008000UL)

/******************************************
 * 8KB SRAM Size
 * Compatible with 8KB SRAM versions of the
 * following parts
 *   4100S
 ******************************************/
//#define CY_SRAM_SIZE    (0x00002000UL)

/* [] END OF FILE */

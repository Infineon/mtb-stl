/*******************************************************************************
* File Name: UART_Debug.c
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for UART debug output.
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

#include "cy_pdl.h"
#include "cybsp.h"
#include "UART_Debug.h"
#include "SelfTest_Config.h"


/*****************************************************************************
* Function Name: DEBUG_PutByteHex
******************************************************************************
*
* Summary:
*  Outputs one byte in HEX form to UART DEBUG output
*
* Parameters:
*  uint8_t value - byte to output
*
* Return:
*  NONE
*
* Note:
*
*****************************************************************************/
void DEBUG_PutByteHex(uint8_t value)
{
    const uint8_t hex[] = "0123456789ABCDEF";
    uint8_t data;
    uint8_t* hex_value;

    hex_value = &data;

    data = hex[value >> 4u];
    #if (SELFTEST_CONFIG_DEBUG == 1)
    Cy_SCB_UART_PutArrayBlocking(CYBSP_UART_HW, hex_value, 1u);
    #else
    (void)hex_value;
    __NOP();
    #endif

    data = hex[value & 0x0Fu];
    #if (SELFTEST_CONFIG_DEBUG == 1)
    Cy_SCB_UART_PutArrayBlocking(CYBSP_UART_HW, hex_value, 1u);
    #else
    (void)hex_value;
    __NOP();
    #endif
}


/*****************************************************************************
* Function Name: DEBUG_PutWordHex
******************************************************************************
*
* Summary:
*  Outputs one 16-bit word in HEX form to UART DEBUG output
*
* Parameters:
*  uint16_t value - word to output
*
* Return:
*  NONE
*
* Note:
*
*****************************************************************************/
void DEBUG_PutWordHex(uint16_t value)
{
    DEBUG_PutByteHex((uint8_t)(value >> 8u));
    DEBUG_PutByteHex((uint8_t)(value & 0xFFu));
}


/*****************************************************************************
* Function Name: DEBUG_PutLongHex
******************************************************************************
*
* Summary:
*  Outputs one 32bit dword in HEX form to UART DEBUG output
*
* Parameters:
*  uint32_t value - dword to output
*
* Return:
*  NONE
*
* Note:
*
*****************************************************************************/
void DEBUG_PutLongHex(uint32_t value)
{
    DEBUG_PutWordHex((uint16_t)(value >> 16u));
    DEBUG_PutWordHex((uint16_t)(value & 0xFFFFu));
}


/*****************************************************************************
* Function Name: DEBUG_PutString
******************************************************************************
*
* Summary:
* Outputs the string of characters
*
* Parameters:
* char* value - Character array
*
* Return:
* NONE
*
* Note:
*
*****************************************************************************/
void DEBUG_PutString(char* value)
{
    #if (SELFTEST_CONFIG_DEBUG == 1)
    Cy_SCB_UART_PutString(CYBSP_UART_HW, value);
    #else
    (void)value;
    __NOP();
    #endif

}


/* [] END OF FILE */

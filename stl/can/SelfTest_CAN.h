/*******************************************************************************
* File Name: SelfTest_CAN.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the CAN self tests according to Class B library.
*
*******************************************************************************
* Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
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
/**
 * \addtogroup group_can
 * \{
 *
 * The CAN block is tested using the internal loopback capability.
 * CAN is configured to accept message IDs in the range 0x50 to 0x55.
 * A message with message ID 0x60 is transmitted and RX fifo is
 * verified to be empty. Similarly, a message with message ID 0x52 is
 * transmitted and the message received in RX FIFO is verified.
 *
 *
 * \note
 * Applicable only to CAT2 devices.
 *
 * \defgroup group_can_functions Functions
 */

#if !defined(SELFTEST_CAN_H)
    #define SELFTEST_CAN_H
#include "cy_pdl.h"
#include "SelfTest_common.h"

#if defined(CY_IP_M0S8CAN)

#include "cy_can.h"

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_can_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_CAN
****************************************************************************//**
*
*  This function performs a self-test of the CAN hardware. It recconfigures the
*  CAN module in the selected loopback mode and transmits test messages.
*  The function verifies if the transmitted messages are received correctly and
*  restores the original configuration.
*
*
* @param base
* The pointer to a CAN instance. <br>
* @param config
* The pointer to the CAN configuration structure.
* @param context
* The pointer to the CAN context structure allocated by the user.
* @param testMode
* The test mode to be used for the self-test.
*
* @return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_CAN(CAN_Type* base, const cy_stc_can_config_t* config,
                     cy_stc_can_context_t* context,
                     cy_en_can_test_mode_t testMode);
/** \} group_can_functions */

#endif // if defined(CY_IP_M0S8CAN)

/** \} group_can */
#endif /* SELFTEST_CAN_H */

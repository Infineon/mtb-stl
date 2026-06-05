/*******************************************************************************
* File Name: SelfTest_CAN.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the CAN self tests according to Class B library.
*
*******************************************************************************
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
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

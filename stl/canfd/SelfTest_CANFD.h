/*******************************************************************************
* File Name: SelfTest_CANFD.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the CAN-FD self tests according to Class B library.
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
*******************************************************************************/
/**
 * \addtogroup group_canfd
 * \{
 *
 * The CAN-FD block is tested using the internal loopback capability.
 * CAN-FD is configure to accept message IDs in the range 0x50 to 0x55
 * and a message with message ID 0x60 is transmitted and RX fifo is
 * verified to be empty. Similarly, a message with message ID 0x52 is
 * transmitted and the message received in RX FIFO is verified.
 *
 * \note
 * Only applicable to CAT1C and CAT2 devices
 *
 * \defgroup group_canfd_functions Functions
 */

#if !defined(SELFTEST_CANFD_H)
    #define SELFTEST_CANFD_H
#include "cy_pdl.h"
#include "cy_canfd.h"
#include "SelfTest_common.h"

#if (defined (CY_IP_MXTTCANFD) || defined (CY_DOXYGEN))

#include "cycfg.h"

/** \cond INTERNAL */
/** Test Mode Type */

/** \endcond */

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_canfd_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_CANFD
****************************************************************************//**
*
* This function configure the CANFD channel in an internal loopback and then
* it is configure to accept message IDs in the range 0x50 to 0x55
* and a message with message ID 0x60 is transmitted and RX fifo is
* verified to be empty. Similarly, a message with message ID 0x50 is
* transmitted and the message received in RX FIFO is verified.
*
*
* \param base
* The pointer to a CAN FD instance
* \param chan
* The CAN FD channel number
* \param config
* The pointer to the CAN FD configuration structure
* \param context
* The pointer to the context structure allocated
* by the user. The structure is used during the CAN FD operation for internal
* configuration and data retention. The user must not modify anything in this
* structure.
* \param test_mode
* internal: will not drive the pin <br>
* external: will drive the external pins along with loopback
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_CANFD(CANFD_Type* base, uint32_t chan,
                       const cy_stc_canfd_config_t* config,
                       cy_stc_canfd_context_t* context,
                       cy_stc_canfd_test_mode_t test_mode);
/** \} group_canfd_functions */

#endif /* if (defined (CY_IP_MXTTCANFD) || defined (CY_DOXYGEN)) */

/** \} group_canfd */
#endif /* SELFTEST_CANFD_H */

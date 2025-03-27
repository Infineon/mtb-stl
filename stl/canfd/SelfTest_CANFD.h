/*******************************************************************************
* File Name: SelfTest_CANFD.h
*
* Description:
*  This file provides the function prototypes, constants and parameter values used
*  for the CAN-FD self tests according to Class B library.
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
/**
* \addtogroup group_canfd
* \{
*
* The CAN-FD block is tested using the internal loopback capability.
* CAN-FD is configure to accept message IDs in the range 0x50 to 0x55
* and a message with message ID 0x60 is transmitted and RX fifo is 
* verified to be empty. Similarly, a message with message ID 0x50 is 
* transmitted and the message received in RX FIFO is verified. 
*
*
* \note
* Only applicable to CAT1C devices
*
* \defgroup group_canfd_functions Functions
*/

#if !defined(SELFTEST_CANFD_H)
    #define SELFTEST_CANFD_H
#include "cy_pdl.h"
#include "cy_canfd.h"
#include "SelfTest_common.h"

#if ((defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) ||  (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))  || ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A256K)) || ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A512K)))

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
* configuration and data retention. User must not modify anything in this
* structure
* \param test_mode 
* internal: will not drive the pin <br>
* external: will drive the external pins along with loopback
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_CANFD(CANFD_Type *base, uint32_t chan,
                                   const cy_stc_canfd_config_t *config,
                                   cy_stc_canfd_context_t *context,
                                   cy_stc_canfd_test_mode_t test_mode);
/** \} group_canfd_functions */

#endif

/** \} group_canfd */
#endif  /* SELFTEST_CANFD_H */
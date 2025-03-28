/*******************************************************************************
* File Name: SelfTest_DMAC.h
*
* Description:
* This file provides constants and parameter values used for DMAC self
* tests.
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
* \addtogroup group_dmac
* \{
*
* The DMAC test performs test on Direct Memory Access Controller using fixed size of DMAC transfer.
*
* \section group_dmac_more_information More Information
*
* This test verifies the DMAC data transfer functionality and it's integrity.
* Due to hardware differences, DMAC test algorithm is different for different
* device families. Refer to SelfTest_DMAC() function description for details.
*
*
* \defgroup group_dmac_macros Macros
* \defgroup group_dmac_functions Functions
*/

#if !defined(SELFTEST_DMAC_H)
    #define SELFTEST_DMAC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

/**
* \addtogroup group_dmac_macros
* \{
*/

/** DMAC data transfer timeout */
#define SELFTEST_DMAC_TIMEOUT   (50UL)

/** \} group_dmac_macros */


#if (defined(CY_IP_M4CPUSS_DMAC) || defined(CY_IP_M7CPUSS_DMAC) || defined (CY_DOXYGEN))

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_dmac_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_DMAC
****************************************************************************//**
*
* This function performs the DMAC channel test routine by transfering data
* between data arrays using DMAC and verifying if the destination block
* contain expected data pattern.
*
* The DMAC blocks are tested using the following procedure: <br>
* 1) A destination block of size 66 bytes is set to 0 with DMA transfers using 16 x 32-bit transfers from a fixed
*    address. <br>
* 2) The destination block is verified to be all 0. <br>
* 3) Another destination block is filled with 00 00 ff by using an 8-bit DMA transfers from a fixed address
*    with an increment of 1 and a length of 64. <br>
* 4) The destination block is verified to contain the correct pattern (shown below with lowest address first):
*    ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000…
*
*
* \param base
* The pointer to the hardware DMA block
* \param channel
* A channel number
* \param descriptor0
* This is the descriptor to be associated with the channel (transfer 0's to destination)
* \param descriptor1
* This is the descriptor to be associated with the channel (transfer pattern to destination)
* \param des0_config
* This is a configuration structure that has all initialization information for the descriptor
* \param des1_config
* This is a configuration structure that has all initialization information for the descriptor
* \param channelConfig
* The structure that has the initialization information for the channel
* \param trigLine
* The input of the trigger mux.
* -> Bit 30 represents if the signal is an input/output. When this bit is set, the trigger activation is for an output
* trigger from the trigger multiplexer. When this bit is reset, the trigger activation is for an input trigger to the trigger multiplexer. <br>
* -> Bits 12:8 represent the trigger group selection. <br>
* -> In case of output trigger line (bit 30 is set): For PERI_ver1: <br>
* -> Bits 6:0 select the output trigger number in the trigger group. For PERI_ver2: <br>
* -> Bits 7:0 select the output trigger number in the trigger group. In case of input trigger line (bit 30 is unset): <br>
* -> Bits 7:0 select the input trigger signal for the trigger multiplexer
*
* \note
* Only applicable for CAT1A and CAT1C devices
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_DMAC(DMAC_Type * base, uint32_t channel, cy_stc_dmac_descriptor_t * descriptor0, cy_stc_dmac_descriptor_t * descriptor1,
                        const cy_stc_dmac_descriptor_config_t * des0_config,const cy_stc_dmac_descriptor_config_t * des1_config,
                        cy_stc_dmac_channel_config_t const * channelConfig, en_trig_output_mdma_t trigLine);

#endif


#if (defined(CY_IP_M0S8CPUSSV3_DMAC) || defined (CY_DOXYGEN))

/*******************************************************************************
* Function Name: SelfTest_DMAC
****************************************************************************//**
*
* This function performs the DMAC channel test routine by transfering data
* between data arrays using DMAC and verifying if the destination block
* contain expected data pattern.
*
* The DMAC blocks are tested using the following procedure: <br>
* 1) A destination block of size 64 bytes is set to 0 with DMA transfers using 16 x 32-bit transfers from a fixed
*    address. <br>
* 2) The destination block is verified to be all 0. <br>
* 3) The same destination block is filled with 00 00 ff by using an 8-bit DMA transfers from a fixed address
*    with an increment of 1 and a length of 64. <br>
* 4) The destination block is verified to contain the correct pattern (shown below with lowest address first):
*    ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000…
*
* \param base
* The pointer to the hardware DMAC block
*
* \param channel
* The DMAC channel number
*
* \param trigLine
* The input trigger to start data transfer for selected DMAC channel.
* Refer to device TRM for details on trigLine value selection
*
* \note
* Only applicable for CAT2 devices
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_DMAC(DMAC_Type * base, uint32_t channel, uint32_t trigLine);

#endif

/** \} group_dmac_functions */

/** \} group_dmac */
#endif
/* [] END OF FILE */
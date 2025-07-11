/*******************************************************************************
* File Name: SelfTest_DMA_DW.h
*
* Description:
* This file provides constants and parameter values used for DMA_DW self
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
 * \addtogroup group_dma
 * \{
 *
 * The DMA test performs test on DMA block using DW transfers.
 *
 * \section group_dma_more_information More Information
 *
 *      The DMA block test procedure:
 *
 *      1) A destination 64-byte block is set to 0 with DW transfers using 16 x 32-bit
 *         transfers from a fixed address.
 *      2) The destination block is verified to be all 0.
 *      3) The same destination block is filled with 00 00 ff by using an 8-bit DMA from a fixed
 *         address with increment 3 and length 22 (64+(3-1))/3.
 *      4) The destination block is verified to contain the correct pattern (shown below with the lowest
 *         address first):
 *         ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000…
 *
 *
 * \defgroup group_dma_functions Functions
 */

#if !defined(SELFTEST_DMA_DW_H)
    #define SELFTEST_DMA_DW_H

#include "SelfTest_common.h"
#include <string.h>
#if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_IP_M7CPUSS_DMA) || \
    defined (CY_DOXYGEN))
/***************************************
* Function Prototypes
***************************************/

/**
 * \addtogroup group_dma_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_DMA_DW
****************************************************************************//**
*
* This function writes a pattern (66 bytes) to the destination using the DMA, and
* the destination block is verified to contain the correct pattern.
*
*
* \param base
* The pointer to the hardware DMA block.<br>
* \param channel
* The channel number.
* \param descriptor0
* The descriptor to associate with the channel (transfer 0's to destination).
* \param descriptor1
* The descriptor to associate with the channel (transfer pattern to destination).
* \param des0_config
* The configuration structure with all initialization information for the descriptor.
* \param des1_config
* The configuration structure with all initialization information for the descriptor.
* \param channelConfig
* The structure that contains the initialization information for the channel.
* \param trigLine
* The input of the trigger mux.
* -> Bit 30 represents if the signal is an input/output. When this bit is set, the trigger
*  activation is for an output trigger from the trigger multiplexer. When this bit is reset,
*  the trigger activation is for an input trigger to the trigger multiplexer. <br>
* -> Bits 12:8 represent the trigger group selection. <br>
* -> For the output trigger line (bit 30 is set): For PERI_ver1: <br>
* -> Bits 6:0 select the output trigger number in the trigger group. For PERI_ver2: <br>
* -> Bits 7:0 select the output trigger number in the trigger group. In case of input trigger line
*  (bit 30 is unset): <br>
* -> Bits 7:0 select the input trigger signal for the trigger multiplexer  <br>
*
*
* \note
* Applicable only for CAT1A, CAT1B(PSoC C3) and CAT1C devices.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/


#if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_DOXYGEN))

    #if defined(CY_DEVICE_PSOC6ABLE2)
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_input_grp0_t trigLine);
    #else
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_output_pdma0_tr_t trigLine);
    #endif /* if defined(CY_DEVICE_PSOC6ABLE2) */






#elif defined (CY_IP_M7CPUSS_DMA)
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_output_pdma0_tr_0_t trigLine);
#endif /* if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_DOXYGEN)) */


/** \} group_dma_functions */


/***************************************
* Initial Parameter Constants
***************************************/


#endif /* if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_IP_M7CPUSS_DMA) ||
          defined (CY_DOXYGEN)) */

/** \} group_dma */
#endif /* if !defined(SELFTEST_DMA_DW_H) */
/* [] END OF FILE */

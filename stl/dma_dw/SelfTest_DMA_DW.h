/*******************************************************************************
* File Name: SelfTest_DMA_DW.h
*
* Description:
* This file provides constants and parameter values used for DMA_DW self
* tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
* Constants
***************************************/
/** Timeout in microseconds for DMA transfer completion */
#define SELFTEST_DMA_DW_TIMEOUT   (50UL)

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


uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        uint32_t trigLine);


/** \} group_dma_functions */


/***************************************
* Initial Parameter Constants
***************************************/


#endif /* if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_IP_M7CPUSS_DMA) ||
          defined (CY_DOXYGEN)) */

/** \} group_dma */
#endif /* if !defined(SELFTEST_DMA_DW_H) */
/* [] END OF FILE */

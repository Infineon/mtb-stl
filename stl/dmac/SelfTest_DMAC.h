/*******************************************************************************
* File Name: SelfTest_DMAC.h
*
* Description:
* This file provides constants and parameter values used for DMAC self
* tests.
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
 * \addtogroup group_dmac
 * \{
 *
 * The DMAC test performs tests on Direct Memory Access Controller using a fixed size of DMAC transfer.
 *
 * \section group_dmac_more_information More Information
 *
 * This test verifies the DMAC data transfer functionality and its integrity.
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
* contains the expected data pattern.
*
* The DMAC block test procedure: <br>
* 1) A 66-bytes destination block is set to 0 with DMA transfers using 16 x 32-bit transfers
*    from a fixed address. <br>
* 2) The destination block is verified to be all 0. <br>
* 3) Another destination block is filled with 00 00 ff by using an 8-bit DMA transfers from a fixed
*    address with an increment of 1 and a length of 64. <br>
* 4) The destination block is verified to contain the correct pattern (shown below with lowest
*    address first): ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000…
*
*
* \param base
* The pointer to the hardware DMA block
* \param channel
* The channel number.
* \param descriptor0
* This is the descriptor to associate with the channel (transfer 0's to destination).
* \param descriptor1
* This is the descriptor to associate with the channel (transfer pattern to destination).
* \param des0_config
* This is a configuration structure with all initialization information for the descriptor.
* \param des1_config
* This is a configuration structure with all initialization information for the descriptor.
* \param channelConfig
* The structure with the initialization information for the channel.
* \param trigLine
* The input of the trigger mux.
* -> Bit 30 represents if the signal is an input/output. When this bit is set, the trigger
*    activation is for an output trigger from the trigger multiplexer. When this bit is reset,
*    the trigger activation is for an input trigger to the trigger multiplexer. <br>
* -> Bits 12:8 represent the trigger group selection. <br>
* -> For the output trigger line (bit 30 is set): For PERI_ver1: <br>
* -> Bits 6:0 select the output trigger number in the trigger group. For PERI_ver2: <br>
* -> Bits 7:0 select the output trigger number in the trigger group. For the input trigger line
*    (bit 30 is unset): <br>
* -> Bits 7:0 select the input trigger signal for the trigger multiplexer.
*
* \note
* Applicable only for CAT1A and CAT1C devices.
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_DMAC(DMAC_Type* base, uint32_t channel, cy_stc_dmac_descriptor_t* descriptor0,
                      cy_stc_dmac_descriptor_t* descriptor1,
                      const cy_stc_dmac_descriptor_config_t* des0_config,
                      const cy_stc_dmac_descriptor_config_t* des1_config,
                      cy_stc_dmac_channel_config_t const* channelConfig,
                      en_trig_output_mdma_t trigLine);

#endif /* if (defined(CY_IP_M4CPUSS_DMAC) || defined(CY_IP_M7CPUSS_DMAC) || defined (CY_DOXYGEN)) */


#if (defined(CY_IP_M0S8CPUSSV3_DMAC) || defined (CY_DOXYGEN))

/*******************************************************************************
* Function Name: SelfTest_DMAC
****************************************************************************//**
*
* This function performs the DMAC channel test routine by transfering data
* between data arrays using DMAC and verifying if the destination block
* contains the expected data pattern.
*
* The DMAC block test procedure: <br>
* 1) A 64-byte destination block is set to 0 with DMA transfers using 16 x 32-bit transfers
*    from a fixed address. <br>
* 2) The destination block is verified to be all 0. <br>
* 3) The same destination block is filled with 00 00 ff by using an 8-bit DMA transfers from a fixed
*    address with an increment of 1 and a length of 64. <br>
* 4) The destination block is verified to contain the correct pattern (shown below with lowest
*    address first): ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000…
*
* \param base
* The pointer to the hardware DMAC block
*
* \param channel
* The DMAC channel number
*
* \param trigLine
* The input trigger to start data transfer for the selected DMAC channel.
* Refer to device TRM for details on trigLine value selection
*
* \note
* Applicable only for CAT2 devices.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_DMAC(DMAC_Type* base, uint32_t channel, uint32_t trigLine);

#endif

/** \} group_dmac_functions */

/** \} group_dmac */
#endif /* if !defined(SELFTEST_DMAC_H) */
/* [] END OF FILE */

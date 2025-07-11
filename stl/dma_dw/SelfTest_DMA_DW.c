/*******************************************************************************
* File Name: SelfTest_DMA_DW.c
*
* Description:
*  This file provides the source code for DMAC Self Tests.
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

#if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW) || defined (CY_IP_M7CPUSS_DMA))
#include "SelfTest_DMA_DW.h"

#ifndef ERRORINJECTION_H
#define ERRORINJECTION_H
#include "SelfTest_ErrorInjection.h"

#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
CY_SECTION_SHAREDMEM static uint32_t data_src_0[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
CY_SECTION_SHAREDMEM static uint8_t data_src_1[66] =
{
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff
};

CY_SECTION_SHAREDMEM static uint32_t data_dst_0[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
CY_SECTION_SHAREDMEM static uint8_t data_dst_1[66] =
{
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};

#else /* if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE) */
static const uint32_t data_src_0[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t data_src_1[66] =
{
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0xff
};

static uint32_t data_dst_0[16];
static uint8_t data_dst_1[66];
#endif /* if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE) */


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
#if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW))
    #if defined(CY_DEVICE_PSOC6ABLE2)
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_input_grp0_t trigLine)
     #else
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_output_pdma0_tr_t trigLine)
    #endif /* if defined(CY_DEVICE_PSOC6ABLE2) */
#elif defined (CY_IP_M7CPUSS_DMA)
uint8_t SelfTest_DMA_DW(DW_Type* base, uint32_t channel, cy_stc_dma_descriptor_t* descriptor0,
                        cy_stc_dma_descriptor_t* descriptor1,
                        const cy_stc_dma_descriptor_config_t* des0_config,
                        const cy_stc_dma_descriptor_config_t* des1_config,
                        cy_stc_dma_channel_config_t const* channelConfig,
                        en_trig_output_pdma0_tr_0_t trigLine)
#endif /* if (defined (CY_IP_M4CPUSS_DMA) || defined (CY_IP_MXDW)) */
{
    uint8_t ret = OK_STATUS;
    (void)memset(data_dst_0, 0xAA, sizeof(data_dst_0));
    (void)memset(data_dst_1, 0, sizeof(data_dst_1));

    /* Init Descriptors */
    cy_en_dma_status_t dma_status = Cy_DMA_Descriptor_Init(descriptor0, des0_config);
    dma_status = Cy_DMA_Descriptor_Init(descriptor1, des1_config);

    #if (ERROR_IN_DMA_DW == 0u)
    /* Set source and dest address */
    /* Descriptor 0 */
    Cy_DMA_Descriptor_SetSrcAddress(descriptor0, data_src_0);
    Cy_DMA_Descriptor_SetDstAddress(descriptor0, data_dst_0);
    /* Descriptor 1 */
    Cy_DMA_Descriptor_SetSrcAddress(descriptor1, data_src_1);
    Cy_DMA_Descriptor_SetDstAddress(descriptor1, data_dst_1);
    #endif /* End (ERROR_IN_DMA_DW == 0) */

    #if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_CleanDCache_by_Addr(descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_CleanDCache_by_Addr(descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    #endif
    /* Enable DMA */
    Cy_DMA_Enable(base);


    dma_status = Cy_DMA_Channel_Init(base, channel, channelConfig);
    Cy_DMA_Channel_Enable(base, channel);

    cy_en_trigmux_status_t trigmux_status = Cy_TrigMux_SwTrigger((uint32_t)trigLine,
                                                                 CY_TRIGGER_TWO_CYCLES);

    uint32_t interruptStatus = Cy_DMA_Channel_GetInterruptStatus(base, channel);
    while (interruptStatus == 0UL)
    {
        interruptStatus = Cy_DMA_Channel_GetInterruptStatus(base, channel);
    }

    #if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_InvalidateDCache_by_Addr(descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_InvalidateDCache_by_Addr(descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    #endif

    int32_t cmpRes;

    #if (ERROR_IN_DMA_DW == 1u)
    data_dst_0[0] = 1;
    #endif /* End (ERROR_IN_DMA_DW == 1u) */
    Cy_SysLib_DelayUs(10u);
    cmpRes = memcmp(data_src_0, data_dst_0, 64);
    if (cmpRes != 0)
    {
        ret = ERROR_STATUS;
    }

    cmpRes = memcmp(data_src_1, data_dst_1, 64);
    if (cmpRes != 0)
    {
        ret = ERROR_STATUS;
    }
    (void)trigmux_status;
    (void)dma_status;
    return ret;
}


#endif /* ifndef ERRORINJECTION_H */
#endif /* ERRORINJECTION_H */
/* [] END OF FILE */

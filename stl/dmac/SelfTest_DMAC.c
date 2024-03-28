/*******************************************************************************
* File Name: SelfTest_DMAC.h
* Version 1.0.0
*
* Description:
*  This file provides the source code for DMAC self tests for  CAT1A and CAT1C devices.
*
*
* Hardware Dependency:
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
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
#if (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7)

#include "SelfTest_DMAC.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

static const uint32_t data_src_0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const uint8_t data_src_1[64] = {0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,
                                0x00,0x00,0xff,0x00};

static uint32_t data_dst_0[16];          /* Descriptor_0 Data Transfer destination */
static uint8_t data_dst_1[64];          /* Descriptor_1 Data Transfer destination */
/*******************************************************************************
 * Function Name: SelfTest_DMA/DW
 ********************************************************************************
 *
 * Summary:

 *
 * Parameters:
 *  None
 *
 * Return:
 *  Result of test:  "0" - pass test;
 *                   "1" - fail test (Shorts to VCC);
 *                   "2" - fail test (Shorts to GND);
 *
 **********************************************************************************/
uint8_t SelfTest_DMAC(DMAC_Type * base, uint32_t channel, cy_stc_dmac_descriptor_t * descriptor0, cy_stc_dmac_descriptor_t * descriptor1,
                        const cy_stc_dmac_descriptor_config_t * des0_config,const cy_stc_dmac_descriptor_config_t * des1_config,
                        cy_stc_dmac_channel_config_t const * channelConfig, en_trig_output_mdma_t trigLine)
{
    uint8_t ret = 0;
    memset(data_dst_0, 0, 16*sizeof(data_dst_0[0]));
    memset(data_dst_1, 0, 64*sizeof(data_dst_1[0]));

    // Init Descriptors
    Cy_DMAC_Descriptor_Init(descriptor0, des0_config);
    Cy_DMAC_Descriptor_Init(descriptor1, des1_config);
	
    // Set source and dest address
    //Descriptor 0
    Cy_DMAC_Descriptor_SetSrcAddress(descriptor0, data_src_0);
	Cy_DMAC_Descriptor_SetDstAddress(descriptor0, data_dst_0);
    // Descriptor 1
	Cy_DMAC_Descriptor_SetSrcAddress(descriptor1, data_src_1);
	Cy_DMAC_Descriptor_SetDstAddress(descriptor1, data_dst_1);

    // Enable DMA
    Cy_DMAC_Enable(base);


    Cy_DMAC_Channel_Init(base, channel, channelConfig);
#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_CleanDCache();
#endif
	Cy_DMAC_Channel_Enable(base, channel);

	Cy_TrigMux_SwTrigger(trigLine, CY_TRIGGER_TWO_CYCLES);
	while(!Cy_DMAC_Channel_GetInterruptStatus(base,channel))
	{

	}
#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_InvalidateDCache();
#endif

    int32_t cmpRes;
    cmpRes = memcmp(data_src_0, data_dst_0, 64);
    if (cmpRes != 0)
    {
        ret = 1;
    }

    cmpRes = memcmp(data_src_1, data_dst_1, 64);
    if (cmpRes != 0)
    {
        ret = 1;
    }

    return ret;
}

#endif
/* [] END OF FILE */

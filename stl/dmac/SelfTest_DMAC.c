/*******************************************************************************
* File Name: SelfTest_DMAC.c
* Version 1.0.0
*
* Description:
*  This file provides the source code for DMAC self tests.
*
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

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)))

#if ((defined(CY_IP_M4CPUSS_DMAC) && (CY_IP_M4CPUSS_DMAC)) || (defined(CY_IP_M7CPUSS_DMAC) && (CY_IP_M7CPUSS_DMAC)))

#include "SelfTest_DMAC.h"

#ifndef ERRORINJECTION_H
#define ERRORINJECTION_H
#include "SelfTest_ErrorInjection.h"

#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
CY_SECTION_SHAREDMEM uint32_t dmac_data_src_0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
CY_SECTION_SHAREDMEM uint8_t dmac_data_src_1[66] = {
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
    0x00,0x00,0xff,
    0x00,0x00,0xff,
    0x00,0x00,0xff
    };

CY_SECTION_SHAREDMEM uint32_t dmac_data_dst_0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
CY_SECTION_SHAREDMEM uint8_t dmac_data_dst_1[66] = {
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0,
    0x0,0x0,0x0
    };
#else
const uint32_t dmac_data_src_0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const uint8_t dmac_data_src_1[66] = {
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
    0x00,0x00,0xff,
    0x00,0x00,0xff,
    0x00,0x00,0xff
    };

uint32_t dmac_data_dst_0[16];
uint8_t dmac_data_dst_1[66];
#endif
/*******************************************************************************
 * Function Name: SelfTest_DMAC
 ********************************************************************************
 *
 * Summary: Any data shared between the bus masters should handle by using non cacheable memory.
 *          Non cacheable memory for source address, destination address, descriptor address
 *          are handled in the DMAC selftest.
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
    (void)memset(dmac_data_dst_0, 0xAA, sizeof(dmac_data_dst_0));
    (void)memset(dmac_data_dst_1, 0, sizeof(dmac_data_dst_1));

    /* Init Descriptors */
    cy_en_dmac_status_t dmac_status = Cy_DMAC_Descriptor_Init(descriptor0, des0_config);
    dmac_status = Cy_DMAC_Descriptor_Init(descriptor1, des1_config);

    /* Set source and dest address */
    /* Descriptor 0*/
    Cy_DMAC_Descriptor_SetSrcAddress(descriptor0, dmac_data_src_0);
    Cy_DMAC_Descriptor_SetDstAddress(descriptor0, dmac_data_dst_0);
    /* Descriptor 1 */
    Cy_DMAC_Descriptor_SetSrcAddress(descriptor1, dmac_data_src_1);
    Cy_DMAC_Descriptor_SetDstAddress(descriptor1, dmac_data_dst_1);
    Cy_DMAC_Enable(base);


    dmac_status = Cy_DMAC_Channel_Init(base, channel, channelConfig);
#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_CleanDCache_by_Addr(descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_CleanDCache_by_Addr(descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
#endif
    Cy_DMAC_Channel_Enable(base, channel);

    cy_en_trigmux_status_t trigmux_status = Cy_TrigMux_SwTrigger((uint32_t)trigLine, CY_TRIGGER_TWO_CYCLES);

    uint32_t interruptStatus = Cy_DMAC_Channel_GetInterruptStatus(base,channel);
    while(interruptStatus == 0UL)
    {
      interruptStatus = Cy_DMAC_Channel_GetInterruptStatus(base,channel);
    }
#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_InvalidateDCache_by_Addr (descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_InvalidateDCache_by_Addr (descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
#endif

    int32_t cmpRes;

#if (ERROR_IN_DMAC == 1u)
        dmac_data_dst_0[0] = 1;
#endif /* End (ERROR_IN_DMA_DW == 1u) */

    cmpRes = memcmp(dmac_data_src_0, dmac_data_dst_0, 64);
    if (cmpRes != 0)
    {
        ret = 1;
    }

    cmpRes = memcmp(dmac_data_src_1, dmac_data_dst_1, 64);
    if (cmpRes != 0)
    {
        ret = 1;
    }
    (void)trigmux_status;
    (void)dmac_status;

    return ret;
}

#endif
#endif
#endif /* ERRORINJECTION_H */
/* [] END OF FILE */

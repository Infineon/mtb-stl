/*******************************************************************************
* File Name: SelfTest_DMAC.c
*
* Description:
*  This file provides the source code for DMAC self test.
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
#include "SelfTest_DMAC.h"
#include "SelfTest_ErrorInjection.h"

#if (defined(CY_IP_M4CPUSS_DMAC) || defined(CY_IP_M7CPUSS_DMAC))
#if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
CY_SECTION_SHAREDMEM static uint32_t dmac_data_src_0[16] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
CY_SECTION_SHAREDMEM static uint8_t dmac_data_src_1[66] =
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

CY_SECTION_SHAREDMEM static uint32_t dmac_data_dst_0[16] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
CY_SECTION_SHAREDMEM static uint8_t dmac_data_dst_1[66] =
{
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0,
    0x0, 0x0, 0x0
};
#else /* if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE) */
static const uint32_t dmac_data_src_0[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t dmac_data_src_1[66] =
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

static uint32_t dmac_data_dst_0[16];
static uint8_t dmac_data_dst_1[66];
#endif /* if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE) */


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
*    address first): ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000�
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
                      en_trig_output_mdma_t trigLine)
{
    uint8_t ret = ERROR_STATUS;
    uint32_t interruptStatus;
    uint32_t guardCnt = 0UL;

    (void)memset(dmac_data_dst_0, 0xAA, sizeof(dmac_data_dst_0));
    (void)memset(dmac_data_dst_1, 0, sizeof(dmac_data_dst_1));

    /* Init Descriptors */
    (void)Cy_DMAC_Descriptor_Init(descriptor0, des0_config);
    (void)Cy_DMAC_Descriptor_Init(descriptor1, des1_config);

    /* Set source and dest address */
    /* Descriptor 0*/
    Cy_DMAC_Descriptor_SetSrcAddress(descriptor0, dmac_data_src_0);
    Cy_DMAC_Descriptor_SetDstAddress(descriptor0, dmac_data_dst_0);
    /* Descriptor 1 */
    Cy_DMAC_Descriptor_SetSrcAddress(descriptor1, dmac_data_src_1);
    Cy_DMAC_Descriptor_SetDstAddress(descriptor1, dmac_data_dst_1);
    Cy_DMAC_Enable(base);


    (void)Cy_DMAC_Channel_Init(base, channel, channelConfig);
    #if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_CleanDCache_by_Addr(descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_CleanDCache_by_Addr(descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    #endif
    Cy_DMAC_Channel_Enable(base, channel);

    (void)Cy_TrigMux_SwTrigger((uint32_t)trigLine, CY_TRIGGER_TWO_CYCLES);

    /* Wait for DMA transfer completion with timeout */
    do
    {
        Cy_SysLib_DelayUs(1u);
        guardCnt++;
        interruptStatus = Cy_DMAC_Channel_GetInterruptStatus(base, channel);
    } while ((interruptStatus == 0UL) && (SELFTEST_DMAC_TIMEOUT > guardCnt));

    #if (CY_CPU_CORTEX_M7) && defined (ENABLE_CM7_DATA_CACHE)
    SCB_InvalidateDCache_by_Addr(descriptor0, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    SCB_InvalidateDCache_by_Addr(descriptor1, (int32_t)sizeof(cy_stc_dmac_descriptor_t));
    #endif

    /* Check for timeout */
    if (SELFTEST_DMAC_TIMEOUT > guardCnt)
    {
        int32_t cmpRes;

        #if (ERROR_IN_DMAC == 1u)
        dmac_data_dst_0[0] = 1;
        #endif /* End (ERROR_IN_DMAC == 1u) */

        cmpRes = memcmp(dmac_data_src_0, dmac_data_dst_0, sizeof(dmac_data_dst_0));
        if (cmpRes == 0)
        {
            cmpRes = memcmp(dmac_data_src_1, dmac_data_dst_1, sizeof(dmac_data_dst_1));
            if (cmpRes == 0)
            {
                ret = OK_STATUS;
            }
        }
    }

    return ret;
}


#endif /* if (defined(CY_IP_M4CPUSS_DMAC) || defined(CY_IP_M7CPUSS_DMAC)) */


#if (defined(CY_IP_M0S8CPUSSV3_DMAC))

static const uint32_t dmac_data_src_0[16U] =
    { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
static const uint8_t dmac_data_src_1[64U]  =
{
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U, 0x00U, 0xFFU,
    0x00U
};

CY_ALIGN(4) static uint8_t dmac_data_dst[64U];

static const cy_stc_dmac_channel_config_t dmac_channel_config =
{
    .priority   = 3UL,
    .enable     = false,
    .descriptor = CY_DMAC_DESCRIPTOR_PING,
};

static const cy_stc_dmac_descriptor_config_t dmac_ping_config =
{
    .srcAddress       = 0UL,
    .dstAddress       = 0UL,
    .dataCount        = 16UL,
    .dataSize         = CY_DMAC_WORD,
    .srcTransferSize  = CY_DMAC_TRANSFER_SIZE_WORD,
    .srcAddrIncrement = true,
    .dstTransferSize  = CY_DMAC_TRANSFER_SIZE_WORD,
    .dstAddrIncrement = true,
    .retrigger        = CY_DMAC_RETRIG_IM,
    .cpltState        = false,
    .interrupt        = true,
    .preemptable      = true,
    .flipping         = true,
    .triggerType      = CY_DMAC_SINGLE_DESCR,
};

static const cy_stc_dmac_descriptor_config_t dmac_pong_config =
{
    .srcAddress       = 0UL,
    .dstAddress       = 0UL,
    .dataCount        = 64UL,
    .dataSize         = CY_DMAC_BYTE,
    .srcTransferSize  = CY_DMAC_TRANSFER_SIZE_DATA,
    .srcAddrIncrement = true,
    .dstTransferSize  = CY_DMAC_TRANSFER_SIZE_DATA,
    .dstAddrIncrement = true,
    .retrigger        = CY_DMAC_RETRIG_IM,
    .cpltState        = false,
    .interrupt        = true,
    .preemptable      = true,
    .flipping         = true,
    .triggerType      = CY_DMAC_SINGLE_DESCR,
};


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
*    address first): ff0000ff0000ff0000ff0000ff0000ff0000ff0000ff0000�
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
uint8_t SelfTest_DMAC(DMAC_Type* base, uint32_t channel, uint32_t trigLine)
{
    uint8_t ret = ERROR_STATUS;
    uint32_t interruptStatus;
    uint32_t dmacIntrMask = 1UL << channel;
    uint32_t guardCnt = 0UL;

    (void)memset(dmac_data_dst, 0xAA, sizeof(dmac_data_dst));

    /* Initialize Descriptors */
    (void)Cy_DMAC_Descriptor_Init(base, channel, CY_DMAC_DESCRIPTOR_PING, &dmac_ping_config);
    (void)Cy_DMAC_Descriptor_Init(base, channel, CY_DMAC_DESCRIPTOR_PONG, &dmac_pong_config);

    /* Set source and destination address */
    /* Descriptor 0 */
    Cy_DMAC_Descriptor_SetSrcAddress(base, channel, CY_DMAC_DESCRIPTOR_PING, dmac_data_src_0);
    Cy_DMAC_Descriptor_SetDstAddress(base, channel, CY_DMAC_DESCRIPTOR_PING, dmac_data_dst);

    /* Descriptor 1 */
    Cy_DMAC_Descriptor_SetSrcAddress(base, channel, CY_DMAC_DESCRIPTOR_PONG, dmac_data_src_1);
    Cy_DMAC_Descriptor_SetDstAddress(base, channel, CY_DMAC_DESCRIPTOR_PONG, dmac_data_dst);

    (void)Cy_DMAC_Channel_Init(base, channel, &dmac_channel_config);

    Cy_DMAC_Channel_Enable(base, channel);
    Cy_DMAC_Enable(base);
    Cy_DMAC_ClearInterrupt(base, dmacIntrMask);

    /* Start first array transfer and check */
    (void)Cy_TrigMux_SwTrigger(trigLine, 2UL);

    do
    {
        Cy_SysLib_DelayUs(1u);
        guardCnt++;
        interruptStatus = Cy_DMAC_GetInterruptStatus(base);
    } while(((interruptStatus & dmacIntrMask) == 0UL) && (SELFTEST_DMAC_TIMEOUT > guardCnt));
    Cy_DMAC_ClearInterrupt(base, dmacIntrMask);

    #if (ERROR_IN_DMAC == 1U)
    dmac_data_dst[0]= 1U;
    #endif

    if ((0 == memcmp((const uint8_t*)dmac_data_src_0, dmac_data_dst,
                     sizeof(dmac_data_dst))) && (SELFTEST_DMAC_TIMEOUT > guardCnt))
    {
        /* First array check passed, start second array transfer and check */
        (void)Cy_TrigMux_SwTrigger(trigLine, 2UL);
        guardCnt = 0UL;

        do
        {
            Cy_SysLib_DelayUs(1u);
            guardCnt++;
            interruptStatus = Cy_DMAC_GetInterruptStatus(base);
        } while(((interruptStatus & dmacIntrMask) == 0UL) && (SELFTEST_DMAC_TIMEOUT > guardCnt));
        Cy_DMAC_ClearInterrupt(base, dmacIntrMask);

        if ((0 == memcmp(dmac_data_src_1, dmac_data_dst,
                         sizeof(dmac_data_dst))) && (SELFTEST_DMAC_TIMEOUT > guardCnt))
        {
            ret = OK_STATUS;
        }
    }
    return ret;
}


#endif /* if (defined(CY_IP_M0S8CPUSSV3_DMAC)) */

/* [] END OF FILE */

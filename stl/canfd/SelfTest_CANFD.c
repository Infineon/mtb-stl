/*******************************************************************************
* File Name: SelfTest_CANFD.c
* Version 1.0.0
*
* Description: This file provides the source code for the APIs to perform
* clock source testing according to the Class B library.
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
#include "cy_canfd.h"
#include "SelfTest_CANFD.h"
#include "SelfTest_ErrorInjection.h"

#if ((defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) ||  (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33))  || ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A256K)) || ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A512K)))
/*******************************************************************************
* Global Variables
*******************************************************************************/
static uint8_t can_data_received_counter = 0;
static cy_stc_canfd_context_t *SelfTest_context; /* This is a shared context structure,
                                 * unique for each channel
                                 */
static CANFD_Type *SelfTest_base;
static const cy_stc_canfd_config_t *SelfTest_config;
static uint32_t SelfTest_chan;


static cy_stc_canfd_t0_t SelfTest_CANFD_T0RegisterBuffer_0 = 
{
    .id = 0x60U,
    .rtr = CY_CANFD_RTR_DATA_FRAME,
    .xtd = CY_CANFD_XTD_STANDARD_ID,
    .esi = CY_CANFD_ESI_ERROR_ACTIVE,
};
static cy_stc_canfd_t1_t SelfTest_CANFD_T1RegisterBuffer_0 = 
{
    .dlc = (uint32_t)15U,
    .brs = (bool)true,
    .fdf = CY_CANFD_FDF_CAN_FD_FRAME,
    .efc = (bool)false,
    .mm =  (uint32_t)0U,
};
static uint32_t SelfTest_CANFD_dataBuffer_0[] = 
{
    0xAAAAAAAAU,
    0x55555555U,
    0x33333333U,
    0x11111111U,
    0x22222222U,
    0x66666666U,
    0x77777777U,
    0x88888888U,
    0x99999999U,
    0xAAAAAAAAU,
    0xBBBBBBBBU,
    0xCCCCCCCCU,
    0xDDDDDDDDU,
    0xEEEEEEEEU,
    0xFFFFFFFFU,
    0x12121212U,
};
static cy_stc_canfd_tx_buffer_t SelfTest_CANFD_txBuffer_0 = 
{
    .t0_f = &SelfTest_CANFD_T0RegisterBuffer_0,
    .t1_f = &SelfTest_CANFD_T1RegisterBuffer_0,
    .data_area_f = SelfTest_CANFD_dataBuffer_0,
};

/*******************************************************************************
* Function Name: SelfTest_CAN_RxMsgCallback
********************************************************************************
*
* CANFD reception callback.
*
*******************************************************************************/
void SelfTest_CAN_RxMsgCallback(bool bRxFifoMsg, uint8_t u8MsgBufOrRxFifoNum,
                           cy_stc_canfd_rx_buffer_t* pstcCanFDmsg)
{
    (void)u8MsgBufOrRxFifoNum;
    bool data_error = false;
	if((true == bRxFifoMsg) && ((pstcCanFDmsg->r0_f->id >= 0x50U) && (pstcCanFDmsg->r0_f->id <= 0x55U))) /* Should receive data in RX-FIFO */
	{
        #if ERROR_IN_CANFD
            SelfTest_CANFD_dataBuffer_0[1] = 0x0;
        #endif
        for(int i = 0; i<15; i++)
        {
                    if(SelfTest_CANFD_dataBuffer_0[i] != pstcCanFDmsg->data_area_f[i])
                    {
                        data_error = true;
                        break;
                    }
        }
        if(false == data_error)
        {
            can_data_received_counter++;
        }
    }
}
static cy_canfd_rx_msg_func_ptr_t rx_call_fxn = &SelfTest_CAN_RxMsgCallback;


/*******************************************************************************
* Function Name: TransmitMessage
********************************************************************************
* Transmits the data according to the given txBuffer configuration structure.
*
* @param txBuffer
* The Tx Buffer configuration structure.
*
* @param index
* The message buffer index (0-31).
*
* @return
*
*******************************************************************************/
uint32_t TransmitMessage(cy_stc_canfd_tx_buffer_t *txBuffer, uint8_t index)
{
	return (uint32_t)(Cy_CANFD_UpdateAndTransmitMsgBuffer(SelfTest_base, SelfTest_chan, txBuffer, index, SelfTest_context));
}


/*******************************************************************************
* Function Name: SelfTest_CANFD
********************************************************************************
*
* This function configure the CANFD H/W in an internal loopback anf then
* CAN-FD is configure to accept message IDs in the range 0x50 to 0x55
* and a message with message ID 0x60 is transmitted and RX fifo is 
* verified to be empty. Similarly, a message with message ID 0x50 is 
* transmitted and the message received in RX FIFO is verified. 
*
*
* @param base 
* The pointer to a CAN FD instance. <br>
* @param chan 
* The CAN FD channel number. <br> 
* @param config 
* The pointer to the CAN FD configuration structure.
* @param context 
* The pointer to the context structure allocated
* by the user. The structure is used during the CAN FD operation for internal
* configuration and data retention. User must not modify anything in this
* structure.
*
* @return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_CANFD(CANFD_Type *base, uint32_t chan,
                                   const cy_stc_canfd_config_t *config,
                                   cy_stc_canfd_context_t *context,
                                   cy_stc_canfd_test_mode_t test_mode)
{
    uint32_t status;
    cy_en_canfd_status_t canfd_status = CY_CANFD_ERROR_TIMEOUT;
    bool global_config_changed = false;
    SelfTest_base = base;
    SelfTest_chan = chan;
    SelfTest_config = config;
    SelfTest_context = context;
  
    /* Enables the configuration changes to set Test mode */
    canfd_status = Cy_CANFD_ConfigChangesEnable(base, chan);

    /* Change the global setting to not receive all frames in RXFIFO_0 */
    if(CY_CANFD_REJECT_NON_MATCHING != SelfTest_config->globalFilterConfig->nonMatchingFramesStandard)
    {
        /* The configuration of a global filter */
        CANFD_GFC(base, chan) =
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_ANFS, CY_CANFD_REJECT_NON_MATCHING) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_ANFE, CY_CANFD_REJECT_NON_MATCHING) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_RRFS, 1UL) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_RRFE, 1UL);
        global_config_changed = true;
    }

    /* Sets the Test mode configuration */
    Cy_CANFD_TestModeConfig(base, chan, test_mode);

    /* Change the rx function callback*/
    SelfTest_context->canFDInterruptHandling.canFDRxInterruptFunction = rx_call_fxn;

    /* Disables the configuration changes */
    canfd_status = Cy_CANFD_ConfigChangesDisable(base, chan);

	can_data_received_counter = 0;

    SelfTest_CANFD_T0RegisterBuffer_0.id = 0x60;
	status = TransmitMessage(&SelfTest_CANFD_txBuffer_0, 0);
    if((uint32_t)CY_CANFD_SUCCESS != status)
    {
        return ERROR_STATUS;
    }
	Cy_SysLib_Delay(1000u);

	SelfTest_CANFD_T0RegisterBuffer_0.id = 0x52;
	status = TransmitMessage(&SelfTest_CANFD_txBuffer_0, 0);
    if((uint32_t)CY_CANFD_SUCCESS != status)
    {
        return ERROR_STATUS;
    }
	Cy_SysLib_Delay(1000u);

    canfd_status = Cy_CANFD_ConfigChangesEnable(base, chan);
    
    /*Restore the original callback fxn*/
    SelfTest_context->canFDInterruptHandling.canFDRxInterruptFunction = SelfTest_config->rxCallback;
    
    /* Restore the global filter setting*/
    if(global_config_changed)
    {
        /* The configuration of a global filter */
        CANFD_GFC(base, chan) =
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_ANFS, SelfTest_config->globalFilterConfig->nonMatchingFramesStandard) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_ANFE, SelfTest_config->globalFilterConfig->nonMatchingFramesExtended) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_RRFS, ((SelfTest_config->globalFilterConfig->rejectRemoteFramesStandard) ? 1UL : 0UL)) |
            _VAL2FLD(CANFD_CH_M_TTCAN_GFC_RRFE, ((SelfTest_config->globalFilterConfig->rejectRemoteFramesExtended) ? 1UL : 0UL));
    }

    /* Sets the Test mode configuration */
    Cy_CANFD_TestModeConfig(base, chan, CY_CANFD_TEST_MODE_DISABLE);
    
    /* Disables the configuration changes */
    canfd_status = Cy_CANFD_ConfigChangesDisable(base, chan);
    (void)(canfd_status);
    
	if ((uint8_t)1U == can_data_received_counter)
	{
		return OK_STATUS;
	}
	else
	{
		return ERROR_STATUS;
	}
}

#endif
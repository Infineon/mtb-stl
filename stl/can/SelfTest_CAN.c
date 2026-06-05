/*******************************************************************************
* File Name: SelfTest_CAN.c
*
* Description:
*  This file provides the source code for the APIs to perform
*  CAN testing according to the Class B library.
*
*******************************************************************************
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
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

#include "SelfTest_CAN.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_M0S8CAN)

#define SELFTEST_CAN_ID_LOW             (0x50U)
#define SELFTEST_CAN_ID_HIGH            (0x55U)
#define SELFTEST_CAN_ID_IN_RANGE        (0x52U)
#define SELFTEST_CAN_ID_OUT_OF_RANGE    (0x60U)
#define SELFTEST_CAN_DATA_LEN_U8        (8U)
#define SELFTEST_CAN_DATA_LEN_U32       (2U)
#define SELFTEST_CAN_RX_TIMEOUT_MS      (1000U)
#define SELFTEST_CAN_EXPECTED_PASS_CNT  (2U)

/*******************************************************************************
* Global Variables
*******************************************************************************/
static uint8_t canDataReceivedCounter = 0U;
static bool canRxIsrFlag = false;


static cy_stc_can_message_frame_t canFrameData =
{
    .id     = SELFTEST_CAN_ID_IN_RANGE,
    .data   =
    {
        0xAAAAAAAAUL,
        0x55555555UL
    },
    .length = SELFTEST_CAN_DATA_LEN_U8,
    .rtr    = false,
    .ide    = false
};

static const cy_stc_can_rx_buffer_config_t rxbConfig =
{
    .acceptanceMask   =
    {
        .id           = 0xFFU,
        .idType       = CY_CAN_IDE_STANDARD_ID,
        .data         = 0xFFFFU,
        .rtr          = 0u,
        .ide          = 0u
    },
    .acceptanceCode   =
    {
        .id           = 0u,
        .idType       = CY_CAN_IDE_STANDARD_ID,
        .data         = 0xFFFFU,
        .rtr          = 0u,
        .ide          = 0u
    },
    .linked           = false,
    .interruptEnabled = true,
    .autoReplyRtr     = false,
    .enable           = true
};


/*******************************************************************************
* Function Name: SelfTest_CAN_RxMsgCallback
********************************************************************************
*
* Description:
*  This function is the callback for CAN message reception. It verifies
*  whether the received message ID and data match the expected values.
*
*******************************************************************************/
static void SelfTest_CAN_RxMsgCallback(uint8_t index, cy_stc_can_message_frame_t* rxMsg)
{
    (void)index;
    bool dataError = false;

    /* Check if the received message ID is in the range */
    if ((SELFTEST_CAN_ID_LOW <= rxMsg->id) && (SELFTEST_CAN_ID_HIGH >= rxMsg->id))
    {
        canRxIsrFlag = true;

        for (uint32_t i = 0UL; i < SELFTEST_CAN_DATA_LEN_U32; i++)
        {
            if (canFrameData.data[i] != rxMsg->data[i])
            {
                /* Data mismatch */
                dataError = true;
                break;
            }
        }

        #if ERROR_IN_CAN
        dataError = true;
        #endif

        if (!dataError)
        {
            canDataReceivedCounter++;
        }
    }

    /* Set ISR flag for the received message with ID out of the range. */
    if (SELFTEST_CAN_ID_OUT_OF_RANGE == rxMsg->id)
    {
        canRxIsrFlag = true;
    }
}


/*******************************************************************************
* Function Name: SelfTest_CAN_SetDisabledRxBufferConfig
********************************************************************************
*
* This function sets the default configuration for the specified range of
* Rx buffers.
*
* @param base
* The pointer to the CAN instance.
* @param startIndex
* The starting index of the RX message buffers to disable.
*
*******************************************************************************/
static void SelfTest_CAN_SetDisabledRxBufferConfig(CAN_Type* base, uint8_t startIndex)
{
    uint8_t index = startIndex;

    cy_stc_can_rx_buffer_config_t disabledRxBufferConfig =
    {
        .enable = false
    };

    for (; index < CY_CAN_MESSAGE_RX_BUFFERS_MAX_CNT; index++)
    {
        (void)Cy_CAN_UpdateRxBufferConfig(base, index, &disabledRxBufferConfig, NULL);
    }
}


/*******************************************************************************
* Function Name: SelfTest_CAN
********************************************************************************
*
*  This function performs a self-test of the CAN hardware. It recconfigures the
*  CAN module in the selected loopback mode and transmits test messages.
*  The function verifies if the transmitted messages are received correctly and
*  restores the original configuration.
*
*
* @param base
* The pointer to a CAN instance. <br>
* @param config
* The pointer to the CAN configuration structure.
* @param context
* The pointer to the CAN context structure allocated by the user.
* @param testMode
* The test mode to be used for the self-test.
*
* @return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_CAN(CAN_Type* base, const cy_stc_can_config_t* config,
                     cy_stc_can_context_t* context,
                     cy_en_can_test_mode_t testMode)
{
    cy_can_rx_msg_func_ptr_t rxCallFxnBkp = NULL;
    bool canCfgChanged = false;

    cy_en_can_status_t canStatus;
    uint8_t result = ERROR_STATUS;
    uint8_t passCnt = 0U;

    uint8_t index;
    uint32_t timeoutCntr = 0U;


    canStatus = Cy_CAN_Stop(base);

    /* Save the current CAN settings and reconfigure them to perform selftest. */
    if (CY_CAN_SUCCESS == canStatus)
    {
        Cy_CAN_SetTestMode(base, testMode);

        rxCallFxnBkp = context->canRxInterruptFunction;
        context->canRxInterruptFunction = (cy_can_rx_msg_func_ptr_t)SelfTest_CAN_RxMsgCallback;

        /* Update filter settings */
        (void)Cy_CAN_UpdateRxBufferConfig(base, 0U, &rxbConfig, NULL);
        SelfTest_CAN_SetDisabledRxBufferConfig(base, 1U);

        canCfgChanged = true;

        canStatus = Cy_CAN_Start(base);
    }

    /* Send the first data packet with ID outsige the range. */
    if (CY_CAN_SUCCESS == canStatus)
    {
        canDataReceivedCounter = 0U;
        canFrameData.id = SELFTEST_CAN_ID_OUT_OF_RANGE;
        canRxIsrFlag = false;
        canStatus = Cy_CAN_Transmit(base, 0u, &canFrameData, true, false, context);
    }

    /* Wait for and check the first data-packet reception. */
    if (CY_CAN_SUCCESS == canStatus)
    {
        while ((false == canRxIsrFlag) && (timeoutCntr < SELFTEST_CAN_RX_TIMEOUT_MS))
        {
            Cy_SysLib_Delay(1u);
            timeoutCntr++;
        }

        if (timeoutCntr >= SELFTEST_CAN_RX_TIMEOUT_MS)
        {
            canStatus = CY_CAN_ERROR_TIMEOUT;
        }

        if (0U == canDataReceivedCounter)
        {
            passCnt++;
        }
    }

    /* Send the second data packet with ID in the range. */
    if (CY_CAN_SUCCESS == canStatus)
    {
        canFrameData.id = SELFTEST_CAN_ID_IN_RANGE;
        canRxIsrFlag = false;
        canStatus = Cy_CAN_Transmit(base, 0u, &canFrameData, true, false, context);
    }

    /* Wait for and check the second data packet reception. */
    if (CY_CAN_SUCCESS == canStatus)
    {
        while ((false == canRxIsrFlag) && (timeoutCntr < SELFTEST_CAN_RX_TIMEOUT_MS))
        {
            Cy_SysLib_Delay(1u);
            timeoutCntr++;
        }

        if (timeoutCntr >= SELFTEST_CAN_RX_TIMEOUT_MS)
        {
            canStatus = CY_CAN_ERROR_TIMEOUT;
        }

        if (1U == canDataReceivedCounter)
        {
            passCnt++;
        }
    }

    /* Restore the original configuration */
    if (canCfgChanged)
    {
        if (CY_CAN_SUCCESS == Cy_CAN_Stop(base))
        {
            context->canRxInterruptFunction = rxCallFxnBkp;

            for (index = 0u; index < config->numOfRxBuffers; index++)
            {
                (void)Cy_CAN_UpdateRxBufferConfig(base, 0U, &(config->rxBuffer[index]), NULL);
            }

            SelfTest_CAN_SetDisabledRxBufferConfig(base, index);

            canStatus = Cy_CAN_Start(base);
        }
    }

    /* Check if the test passed */
    if ((CY_CAN_SUCCESS == canStatus) && (SELFTEST_CAN_EXPECTED_PASS_CNT == passCnt))
    {
        result = OK_STATUS;
    }

    return result;
}


#endif // if defined(CY_IP_M0S8CAN)

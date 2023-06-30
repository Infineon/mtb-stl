/*******************************************************************************
* File Name: SelfTest_UART_SCB.c
* Version 2.0
*
* Description:
*  This file provides the source code to do the UART self tests
*  according to Class B library for PSoC4
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*
*******************************************************************************
* Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "SelfTest_UART_SCB.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

static uint8_t SelfTest_UART_SCB_Byte(CySCB_Type* base, uint8_t transmitByte);

/*****************************************************************************
* Function Name: SelfTest_UART_SCB_Byte
******************************************************************************
*
* Summary:
*  Test internal loopback of UART (transmitted byte must be equal to received)
*
* Parameters:
*  CySCB_Type *base - pointer to SCB hardware to configure
*  uint8_transmitByte - data to be transmitted over UART
*
* Return:
*  0 - test success
*  1 - test failed
*
* Note:
*  During call, function transmits and receives only one byte
*  in future calls the function transmits next byte value
*  to limit function execution time
*
*****************************************************************************/
static uint8_t SelfTest_UART_SCB_Byte(CySCB_Type* base, uint8_t transmitByte)
{
    uint16_t guardCnt = 0u;
    uint8_t ret = OK_STATUS;

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_UART_SCB == 1u)
    if (transmitByte == UART_SCB_TRANSMIT_BYTE_ERROR)
    {
        /* Change value for UART transferring to unexpected  */
        transmitByte++;
    }
    #endif /* End (ERROR_IN_UART_SCB == 1u) */

    /* Transmit byte */
    (void)Cy_SCB_UART_Put(base, transmitByte);

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_UART_SCB == 1U)
    if (transmitByte == (UART_SCB_TRANSMIT_BYTE_ERROR + 1u))
    {
        /* Restore expected value */
        transmitByte--;
    }
    #endif /* End (ERROR_IN_UART_SCB == 1u) */

    /* Wait while byte will be sent */
    /* Use guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while ((0u != (Cy_SCB_UART_GetNumInTxFifo(base) + Cy_SCB_GetTxSrValid(base))) &&
             (guardCnt < UART_TX_DATA_TIME));

    /* If timeout */
    if (guardCnt >= UART_TX_DATA_TIME)
    {
        ret = ERROR_STATUS;
    }
    else
    {
        /* Reset GuardCnt */
        guardCnt = 0u;

        do
        {
            guardCnt++;
            Cy_SysLib_DelayUs(10u);
        } while ((0u == (Cy_SCB_UART_GetNumInRxFifo(base) + Cy_SCB_GetRxSrValid(base))) &&
                 (guardCnt < UART_RX_DATA_TIME));

        /* If timeout, error status is returned */
        if (guardCnt >= UART_RX_DATA_TIME)
        {
            ret = ERROR_STATUS;
        }
        else
        {
            /* Check if received and transmitted values are the same */
            if (transmitByte != ((uint8_t)Cy_SCB_UART_Get(base)))
            {
                /* If not - return ERROR status */
                ret = ERROR_STATUS;
            }
        }
    }

    /* Return result */
    return ret;
}


/*****************************************************************************
* Function Name: SelfTest_UART_SCB
******************************************************************************
*
* Summary:
*  Test loopback of UART (transmitted byte must be equal to received)
*
* Parameters:
*  CySCB_Type *base - pointer to SCB hardware to configure
*
* Return:
*  1 - test failed
*  2 - Still testing
*  3 - Test completed OK
*  4 - Error, TX not empty
*  5 - Error, RX not empty
*  6 - Error, TX not enabled
*  7 - Error, RX not enabled
*
* Note:
*  During call, function transmits and receives bytes from 0x01 to 0xFF.
*  User is responsible for routing the loop back before the test.
*
*****************************************************************************/
uint8_t SelfTest_UART_SCB(CySCB_Type* base)
{
    /* Byte for transmitting/receiving */
    uint8_t ret;
    uint16_t guardCnt = 0u;
    static uint8_t byteToTest = 5u;
    uint32_t rxUartInterruptMask = 0u;
    uint32_t txUartInterruptMask = 0u;

    /* Check if UART enabled */
    if (((SCB_CTRL(base) & SCB_CTRL_ENABLED_Msk) == 0u) ||
        ((SCB_CTRL(base) & SCB_CTRL_MODE_Msk) !=
         (((uint32_t)CY_SCB_CTRL_MODE_UART) << SCB_CTRL_MODE_Pos)))
    {
        /* Return error if UART is not enabled */
        ret = ERROR_UART_NOT_ENABLE;
    }
    else
    {
        /* Wait for end of user receiving */
        /* Don't test if the buffer contains a data */
        while ((0u != (Cy_SCB_UART_GetNumInRxFifo(base) + Cy_SCB_GetRxSrValid(base))) &&
               (guardCnt < UART_RX_DATA_TIME))
        {
            guardCnt++;
            Cy_SysLib_DelayUs(1u);
        }

        /* If timeout */
        if (guardCnt >= UART_RX_DATA_TIME)
        {
            ret = ERROR_RX_NOT_EMPTY;
        }
        else
        {
            /* Wait for end of user transmitting and don't test if the buffer contains data */
            /* Use guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
            do
            {
                guardCnt++;
                Cy_SysLib_DelayUs(1u);
            } while ((0u != (Cy_SCB_UART_GetNumInTxFifo(base) + Cy_SCB_GetTxSrValid(base))) &&
                     (guardCnt < UART_TX_DATA_TIME));

            /* If timeout */
            if (guardCnt >= UART_TX_DATA_TIME)
            {
                ret = ERROR_TX_NOT_EMPTY;
            }
            else
            {
                /* Clear RX, TX buffers */
                Cy_SCB_UART_ClearRxFifo(base);
                Cy_SCB_UART_ClearTxFifo(base);

                /* Disable RX and TX interrupts before the test */
                rxUartInterruptMask = Cy_SCB_GetRxInterruptMask(base);
                Cy_SCB_SetRxInterruptMask(base, 0x0u);

                txUartInterruptMask = Cy_SCB_GetTxInterruptMask(base);
                Cy_SCB_SetTxInterruptMask(base, 0x0u);

                /* Transmit bytes from 0x01 to 0xFF*/
                ret = SelfTest_UART_SCB_Byte(base, byteToTest);

                /* Enabled RX and TX interrupts after the test */
                Cy_SCB_SetRxInterruptMask(base, rxUartInterruptMask);
                Cy_SCB_SetTxInterruptMask(base, txUartInterruptMask);

                /* Check result of test */
                if (ret == OK_STATUS)
                {
                    /* If test was performed with all values from 0x00 to 0xFF */
                    if (byteToTest == UART_TEST_RANGE)
                    {
                        /* Return status, that test fully completed */
                        ret = PASS_COMPLETE_STATUS;
                    }
                    /* If not */
                    else
                    {
                        /* Return status, that error was not detected but test is not fully
                           completed */
                        ret = PASS_STILL_TESTING_STATUS;
                    }
                }

                /* Increment index */
                byteToTest++;

                if (byteToTest >= UART_TEST_RANGE)
                {
                    byteToTest = 0u;
                }
            }
        }

        /* Need to clear buffer after MUX switch */
        Cy_SysLib_DelayUs(100u);
        Cy_SCB_UART_ClearRxFifo(base);
        Cy_SCB_UART_ClearTxFifo(base);
    }

    /* If something in the performance of the code went wrong Return ERROR status */
    return ret;
}


/* [] END OF FILE */

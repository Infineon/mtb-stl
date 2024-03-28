/*******************************************************************************
* File Name: SelfTest_SPI_SCB.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to perform the SPI self tests
*  according to Class B library for CAT2(PSoC4), CAT1A, CAT1C devices.
*
* Related Document:
*  AN36847: PSoC 4 IEC 60730 Class B and IEC 61508 SIL Safety Software Library
*  for ModusToolbox
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
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
#include "SelfTest_SPI_SCB.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

static uint8_t SelfTest_SPI_SCB_Byte(CySCB_Type* base, uint8_t transmitByte);

/*****************************************************************************
* Function Name: SelfTest_SPI_SCB_Byte
******************************************************************************
*
* Summary:
*  Test internal loopback of SPI (transmitted byte must be equal to received)
*
* Parameters:
*  CySCB_Type *base - pointer to SCB hardware to configure
*  uint8 transmitByte - data to be transmitted over SPI
*
* Return:
*  0 - test success
*  1 - test failed
*
* Note:
*  During call function transmits and receives only one byte.
*  In next call function transmits incremented byte value in order to
*  decrease function execution time
*
*****************************************************************************/
static uint8_t SelfTest_SPI_SCB_Byte(CySCB_Type* base, uint8_t transmitByte)
{
    uint8_t guardCnt = 0u;
    uint8_t ret = OK_STATUS;

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_SPI_SCB == 1u)
    if (transmitByte == SPI_SCB_TRANSMIT_BYTE_ERROR)
    {
        /* Change value for SPI transferring to unexpected */
        transmitByte++;
    }
    #endif /* End (ERROR_IN_SPI_SCB) */

    /* Transmit byte */
    (void)Cy_SCB_SPI_Write(base, transmitByte);

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_SPI_SCB == 1u)
    if (transmitByte == (SPI_SCB_TRANSMIT_BYTE_ERROR + 1u))
    {
        /* Restore expected value */
        transmitByte--;
    }
    #endif /* End (ERROR_IN_SPI_SCB) */

    /* Wait while byte is sent */
    /* Use guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
    do
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    } while ((0u == (Cy_SCB_SPI_GetNumInRxFifo(base))) && (guardCnt < SPI_TXRX_DATA_TIME));

    /* If timeout */
    if (guardCnt == SPI_TXRX_DATA_TIME)
    {
        ret = ERROR_STATUS;
    }
    else
    {
        /* Check if received and transmitted values are the same */
        if (Cy_SCB_SPI_Read(base) != transmitByte)
        {
            /* If not - return ERROR status */
            ret = ERROR_STATUS;
        }
    }

    /* Return result */
    return ret;
}


/*****************************************************************************
* Function Name: SelfTests_SPI_SCB
******************************************************************************
*
* Summary:
*  Test internal loopback of SPI (transmitted byte must be equal to received)
*
* Parameters:
*  CySCB_Type *base - pointer to SCB hardware to configure
*
* Return:
*  1 - test failed
*  2 - still testing
*  3 - Test completed
*  4 - TX Not empty
*  5 - RX Not empty
*
* Note:
*  During call the function transmits and receives bytes from 0x01 to 0xFF
*
*****************************************************************************/
uint8_t SelfTest_SPI_SCB(CySCB_Type* base)
{
    /* Byte for transmitting/receiving*/
    uint8_t ret;
    uint16_t guardCnt = 0u;
    static uint8_t byteToTest = 0x00u;
    uint32_t rxSpiInterruptMask = 0u;
    uint32_t txSpiInterruptMask = 0u;

    /* Wait for end of user transmitting and don't test if the buffer contains a data */
    /* Use guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
    while ((0u != (Cy_SCB_SPI_GetNumInTxFifo(base) + Cy_SCB_GetTxSrValid(base))) &&
           (guardCnt < SPI_TXRX_DATA_TIME))
    {
        guardCnt++;
        Cy_SysLib_DelayUs(1u);
    }

    /* If timeout */
    if (guardCnt >= SPI_TXRX_DATA_TIME)
    {
        ret = ERROR_TX_NOT_EMPTY;
    }
    else
    {
        guardCnt = 0u;

        /* Wait until SPI Master deactivates slave select to ensure that the last
         * data element has been completely transferred.
         */
        while ((false != Cy_SCB_SPI_IsBusBusy(base)) && (guardCnt < SPI_TXRX_DATA_TIME))
        {
            guardCnt++;
            Cy_SysLib_DelayUs(1u);
        }

        /* If timeout */
        if (guardCnt >= SPI_TXRX_DATA_TIME)
        {
            ret = ERROR_RX_NOT_EMPTY;
        }
        else
        {
            /* Clear RX, TX buffers */
            Cy_SCB_SPI_ClearRxFifo(base);
            Cy_SCB_SPI_ClearTxFifo(base);

            /* Disable RX and TX interrupts before the test */
            rxSpiInterruptMask = Cy_SCB_GetRxInterruptMask(base);
            Cy_SCB_SetRxInterruptMask(base, 0x0u);

            txSpiInterruptMask = Cy_SCB_GetTxInterruptMask(base);
            Cy_SCB_SetTxInterruptMask(base, 0x0u);

            /* Transmit bytes from 0x01 to 0xFF
             *  Send and receive byte and compare if they are the same
             */
            ret = SelfTest_SPI_SCB_Byte(base, byteToTest);
            byteToTest++;

            /* Enabled RX and TX interrupts after the test */
            Cy_SCB_SetRxInterruptMask(base, rxSpiInterruptMask);
            Cy_SCB_SetTxInterruptMask(base, txSpiInterruptMask);


            /* Check result of test */
            if (ret == OK_STATUS)
            {
                /* If "bResult" was not set - currently test passed */
                /* Check if test was performed with all values from 0x00 to 0xFF */

                /* If test was performed with all values from 0x00 to 0xFF */
                if (byteToTest == 0x00u)
                {
                    /* Return status, that test fully completed */
                    ret = PASS_COMPLETE_STATUS;
                }
                else
                {
                    /* Return status, that error was not detected but test is not fully completed */
                    ret = PASS_STILL_TESTING_STATUS;
                }
            }
        }
    }

    /* If something in the performance of the code went wrong */
    /* Return ERROR status */
    return ret;
}


/* [] END OF FILE */

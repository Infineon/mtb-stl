/*******************************************************************************
* File Name: SelfTest_SPI_SCB.c
*
* Description:
*  This file provides the source code to perform the SPI self tests
*  according to Class B library.
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

#include "cy_pdl.h"
#include "SelfTest_SPI_SCB.h"
#include "SelfTest_ErrorInjection.h"

static uint8_t SelfTest_SPI_SCB_Byte(CySCB_Type* base, uint8_t transmitByte);

/*****************************************************************************
* Function Name: SelfTest_SPI_SCB_Byte
******************************************************************************
*
* Summary:
*  This function tests an internal loopback of SPI (transmitted byte must be equal to received).
*
* Parameters:
*  CySCB_Type *base - The pointer to SCB hardware to configure.
*  uint8 transmitByte - Data to transmit over SPI.
*
* Return:
*  0 - test success
*  1 - test failed
*
* Note:
*  During a call, the function transmits and receives only one byte.
*  In the next call, the function transmits the incremented byte value in order to
*  decrease the function execution time.
*
*****************************************************************************/
static uint8_t SelfTest_SPI_SCB_Byte(CySCB_Type* base, uint8_t transmitByte)
{
    uint8_t guardCnt = 0u;
    uint8_t ret = OK_STATUS;

    /* Check if an intentional error should be made for testing */
    #if (ERROR_IN_SPI_SCB == 1u)
    if (transmitByte == SPI_SCB_TRANSMIT_BYTE_ERROR)
    {
        /* Change the value for SPI transferring to unexpected */
        transmitByte++;
    }
    #endif /* End (ERROR_IN_SPI_SCB) */

    /* Transmit byte */
    (void)Cy_SCB_SPI_Write(base, transmitByte);

    /* Check if an intentional error should be made for testing */
    #if (ERROR_IN_SPI_SCB == 1u)
    if (transmitByte == (SPI_SCB_TRANSMIT_BYTE_ERROR + 1u))
    {
        /* Restore the expected value */
        transmitByte--;
    }
    #endif /* End (ERROR_IN_SPI_SCB) */

    /* Wait while a byte is sent */
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
        /* Check if the received and transmitted values are the same */
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
*  This function tests an internal loopback of SPI (transmitted byte must be equal to received).
*
* Parameters:
*  CySCB_Type *base - The pointer to SCB hardware to configure.
*
* Return:
*  1 - test failed
*  2 - still testing
*  3 - Test completed
*  4 - TX Not empty
*  5 - RX Not empty
*
* Note:
*  During a call, the function transmits and receives bytes from 0x01 to 0xFF.
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

    /* Wait for the end of user transmitting. Do not test if the buffer contains data. */
    /* Use a guard interval > (spi_bitrate * spi_data_bits * spi_tx_buf_size) */
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
         * data element was completely transferred.
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
                /* Check if the test was performed with all values from 0x00 to 0xFF */

                /* If test was performed with all values from 0x00 to 0xFF */
                if (byteToTest == 0x00u)
                {
                    /* Return the status that test fully completed */
                    ret = PASS_COMPLETE_STATUS;
                }
                else
                {
                    /* Return the status that an error was not detected but the test is not fully completed */
                    ret = PASS_STILL_TESTING_STATUS;
                }
            }
        }
    }

    /* If something in the code went wrong */
    /* Return ERROR status */
    return ret;
}


/* [] END OF FILE */

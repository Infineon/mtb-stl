/*******************************************************************************
* File Name: SelfTest_I2C_SCB.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to do the I2C self tests
*  according to Class B library.
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
#include "SelfTest_I2C_SCB.h"
#include "SelfTest_ErrorInjection.h"

static uint8_t SelfTest_I2C_SCB_Write(CySCB_Type* master_base,
                                      cy_stc_scb_i2c_context_t* master_context,
                                      CySCB_Type* slave_base,
                                      cy_stc_scb_i2c_context_t* slave_context,
                                      uint8_t* slave_read_buf, uint8_t* slave_write_buf,
                                      uint8_t transmitData);
static uint8_t SelfTest_I2C_SCB_Read(CySCB_Type* master_base,
                                     cy_stc_scb_i2c_context_t* master_context,
                                     CySCB_Type* slave_base,
                                     cy_stc_scb_i2c_context_t* slave_context,
                                     uint8_t* slave_read_buf, uint8_t transmitData);

/*******************************************************************************
* Function Declaration
*******************************************************************************/

static cy_stc_scb_i2c_master_xfer_config_t masterTransferCfg =
{
    .slaveAddress = I2C_SLAVE_ADDR,
    .buffer       = NULL,
    .bufferSize   = 0U,
    .xferPending  = false
};

/*****************************************************************************
* Function Name: SelfTest_I2C_SCB_Write
******************************************************************************
*
* Summary:
*  I2C master under test sends data to I2C slave for self-test.
*  Slave read the data and write the complement[1's] to the slave read buffer
*
* Parameters:
*  CySCB_Type* master_base - The pointer to the master I2C SCB instance.
*  cy_stc_scb_i2c_context_t* master_context - The pointer to the master
*    I2C SCB context.
*  CySCB_Type* slave_base - The pointer to the slave I2C SCB instance.
*  cy_stc_scb_i2c_context_t* slave_context - The pointer to the slave
*    I2C SCB context.
*  uint8_t* slave_read_buf - The pointer to slave read buffer.
*  uint8_t* slave_write_buf - The pointer to slave write buffer.
*  uint8 transmitData - data to be transmitted over I2C
*
* Return:
*  0 - test success
*  1 - test failed
*  4 - I2C Master Busy
*
* Note:
*
*****************************************************************************/
static uint8_t SelfTest_I2C_SCB_Write(CySCB_Type* master_base,
                                      cy_stc_scb_i2c_context_t* master_context,
                                      CySCB_Type* slave_base,
                                      cy_stc_scb_i2c_context_t* slave_context,
                                      uint8_t* slave_read_buf, uint8_t* slave_write_buf,
                                      uint8_t transmitData)
{
    uint8_t ret = OK_STATUS;
    uint16_t guardCnt = 0u;
    uint8_t txBuffer[PACKET_SIZE];

    /* Check if intentional error should be made for testing */
    #if (ERROR_IN_I2C_SCB == 1u)
    if (transmitData == (I2C_SCB_TRANSMIT_BYTE_ERROR))
    {
        /* Restore expected value */
        transmitData--;
    }
    #endif /* End (ERROR_IN_UART_SCB == 1u) */

    txBuffer[0] = transmitData;
    /* Setup transfer specific parameters */
    masterTransferCfg.buffer     = txBuffer;
    masterTransferCfg.bufferSize = PACKET_SIZE;

    /* Start I2C write and check status*/
    if (CY_SCB_I2C_SUCCESS == Cy_SCB_I2C_MasterWrite(master_base, &masterTransferCfg,
                                                     master_context))
    {
        /* If I2C write started without errors,
         *  wait until I2C Master completes write transfer
         */
        while ((0u !=
                (Cy_SCB_I2C_MasterGetStatus(master_base,
                                            master_context) & CY_SCB_I2C_MASTER_BUSY)) &&
               (guardCnt < I2C_DATA_TIME))
        {
            guardCnt++;
            Cy_SysLib_DelayUs(1u);
            /* Wait */
        }
        /* If timeout */
        if (guardCnt >= I2C_DATA_TIME)
        {
            ret = ERROR_STATUS;
        }
        else
        {
            /* Start I2C slave read and check status */
            if (0u !=
                (Cy_SCB_I2C_SlaveGetStatus(slave_base, slave_context) & CY_SCB_I2C_SLAVE_WR_CMPLT))
            {
                /* Check the packet length */
                if (PACKET_SIZE == Cy_SCB_I2C_SlaveGetWriteTransferCount(slave_base, slave_context))
                {
                    /* Read the slave write buffer, complement the data and write to read buffer */
                    slave_read_buf[0] = ~(slave_write_buf[0]);

                    /* Clear the Master and slave write buffer and status */
                    Cy_SCB_I2C_SlaveConfigWriteBuf(slave_base, slave_write_buf,
                                                   PACKET_SIZE, slave_context);

                    (void)Cy_SCB_I2C_SlaveClearWriteStatus(slave_base, slave_context);
                }
                else
                {
                    ret = ERROR_STATUS;
                }
            }
            else
            {
                ret = ERROR_STATUS;
            }
        }
    }
    else
    {
        ret = I2C_MASTER_BUSY_STATUS;
    }

    return ret;
}


/*****************************************************************************
* Function Name: SelfTest_I2C_SCB_Read
******************************************************************************
*
* Summary:
*  Master reads the data from slave and check the data is valid by comparing
*  with the complement of data, which is send by master in previous transaction.
*
* Parameters:
*  CySCB_Type* master_base - The pointer to the master I2C SCB instance.
*  cy_stc_scb_i2c_context_t* master_context - The pointer to the master
*    I2C SCB context.
*  CySCB_Type* slave_base - The pointer to the slave I2C SCB instance.
*  cy_stc_scb_i2c_context_t* slave_context - The pointer to the slave
*    I2C SCB context.
*  uint8_t* slave_read_buf - The pointer to slave read buffer.
*  uint8 transmitData - data to be compared with received data from I2C slave
*
* Return:
*  0 - test success
*  1 - test failed
*  4 - I2C Master Busy
*
* Note:
*
*****************************************************************************/
static uint8_t SelfTest_I2C_SCB_Read(CySCB_Type* master_base,
                                     cy_stc_scb_i2c_context_t* master_context,
                                     CySCB_Type* slave_base,
                                     cy_stc_scb_i2c_context_t* slave_context,
                                     uint8_t* slave_read_buf, uint8_t transmitData)
{
    uint8_t ret = OK_STATUS;
    uint16_t guardCnt = 0u;
    uint8_t byteReceivedMaster[PACKET_SIZE];

    /* Setup transfer specific parameters */
    masterTransferCfg.buffer     = byteReceivedMaster;
    masterTransferCfg.bufferSize = PACKET_SIZE;

    if (CY_SCB_I2C_SUCCESS ==
        Cy_SCB_I2C_MasterRead(master_base, &masterTransferCfg, master_context))
    {
        /* If I2C read started without errors, wait until master complete read transfer */
        while ((0u !=
                (Cy_SCB_I2C_MasterGetStatus(master_base,
                                            master_context) & CY_SCB_I2C_MASTER_BUSY)) &&
               (guardCnt < I2C_DATA_TIME))
        {
            guardCnt++;
            Cy_SysLib_DelayUs(1u);
            /* Wait */
        }
        /* If timeout */
        if (guardCnt >= I2C_DATA_TIME)
        {
            ret = ERROR_STATUS;
        }
        else
        {
            /* Display transfer status */
            if (0u ==
                (CY_SCB_I2C_MASTER_ERR & Cy_SCB_I2C_MasterGetStatus(master_base, master_context)))
            {
                /* Check packet structure */
                if (Cy_SCB_I2C_MasterGetTransferCount(master_base, master_context) == PACKET_SIZE)
                {
                    uint8_t compTransmitData = ~transmitData;
                    /* Check packet received to the 1's complement of packet sent */
                    if (byteReceivedMaster[0] == compTransmitData)
                    {
                        ret = OK_STATUS;
                    }
                    else
                    {
                        ret = ERROR_STATUS;
                    }
                }
                else
                {
                    ret = ERROR_STATUS;
                }
            }
            else
            {
                ret = ERROR_STATUS;
            }
        }
        /* Clear the Master and slave read buffer and status */
        Cy_SCB_I2C_SlaveConfigReadBuf(slave_base, slave_read_buf,
                                      PACKET_SIZE, slave_context);
        (void)Cy_SCB_I2C_SlaveClearReadStatus(slave_base, slave_context);
    }
    else
    {
        ret = I2C_MASTER_BUSY_STATUS;
    }

    return ret;
}


/*****************************************************************************
* Function Name: SelfTest_I2C_SCB
******************************************************************************
*
* Summary:
*  I2C master under test sends data to I2C slave for self-test.
*  Slave read the data and write the complement[1's] to the slave read buffer
*  Master read back the data from slave and check the data is corrupted.
*
* Parameters:
*  CySCB_Type* master_base - The pointer to the master I2C SCB instance.
*  cy_stc_scb_i2c_context_t* master_context - The pointer to the master
*    I2C SCB context.
*  CySCB_Type* slave_base - The pointer to the slave I2C SCB instance.
*  cy_stc_scb_i2c_context_t* slave_context - The pointer to the slave
*    I2C SCB context.
*  uint8_t* slave_read_buf - The pointer to slave read buffer.
*  uint8_t* slave_write_buf - The pointer to slave write buffer.
*
* Return:
*  1 - Test failed
*  2 - Still testing
*  3 - Test completed OK
*
* Note:
*  During call, function transmits from 0x01 to 0xFF
*  Clear I2C Master status, read buffer and write buffer
*
*****************************************************************************/
uint8_t SelfTest_I2C_SCB(CySCB_Type* master_base, cy_stc_scb_i2c_context_t* master_context,
                         CySCB_Type* slave_base, cy_stc_scb_i2c_context_t* slave_context,
                         uint8_t* slave_read_buf, uint8_t* slave_write_buf)
{
    uint8_t ret = OK_STATUS;
    static uint8_t i2cMode = WRITE;

    /* Byte for transmitting */
    static uint8_t byteToTest = { 7u };
    static uint8_t i2cBusBusyWriteCount = 0u;
    static uint8_t i2cBusBusyReadCount = 0u;

    if (WRITE == i2cMode)
    {
        /* I2C Master sends data to test. The slave reads the data and
         *  update the Slave Read buffer with 1's complement.
         */
        ret = SelfTest_I2C_SCB_Write(master_base, master_context, slave_base, slave_context,
                                     slave_read_buf, slave_write_buf, byteToTest);
        if (OK_STATUS == ret)
        {
            i2cBusBusyWriteCount = 0u;
            /* I2C Write complete, start slave read mode */
            i2cMode = READ;
        }
        /* I2C Master Busy, update retry counter */
        else if (I2C_MASTER_BUSY_STATUS == ret)
        {
            i2cBusBusyWriteCount++;
            /* retry counter expires */
            if (i2cBusBusyWriteCount >= I2C_MASTER_BUSY_RETRY)
            {
                ret = ERROR_STATUS;
            }
        }
        else
        {
            ret = ERROR_STATUS;
        }
    }

    if (READ == i2cMode)
    {
        /* I2C Master reads the data from slave read buffer. Check the data is correct */
        ret = SelfTest_I2C_SCB_Read(master_base, master_context, slave_base, slave_context,
                                    slave_read_buf, byteToTest);
        if (OK_STATUS == ret)
        {
            i2cBusBusyReadCount = 0u;
            /* I2C Read completed, change to write mode and update the data to test*/
            i2cMode = WRITE;
            byteToTest++;
        }
        /* I2C Master Busy, update retry counter */
        else if (I2C_MASTER_BUSY_STATUS == ret)
        {
            i2cBusBusyReadCount++;
            /* retry counter expires */
            if (i2cBusBusyReadCount >= I2C_MASTER_BUSY_RETRY)
            {
                ret = ERROR_STATUS;
            }
        }
        else
        {
            ret = ERROR_STATUS;
        }
    }
    /* Check result of test */
    if (ret == OK_STATUS)
    {
        /* If test was performed with all values from 0x00 to 0xFF */
        if (byteToTest == I2C_TEST_RANGE)
        {
            byteToTest = 0u;
            /* Return status, that test fully completed */
            ret = PASS_COMPLETE_STATUS;
        }/* If not */
        else
        {
            /* Return status, that error was not detected but test is not fully completed */
            ret = PASS_STILL_TESTING_STATUS;
        }
    }
    else if (I2C_MASTER_BUSY_STATUS == ret)
    {
        /* Return status, that error was not detected but test is not fully completed */
        ret = PASS_STILL_TESTING_STATUS;
    }
    else
    {
        ret = ERROR_STATUS;
    }

    return ret;
}


/* [] END OF FILE */

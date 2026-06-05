/*******************************************************************************
 * File Name: SelfTest_UART_master_message.c
 *
 * Description:
 *  This file provides the source code to the API for the UART master
 *  packet protocol.
 *
 * Note:
 *  Protocol description
 *  Master(A) slave(B) communication:
 *     |---|    --- Request send --->    |---|
 *     | A |    {Wait for respond}       | B |
 *     |---|    <--- Respond send ---    |---|
 *  Packet format:
 *  <STX><ADDR><DL><[Data bytes length equal DL]><CRCH><CRCL>
 *  STX  - 0x02 begin packet marker. Unique byte of start packet.
 *  ADDR - The device address.
 *  DL   - The data length in bytes [1..255].
 *  CRCH - MSB of CRC-16 calculated from ADDR to the last data byte.
 *  CRCL - LSB of CRC-16 calculated from ADDR to the last data byte.
 *  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals ST,
 *  then it is exchanged with a two-byte sequence <ESC><STX+1>
 *  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals ESC,
 *  then it is exchanged with a two-byte sequence <ESC><ESC+1>.
 *
 ********************************************************************************
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
 ********************************************************************************/

/*******************************************************************************
 * Includes
 ********************************************************************************/

#include "cy_pdl.h"
#include "SelfTest_CRC_calc.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_UART_master_message.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/

/* Define UART message tstatus */
#define UM_SEND_START        (0u)
#define UM_SEND_ADDR         (1u)
#define UM_SEND_DL           (2u)
#define UM_SEND_DATA         (3u)
#define UM_SEND_CRC_H        (4u)
#define UM_SEND_CRC_L        (5u)
#define UM_SEND_COMPLETE     (6u)

/* Define UART message rstatus */
#define UM_RECEIVE_START     (0u)
#define UM_RECEIVE_ADDR      (1u)
#define UM_RECEIVE_DL        (2u)
#define UM_RECEIVE_DATA      (3u)
#define UM_RECEIVE_CRC_H     (4u)
#define UM_RECEIVE_CRC_L     (5u)
#define UM_RECEIVE_COMPLETE  (6u)

/* Define protocol control symbols */
#if !defined(STX)
/* Packet Start unique symbol */
#define STX                  (0x02u)
#endif /* End STX */

#if !defined(ESC)
/* 2xbyte change symbol */
#define ESC                  (0x1Bu)
#endif /* End ESC */


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

static void UART_MessageRxInt(uint32_t IntMask);
static void UART_MessageTxInt(uint32_t IntMask);


/* Protocol master control status structure */
static volatile struct
{
    CySCB_Type* scb_base;
    uint8_t address;
    uint8_t* txptr;
    uint8_t txcnt;
    uint16_t tcrc;
    uint8_t tescflg;
    uint8_t tstatus;
    uint8_t* rxptr;
    uint8_t rxcnt;
    uint8_t rlen;
    uint8_t crcnt;
    uint16_t rcrc;
    uint8_t rescflg;
    uint8_t rstatus;
    uint8_t gstatus;
} UART_Master_Struct;

/* Guard Timer details */
static volatile struct
{
    TCPWM_Type* counter_base;
    uint32_t cntNum;
    uint32_t cntMsk;
} Counter_Struct;


/*****************************************************************************
* Function Name: UART_MessageTxInt
******************************************************************************
*
* Summary:
* UART transmission interrupt
*
* Parameters:
*  IntMask - Interrupt source mask
*
* Return:
* NONE
*
* Theory:
* This function provides data transmitting and forms packets.
*
*****************************************************************************/
static void UART_MessageTxInt(uint32_t IntMask)
{
    /* It is possible to put a new byte into TX buffer */
    switch (UART_Master_Struct.tstatus)
    {
        case UM_SEND_START:

            /* Send STX */
            (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, STX);

            /* Next state */
            UART_Master_Struct.tstatus = UM_SEND_ADDR;

            /* Reset TX crc*/
            UART_Master_Struct.tcrc = 0u;
            break;

        case UM_SEND_ADDR:

            /* Send Address */
            if (UART_Master_Struct.tescflg != 0u)
            {
                /* send changed address */
                UART_Master_Struct.tescflg = 0u;
                (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                      (uint32_t)UART_Master_Struct.address + 1u);

                UART_Master_Struct.tstatus = UM_SEND_DL;
                UART_Master_Struct.tcrc =
                    SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                               UART_Master_Struct.address + 1u);
            }
            else
            {
                const uint32_t uart_master_struct_addr = UART_Master_Struct.address;
                if ((uart_master_struct_addr == STX) || (uart_master_struct_addr == ESC))
                {
                    /* Change the procedure */
                    UART_Master_Struct.tescflg = 1u;
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);

                    UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, ESC);
                }
                else
                {
                    /* Send an address */
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                          (uint32_t)UART_Master_Struct.address);
                    UART_Master_Struct.tstatus = UM_SEND_DL;
                    UART_Master_Struct.tcrc =
                        SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                                   UART_Master_Struct.address);
                }
            }
            break;

        case UM_SEND_DL:

            /* Send the Pack length */
            if (UART_Master_Struct.tescflg != 0u)
            {
                /* Send the changed length */
                UART_Master_Struct.tescflg = 0u;
                (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                      (uint32_t)UART_Master_Struct.txcnt + 1u);

                UART_Master_Struct.tstatus = UM_SEND_DATA;
                UART_Master_Struct.tcrc =
                    SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                               UART_Master_Struct.txcnt + 1u);
            }
            else
            {
                const uint32_t uart_master_struct_txcnt = UART_Master_Struct.txcnt;
                if ((uart_master_struct_txcnt == STX) || (uart_master_struct_txcnt == ESC))
                {
                    /* Change the procedure */
                    UART_Master_Struct.tescflg = 1u;
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                    UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                                                         ESC);
                }
                else
                {
                    /* Send a length */
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                          (uint32_t)UART_Master_Struct.txcnt);

                    UART_Master_Struct.tstatus = UM_SEND_DATA;
                    UART_Master_Struct.tcrc =
                        SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                                   UART_Master_Struct.txcnt);
                }
            }
            break;

        case UM_SEND_DATA:

            /* Send Data bytes */
            if (UART_Master_Struct.tescflg != 0u)
            {
                /* Send the changed data */
                UART_Master_Struct.tescflg = 0u;
                (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                      (uint32_t)(*UART_Master_Struct.txptr) + 1u);
                UART_Master_Struct.tcrc =
                    SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                               *UART_Master_Struct.txptr + 1u);

                UART_Master_Struct.txptr++;
                UART_Master_Struct.txcnt--;
            }
            else
            {
                const uint8_t uart_master_struct_txptr = *UART_Master_Struct.txptr;
                if ((uart_master_struct_txptr == STX) || (uart_master_struct_txptr == ESC))
                {
                    /* Change the procedure */
                    UART_Master_Struct.tescflg = 1u;
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                    UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                                                         ESC);
                }
                else
                {
                    /* Send data */
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                          (uint32_t)(*UART_Master_Struct.txptr));
                    UART_Master_Struct.tcrc =
                        SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc,
                                                   *UART_Master_Struct.txptr);

                    UART_Master_Struct.txptr++;
                    UART_Master_Struct.txcnt--;
                }
            }

            if (UART_Master_Struct.txcnt == 0u)
            {
                /* All data sent
                 * Disable the buffer empty interrupt
                 * and wait for sending all bytes
                 * for CRC calculation */
                UART_Master_Struct.tstatus = UM_SEND_CRC_H;
            }
            break;

        case UM_SEND_CRC_H:

            /* Send High byte of CRC16 */
            if (UART_Master_Struct.tescflg != 0u)
            {
                /* Send the changed MSB of CRC */
                UART_Master_Struct.tescflg = 0u;
                (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                      ((uint32_t)UART_Master_Struct.tcrc >> 8u) + 1u);
                UART_Master_Struct.tstatus = UM_SEND_CRC_L;
            }
            else
            {
                uint8_t uart_master_struct_tcrc = (uint8_t)(UART_Master_Struct.tcrc >> 8u);
                if (((uart_master_struct_tcrc) == STX) || ((uart_master_struct_tcrc) == ESC))
                {
                    /* Change the procedure */
                    UART_Master_Struct.tescflg = 1u;
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                }
                else
                {
                    /* Send MSB of CRC */
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                          (uint32_t)UART_Master_Struct.tcrc >> 8u);
                    UART_Master_Struct.tstatus = UM_SEND_CRC_L;
                }
            }
            break;

        case UM_SEND_CRC_L:

            /* Send the Low byte of CRC16 */
            if (UART_Master_Struct.tescflg != 0u)
            {
                /* Send the changed LSB of CRC */
                UART_Master_Struct.tescflg = 0u;
                (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                      (uint32_t)UART_Master_Struct.tcrc + 1u);
                UART_Master_Struct.tstatus = UM_SEND_COMPLETE;
            }
            else
            {
                uint8_t uart_master_struct_tcrc = (uint8_t)(UART_Master_Struct.tcrc);
                if ((uart_master_struct_tcrc == STX) || (uart_master_struct_tcrc == ESC))
                {
                    /* Change the procedure */
                    UART_Master_Struct.tescflg = 1u;
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                }
                else
                {
                    /* Send LSB of CRC */
                    (void)Cy_SCB_UART_Put(UART_Master_Struct.scb_base,
                                          (uint32_t)UART_Master_Struct.tcrc);

                    UART_Master_Struct.tstatus = UM_SEND_COMPLETE;
                }
            }
            break;

        case UM_SEND_COMPLETE:

            /* Physical end of packet's transmit */
            Cy_SCB_SetTxInterruptMask(UART_Master_Struct.scb_base, 0UL);

            /* End of request !!! */
            break;

        default:
            /* Do not get here */
            break;
    }

    /* Re-enable TX */
    Cy_SCB_ClearTxInterrupt(UART_Master_Struct.scb_base, IntMask);
}


/*****************************************************************************
* Function Name: UART_MessageRxInt
******************************************************************************
*
* Summary:
* UART receiving interrupt
*
* Parameters:
*  IntMask - Interrupt source mask
*
* Return:
* NONE
*
* Theory:
* This function provides data receiving and its ejecting from the packets.
*
*****************************************************************************/
static void UART_MessageRxInt(uint32_t IntMask)
{
    /* Get the received byte from HW buffer */
    uint8_t bt;

    if (IntMask == CY_SCB_RX_INTR_NOT_EMPTY)
    {
        bt = (uint8_t)Cy_SCB_UART_Get(UART_Master_Struct.scb_base);
        Cy_SCB_ClearRxInterrupt(UART_Master_Struct.scb_base, IntMask);

        if (bt == STX)
        {
            /* STX received, start packet analysis */
            UART_Master_Struct.rstatus = UM_RECEIVE_ADDR;
            UART_Master_Struct.rcrc = 0u;
        }
        else
        {
            const uint8_t uart_master_struct_rstatus = UART_Master_Struct.rstatus;
            /* Update CRC */
            if ((uart_master_struct_rstatus != UM_RECEIVE_CRC_H) &&
                (uart_master_struct_rstatus != UM_RECEIVE_CRC_L))
            {
                #if (ERROR_IN_PROTOCOL == 1)
                UART_Master_Struct.rcrc =
                    SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.rcrc, bt + 1);
                #else
                UART_Master_Struct.rcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.rcrc, bt);
                #endif /* End ERROR_IN_PROTOCOL */
            }

            /* Received byte does not equal STX */
            if (bt == ESC)
            {
                /* ESC received, set a flag and analyze the next byte */
                UART_Master_Struct.rescflg = 1u;
            }
            else
            {
                if (UART_Master_Struct.rescflg != 0u)
                {
                    /* The previous byte was ESC. Change the 2-byte sequence to one-byte */
                    UART_Master_Struct.rescflg = 0u;
                    bt -= 1u;
                }

                /* Analyse the byte */
                switch (UART_Master_Struct.rstatus)
                {
                    case UM_RECEIVE_ADDR:

                        /* Receive the address */
                        if (bt != UART_Master_Struct.address)
                        {
                            /* Invalid address, wait for another packet */
                            UART_Master_Struct.rstatus = UM_RECEIVE_START;
                        }
                        else
                        {
                            /* The adress is valid, receive the data size */
                            UART_Master_Struct.rstatus = UM_RECEIVE_DL;
                        }
                        break;

                    case UM_RECEIVE_DL:

                        /* Receive the packet data length
                         * and store this value for
                         * detecting the end of data bytes */
                        UART_Master_Struct.rlen = bt;
                        UART_Master_Struct.rstatus = UM_RECEIVE_DATA;
                        UART_Master_Struct.crcnt = 0u;
                        break;

                    case UM_RECEIVE_DATA:

                        /* Receive the packet data
                         * change the 2-byte sequence and
                         * store data to the buffer */
                        if (UART_Master_Struct.crcnt < UART_Master_Struct.rxcnt)
                        {
                            UART_Master_Struct.rxptr[UART_Master_Struct.crcnt] = bt;
                            UART_Master_Struct.crcnt++;
                        }

                        /* Are all data bytes received? */
                        if (UART_Master_Struct.crcnt >= UART_Master_Struct.rlen)
                        {
                            /* Stop crc calculation
                             * Store CRC value */
                            UART_Master_Struct.rstatus = UM_RECEIVE_CRC_H;
                        }
                        break;

                    case UM_RECEIVE_CRC_H:

                        /* Receive CRC16 high byte */
                        if (bt == (uint8_t)(UART_Master_Struct.rcrc >> 8u))
                        {
                            UART_Master_Struct.rstatus = UM_RECEIVE_CRC_L;
                        }
                        else
                        {
                            /* The packet is broken */
                            UART_Master_Struct.rstatus = UM_RECEIVE_START;
                        }
                        break;

                    case UM_RECEIVE_CRC_L:

                        /* Receive CRC16 low byte */
                        if (bt == (uint8_t)UART_Master_Struct.rcrc)
                        {
                            /* Packet is received */
                            Cy_SCB_SetRxInterruptMask(UART_Master_Struct.scb_base, 0UL);

                            Cy_TCPWM_SetInterruptMask(Counter_Struct.counter_base,
                                                      Counter_Struct.cntNum, 0);
                            #if defined(CY_IP_MXTCPWM)
                            Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base,
                                                              Counter_Struct.cntNum);
                            #else
                            Cy_TCPWM_TriggerStopOrKill(Counter_Struct.counter_base,
                                                       Counter_Struct.cntMsk);
                            #endif
                            UART_Master_Struct.rstatus = UM_RECEIVE_COMPLETE;
                            UART_Master_Struct.gstatus = UM_COMPLETE;
                        }
                        else
                        {
                            /* The packet is broken */
                            UART_Master_Struct.rstatus = UM_RECEIVE_START;
                        }
                        break;

                    default:
                        /* Should not get here */
                        break;
                }
            }
        }
    }
}


/*****************************************************************************
* Function Name: UartMesMaster_Msg_ISR
******************************************************************************
*
* Summary:
* UART receiving/transmitting the interrupt handler.
*
* Parameters:
* NONE
*
* Return:
* NONE
*
* Theory:
*
*****************************************************************************/
void UartMesMaster_Msg_ISR(void)
{
    uint32_t txIntStatus, rxIntStatus;

    txIntStatus = Cy_SCB_GetTxInterruptStatusMasked(UART_Master_Struct.scb_base);
    rxIntStatus = Cy_SCB_GetRxInterruptStatusMasked(UART_Master_Struct.scb_base);

    if (rxIntStatus != 0u)
    {
        UART_MessageRxInt(rxIntStatus);
    }

    if (txIntStatus != 0u)
    {
        UART_MessageTxInt(txIntStatus);
    }
}


/*****************************************************************************
* Function Name: UartMesMaster_Timeout_ISR
******************************************************************************
*
* Summary:
* UART timer guard interrupt
*
* Parameters:
* NONE
*
* Return:
* NONE
*
* Theory:
* This function provides the timeout control when data is sent but the response
* is not received during the packet transaction timeout.
*
*****************************************************************************/

void UartMesMaster_Timeout_ISR(void)
{
    /* Something is wrong
     * no respond during the guard time.
     * Stop the process and indicate an error. */
    uint32_t counterStatus;
    counterStatus = Cy_TCPWM_GetInterruptStatus(Counter_Struct.counter_base, Counter_Struct.cntNum);

    UART_Master_Struct.gstatus = UM_ERROR;
    #if defined(CY_IP_MXTCPWM)
    Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base, Counter_Struct.cntNum);
    #else
    Cy_TCPWM_TriggerStopOrKill(Counter_Struct.counter_base, Counter_Struct.cntMsk);
    #endif
    Cy_TCPWM_SetInterruptMask(Counter_Struct.counter_base, Counter_Struct.cntNum, 0);
    Cy_TCPWM_ClearInterrupt(Counter_Struct.counter_base, Counter_Struct.cntNum, counterStatus);
}


/*****************************************************************************
* Function Name: UartMesMaster_Init
******************************************************************************
*
* Summary:
*  Initialize UART master protocol unit
*
* Parameters:
*  CySCB_Type* uart_base - The pointer to the master UART SCB instance.
*  TCPWM_Type* counter_base - The pointer to the master TCPWM instance.
*  uint32_t cntNum - The Counter instance number in the selected TCPWM.
*
* Return:
*  NONE
*
* Theory:
*  This function starts the components and
*  initializes the control status structure.
*
*****************************************************************************/
void UartMesMaster_Init(CySCB_Type* uart_base, TCPWM_Type* counter_base, uint32_t cntNum)
{
    /* Set initial data to control the status structure */
    UART_Master_Struct.gstatus = UM_ERROR;

    UART_Master_Struct.scb_base = uart_base;
    Counter_Struct.counter_base = counter_base;
    Counter_Struct.cntNum = cntNum;
    Counter_Struct.cntMsk = (1UL << cntNum);

    Cy_SCB_UART_Enable(UART_Master_Struct.scb_base);
    Cy_SCB_SetTxInterruptMask(UART_Master_Struct.scb_base, 0);
    Cy_SCB_SetRxInterruptMask(UART_Master_Struct.scb_base, 0);
    Cy_SCB_UART_ClearRxFifo(UART_Master_Struct.scb_base);
    Cy_SCB_UART_ClearTxFifo(UART_Master_Struct.scb_base);
}


/*****************************************************************************
* Function Name: UartMesMaster_DataProc
******************************************************************************
*
* Summary:
* Starts the background process of the packet's transmission.
*
* Parameters:
* address - The slave address for data transfer.
* txd     - The pointer to transmitted data.
* tlen    - The size of transmitted data in bytes.
* rxd     - The pointer to the buffer where the received data will be stored.
* rlen    - The size of the received data buffer.
*
* Return:
* If the unit started a message process, the function returns 0.
* If not (because the unit is busy or input data is invalid), the function returns 1.
* use UartMesMaster_State() to check busy state
*
* Theory:
*  This function fills the control status structure and enables the unit interrupts.
*
*****************************************************************************/
uint8_t UartMesMaster_DataProc(uint8_t address, uint8_t* txd, uint8_t tlen, uint8_t* rxd,
                               uint8_t rlen)
{
    uint8_t ret = 0u;
    const uint8_t uart_master_struct_gstatus = UART_Master_Struct.gstatus;
    /* Check if it possible to start a request */
    if ((tlen == 0u) || (uart_master_struct_gstatus == UM_BUSY))
    {
        ret = 1u;
    }
    else
    {
        /* Init the structure */
        UART_Master_Struct.tcrc = 0u;
        UART_Master_Struct.rcrc = 0u;
        UART_Master_Struct.txptr = txd;
        UART_Master_Struct.txcnt = tlen;
        UART_Master_Struct.rxptr = rxd;
        UART_Master_Struct.rxcnt = rlen;
        UART_Master_Struct.tescflg = 0u;
        UART_Master_Struct.rescflg = 0u;
        UART_Master_Struct.address = address;
        UART_Master_Struct.tstatus = UM_SEND_START;
        UART_Master_Struct.rstatus = UM_RECEIVE_START;
        UART_Master_Struct.gstatus = UM_BUSY;

        /* Prepare the components, enable the interrupts, and start the process */
        Cy_SCB_UART_ClearRxFifo(UART_Master_Struct.scb_base);
        Cy_SCB_UART_ClearTxFifo(UART_Master_Struct.scb_base);

        Cy_TCPWM_Counter_SetCounter(Counter_Struct.counter_base, Counter_Struct.cntNum, UM_TIMEOUT);
        #if defined(CY_IP_MXTCPWM)
        Cy_TCPWM_TriggerStart_Single(Counter_Struct.counter_base, Counter_Struct.cntNum);
        #else
        Cy_TCPWM_TriggerStart(Counter_Struct.counter_base, Counter_Struct.cntMsk);
        #endif
        Cy_SCB_SetTxInterruptMask(UART_Master_Struct.scb_base,
                                  CY_SCB_TX_INTR_NOT_FULL | CY_SCB_TX_INTR_UART_DONE);
        Cy_SCB_SetRxInterruptMask(UART_Master_Struct.scb_base, CY_SCB_RX_INTR_NOT_EMPTY);

        Cy_TCPWM_SetInterruptMask(Counter_Struct.counter_base, Counter_Struct.cntNum,
                                  CY_TCPWM_INT_ON_TC);
    }

    return ret;
}


/*****************************************************************************
* Function Name: UartMesMaster_State
******************************************************************************
*
* Summary:
* Returns the current master unit state.
*
* Parameters:
* NONE
*
* Return:
* UM_ERROR    - The last message process ended with an error (the slave did not
*               respond or invalid responded data format).
* UM_COMPLETE - The last message process succeeded (responded data is in the
*               received buffer).
* UM_BUSY     - The unit is busy with the message process.
*
*****************************************************************************/
uint8_t UartMesMaster_State(void)
{
    return UART_Master_Struct.gstatus;
}


/*****************************************************************************
* Function Name: UartMesMaster_GetDataSize
******************************************************************************
*
* Summary:
* Returns the received data size.
*
* Parameters:
* NONE
*
* Return:
* Received the data size in the buffer, the result is valid only if the unit state is UM_COMPLETE.
* Use UartMesMaster_State() to check this condition.
*
*****************************************************************************/
uint8_t UartMesMaster_GetDataSize(void)
{
    uint8_t ret = 0u;

    if (UART_Master_Struct.gstatus == UM_COMPLETE)
    {
        ret = UART_Master_Struct.rlen;
    }

    return ret;
}


/* [] END OF FILE */

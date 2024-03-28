/*******************************************************************************
* File Name: SelfTest_UART_slave_message.c
* Version 1.0.0
*
*
* Description:
*  This file provides the source code to the API for the UART slave
*  packet protocol for CAT2(PSoC4), CAT1A, CAT1C devices.
*
* Note:
*  Protocol description
*  Master(A) slave(B) communication:
*     |---|    --- Request send --->    |---|
*     | A |    {Wait for respond}       | B |
*     |---|    <--- Respond send ---    |---|
*  Packet format:
*  <STX><ADDR><DL><[Data bytes length equal DL]><CRCH><CRCL>
*  STX        - 0x02 begin packet marker. Unique byte of start packet
*  ADDR    - device address
*  DL        - data length in bytes [1..255]
*  CRCH    - MSB of CRC-16 that calculated from ADDR to last data byte
*  CRCL    - LSB of CRC-16    that calculated from ADDR to last data byte
*  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals STX
*  then it's exchanged with two byte sequence <ESC><STX+1>
*  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals ESC
*  then it's exchanged with two byte sequence <ESC><ESC+1>
*
* Hardware Dependency:
*  PSoC 4100S Max Device
*  PSoC 4500S Device
*  CY8C624ABZI-S2D44
*  CY8C6245LQI-S3D72
*  XMC7200D-E272K8384
********************************************************************************
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
********************************************************************************/

/*******************************************************************************
* Includes
********************************************************************************/

#include "cy_pdl.h"
#include "SelfTest_CRC_calc.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_UART_slave_message.h"

/*******************************************************************************
* Macros
********************************************************************************/

/* Received buffer size */
#define MAX_MESSAGE_SIZE     (40u)

/* define UART message rstatus */
#define UMS_RECEIVE_START    (0u)
#define UMS_RECEIVE_ADDR     (1u)
#define UMS_RECEIVE_DL       (2u)
#define UMS_RECEIVE_DATA     (3u)
#define UMS_RECEIVE_CRC_H    (4u)
#define UMS_RECEIVE_CRC_L    (5u)
#define UMS_RECEIVE_COMPLETE (6u)

/* define UART message tstatus */
#define UMS_SEND_START       (0u)
#define UMS_SEND_ADDR        (1u)
#define UMS_SEND_DL          (2u)
#define UMS_SEND_DATA        (3u)
#define UMS_SEND_CRC_H       (4u)
#define UMS_SEND_CRC_L       (5u)
#define UMS_SEND_COMPLETE    (6u)

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

static void UART_MessageTxSlaveInt(uint32_t IntMask);
static void UART_MessageRxSlaveInt(uint32_t IntMask);


/* Protocol slave control status structure */
static volatile struct
{
    CySCB_Type* scb_base;
    uint8_t address;
    uint8_t message[MAX_MESSAGE_SIZE];
    uint8_t rxcnt;
    uint8_t rlen;
    uint16_t rcrc;
    uint8_t rescflg;
    uint8_t rstatus;
    uint8_t * txptr;
    uint8_t txcnt;
    uint16_t tcrc;
    uint8_t tescflg;
    uint8_t tstatus;
    uint8_t gstatus;
}UART_Slave_Struct;




/*****************************************************************************
* Function Name: UART_MessageTxSlaveInt
******************************************************************************
*
* Summary:
* UART transmission interrupt
*
* Parameters:
*  IntMask - uart interrupt source mask
*
* Return:
* NONE
*
* Theory:
* This function provides data transmitting and forms the packets
*
*****************************************************************************/
static void UART_MessageTxSlaveInt(uint32_t IntMask)
{
    
    /* It's possible to put new byte into TX buffer */
    switch(UART_Slave_Struct.tstatus)
    {
        
    case UMS_SEND_START:
    
        /* Send STX */
        (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, STX);
        
        /* Interrupt only when physical transmission finished */
        UART_Slave_Struct.tstatus = UMS_SEND_ADDR;
        
        /* Reset TX crc*/
        UART_Slave_Struct.tcrc = 0u;
        break;
        
    case UMS_SEND_ADDR:
        
        /* Send Address */
        if(UART_Slave_Struct.tescflg != 0u)
        {
        
            /* send changed address */
            UART_Slave_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.address + 1u);
            
            UART_Slave_Struct.tstatus = UMS_SEND_DL;
            UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, UART_Slave_Struct.address + 1u);
        }
        else
        {
            if((UART_Slave_Struct.address == STX) || (UART_Slave_Struct.address == ESC))
            {
            
                /* need change procedure */
                UART_Slave_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ESC);
                
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send address */
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.address);
                
                UART_Slave_Struct.tstatus = UMS_SEND_DL;
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, UART_Slave_Struct.address);
            }
        }
        break;
        
    case UMS_SEND_DL:
        
        /* Send Pack length */
        if(UART_Slave_Struct.tescflg != 0u)
        {
        
            /* send changed length */
            UART_Slave_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.txcnt + 1u);
            
            UART_Slave_Struct.tstatus = UMS_SEND_DATA;
            UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, UART_Slave_Struct.txcnt + 1u);
        }
        else
        {
            if((UART_Slave_Struct.txcnt == STX) || (UART_Slave_Struct.txcnt == ESC))
            {
            
                /* need change procedure */
                UART_Slave_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ESC);
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send length */
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.txcnt);
                
                UART_Slave_Struct.tstatus = UMS_SEND_DATA;
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, UART_Slave_Struct.txcnt);
            }
        }
        break;
        
    case UMS_SEND_DATA:
        
        /* Send Data bytes */
        if(UART_Slave_Struct.tescflg != 0u)
        {
        
            /* send changed data */
            UART_Slave_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)(*UART_Slave_Struct.txptr) + 1u);
            UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, * UART_Slave_Struct.txptr + 1u);
            
            UART_Slave_Struct.txptr++;
            UART_Slave_Struct.txcnt--;
        }
        else
        {
            if((*UART_Slave_Struct.txptr == STX) || ( * UART_Slave_Struct.txptr == ESC))
            {
            
                /* need change procedure */
                UART_Slave_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ESC);
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send data */
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)(*UART_Slave_Struct.txptr));
                UART_Slave_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.tcrc, *UART_Slave_Struct.txptr);
                
                UART_Slave_Struct.txptr++;
                UART_Slave_Struct.txcnt--;
            }
        }
        
        if(UART_Slave_Struct.txcnt == 0u)
        {
            
            /* all data sent 
             * disable buffer empty interrupt
             * and wait for sending all bytes 
             * for CRC calculation */
            UART_Slave_Struct.tstatus = UMS_SEND_CRC_H;
        }
        break;
        
    case UMS_SEND_CRC_H:
        
        /* Send High byte of CRC16 */
        if(UART_Slave_Struct.tescflg != 0u)
        {
        
            /* send changed MSB of CRC */
            UART_Slave_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ((uint32_t)UART_Slave_Struct.tcrc >> 8u) + 1u);
            UART_Slave_Struct.tstatus = UMS_SEND_CRC_L;
        }
        else
        {
            if(((uint8_t)(UART_Slave_Struct.tcrc >> 8u) == STX) || ((uint8_t)(UART_Slave_Struct.tcrc >> 8u) == ESC))
            {
            
                /* need change procedure */
                UART_Slave_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ESC);
            }
            else
            {
                
                /* send MSB of CRC */
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.tcrc >> 8u);
                UART_Slave_Struct.tstatus = UMS_SEND_CRC_L;
            }
        }
        break;
        
    case UMS_SEND_CRC_L:
        
        /* Send Low byte of CRC16 */
        if(UART_Slave_Struct.tescflg !=0u)
        {
        
            /* send changed LSB of CRC */
            UART_Slave_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.tcrc + 1u);
            UART_Slave_Struct.tstatus = UMS_SEND_COMPLETE;
        }
        else
        {
            if(((uint8_t)UART_Slave_Struct.tcrc == STX) || ((uint8_t)UART_Slave_Struct.tcrc == ESC))
            {
            
                /* need change procedure */
                UART_Slave_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, ESC);
            }
            else
            {
                
                /* send LSB of CRC */
                (void) Cy_SCB_UART_Put(UART_Slave_Struct.scb_base, (uint32_t)UART_Slave_Struct.tcrc);
                
                UART_Slave_Struct.tstatus = UMS_SEND_COMPLETE;
            }
        }
        break;
        
    case UMS_SEND_COMPLETE:
        
        /* Physical end of packet's transmit */
        Cy_SCB_SetTxInterruptMask(UART_Slave_Struct.scb_base, 0UL);
        
        UART_Slave_Struct.rstatus = UMS_RECEIVE_START;
        UART_Slave_Struct.gstatus = UM_IDLE;
        
        
        /* End of response !!! */
        break;
        
    default:
        /* Should not get here */
        break;
    }
        
    Cy_SCB_ClearTxInterrupt(UART_Slave_Struct.scb_base, IntMask);
}

/*****************************************************************************
* Function Name: UART_MessageRxSlaveInt
******************************************************************************
*
* Summary:
* UART receiving interrupt
*
* Parameters:
*  IntMask - uart interrupt source mask
*
* Return:
* NONE
*
* Theory:
* This function provides data receiving and it ejecting from packets
*
*****************************************************************************/
static void UART_MessageRxSlaveInt(uint32_t IntMask)
{
    
    /* Get received byte from HW buffer */
    uint8_t bt;
    
    if (IntMask == CY_SCB_RX_INTR_NOT_EMPTY)
    {
    
        bt = (uint8_t)Cy_SCB_UART_Get(UART_Slave_Struct.scb_base);
        Cy_SCB_ClearRxInterrupt(UART_Slave_Struct.scb_base, IntMask);
        
        if ((bt == STX) && (UART_Slave_Struct.gstatus != UM_PACKREADY))
        {
        
            /* STX received, start packet analyses */
            UART_Slave_Struct.rstatus = UMS_RECEIVE_ADDR;
            UART_Slave_Struct.rcrc = 0u;
        }
        else
        {
            
            /* Update CRC */
            if ((UART_Slave_Struct.rstatus != UMS_RECEIVE_CRC_H) && (UART_Slave_Struct.rstatus != UMS_RECEIVE_CRC_L))
            {
                UART_Slave_Struct.rcrc = SelfTests_CRC16_CCITT_Byte(UART_Slave_Struct.rcrc, bt);
            }
            
            /* received byte doesn't equal STX */
            if (bt == ESC)
            {
        
                /* ESC received, set flag and analyze next byte */
                UART_Slave_Struct.rescflg = 1u;
            }
            else
            {
                if(UART_Slave_Struct.rescflg != 0u)
                {
              
                    /* previous byte was ESC, change 2xbyte sequence to one byte */
                    UART_Slave_Struct.rescflg = 0u;
                    bt -= 1u;
                }
        
                /* analyze byte */
                switch(UART_Slave_Struct.rstatus)
                {
                    
                case UMS_RECEIVE_ADDR:
            
                    /* Receive address */
                    if(bt != UART_Slave_Struct.address)
                    {
                
                        /* Invalid address, wait for another packet */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_START;
                    }
                    else
                    {
                    
                        /* address valid, receive data size */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_DL;
                    }
                    break;
                    
                case UMS_RECEIVE_DL:
                
                    /* Receive packet data length 
                     * and storage this value for 
                     * detecting end of data bytes */
                    UART_Slave_Struct.rlen = bt;
                    UART_Slave_Struct.rstatus = UMS_RECEIVE_DATA;
                    UART_Slave_Struct.rxcnt = 0u;
                    break;
                    
                case UMS_RECEIVE_DATA:
                
                    /* Receive packet data
                     * change 2x byte sequence and
                     * storage data to buffer */
                    if(UART_Slave_Struct.rxcnt < MAX_MESSAGE_SIZE)
                    {
                
                        UART_Slave_Struct.message[UART_Slave_Struct.rxcnt] = bt;
                        UART_Slave_Struct.rxcnt++;
                    }
                
                    /* All data bytes received ? */
                    if(UART_Slave_Struct.rxcnt >= UART_Slave_Struct.rlen)
                    {
                
                        /* Stop crc calculation
                         * storage CRC value */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_CRC_H;
                    }
                    break;
                    
                case UMS_RECEIVE_CRC_H:
                
                    /* Receive CRC16 high byte */
                    if(bt == (uint8_t)(UART_Slave_Struct.rcrc >> 8u))
                    {
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_CRC_L;
                    }
                    else
                    {
                
                        /* packet broken */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_START;
                    }
                    break;
                    
                case UMS_RECEIVE_CRC_L:
                
                    /* Receive CRC16 low byte */
                    if(bt == (uint8_t)(UART_Slave_Struct.rcrc & 0xFFu))
                    {
                
                        /* Packet received!!! */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_COMPLETE;
                        UART_Slave_Struct.gstatus = UM_PACKREADY;
                    }
                    else
                    {
                    
                        /* packet broken */
                        UART_Slave_Struct.rstatus = UMS_RECEIVE_START;
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
* Function Name: UART_SlaveIntHandler
******************************************************************************
*
* Summary:
* UART receiving/transmitting int handler
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
void UartMesSlave_Msg_ISR(void)
{
    uint32_t txIntStatus, rxIntStatus;
    
    txIntStatus = Cy_SCB_GetTxInterruptStatusMasked(UART_Slave_Struct.scb_base);
    rxIntStatus = Cy_SCB_GetRxInterruptStatusMasked(UART_Slave_Struct.scb_base);
    
    if (rxIntStatus != 0u)
    {
        UART_MessageRxSlaveInt(rxIntStatus);
    }
    
    if (txIntStatus != 0u)
    {
        UART_MessageTxSlaveInt(txIntStatus);
    }
}

/*****************************************************************************
* Function Name: UartMesSlave_Init
******************************************************************************
*
* Summary:
*  Initialize UART slave protocol unit
*
* Parameters:
*  CySCB_Type* uart_base - The pointer to the slave UART SCB instance.
*  uint8_t address - slave bus address
*
* Return:
*  NONE
*
* Theory:
*  This function starts needed components and
*  initialize control status structure
*
*****************************************************************************/
void UartMesSlave_Init(CySCB_Type* uart_base, uint8_t address)
{
    
    /* set initial data to control status structure */
    UART_Slave_Struct.scb_base = uart_base;
    UART_Slave_Struct.address = address;
    UART_Slave_Struct.gstatus = UM_IDLE;
    UART_Slave_Struct.rstatus = UMS_RECEIVE_START;
    UART_Slave_Struct.tstatus = UMS_SEND_COMPLETE;
    UART_Slave_Struct.rescflg = 0u;
    
    /* start components */
    Cy_SCB_UART_Enable(UART_Slave_Struct.scb_base);
    Cy_SCB_SetTxInterruptMask(UART_Slave_Struct.scb_base, 0UL);
    Cy_SCB_SetRxInterruptMask(UART_Slave_Struct.scb_base, CY_SCB_RX_INTR_NOT_EMPTY);
    Cy_SCB_UART_ClearRxFifo(UART_Slave_Struct.scb_base);
    Cy_SCB_UART_ClearTxFifo(UART_Slave_Struct.scb_base);
}


/*****************************************************************************
* Function Name: UartMesSlave_State
******************************************************************************
*
* Summary:
* Returns current slave unit state
*
* Parameters:
* NONE
*
* Return:
* UM_IDLE            - unit wait for packet from master
* UM_PACKREADY        - unit receive the packet. That is have the marker and
*                       respond will must sent
* UM_RESPOND        - unit send respond
*
*****************************************************************************/
uint8_t UartMesSlave_State(void)
{
    return UART_Slave_Struct.gstatus;
}


/*****************************************************************************
* Function Name: UartMesSlave_GetDataSize
******************************************************************************
*
* Summary:
* Returns received data size
*
* Parameters:
* NONE
*
* Return:
*    Received data size in buffer, result valid only if unit state is UM_PACKREADY
* use UartMesSlave_State() to check this condition
*
*****************************************************************************/
uint8_t UartMesSlave_GetDataSize(void)
{
    uint8_t ret = 0u;
    
    if(UART_Slave_Struct.gstatus == UM_PACKREADY)
    {
        ret = UART_Slave_Struct.rlen;
    }
    
    return ret;
}


/*****************************************************************************
* Function Name: UartMesSlave_Respond
******************************************************************************
*
* Summary:
* Start background sending of the respond
*
* Parameters:
* txd     - pointer to sent data
* tlen     - size of sent data in bytes
*
* Return:
* if the unit begins a response process and function returns 0
* if not (cause IDLE or RESPOND state) function returns 1
* use UartMesSlave_State() to check this condition
*
*****************************************************************************/
uint8_t UartMesSlave_Respond(uint8_t * txd, uint8_t tlen)
{
    uint8_t ret = 0u;
    
    /* is it possible to start responding ? */
    if((tlen == 0u) || (UART_Slave_Struct.gstatus == UM_IDLE))
    {
        ret = 1u;
    }
    else
    {
    
        /* Init the structure */
        UART_Slave_Struct.tcrc = 0u;
        UART_Slave_Struct.rcrc = 0u;
        UART_Slave_Struct.txptr = txd;
        UART_Slave_Struct.txcnt = tlen;
        UART_Slave_Struct.tescflg = 0u;
        UART_Slave_Struct.tstatus = UMS_SEND_START;
        UART_Slave_Struct.gstatus = UM_RESPOND;
    
        /* Enable interrupts and start the process */
        Cy_SCB_SetTxInterruptMask(UART_Slave_Struct.scb_base, CY_SCB_TX_INTR_NOT_FULL | CY_SCB_TX_INTR_UART_DONE);
    }
    
    return ret;
}


/*****************************************************************************
* Function Name: UartMesSlave_GetDataPtr
******************************************************************************
*
* Summary:
* Returns pointer to received data buffer
*
* Parameters:
* NONE
*
* Return:
* pointer to received data buffer
*
*****************************************************************************/
volatile uint8_t * UartMesSlave_GetDataPtr(void)
{
    return UART_Slave_Struct.message;
}


/* [] END OF FILE */

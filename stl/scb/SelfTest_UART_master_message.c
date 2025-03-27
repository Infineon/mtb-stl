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
********************************************************************************
* Copyright 2020-2025, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "SelfTest_UART_master_message.h"

/*******************************************************************************
* Macros
********************************************************************************/

/* define UART message tstatus */
#define UM_SEND_START        (0u)
#define UM_SEND_ADDR         (1u)
#define UM_SEND_DL           (2u)
#define UM_SEND_DATA         (3u)
#define UM_SEND_CRC_H        (4u)
#define UM_SEND_CRC_L        (5u)
#define UM_SEND_COMPLETE     (6u)

/* define UART message rstatus */
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
    uint8_t * txptr;
    uint8_t txcnt;
    uint16_t tcrc;
    uint8_t tescflg;
    uint8_t tstatus;
    uint8_t * rxptr;
    uint8_t rxcnt;
    uint8_t rlen;
    uint8_t crcnt;
    uint16_t rcrc;
    uint8_t rescflg;
    uint8_t rstatus;
    uint8_t gstatus;
}UART_Master_Struct;

/* Guard Timer details */
static volatile struct
{
    TCPWM_Type* counter_base;
    uint32_t cntNum;
    uint32_t cntMsk;
}Counter_Struct;


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
* This function provides data transmitting and forms the packets
*
*****************************************************************************/
static void UART_MessageTxInt(uint32_t IntMask)
{

    /* It's possible to put new byte into TX buffer */
    switch(UART_Master_Struct.tstatus)
    {
    case UM_SEND_START:
        
        /* Send STX */
        (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, STX);
        
        /* Next state */
        UART_Master_Struct.tstatus = UM_SEND_ADDR;
        
        /* Reset TX crc*/
        UART_Master_Struct.tcrc = 0u;
        break;
        
    case UM_SEND_ADDR:
        
        /* Send Address */
        if(UART_Master_Struct.tescflg != 0u)
        {
            /* send changed address */
            UART_Master_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.address + 1u);
            
            UART_Master_Struct.tstatus = UM_SEND_DL;
            UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, UART_Master_Struct.address + 1u);
        }
        else
        {
            const uint32_t uart_master_struct_addr = UART_Master_Struct.address;
            if((uart_master_struct_addr == STX) || (uart_master_struct_addr == ESC))
            {
                
                /* need change procedure */
                UART_Master_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send address */
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.address);
                UART_Master_Struct.tstatus = UM_SEND_DL;
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, UART_Master_Struct.address);
            }    
        }
        break;
        
    case UM_SEND_DL:
        
        /* Send Pack length */
        if(UART_Master_Struct.tescflg != 0u)
        {
            
            /* send changed length */
            UART_Master_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.txcnt + 1u);
            
            UART_Master_Struct.tstatus = UM_SEND_DATA;
            UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, UART_Master_Struct.txcnt + 1u);
        }
        else
        {
            const uint32_t uart_master_struct_txcnt = UART_Master_Struct.txcnt;
            if((uart_master_struct_txcnt == STX) || (uart_master_struct_txcnt == ESC))
            {
                
                /* need change procedure */
                UART_Master_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send length */
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.txcnt);
                
                UART_Master_Struct.tstatus = UM_SEND_DATA;
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, UART_Master_Struct.txcnt);
            }
        }
        break;
        
    case UM_SEND_DATA:
        
        /* Send Data bytes */
        if(UART_Master_Struct.tescflg != 0u)
        {
            
            /* send changed data */
            UART_Master_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)(*UART_Master_Struct.txptr) + 1u);
            UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, *UART_Master_Struct.txptr + 1u);
            
            UART_Master_Struct.txptr++;
            UART_Master_Struct.txcnt--;
        }
        else
        {
            const uint8_t uart_master_struct_txptr = *UART_Master_Struct.txptr; 
            if((uart_master_struct_txptr == STX) || (uart_master_struct_txptr == ESC))
            {
                
                /* need change procedure */
                UART_Master_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, ESC);
            }
            else
            {
                
                /* send data */
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)(*UART_Master_Struct.txptr));
                UART_Master_Struct.tcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.tcrc, *UART_Master_Struct.txptr);
                
                UART_Master_Struct.txptr++;
                UART_Master_Struct.txcnt--;
            }
        }
        
        if(UART_Master_Struct.txcnt == 0u)
        {
            
            /* all data sent 
             * disable buffer empty interrupt
             * and wait for sending all bytes 
             * for CRC calculation */
            UART_Master_Struct.tstatus = UM_SEND_CRC_H;
        }
        break;
        
    case UM_SEND_CRC_H:
        
        /* Send High byte of CRC16 */
        if(UART_Master_Struct.tescflg != 0u)
        {
            
            /* send chanded MSB of CRC */
            UART_Master_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ((uint32_t)UART_Master_Struct.tcrc >> 8u) + 1u);
            UART_Master_Struct.tstatus = UM_SEND_CRC_L;
        }
        else
        {
            uint8_t uart_master_struct_tcrc = (uint8_t)(UART_Master_Struct.tcrc >> 8u);
            if(((uart_master_struct_tcrc) == STX)||((uart_master_struct_tcrc) == ESC))
            {
                
                /* need change procedure */
                UART_Master_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
            }
            else
            {
                
                /* send MSB of CRC */
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.tcrc >> 8u);
                UART_Master_Struct.tstatus = UM_SEND_CRC_L;
            }
        }
        break;
        
    case UM_SEND_CRC_L:
        
        /* Send Low byte of CRC16 */
        if(UART_Master_Struct.tescflg != 0u)
        {
        
            /* send chanded LSB of CRC */
            UART_Master_Struct.tescflg = 0u;
            (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.tcrc + 1u);
            UART_Master_Struct.tstatus = UM_SEND_COMPLETE;
        }
        else
        {
            uint8_t uart_master_struct_tcrc = (uint8_t)(UART_Master_Struct.tcrc);
            if((uart_master_struct_tcrc == STX) || (uart_master_struct_tcrc == ESC))
            {
            
                /* need change procedure */
                UART_Master_Struct.tescflg = 1u;
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, ESC);
            }
            else
            {
                
                /* send LSB of CRC */
                (void) Cy_SCB_UART_Put(UART_Master_Struct.scb_base, (uint32_t)UART_Master_Struct.tcrc);
                
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
        /* Should not get here */
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
* This function provides data receiving and it ejecting from packets
*
*****************************************************************************/
static void UART_MessageRxInt(uint32_t IntMask)
{
    
    /* Get received byte from HW buffer */
    uint8_t bt;
        
    if (IntMask == CY_SCB_RX_INTR_NOT_EMPTY)
    {
        
        bt = (uint8_t)Cy_SCB_UART_Get(UART_Master_Struct.scb_base);
        Cy_SCB_ClearRxInterrupt(UART_Master_Struct.scb_base, IntMask);
        
        if(bt == STX)
        {
        
            /* STX received     * start packet analyses */
            UART_Master_Struct.rstatus = UM_RECEIVE_ADDR;
            UART_Master_Struct.rcrc = 0u;
        }
        else
        {
            const uint8_t uart_master_struct_rstatus = UART_Master_Struct.rstatus;
            /* Update CRC */
            if ((uart_master_struct_rstatus != UM_RECEIVE_CRC_H) && (uart_master_struct_rstatus != UM_RECEIVE_CRC_L))
            {
                #if (ERROR_IN_PROTOCOL == 1)
                    UART_Master_Struct.rcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.rcrc, bt + 1);
                #else
                    UART_Master_Struct.rcrc = SelfTests_CRC16_CCITT_Byte(UART_Master_Struct.rcrc, bt);
                #endif /* End ERROR_IN_PROTOCOL */
            }
            
            /* received byte doesn't equal STX */
            if (bt == ESC)
            {
        
                /* ESC received, set flag and analyze next byte */
                UART_Master_Struct.rescflg = 1u;
            }
            else
            {
                if (UART_Master_Struct.rescflg != 0u)
                {
              
                    /* previous byte was ESC change 2xbyte sequence to one byte */
                    UART_Master_Struct.rescflg = 0u;
                    bt -= 1u;
                }
            
                /* analyse byte */
                switch(UART_Master_Struct.rstatus)
                {
                    
                case UM_RECEIVE_ADDR:
            
                    /* Receive address */
                    if(bt != UART_Master_Struct.address)
                    {
                
                        /* Invalid address, wait to another packet */
                        UART_Master_Struct.rstatus = UM_RECEIVE_START;
                    }
                    else
                    {
                    
                        /* adress valid, receive data size */
                        UART_Master_Struct.rstatus = UM_RECEIVE_DL;
                    }
                    break;
                    
                case UM_RECEIVE_DL:
                
                    /* Receive packet data length 
                     * and storage this value for 
                     * detecting end of data bytes */
                    UART_Master_Struct.rlen = bt;
                    UART_Master_Struct.rstatus = UM_RECEIVE_DATA;
                    UART_Master_Struct.crcnt = 0u;
                    break;
                    
                case UM_RECEIVE_DATA:
                
                    /* Receive packet data
                     * change 2x byte sequence and
                     * storage data to buffer */
                    if(UART_Master_Struct.crcnt < UART_Master_Struct.rxcnt)
                    {
                        UART_Master_Struct.rxptr[UART_Master_Struct.crcnt] = bt;
                        UART_Master_Struct.crcnt++;
                    }
                
                    /* All data bytes received ? */
                    if(UART_Master_Struct.crcnt >= UART_Master_Struct.rlen)
                    {
                
                        /* Stop crc calculation
                         * storage CRC value */
                        UART_Master_Struct.rstatus = UM_RECEIVE_CRC_H;
                    }
                    break;
                    
                case UM_RECEIVE_CRC_H:
                
                    /* Receive CRC16 high byte */
                    if(bt == (uint8_t)(UART_Master_Struct.rcrc >> 8u))
                    {
                        UART_Master_Struct.rstatus = UM_RECEIVE_CRC_L;
                    }
                    else
                    {
                
                        /* packet broken */
                        UART_Master_Struct.rstatus = UM_RECEIVE_START;
                    }
                    break;
                    
                case UM_RECEIVE_CRC_L:
            
                    /* Receive CRC16 low byte */
                    if(bt == (uint8_t)UART_Master_Struct.rcrc)
                    {
                
                        /* Packet received!!! */
                        Cy_SCB_SetRxInterruptMask(UART_Master_Struct.scb_base, 0UL);
                        
                        Cy_TCPWM_SetInterruptMask(Counter_Struct.counter_base, Counter_Struct.cntNum, 0);
#if ((defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
                        Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base, Counter_Struct.cntNum);
#else
   #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A256K))
                        Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base, Counter_Struct.cntMsk);
   #else
                        Cy_TCPWM_TriggerStopOrKill(Counter_Struct.counter_base, Counter_Struct.cntMsk);
   #endif
#endif                        
                        UART_Master_Struct.rstatus = UM_RECEIVE_COMPLETE;
                        UART_Master_Struct.gstatus = UM_COMPLETE;
                        
                    }
                    else
                    {
                    
                        /* packet broken */
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
* This function provides timeout control when data is sent but response didn't
* is not received during packet transaction timeout
*
*****************************************************************************/

void UartMesMaster_Timeout_ISR(void)
{
    
    /* something is wrong :-(
     * no respond during guard time 
     * stop process and indicate error */
    uint32_t counterStatus;
    counterStatus = Cy_TCPWM_GetInterruptStatus(Counter_Struct.counter_base, Counter_Struct.cntNum);

    UART_Master_Struct.gstatus = UM_ERROR;
#if ((defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
    Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base, Counter_Struct.cntNum);
#else
    #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && defined (CY_DEVICE_PSOC6A256K))
    Cy_TCPWM_TriggerStopOrKill_Single(Counter_Struct.counter_base, Counter_Struct.cntMsk);
    #else
    Cy_TCPWM_TriggerStopOrKill(Counter_Struct.counter_base, Counter_Struct.cntMsk);
    #endif

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
*  This function starts needed components and
*  initializes control status structure
*
*****************************************************************************/
void UartMesMaster_Init(CySCB_Type* uart_base, TCPWM_Type* counter_base,  uint32_t cntNum)
{
    /* set initial data to control status structure */
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
* Start the background process of the packet's transmission
*
* Parameters:
* address    - slave address for data transfer
* txd         - pointer to transmitted data
* tlen         - size of transmitted data in bytes
* rxd        - pointer to the buffer where the received data will be stored
* rlen        - size of received data buffer
*
* Return:
* if the unit began a message process function return 0
* if not (cause unit busy or invalid input data) function return 1
* use UartMesMaster_State() to check busy state
*
* Theory:
*  This function fills control status structure and enables unit interrupts
*
*****************************************************************************/
uint8_t UartMesMaster_DataProc(uint8_t address, uint8_t *txd, uint8_t tlen, uint8_t * rxd, uint8_t rlen)
{
    uint8_t ret = 0u;
    const uint8_t uart_master_struct_gstatus = UART_Master_Struct.gstatus; 
    /* is it possible to start request ? */
    if((tlen == 0u) || (uart_master_struct_gstatus == UM_BUSY))
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
    
        /* Prepare components, enable interrupts and start the process */
        Cy_SCB_UART_ClearRxFifo(UART_Master_Struct.scb_base);
        Cy_SCB_UART_ClearTxFifo(UART_Master_Struct.scb_base);
        
        Cy_TCPWM_Counter_SetCounter(Counter_Struct.counter_base, Counter_Struct.cntNum, UM_TIMEOUT);
#if ((defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
        Cy_TCPWM_TriggerStart_Single(Counter_Struct.counter_base, Counter_Struct.cntNum);
#else
    #if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) && (defined(CY_DEVICE_PSOC6A256K)))
        Cy_TCPWM_TriggerStart_Single(Counter_Struct.counter_base, Counter_Struct.cntMsk);
    #else
        Cy_TCPWM_TriggerStart(Counter_Struct.counter_base, Counter_Struct.cntMsk);
    #endif
#endif
        Cy_SCB_SetTxInterruptMask(UART_Master_Struct.scb_base, CY_SCB_TX_INTR_NOT_FULL | CY_SCB_TX_INTR_UART_DONE);
        Cy_SCB_SetRxInterruptMask(UART_Master_Struct.scb_base, CY_SCB_RX_INTR_NOT_EMPTY);
        
        Cy_TCPWM_SetInterruptMask(Counter_Struct.counter_base, Counter_Struct.cntNum, CY_TCPWM_INT_ON_TC);
    }
    
    return ret;
}

/*****************************************************************************
* Function Name: UartMesMaster_State
******************************************************************************
*
* Summary:
* Returns current master unit state
*
* Parameters:
* NONE
*
* Return:
* UM_ERROR        - last message process ended with error (the slave didn't
*                     respond or invalid responded data format)
* UM_COMPLETE    - last message process succeeded (responded data is in the
*                   received buffer)
* UM_BUSY        - unit is busy with message process
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
* Returns received data size
*
* Parameters:
* NONE
*
* Return:
* Received data size in buffer, result valid only if unit state is UM_COMPLETE
* use UartMesMaster_State() to check this condition
*
*****************************************************************************/
uint8_t UartMesMaster_GetDataSize(void)
{
    uint8_t ret = 0u;
    
    if(UART_Master_Struct.gstatus == UM_COMPLETE)
    {
        ret = UART_Master_Struct.rlen;
    }
    
    return ret;
}


/* [] END OF FILE */

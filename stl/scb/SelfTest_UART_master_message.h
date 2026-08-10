/*******************************************************************************
 * File Name: SelfTest_UART_master_message.h
 *
 * Description:
 *  This file provides the source code to the API for the UART master
 *  packet protocol.
 *
 * Note:
 *  Protocol description
 *  Master(A) slave(B) communication:
 *    |---|    --- Request send --->    |---|
 *    | A |    {Wait for respond}       | B |
 *    |---|    <--- Respond send ---    |---|
 *  Packet format:
 *  <STX><ADDR><DL><[Data bytes length equal DL]><CRCH><CRCL>
 *  STX  - 0x02 starts the packet marker. The unique byte of the start packet.
 *  ADDR - The device address.
 *  DL   - The data length in bytes [1..255].
 *  CRCH - MSB of CRC-16 calculated from ADDR to the last data byte.
 *  CRCL - LSB of CRC-16 calculated from ADDR to the last data byte.
 *  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals STX,
 *  then it is exchanged with the two-byte sequence <ESC><STX+1>.
 *  If there is a byte <ADDR> <DL> <[Data]> or <[CRC]> that equals ESC,
 *  then it is exchanged with the two-byte sequence <ESC><ESC+1>.
 *
 ********************************************************************************
 * (c) 2023-2026, Infineon Technologies AG, or an affiliate of Infineon
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

/**
 * \addtogroup group_uart_data_tsf
 * \{
 *
 */

#if !defined(UART_master_message_H)
    #define UART_master_message_H

#include "SelfTest_common.h"


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/**
 * \addtogroup group_uart_data_tsf_functions_master
 * \{
 */

/*******************************************************************************
* Function Name: UartMesMaster_Init
****************************************************************************//**
*
* Initializes the UART master protocol unit. This function starts the required components and
* initializes the control status structure.
*
* \note
* This function enables the UART peripheral, clears TX/RX FIFOs, and sets the
* interrupt masks used by the message protocol. Use a UART instance dedicated
* to this protocol or reinitialize the application UART configuration after
* calling \ref UartMesMaster_DeInit.
*
*
* \param uart_base
* The pointer to the master UART SCB instance.
* \param counter_base
* The pointer to the master TCPWM instance.
* \param cntNum
* The Counter instance number in the selected TCPWM.
*
*******************************************************************************/
void UartMesMaster_Init(CySCB_Type* uart_base, TCPWM_Type* counter_base, uint32_t cntNum);

/*******************************************************************************
* Function Name: UartMesMaster_DeInit
****************************************************************************//**
*
* De-initializes the UART master protocol unit. Disables all TX/RX interrupts,
* flushes the FIFOs, stops the guard timer, disables the UART peripheral, and
* resets the internal control status structure.
*
*******************************************************************************/
void UartMesMaster_DeInit(void);

/*******************************************************************************
* Function Name: UartMesMaster_DataProc
****************************************************************************//**
*
* Starts the background process of the packet's transmission.
*
*
* \param address
* Slave address for data transfer.
* \param txd
* The pointer to transmitted data.
* \param tlen
* The size of transmitted data in bytes.
* \param rxd
* The pointer to the buffer where the received data will be stored.
* \param rlen
* The size of the received data buffer.
*
* \note
* This function starts an interrupt-driven background transaction. Do not call
* it again while \ref UartMesMaster_State returns UM_BUSY. The \p txd and
* \p rxd buffers must remain valid and unmodified until the state becomes
* UM_COMPLETE or UM_ERROR.
*
* \return
*  \ref UART_MASTER_MESSAGE_STARTED_STATUS (0) - The unit started a message process <br>
*  \ref UART_MASTER_MESSAGE_NOT_STARTED_STATUS (1) - The unit did not start a message process
*
* \note
* Use UartMesMaster_State() to check busy state
*
*******************************************************************************/
uint8_t UartMesMaster_DataProc(uint8_t address, uint8_t* txd, uint8_t tlen, uint8_t* rxd,
                               uint8_t rlen);

/*******************************************************************************
* Function Name: UartMesMaster_State
****************************************************************************//**
*
* Returns the current master unit state.
*
* \return
* UM_ERROR    - The last message process ended with an error (the slave did not
*               respond or teh invalid  format of responded data). <br>
* UM_COMPLETE - The last message process succeeded (responded data is in the
*               received buffer). <br>
* UM_BUSY     - The unit is busy with the message process.
*
*
*******************************************************************************/
uint8_t UartMesMaster_State(void);

/*******************************************************************************
* Function Name: UartMesMaster_GetDataSize
****************************************************************************//**
*
* Returns the size of the received data.
*
* \return
* Received the data size in the buffer.

* \note
*  The result is valid only if the unit state is UM_COMPLETE.
*  Use UartMesMaster_State() to check this condition.
*
*******************************************************************************/
uint8_t UartMesMaster_GetDataSize(void);

/*******************************************************************************
* Function Name: UartMesMaster_Timeout_ISR
****************************************************************************//**
*
* UART timer guard interrupt. This function provides the timeout control when data is
* sent but the response is not received after the packet transaction.
*
*******************************************************************************/
void UartMesMaster_Timeout_ISR(void);

/*******************************************************************************
* Function Name: UartMesMaster_Msg_ISR
****************************************************************************//**
*
* Interrupt handler for UART to receive/transmit
*
*******************************************************************************/
void UartMesMaster_Msg_ISR(void);

/** \} group_uart_data_tsf_functions_master */


/** \cond INTERNAL */

/*******************************************************************************
* Enumerated Types and Parameters
*******************************************************************************/

/* Master states */
#define UM_ERROR               (0u)
#define UM_COMPLETE            (1u)
#define UM_BUSY                (2u)

/* Protocol Timeout (timer ticks) */
#define UM_TIMEOUT             (1000u)

/** \endcond */

/** \} group_uart_data_tsf */

#endif /* End __UART_master_message_H */


/* [] END OF FILE */

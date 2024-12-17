/*******************************************************************************
* File Name: SelfTest_UART_master_message.h
* Version 1.0.0
*
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

/**
* \addtogroup group_uart_data_tsf
* \{
*
*/

#if !defined(UART_master_message_H)
    #define UART_master_message_H


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
* Initialize UART master protocol unit. This function starts needed components and
* initializes control status structure
*
*
* \param uart_base 
* The pointer to the master UART SCB instance. <br>
* \param counter_base
* The pointer to the master TCPWM instance.
* \param cntNum 
* The Counter instance number in the selected TCPWM.
*
* \return
*  NONE
*
*******************************************************************************/
void UartMesMaster_Init(CySCB_Type* uart_base, TCPWM_Type* counter_base,  uint32_t cntNum);

/*******************************************************************************
* Function Name: UartMesMaster_DataProc
****************************************************************************//**
*
* Start the background process of the packet's transmission
*
*
* \param address 
* Slave address for data transfer <br>
* \param txd 
* Pointer to transmitted data <br>
* \param tlen 
* size of transmitted data in bytes <br>
* \param rxd 
* pointer to the buffer where the received data will be stored <br>
* \param rlen 
* size of received data buffer
*
* \return
*  0 - If the unit began a message process
*  1 - if not (cause unit busy or invalid input data)
*
* \note
* use UartMesMaster_State() to check busy state
*
*******************************************************************************/
uint8_t UartMesMaster_DataProc(uint8_t address, uint8_t * txd, uint8_t tlen, uint8_t * rxd, uint8_t rlen);

/*******************************************************************************
* Function Name: UartMesMaster_State
****************************************************************************//**
*
* Returns current master unit state
*
*
*
*
* \return
* UM_ERROR        - last message process ended with error (the slave didn't
*                     respond or invalid responded data format)
* UM_COMPLETE     - last message process succeeded (responded data is in the
*                   received buffer)
* UM_BUSY         - unit is busy with message process
*
*
*******************************************************************************/
uint8_t UartMesMaster_State(void);

/*******************************************************************************
* Function Name: UartMesMaster_GetDataSize
****************************************************************************//**
*
* Returns size of the received data
*
*
*
*
* \return
*  Received data size in buffer

* \note
*  Result valid only if unit state is UM_COMPLETE
*  use UartMesMaster_State() to check this condition.
*
*
*******************************************************************************/
uint8_t UartMesMaster_GetDataSize(void);

/*******************************************************************************
* Function Name: UartMesMaster_Timeout_ISR
****************************************************************************//**
*
* UART timer guard interrupt. This function provides timeout control when data is
* sent but response is not received after packet transaction.
*
*
*
*
* \return
* NONE
*
*
*******************************************************************************/
void UartMesMaster_Timeout_ISR(void);

/*******************************************************************************
* Function Name: UartMesMaster_Msg_ISR
****************************************************************************//**
*
* Interrupt handler for UART to receive/transmit
*
*
*
*
* \return
* NONE
*
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

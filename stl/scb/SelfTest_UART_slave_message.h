/*******************************************************************************
* File Name: SelfTest_UART_slave_message.h
*
* Description:
*  This file provides the source code to the API for the UART slave
*  packet protocol.
*
* Note:
*  Protocol description
*  Master(A) slave(B) communication:
*    |---|    --- Request send --->    |---|
*    | A |    {Wait for respond}       | B |
*    |---|    <--- Respond send ---    |--
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
/**
* \addtogroup group_uart_data_tsf
* \{
*
* The UART SCB Components are used to physically generate the signals. 
* The software CRC-16 calculation is applied to every sent/received byte (except STX and the CRC itself). To detect an unsuccessful packet transaction, the timer is used.
*
* Three interrupts implemented in this project provide a fully interrupt-driven background process: <br>
*  1) The transmit interrupt in the UART is configured for a FIFO not full event to take the new data from the RAM and place 
      it into the TX buffer, and for a transmit complete event to start or stop the CRC calculation. <br>
*  2) The receive interrupt in the UART is configured for a FIFO not empty event to analyze the received data, calculate the
      CRC, and store the received data into RAM. <br>
*  3) The timer interrupt is used to detect the end of an unsuccessful transmission.
*
* \section group_uart_data_tsf_more_information More Information
*
*    Protocol description
*    Master(A) slave(B) communication:
*    \code |---|    --- Request send --->    |---| \endcode
*    \code | A |    {Wait for respond}       | B | \endcode
*    \code |---|    <--- Respond send ---    |---| \endcode
*    Packet format: <br>
*    1) \code <STX><ADDR><DL><[Data bytes length equal DL]><CRCH><CRCL> \endcode
*
*          a) STX        - 0x02 begin packet marker. Unique byte of start packet
*          b) ADDR       - device address
*          c) DL         - data length in bytes [1..255]
*          d) CRCH       - MSB of CRC-16 that calculated from ADDR to last data byte
*          e) CRCL       - LSB of CRC-16    that calculated from ADDR to last data byte
*
*    2) If there is a byte \code <ADDR> <DL> <[Data]> or <[CRC]> \endcode that equals STX
*       then its exchanged with two byte sequence \code <ESC><STX+1>. \endcode
*
*    3) If there is a byte \code <ADDR> <DL> <[Data]> or <[CRC]> \endcode that equals ESC
*       then its exchanged with two byte sequence \code <ESC><ESC+1>. \endcode
*
*
* \defgroup group_uart_data_tsf_functions_master Master
* \defgroup group_uart_data_tsf_functions_slave Slave
*/

#if !defined(UART_slave_message_H)
    #define UART_slave_message_H


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/**
* \addtogroup group_uart_data_tsf_functions_slave
* \{
*/

/*******************************************************************************
* Function Name: UartMesSlave_Init
****************************************************************************//**
*
* Initialize UART slave protocol unit. This function starts needed components and
* initialize control status structure.
*
*
* \param uart_base 
* The pointer to the slave UART SCB instance
* \param address 
* Slave bus address
*
*******************************************************************************/
void UartMesSlave_Init(CySCB_Type* uart_base, uint8_t address);

/*******************************************************************************
* Function Name: UartMesSlave_Respond
****************************************************************************//**
*
* Start background sending of the respond
*
*
* \param txd 
*  Pointer to sent data
* \param tlen 
*  Size of sent data in bytes
*
*
* \return
*  0 - If the unit begins a response process <br>
*  1 - If not (cause IDLE or RESPOND state)
*
* \note 
*  Use UartMesSlave_State() to check this condition
*******************************************************************************/
uint8_t UartMesSlave_Respond(uint8_t * txd, uint8_t tlen);

/*******************************************************************************
* Function Name: UartMesSlave_State
****************************************************************************//**
*
* Returns state of the current slave unit
*
*
* \return
*  UM_IDLE             - unit wait for packet from master <br>
*  UM_PACKREADY        - unit receive the packet. Respond will be send <br>
*  UM_RESPOND          - unit send respond
*
*******************************************************************************/
uint8_t UartMesSlave_State(void);

/*******************************************************************************
* Function Name: UartMesSlave_GetDataSize
****************************************************************************//**
*
* Returns received data size
*
*
* \return
*  Size of the Received data in buffer
*
* \note 
*  Result valid only if unit state is UM_PACKREADY.
*  Use UartMesSlave_State() to check this condition
*******************************************************************************/
uint8_t UartMesSlave_GetDataSize(void);

/*******************************************************************************
* Function Name: UartMesSlave_GetDataPtr
****************************************************************************//**
*
* Returns pointer to received data buffer
*
*
* \return
*  Pointer to received data buffer
*
*******************************************************************************/
volatile uint8_t * UartMesSlave_GetDataPtr(void);

/*******************************************************************************
* Function Name: UartMesSlave_Msg_ISR
****************************************************************************//**
*
* Interrupt handler for UART to receive/transmit.
*
*******************************************************************************/
void UartMesSlave_Msg_ISR(void);

/** \} group_uart_data_tsf_functions_slave */


/** \cond INTERNAL */
/*******************************************************************************
* Enumerated Types and Parameters
*******************************************************************************/

#define RX_TEST_SIZE        (10u)
#define RX_BUFF_SIZE        (16u)

#define UM_IDLE             (0u)
#define UM_PACKREADY        (1u)
#define UM_RESPOND          (2u)

/** \endcond */

/** \} group_uart_data_tsf */

#endif /* End __UART_slave_message_H */


/* [] END OF FILE */

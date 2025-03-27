/*******************************************************************************
* File Name: SelfTest_CRC_calc.c
*
* Description:
* This file provides the source code for CRC32 implementation.
*
*******************************************************************************
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
*******************************************************************************/

#include "SelfTest_CRC_calc.h"


/*******************************************************************************
 * Table for CRC32 calculation
 ********************************************************************************
 * Poly:
 *  0xEDB88320
 ********************************************************************************/
static const uint32_t CRC_32_Tab[256] =
{
    0x00000000uL, 0xedb88320uL, 0x36c98560uL, 0xdb710640uL, 0x6d930ac0uL, 0x802b89e0uL,
    0x5b5a8fa0uL, 0xb6e20c80uL,
    0xdb261580uL, 0x369e96a0uL, 0xedef90e0uL, 0x005713c0uL, 0xb6b51f40uL, 0x5b0d9c60uL,
    0x807c9a20uL, 0x6dc41900uL,
    0x5bf4a820uL, 0xb64c2b00uL, 0x6d3d2d40uL, 0x8085ae60uL, 0x3667a2e0uL, 0xdbdf21c0uL,
    0x00ae2780uL, 0xed16a4a0uL,
    0x80d2bda0uL, 0x6d6a3e80uL, 0xb61b38c0uL, 0x5ba3bbe0uL, 0xed41b760uL, 0x00f93440uL,
    0xdb883200uL, 0x3630b120uL,
    0xb7e95040uL, 0x5a51d360uL, 0x8120d520uL, 0x6c985600uL, 0xda7a5a80uL, 0x37c2d9a0uL,
    0xecb3dfe0uL, 0x010b5cc0uL,
    0x6ccf45c0uL, 0x8177c6e0uL, 0x5a06c0a0uL, 0xb7be4380uL, 0x015c4f00uL, 0xece4cc20uL,
    0x3795ca60uL, 0xda2d4940uL,
    0xec1df860uL, 0x01a57b40uL, 0xdad47d00uL, 0x376cfe20uL, 0x818ef2a0uL, 0x6c367180uL,
    0xb74777c0uL, 0x5afff4e0uL,
    0x373bede0uL, 0xda836ec0uL, 0x01f26880uL, 0xec4aeba0uL, 0x5aa8e720uL, 0xb7106400uL,
    0x6c616240uL, 0x81d9e160uL,
    0x826a23a0uL, 0x6fd2a080uL, 0xb4a3a6c0uL, 0x591b25e0uL, 0xeff92960uL, 0x0241aa40uL,
    0xd930ac00uL, 0x34882f20uL,
    0x594c3620uL, 0xb4f4b500uL, 0x6f85b340uL, 0x823d3060uL, 0x34df3ce0uL, 0xd967bfc0uL,
    0x0216b980uL, 0xefae3aa0uL,
    0xd99e8b80uL, 0x342608a0uL, 0xef570ee0uL, 0x02ef8dc0uL, 0xb40d8140uL, 0x59b50260uL,
    0x82c40420uL, 0x6f7c8700uL,
    0x02b89e00uL, 0xef001d20uL, 0x34711b60uL, 0xd9c99840uL, 0x6f2b94c0uL, 0x829317e0uL,
    0x59e211a0uL, 0xb45a9280uL,
    0x358373e0uL, 0xd83bf0c0uL, 0x034af680uL, 0xeef275a0uL, 0x58107920uL, 0xb5a8fa00uL,
    0x6ed9fc40uL, 0x83617f60uL,
    0xeea56660uL, 0x031de540uL, 0xd86ce300uL, 0x35d46020uL, 0x83366ca0uL, 0x6e8eef80uL,
    0xb5ffe9c0uL, 0x58476ae0uL,
    0x6e77dbc0uL, 0x83cf58e0uL, 0x58be5ea0uL, 0xb506dd80uL, 0x03e4d100uL, 0xee5c5220uL,
    0x352d5460uL, 0xd895d740uL,
    0xb551ce40uL, 0x58e94d60uL, 0x83984b20uL, 0x6e20c800uL, 0xd8c2c480uL, 0x357a47a0uL,
    0xee0b41e0uL, 0x03b3c2c0uL,
    0xe96cc460uL, 0x04d44740uL, 0xdfa54100uL, 0x321dc220uL, 0x84ffcea0uL, 0x69474d80uL,
    0xb2364bc0uL, 0x5f8ec8e0uL,
    0x324ad1e0uL, 0xdff252c0uL, 0x04835480uL, 0xe93bd7a0uL, 0x5fd9db20uL, 0xb2615800uL,
    0x69105e40uL, 0x84a8dd60uL,
    0xb2986c40uL, 0x5f20ef60uL, 0x8451e920uL, 0x69e96a00uL, 0xdf0b6680uL, 0x32b3e5a0uL,
    0xe9c2e3e0uL, 0x047a60c0uL,
    0x69be79c0uL, 0x8406fae0uL, 0x5f77fca0uL, 0xb2cf7f80uL, 0x042d7300uL, 0xe995f020uL,
    0x32e4f660uL, 0xdf5c7540uL,
    0x5e859420uL, 0xb33d1700uL, 0x684c1140uL, 0x85f49260uL, 0x33169ee0uL, 0xdeae1dc0uL,
    0x05df1b80uL, 0xe86798a0uL,
    0x85a381a0uL, 0x681b0280uL, 0xb36a04c0uL, 0x5ed287e0uL, 0xe8308b60uL, 0x05880840uL,
    0xdef90e00uL, 0x33418d20uL,
    0x05713c00uL, 0xe8c9bf20uL, 0x33b8b960uL, 0xde003a40uL, 0x68e236c0uL, 0x855ab5e0uL,
    0x5e2bb3a0uL, 0xb3933080uL,
    0xde572980uL, 0x33efaaa0uL, 0xe89eace0uL, 0x05262fc0uL, 0xb3c42340uL, 0x5e7ca060uL,
    0x850da620uL, 0x68b52500uL,
    0x6b06e7c0uL, 0x86be64e0uL, 0x5dcf62a0uL, 0xb077e180uL, 0x0695ed00uL, 0xeb2d6e20uL,
    0x305c6860uL, 0xdde4eb40uL,
    0xb020f240uL, 0x5d987160uL, 0x86e97720uL, 0x6b51f400uL, 0xddb3f880uL, 0x300b7ba0uL,
    0xeb7a7de0uL, 0x06c2fec0uL,
    0x30f24fe0uL, 0xdd4accc0uL, 0x063bca80uL, 0xeb8349a0uL, 0x5d614520uL, 0xb0d9c600uL,
    0x6ba8c040uL, 0x86104360uL,
    0xebd45a60uL, 0x066cd940uL, 0xdd1ddf00uL, 0x30a55c20uL, 0x864750a0uL, 0x6bffd380uL,
    0xb08ed5c0uL, 0x5d3656e0uL,
    0xdcefb780uL, 0x315734a0uL, 0xea2632e0uL, 0x079eb1c0uL, 0xb17cbd40uL, 0x5cc43e60uL,
    0x87b53820uL, 0x6a0dbb00uL,
    0x07c9a200uL, 0xea712120uL, 0x31002760uL, 0xdcb8a440uL, 0x6a5aa8c0uL, 0x87e22be0uL,
    0x5c932da0uL, 0xb12bae80uL,
    0x871b1fa0uL, 0x6aa39c80uL, 0xb1d29ac0uL, 0x5c6a19e0uL, 0xea881560uL, 0x07309640uL,
    0xdc419000uL, 0x31f91320uL,
    0x5c3d0a20uL, 0xb1858900uL, 0x6af48f40uL, 0x874c0c60uL, 0x31ae00e0uL, 0xdc1683c0uL,
    0x07678580uL, 0xeadf06a0uL
};

/*******************************************************************************
* Table for CRC16 CCITT calculation
*******************************************************************************/
static const uint16_t CRC_16_Tab[256] =
{
    0x0000u, 0x1021u, 0x2042u, 0x3063u, 0x4084u, 0x50a5u, 0x60c6u, 0x70e7u,
    0x8108u, 0x9129u, 0xa14au, 0xb16bu, 0xc18cu, 0xd1adu, 0xe1ceu, 0xf1efu,
    0x1231u, 0x0210u, 0x3273u, 0x2252u, 0x52b5u, 0x4294u, 0x72f7u, 0x62d6u,
    0x9339u, 0x8318u, 0xb37bu, 0xa35au, 0xd3bdu, 0xc39cu, 0xf3ffu, 0xe3deu,
    0x2462u, 0x3443u, 0x0420u, 0x1401u, 0x64e6u, 0x74c7u, 0x44a4u, 0x5485u,
    0xa56au, 0xb54bu, 0x8528u, 0x9509u, 0xe5eeu, 0xf5cfu, 0xc5acu, 0xd58du,
    0x3653u, 0x2672u, 0x1611u, 0x0630u, 0x76d7u, 0x66f6u, 0x5695u, 0x46b4u,
    0xb75bu, 0xa77au, 0x9719u, 0x8738u, 0xf7dfu, 0xe7feu, 0xd79du, 0xc7bcu,
    0x48c4u, 0x58e5u, 0x6886u, 0x78a7u, 0x0840u, 0x1861u, 0x2802u, 0x3823u,
    0xc9ccu, 0xd9edu, 0xe98eu, 0xf9afu, 0x8948u, 0x9969u, 0xa90au, 0xb92bu,
    0x5af5u, 0x4ad4u, 0x7ab7u, 0x6a96u, 0x1a71u, 0x0a50u, 0x3a33u, 0x2a12u,
    0xdbfdu, 0xcbdcu, 0xfbbfu, 0xeb9eu, 0x9b79u, 0x8b58u, 0xbb3bu, 0xab1au,
    0x6ca6u, 0x7c87u, 0x4ce4u, 0x5cc5u, 0x2c22u, 0x3c03u, 0x0c60u, 0x1c41u,
    0xedaeu, 0xfd8fu, 0xcdecu, 0xddcdu, 0xad2au, 0xbd0bu, 0x8d68u, 0x9d49u,
    0x7e97u, 0x6eb6u, 0x5ed5u, 0x4ef4u, 0x3e13u, 0x2e32u, 0x1e51u, 0x0e70u,
    0xff9fu, 0xefbeu, 0xdfddu, 0xcffcu, 0xbf1bu, 0xaf3au, 0x9f59u, 0x8f78u,
    0x9188u, 0x81a9u, 0xb1cau, 0xa1ebu, 0xd10cu, 0xc12du, 0xf14eu, 0xe16fu,
    0x1080u, 0x00a1u, 0x30c2u, 0x20e3u, 0x5004u, 0x4025u, 0x7046u, 0x6067u,
    0x83b9u, 0x9398u, 0xa3fbu, 0xb3dau, 0xc33du, 0xd31cu, 0xe37fu, 0xf35eu,
    0x02b1u, 0x1290u, 0x22f3u, 0x32d2u, 0x4235u, 0x5214u, 0x6277u, 0x7256u,
    0xb5eau, 0xa5cbu, 0x95a8u, 0x8589u, 0xf56eu, 0xe54fu, 0xd52cu, 0xc50du,
    0x34e2u, 0x24c3u, 0x14a0u, 0x0481u, 0x7466u, 0x6447u, 0x5424u, 0x4405u,
    0xa7dbu, 0xb7fau, 0x8799u, 0x97b8u, 0xe75fu, 0xf77eu, 0xc71du, 0xd73cu,
    0x26d3u, 0x36f2u, 0x0691u, 0x16b0u, 0x6657u, 0x7676u, 0x4615u, 0x5634u,
    0xd94cu, 0xc96du, 0xf90eu, 0xe92fu, 0x99c8u, 0x89e9u, 0xb98au, 0xa9abu,
    0x5844u, 0x4865u, 0x7806u, 0x6827u, 0x18c0u, 0x08e1u, 0x3882u, 0x28a3u,
    0xcb7du, 0xdb5cu, 0xeb3fu, 0xfb1eu, 0x8bf9u, 0x9bd8u, 0xabbbu, 0xbb9au,
    0x4a75u, 0x5a54u, 0x6a37u, 0x7a16u, 0x0af1u, 0x1ad0u, 0x2ab3u, 0x3a92u,
    0xfd2eu, 0xed0fu, 0xdd6cu, 0xcd4du, 0xbdaau, 0xad8bu, 0x9de8u, 0x8dc9u,
    0x7c26u, 0x6c07u, 0x5c64u, 0x4c45u, 0x3ca2u, 0x2c83u, 0x1ce0u, 0x0cc1u,
    0xef1fu, 0xff3eu, 0xcf5du, 0xdf7cu, 0xaf9bu, 0xbfbau, 0x8fd9u, 0x9ff8u,
    0x6e17u, 0x7e36u, 0x4e55u, 0x5e74u, 0x2e93u, 0x3eb2u, 0x0ed1u, 0x1ef0u
};

/*******************************************************************************
 * Function Name: SelfTests_CRC32_Byte
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of one byte "val"
 *
 * Parameters:
 *  uint32 crc - current CRC
 *  uint8 val - value to calculate CRC
 *
 * Return:
 *  uint32 calc_crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/
uint32_t SelfTests_CRC32_Byte(uint32_t crc, uint8_t val)
{
    uint8_t tbl_idx;
    uint32_t calc_crc = crc;

    tbl_idx = (uint8_t)((calc_crc >> 24u) ^ val);
    calc_crc = CRC_32_Tab[tbl_idx] ^ (calc_crc << 8);

    return calc_crc;
}


/*******************************************************************************
 * Function Name: SelfTests_CRC32_ACC
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of area defined in parameters "BaseAdress"
 *  and "len"
 *
 * Parameters:
 *  uint32 crc - current CRC
 *  uint32 BaseAdress - Address from which CRC needs to calculate
 *  uint32 len - length of area for CRC calculation
 *
 * Return:
 *  uint32 calc_crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/
uint32_t SelfTests_CRC32_ACC(uint32_t crc, uint32_t BaseAdress, uint32_t len)
{
    uint32_t calc_crc = crc;
    uint8_t* regPointer;
    regPointer = (uint8_t*)BaseAdress;

    for (uint32_t i = 0UL; i < len; i++)
    {
        calc_crc = SelfTests_CRC32_Byte(calc_crc, *regPointer);
        regPointer++;
    }
    return calc_crc;
}


/*******************************************************************************
 * Function Name: SelfTests_CRC32
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of memory area defined in parameters
 *  "BaseAdress" and "len"
 *
 * Parameters:
 *  uint32 BaseAdress - Address from which CRC needs to calculate
 *  uint32 len - length of area for CRC calculation
 *
 * Return:
 *  uint32 calc_crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/
uint32_t SelfTests_CRC32(uint32_t BaseAdress, uint32_t len)
{
    return SelfTests_CRC32_ACC(CRC32_INIT_VALUE, BaseAdress, len);
}


/*******************************************************************************
 * Function Name: SelfTests_CRC16_CCITT_Byte
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of one byte "val"
 *
 * Parameters:
 *  uint16 crc - current CRC
 *  uint8 val - Address from which CRC needs to calculate
 *
 * Return:
 *  uint16 crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/
uint16_t SelfTests_CRC16_CCITT_Byte(uint16_t crc, uint8_t val)
{
    return (uint16_t)(crc << 8u) ^ CRC_16_Tab[((crc >> 8u) ^ val)];
}


/*******************************************************************************
 * Function Name: SelfTests_CRC16_CCITT_ACC
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of area defined in parameters "BaseAdress"
 *  and "len"
 *
 * Parameters:
 *  uint16 crc - current CRC
 *  uint32 BaseAdress - Address from which CRC needs to calculate
 *  uint16 len - length of area for CRC calculation
 *
 * Return:
 *  uint16 crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/

uint16_t SelfTests_CRC16_CCITT_ACC(uint16_t crc, uint32_t BaseAdress, uint16_t len)
{
    uint16_t calc_crc = crc;
    uint8_t* regPointer;
    regPointer = (uint8_t*)BaseAdress;

    for (uint32_t i = 0UL; i < len; i++)
    {
        calc_crc = SelfTests_CRC16_CCITT_Byte(calc_crc, *regPointer);
        regPointer++;
    }

    return calc_crc;
}


/*******************************************************************************
 * Function Name: SelfTests_CRC16_CCITT
 *******************************************************************************
 *
 * Summary:
 *  This function calculates CRC of memory area defined in parameters
 *  "BaseAdress" and "len"
 *
 * Parameters:
 *  uint32 BaseAdress - Address from which CRC needs to calculate
 *  uint16 len - length of area for CRC calculation
 *
 * Return:
 *  uint16 crc - calculated CRC value
 *
 * Note:
 *
 ******************************************************************************/
uint16_t SelfTests_CRC16_CCITT(uint32_t BaseAdress, uint16_t len)
{
    return SelfTests_CRC16_CCITT_ACC(CRC16_CCITT_INIT_VALUE, BaseAdress, len);
}


/* [] END OF FILE */

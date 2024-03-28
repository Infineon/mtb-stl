/*******************************************************************************
* File Name: SelfTest_IO.c
* Version 1.0.0
*
* Description:
*  This file provides the source code for I/O self tests for CAT2(PSoC4), CAT1A, 
*  CAT1C devices.
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

#include "SelfTest_IO.h"
#include "SelfTest_ErrorInjection.h"
#include "SelfTest_Config.h"

/* This variable used to return number of pin, which cause error in test */
static uint8_t errorPinNum = 0u;

/* Table of constants which defines pins that should be tested             */
/* PintToTest[0] represents PORT0, PintToTest[1] -> PORT1 ... etc.         */
/* Each pin is represented by the corresponding bit in PintToTest    table */
/* PIN 0 is represented by the LSB and PIN 7 by MSB                        */
/* If pin should be tested, a corresponding bit should be set to "1"       */

#if CY_CPU_CORTEX_M0P
#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)
static const uint8_t PinToTest[] =
{
    /* Below mask is based on the project setting and CY8CKIT-41S MAX kit hardware */
    0x00u,            /* PORT0 mask */
    0x7Cu,            /* PORT1 mask */
    0x0Fu,            /* PORT2 mask */
    0x00u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
    0x00u,            /* PORT5 mask */
    0x0Fu,            /* PORT6 mask */
    0x34u,            /* PORT7 mask */
    0x0Fu,            /* PORT8 mask */
    0x00u,            /* PORT9 mask */
    0x3Fu,            /* PORT10 mask */
    0x00u,            /* PORT11 mask */
    0x02u,            /* PORT12 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2,  GPIO_PRT3, \
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6,  GPIO_PRT7, \
    GPIO_PRT8, GPIO_PRT9, GPIO_PRT10, GPIO_PRT11, \
    GPIO_PRT12
};
#endif // if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)

#if defined(CY_DEVICE_SERIES_PSOC_4500S)
static const uint8_t PinToTest[] =
{
    /* Below mask is based on the project setting and CY8CKIT-45S MAX kit hardware */
    0x06u,            /* PORT0 mask */
    0x38u,            /* PORT1 mask */
    0x4Fu,            /* PORT2 mask */
    0x13u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
    0xE3u,            /* PORT5 mask */
    0x16u,            /* PORT6 mask */
    0x00u,            /* PORT7 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6, GPIO_PRT7,
};
#endif // if defined(CY_DEVICE_SERIES_PSOC_4500S)

#if  defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS) && (CY_FLASH_SIZE == 0x00040000UL)
static const uint8_t PinToTest[] =
{
    /* Below mask is based on the project setting and CY8CKIT-45S MAX kit hardware */
    0x06u,            /* PORT0 mask */
    0x38u,            /* PORT1 mask */
    0x4Fu,            /* PORT2 mask */
    0x10u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
    0xE3u,            /* PORT5 mask */
    0x16u,            /* PORT6 mask */
    0x00u,            /* PORT7 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6, GPIO_PRT7,
};
#endif // if  defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS) && (CY_FLASH_SIZE == 0x00040000UL)

#if  defined(CY_DEVICE_SERIES_PSOC_4100S)
static const uint8_t PinToTest[] =
{
    /* Below mask is based on the project setting and CY8CKIT-45S MAX kit hardware */
    0xCCu,            /* PORT0 mask */
    0x1Fu,            /* PORT1 mask */
    0xC2u,            /* PORT2 mask */
    0xF0u,            /* PORT3 mask */
    0x0Cu,            /* PORT4 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4,
};
#endif // if  defined(CY_DEVICE_SERIES_PSOC_4100S)


#elif CY_CPU_CORTEX_M4
static const uint8_t PinToTest[] =
    {
        /* Below mask is based on the project setting and CY8CKIT-45S MAX kit hardware */
        0x00u, /* PORT0 mask */
        0x00u, /* PORT1 mask */
        0x00u, /* PORT2 mask */
        0x00u, /* PORT3 mask */
        0x00u, /* PORT4 mask */
        0x80u, /* PORT5 mask */
        0x00u, /* PORT6 mask */
        0x00u, /* PORT7 mask */
        0x00u, /* PORT8 mask */
        0x00u, /* PORT9 mask */
        0x00u, /* PORT10 mask */
        0x00u, /* PORT11 mask */
        0x00u, /* PORT12 mask */
        0x00u, /* PORT13 mask */
        0x00u, /* PORT14 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type *PORT_Regs[] =
    {
        GPIO_PRT0,
        GPIO_PRT1,
        GPIO_PRT2,
        GPIO_PRT3,
        GPIO_PRT4,
        GPIO_PRT5,
        GPIO_PRT6,
        GPIO_PRT7,
        GPIO_PRT8,
        GPIO_PRT9,
        GPIO_PRT10,
        GPIO_PRT11,
        GPIO_PRT12,
        GPIO_PRT13,
        GPIO_PRT14,
};


#elif CY_CPU_CORTEX_M7
static const uint8_t PinToTest[] =
    {
        /* Below mask is based on the project setting and CY8CKIT-45S MAX kit hardware */
        0x00u, /* PORT0 mask */
        0x00u, /* PORT1 mask */
        0x00u, /* PORT2 mask */
        0x00u, /* PORT3 mask */
        0x00u, /* PORT4 mask */
        0x10u, /* PORT5 mask */
        0x00u, /* PORT6 mask */
        0x00u, /* PORT7 mask */
        0x00u, /* PORT8 mask */
        0x00u, /* PORT9 mask */
        0x00u, /* PORT10 mask */
        0x00u, /* PORT11 mask */
        0x00u, /* PORT12 mask */
        0x00u, /* PORT13 mask */
        0x00u, /* PORT14 mask */
        0x00u, /* PORT15 mask */
        0x00u, /* PORT16 mask */
        0x00u, /* PORT17 mask */
        0x00u, /* PORT18 mask */
        0x00u, /* PORT19 mask */
        0x00u, /* PORT20 mask */
        0x00u, /* PORT21 mask */
        0x00u, /* PORT22 mask */
        0x00u, /* PORT23 mask */
        0x00u, /* PORT24 mask */
        0x00u, /* PORT25 mask */
        0x00u, /* PORT26 mask */
        0x00u, /* PORT27 mask */
        0x00u, /* PORT28 mask */
        0x00u, /* PORT29 mask */
        0x00u, /* PORT30 mask */
        0x00u, /* PORT31 mask */
        0x00u, /* PORT32 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type *PORT_Regs[] =
    {
        GPIO_PRT0,
        GPIO_PRT1,
        GPIO_PRT2,
        GPIO_PRT3,
        GPIO_PRT4,
        GPIO_PRT5,
        GPIO_PRT6,
        GPIO_PRT7,
        GPIO_PRT8,
        GPIO_PRT9,
        GPIO_PRT10,
        GPIO_PRT11,
        GPIO_PRT12,
        GPIO_PRT13,
        GPIO_PRT14,
        GPIO_PRT15,
        GPIO_PRT16,
        GPIO_PRT17,
        GPIO_PRT18,
        GPIO_PRT19,
        GPIO_PRT20,
        GPIO_PRT21,
        GPIO_PRT22,
        GPIO_PRT23,
        GPIO_PRT24,
        GPIO_PRT25,
        GPIO_PRT26,
        GPIO_PRT27,
        GPIO_PRT28,
        GPIO_PRT29,
        GPIO_PRT30,
        GPIO_PRT31,
        GPIO_PRT32,
};

#endif
/*******************************************************************************
 * Function Name: SelfTest_IO_GetPortError
 ********************************************************************************
 *
 * Summary:
 *  This function returns a PORT number that cause an error in SelfTest_IO function.
 *
 * Parameters:
 *  None
 *
 * Return: PORT number that cause an error
 *
 **********************************************************************************/
uint8_t SelfTest_IO_GetPortError(void)
{
    return (errorPinNum / IO_PINS);
}


/*******************************************************************************
 * Function Name: SelfTest_IO_GetPinError
 ********************************************************************************
 *
 * Summary:
 *  This function returns a PIN number that cause an error in SelfTest_IO function.
 *
 * Parameters:
 *  None
 *
 * Return: PIN number that cause an error
 *
 **********************************************************************************/
uint8_t SelfTest_IO_GetPinError(void)
{
    return (errorPinNum & IO_PINS_MASK);
}


/*******************************************************************************
 * Function Name: SelfTest_IO
 ********************************************************************************
 *
 * Summary:
 *  This function performs I/O tests to detect pin shorts to Ground or Vcc.
 *  Not all pins maybe compatible with this test based on applications specifics.
 *  The users must fill up the "PintToTest" table corresponding to their needs.
 *
 * Parameters:
 *  None
 *
 * Return:
 *  Result of test:  "0" - pass test;
 *                   "1" - fail test (Shorts to VCC);
 *                   "2" - fail test (Shorts to GND);
 *
 **********************************************************************************/
uint8_t SelfTest_IO(void)
{
    /* Function result */
    uint8_t ret = OK_STATUS;

    /* Ports index */
    uint8_t portNum;

    /* Pin index */
    uint8_t pinNum;

    /* Variable to save PORT data register */
    uint32_t savePortDR;

    /* Variable to save PORT config register */
    uint32_t savePortPC;

    /* Disable global interrupts */
    __disable_irq();

    /* Run through all ports */
    for (portNum = 0u; ((portNum < IO_PORTS) && (ret == OK_STATUS)); portNum++)
    {
        /* Save PORT state */
#if CY_CPU_CORTEX_M0P
        savePortDR = (GPIO_PRT_DR(PORT_Regs[portNum]));
        savePortPC = (GPIO_PRT_PC(PORT_Regs[portNum]));
#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7)
        savePortDR = (GPIO_PRT_OUT(PORT_Regs[portNum]));
        savePortPC = (GPIO_PRT_CFG(PORT_Regs[portNum]));
#endif
        /* Run through all pins of current port */
        for (pinNum = 0u; ((pinNum < IO_PINS) && (ret == OK_STATUS)); pinNum++)
        {
            /* If pin should be tested */
            if ((PinToTest[portNum] & (uint8_t)(1u << pinNum)) != 0u)
            {
                /* Set pin mode to resistive pull-up */
                Cy_GPIO_SetDrivemode(PORT_Regs[portNum], pinNum, CY_GPIO_DM_PULLUP);

                /* Set pin to "1" */
                Cy_GPIO_Set(PORT_Regs[portNum], pinNum);

                #if (ERROR_IN_DIGITAL_IO == 1u)

                /* Set pin to "0" and produce error */
                Cy_GPIO_Clr(PORT_Regs[portNum], pinNum);
                #endif /* End (ERROR_IN_DIGITAL_IO == 1u) */

                /* Wait for applying the drive mode */
                Cy_SysLib_DelayCycles(DELAY_DRIVE_MODE_SETUP);

                /* Test if pin is not connected to GND */
                if (Cy_GPIO_Read(PORT_Regs[portNum], pinNum) == 0x00u)
                {
                    /* Test fail */
                    ret = SHORT_TO_GND;
                    errorPinNum = (portNum * 8u) + pinNum;
                }

                /* Set pin mode to resistive pull-down */
                Cy_GPIO_SetDrivemode(PORT_Regs[portNum], pinNum, CY_GPIO_DM_PULLDOWN);

                /* Set pin to "0" */
                Cy_GPIO_Clr(PORT_Regs[portNum], pinNum);

                /* Wait for applying the drive mode (critical delay) */
                Cy_SysLib_DelayCycles(DELAY_DRIVE_MODE_SETUP);

                /* Test if pin is not connected to VCC */
                if (Cy_GPIO_Read(PORT_Regs[portNum], pinNum) == 0x01u)
                {
                    /* Test fail */
                    ret = SHORT_TO_VCC;
                    errorPinNum = (portNum * 8u) + pinNum;

                    break;
                }
            }
        }

        /* Restore PORT state */
#if CY_CPU_CORTEX_M0P
        GPIO_PRT_DR(PORT_Regs[portNum]) = savePortDR;
        GPIO_PRT_PC(PORT_Regs[portNum]) = savePortPC;
#elif (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7)
        GPIO_PRT_OUT(PORT_Regs[portNum]) = savePortDR;
        GPIO_PRT_CFG(PORT_Regs[portNum]) = savePortPC;
#endif

    }

    /* Enable global interrupts */
    __enable_irq();

    return ret;
}

/* [] END OF FILE */

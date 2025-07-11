/*******************************************************************************
* File Name: SelfTest_IO.c
*
* Description:
*  This file provides the source code for the I/O self tests.
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
#include "cy_pdl.h"

#include "SelfTest_IO.h"
#include "SelfTest_ErrorInjection.h"

/* This variable is used to return the number of the pin, which causes an error in the test */
static uint8_t errorPinNum = 0u;
static const uint8_t* pinMask = NULL;

/* The table of constants, which defines the pins to be tested             */
/* PintToTest[0] represents PORT0, PintToTest[1] -> PORT1 ... etc.         */
/* Each pin is represented by the corresponding bit in PintToTest    table */
/* PIN 0 is represented by the LSB and PIN 7 by MSB                        */
/* If the pin should be tested, set a corresponding bit to "1".            */

#if CY_CPU_CORTEX_M0P
#if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-41S MAX kit hardware. */
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
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4100S_MAX) */

#if defined(CY_DEVICE_SERIES_PSOC_4500S)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-45S MAX kit hardware. */
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
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4500S) */

#if defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-149 kit hardware. */
    0x09u,            /* PORT0 mask */
    0x28u,            /* PORT1 mask */
    0x5Au,            /* PORT2 mask */
    0x60u,            /* PORT3 mask */
    0x80u,            /* PORT4 mask */
    0x48u,            /* PORT5 mask */
    0x00u,            /* PORT6 mask */
    0x00u,            /* PORT7 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6, GPIO_PRT7,
};
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4100S_PLUS) */

#if  defined(CY_DEVICE_SERIES_PSOC_4100S)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-041-41XX kit hardware. */
    0x4Fu,            /* PORT0 mask */
    0xBFu,            /* PORT1 mask */
    0xAFu,            /* PORT2 mask */
    0xC0u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4,
};
#endif /* if  defined(CY_DEVICE_SERIES_PSOC_4100S) */

#if defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CPROTO-041TP kit hardware. */
    0x00u,            /* PORT0 mask */
    0x00u,            /* PORT1 mask */
    0x7Fu,            /* PORT2 mask */
    0x02u,            /* PORT3 mask */
    0x07u,            /* PORT4 mask */
    0x00u,            /* PORT5 mask */
    0x04u,            /* PORT6 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6,
};
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4100T_PLUS) */

#if defined(CY_DEVICE_SERIES_PSOC_4000T)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CPROTO-040T kit hardware. */
    0x1Fu,            /* PORT0 mask */
    0x00u,            /* PORT1 mask */
    0x33u,            /* PORT2 mask */
    0x00u,            /* PORT3 mask */
    0x03u,            /* PORT4 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4,
};
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4000T) */

#if defined(CY_DEVICE_SERIES_PSOC_4000S)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-145-40XX kit hardware. */
    0x00u,            /* PORT0 mask */
    0x04u,            /* PORT1 mask */
    0x80u,            /* PORT2 mask */
    0x80u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4,
};
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4000S) */

#if defined(CY_DEVICE_SERIES_PSOC_4700S)
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-148 kit hardware. */
    0x00u,            /* PORT0 mask */
    0x00u,            /* PORT1 mask */
    0xF0u,            /* PORT2 mask */
    0x30u,            /* PORT3 mask */
    0x00u,            /* PORT4 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
{
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3, \
    GPIO_PRT4,
};
#endif /* if defined(CY_DEVICE_SERIES_PSOC_4700S) */

#elif CY_CPU_CORTEX_M4
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-45S MAX kit hardware. */
    0x00u,     /* PORT0 mask */
    0x00u,     /* PORT1 mask */
    0x00u,     /* PORT2 mask */
    0x00u,     /* PORT3 mask */
    0x00u,     /* PORT4 mask */
    0x80u,     /* PORT5 mask */
    0x00u,     /* PORT6 mask */
    0x00u,     /* PORT7 mask */
    0x00u,     /* PORT8 mask */
    0x00u,     /* PORT9 mask */
    0x00u,     /* PORT10 mask */
    0x00u,     /* PORT11 mask */
    0x00u,     /* PORT12 mask */
    0x00u,     /* PORT13 mask */
    0x00u,     /* PORT14 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
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
    /* The below mask is based on the project setting and CY8CKIT-45S MAX kit hardware. */
    0x00u,     /* PORT0 mask */
    0x00u,     /* PORT1 mask */
    0x00u,     /* PORT2 mask */
    0x00u,     /* PORT3 mask */
    0x00u,     /* PORT4 mask */
    0x10u,     /* PORT5 mask */
    0x00u,     /* PORT6 mask */
    0x00u,     /* PORT7 mask */
    0x00u,     /* PORT8 mask */
    0x00u,     /* PORT9 mask */
    0x00u,     /* PORT10 mask */
    0x00u,     /* PORT11 mask */
    0x00u,     /* PORT12 mask */
    0x00u,     /* PORT13 mask */
    0x00u,     /* PORT14 mask */
    0x00u,     /* PORT15 mask */
    0x00u,     /* PORT16 mask */
    0x00u,     /* PORT17 mask */
    0x00u,     /* PORT18 mask */
    0x00u,     /* PORT19 mask */
    0x00u,     /* PORT20 mask */
    0x00u,     /* PORT21 mask */
    0x00u,     /* PORT22 mask */
    0x00u,     /* PORT23 mask */
    0x00u,     /* PORT24 mask */
    0x00u,     /* PORT25 mask */
    0x00u,     /* PORT26 mask */
    0x00u,     /* PORT27 mask */
    0x00u,     /* PORT28 mask */
    0x00u,     /* PORT29 mask */
    0x00u,     /* PORT30 mask */
    0x00u,     /* PORT31 mask */
    0x00u,     /* PORT32 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
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
#elif CY_CPU_CORTEX_M33
static const uint8_t PinToTest[] =
{
    /* The below mask is based on the project setting and CY8CKIT-45S MAX kit hardware. */
    0x00u,     /* PORT0 mask */
    0x00u,     /* PORT1 mask */
    0x00u,     /* PORT2 mask */
    0x08u,     /* PORT3 mask */
    0x00u,     /* PORT4 mask */
    0x00u,     /* PORT5 mask */
    0x00u,     /* PORT6 mask */
    0x00u,     /* PORT7 mask */
    0x00u,     /* PORT8 mask */
    0x00u,     /* PORT9 mask */
};

/* IO ports register addresses */
static GPIO_PRT_Type* PORT_Regs[] =
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
};

#endif /* if CY_CPU_CORTEX_M0P */
/*******************************************************************************
 * Function Name: SelfTest_IO_GetPortError
 ********************************************************************************
 *
 * Summary:
 *  This function returns a PORT number that causes an error in the SelfTest_IO function.
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
 *  This function returns a PIN number that causes an error in the SelfTest_IO function.
 *
 * Parameters:
 *  None
 *
 * Return: The PIN number that causes an error.
 *
 **********************************************************************************/
uint8_t SelfTest_IO_GetPinError(void)
{
    return (errorPinNum & IO_PINS_MASK);
}


/*******************************************************************************
 * Function Name: SelfTest_IO_SetPinMask
 ********************************************************************************
 *
 * Summary:
 *  This function sets a custom pin mask to be used in the SelfTest_IO function.
 *
 * Parameters:
 *  pinMaskArr - The custom pin mask array. The length of the array must be equal
 *  to the IO_PORTS value. Each element of the array is a mask of the GPIO port pins,
 *  that will be tested in the SelfTest_IO function. The Port sequence is the same
 *  as in the PORT_Regs array. Pass the NULL value to use the default PIN mask.
 *
 **********************************************************************************/
void SelfTest_IO_SetPinMask(const uint8_t* pinMaskArr)
{
    pinMask = pinMaskArr;
}


/*******************************************************************************
 * Function Name: SelfTest_IO
 ********************************************************************************
 *
 * Summary:
 *  This function performs I/O tests to detect pin shorts to Ground or Vcc.
 *  Not all pins may be compatible with this test based on the applications specifics.
 *  By default, this function uses the "PinToTest" array to determine which pins
 *  to test. To set a custom pin mask, use the SelfTest_IO_SetPinMask() function.
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
    uint8_t ret = OK_STATUS;
    uint8_t portNum;
    uint8_t pinNum;

    /* Variable to save PORT data and config registers */
    uint32_t savePortDR;
    uint32_t savePortPC;

    const uint8_t* pinToTestPtr = PinToTest;

    /* Use custom pin mask if set */
    if (pinMask != NULL)
    {
        pinToTestPtr = pinMask;
    }

    /* Disable the global interrupts */
    __disable_irq();

    /* Run through all ports */
    portNum = 0u;
    while ((portNum < IO_PORTS) && (ret == OK_STATUS))
    {
        /* Save PORT state */
        #if defined(CY_IP_M0S8IOSS)
        savePortDR = (GPIO_PRT_DR(PORT_Regs[portNum]));
        savePortPC = (GPIO_PRT_PC(PORT_Regs[portNum]));
        #elif (defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS40IOSS) || defined (CY_IP_MXS22IOSS))
        savePortDR = (GPIO_PRT_OUT(PORT_Regs[portNum]));
        savePortPC = (GPIO_PRT_CFG(PORT_Regs[portNum]));
        #endif
        /* Run through all pins of current port */
        pinNum = 0u;
        while ((pinNum < IO_PINS) && (ret == OK_STATUS))
        {
            /* If a pin should be tested */
            if ((pinToTestPtr[portNum] & (uint8_t)(1u << pinNum)) != 0u)
            {
                /* Set pin mode to resistive pull-up */
                Cy_GPIO_SetDrivemode(PORT_Regs[portNum], pinNum, CY_GPIO_DM_PULLUP);

                /* Set pin to "1" */
                Cy_GPIO_Set(PORT_Regs[portNum], pinNum);

                #if (ERROR_IN_DIGITAL_IO == 1u)

                /* Set pin to "0" and produce error */
                Cy_GPIO_Clr(PORT_Regs[portNum], pinNum);
                #endif /* End (ERROR_IN_DIGITAL_IO == 1u) */

                /* Wait for applying Drive mode */
                Cy_SysLib_DelayCycles(DELAY_DRIVE_MODE_SETUP);

                /* Test if pin is not connected to GND */
                if (Cy_GPIO_Read(PORT_Regs[portNum], pinNum) == 0x00u)
                {
                    /* Test fail */
                    ret = SHORT_TO_GND;
                    errorPinNum = (portNum * 8u) + pinNum;
                    continue;
                }

                /* Set pin mode to resistive pull-down */
                Cy_GPIO_SetDrivemode(PORT_Regs[portNum], pinNum, CY_GPIO_DM_PULLDOWN);

                /* Set pin to "0" */
                Cy_GPIO_Clr(PORT_Regs[portNum], pinNum);

                /* Wait for applying Drive mode (critical delay) */
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
            pinNum++;
        }

        /* Restore PORT state */
        #if defined(CY_IP_M0S8IOSS)
        GPIO_PRT_DR(PORT_Regs[portNum]) = savePortDR;
        GPIO_PRT_PC(PORT_Regs[portNum]) = savePortPC;
        #elif (defined (CY_IP_MXS40SIOSS) || defined (CY_IP_MXS40IOSS) || defined (CY_IP_MXS22IOSS))
        GPIO_PRT_OUT(PORT_Regs[portNum]) = savePortDR;
        GPIO_PRT_CFG(PORT_Regs[portNum]) = savePortPC;
        #endif
        portNum++;
    }

    /* Enable the global interrupts */
    __enable_irq();

    return ret;
}


/* [] END OF FILE */

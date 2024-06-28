/* *****************************************************************************
* File Name: SelfTest_RAM.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to the API for runtime SRAM self tests for
*  CAT2(PSoC4), CAT1A, CAT1C devices.
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
#include "SelfTest_RAM.h"
#include "SelfTest_Config.h"

#if defined(__ICCARM__)
/* variable to set start address for test Stack block */
volatile uint32_t test_Stack_Addr;
/* variable to set start address for test RAM block */
volatile uint32_t test_SRAM_Addr;
#endif

/*******************************************************************************
 * Function Name: SelfTests_Init_March_SRAM_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  uint8_t shift - set shift from start address for March RAM test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_March_SRAM_Test(uint8_t shift)
{
    /* Set base address for March RAM test */
    March_Test_Init(shift);
}

/*******************************************************************************
 * Function Name: SelfTests_Init_GALPAT_SRAM_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  uint8_t shift - set shift from start address for GALPAT RAM test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_GALPAT_SRAM_Test(uint8_t shift)
{
    /* Set base address for March RAM test */
    GALPAT_Test_Init(shift);
}


/*******************************************************************************
 * Function Name: SelfTests_SRAM_March
 ********************************************************************************
 *
 * Summary:
 *  This function perform SRAM self test using March method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "1" - fail test; "2" - still testing; "3" - Test complete OK
 *
 **********************************************************************************/
uint8_t SelfTests_SRAM_March(void)
{
    uint8_t test_Status;

    /* Disable global interrupts */
    __disable_irq();

    /* Perform March test */
    test_Status = March_Test_SRAM();

    /* Enable global interrupts */
    __enable_irq();

    return test_Status;
}

/*******************************************************************************
 * Function Name: SelfTests_SRAM_GALPAT
 ********************************************************************************
 *
 * Summary:
 *  This function perform SRAM self test using GALPAT method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "1" - fail test; "2" - still testing; "3" - Test complete OK
 *
 **********************************************************************************/
uint8_t SelfTests_SRAM_GALPAT(void)
{
    uint8_t test_Status;

    /* Disable global interrupts */
    __disable_irq();

    /* Perform GALPAT test */
    test_Status = GALPAT_Test_SRAM();

    /* Enable global interrupts */
    __enable_irq();

    return test_Status;
}


/*******************************************************************************
 * Function Name: SelfTests_Init_March_Stack_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  uint8_t shift - set shift from start address for March Stack test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_March_Stack_Test(uint8_t shift)
{
    /* Set base address for March Stack test */
    March_Test_Stack_Init(shift);
}

/*******************************************************************************
 * Function Name: SelfTests_Init_GALPAT_Stack_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  uint8_t shift - set shift from start address for March Stack test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_GALPAT_Stack_Test(uint8_t shift)
{
    /* Set base address for March Stack test */
    GALPAT_Test_Stack_Init(shift);
}


/*******************************************************************************
 * Function Name: SelfTests_Stack_March
 ********************************************************************************
 *
 * Summary:
 *  This function perform Stack RAM self test using March method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "1" - fail test; "2" - still testing; "3" - Test complete OK
 *
 **********************************************************************************/
uint8_t SelfTests_Stack_March(void)
{
    uint8_t test_Status;

    /* Disable global interrupts */
    __disable_irq();

    /* Perform March test */
    test_Status = March_Test_Stack();

    /* Enable global interrupts */
    __enable_irq();

    return test_Status;
}

/*******************************************************************************
 * Function Name: SelfTests_Stack_GALPAT
 ********************************************************************************
 *
 * Summary:
 *  This function perform Stack RAM self test using GALPAT method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "1" - fail test; "2" - still testing; "3" - Test complete OK
 *
 **********************************************************************************/
uint8_t SelfTests_Stack_GALPAT(void)
{
    uint8_t test_Status;

    /* Disable global interrupts */
    __disable_irq();

    /* Perform GALPAT test */
    test_Status = GALPAT_Test_Stack();

    /* Enable global interrupts */
    __enable_irq();

    return test_Status;
}
/* [] END OF FILE */

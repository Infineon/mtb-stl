/*******************************************************************************
* File Name: SelfTest_RAM.h
*
* Description:
*  This file provides constants and parameter values used for SRAM
*  self tests.
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

/**
* \addtogroup group_ram
* \{
*
* To meet Class B requirement, SRAM test must be checked for “DC fault”. Either of the 2
* methods can be used:
*
*      1) March/Checkerboard:  This test is destructive. The RAM area is written
*                           with  alternate “0” and “1” to memory, and verifies if the
*                           writen data is right by reading back.
*      2) GALPAT:              This test is destructive. This test initilizes the chosen
*                           range of memory uniformly (i.e. all 0 s or all 1 s). The first
*                           memory cell to be tested is then inverted and all the remaining
*                           cells are inspected to ensure that their contents are correct.
*                           After every read access to one of the remaining cells, the inverted
*                           cell is also checked. This procedure is repeated for each cell
*                           in the chosen memory range. A second run is carried out with the
*                           opposite initialisation. Any difference produces a failure message.
*                           It can detect stuck-at faults and direct coupling faults.
*
* \note
* GALPAT test is time consuming. This tests can be implemented at startup procedure
* to test entire SRAM area
*
*
* \defgroup group_sram_enums Enumerated Types 
* \defgroup group_sram_functions SRAM
* \defgroup group_sram_stack_functions STACK
*/

#if !defined(SELFTEST_RAM_H)
    #define SELFTEST_RAM_H
#include "SelfTest_common.h"

/***************************************
*       Enumerations
***************************************/
/**
* \addtogroup group_sram_enums
* \{
*/

/** SRAM test algorithm type selection enumeration */

typedef enum
{
    SRAM_MARCH_TEST_MODE = 0,  /**< Selects March test algorithm for sram */
    SRAM_GALPAT_TEST_MODE = 1  /**< Selects Galpat test algorithm for sram */
}stl_sram_test_mode_t;

/** \} group_sram_enums */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_sram_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_SRAM
****************************************************************************//**
*
*  This function perform self test on a block of RAM. It can detect stuck-at faults and direct coupling faults. 
*  The RAM tests are destructible 
*  in nature so function also accepts temporary buffer which can be used to store/restore
*  memory block under test. The RAM block under test must not overlap with memory that
*  is not supposed be overwritten for e.g. stack. This kind of situation must be tested
*  at the startup.  Because of RAM destructible behavior, it is advisable to disable
*  interrupts before starting the test.
*
* \param type
* The type of RAM test to be run
* \param startAddr
* The pointer to start of the RAM block to be tested
* \param size
* The size of RAM block to be tested
* \param buffAddr
* The pointer to start of the buffer to be used to store/restore data of RAM block to be tested. 
* If NULL, test would be performed without store/restore being performed.
* \param buffSize 
* The size of buffer which is used to store/restore. If buffSize is smaller than size,
* the test internally loops through the RAM block operating on memory size
* equal to buffSize in each iteration.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/

uint8_t SelfTest_SRAM(stl_sram_test_mode_t type, uint8_t *startAddr, uint32_t size, uint8_t *buffAddr, uint32_t buffSize);

/** \} group_sram_functions */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_sram_stack_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_SRAM_Stack
****************************************************************************//**
*
*  This function perform self test on Stack. It can detect stuck-at faults and direct coupling faults. 
*  The RAM tests are destructible 
*  in nature so this function copies the stack in use to the provided RAM area. Once the test
*  is completed, stack is copied back to its original location.
*  Because of RAM destructible behavior, it is advisable to disable
*  interrupts before starting the test.
*
* \param stackBase
* The pointer to the Stack Base
* \param stackSize
* The size of Stack
* \param altStackBase
* The pointer to start of the RAM area to be used as alternate Stack Base. RAM address 
* from altStackBase to (altStackBase - stackSize) is used to store/restore the Stack under test. 
* The existing content of this area will be destructed and this area must not overlap with the Stack 
* under test. The platform might set stack limit using __set_MSPLIM, ensure that 
* the alternate stack is within the stack limit set by the platform
* 
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/

uint8_t SelfTest_SRAM_Stack(uint8_t *stackBase, uint32_t stackSize, uint8_t *altStackBase);

/** \} group_sram_stack_functions */

/** \} group_ram */

#endif /* SELFTEST_RAM_H */

/* [] END OF FILE */

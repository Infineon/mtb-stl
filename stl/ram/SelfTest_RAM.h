/*******************************************************************************
* File Name: SelfTest_RAM.h
*
* Description:
*  This file provides constants and parameter values used for the SRAM
*  self tests.
*
*******************************************************************************
* (c) 2020-2026, Infineon Technologies AG, or an affiliate of Infineon
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
*******************************************************************************/

/**
 * \addtogroup group_ram
 * \{
 *
 * To meet the Class B requirement, SRAM test must be checked for DC fault. Either of the two
 * tests (both are destructive) can be used:
 *
 *      1) March/Checkerboard: The RAM area is written
 *                             with  alternate “0” and “1” to memory, and verifies if the
 *                             written data is correct by reading back.
 *      2) GALPAT:             This test initilizes the chosen
 *                             range of memory uniformly (i.e. all 0 s or all 1 s). The first
 *                             memory cell to be tested is then inverted and all the remaining
 *                             cells are inspected for the correct contents.
 *                             After every read access to one of the remaining cells, the inverted
 *                             cell is also checked. This procedure is repeated for each cell
 *                             in the chosen memory range. A second run is done with the
 *                             opposite initialisation. Any difference produces a failure message.
 *                             It can detect stuck-at faults and direct coupling faults.
 *
 * \note
 * GALPAT test is time consuming. These tests can be implemented at the startup procedure
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
} stl_sram_test_mode_t;

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
*  This function performs the self test on a block of RAM. It can detect stuck-at faults and direct
*  coupling faults. The RAM tests are destructible in nature, so the function also accepts a temporary
*  buffer that can be used to store/restore memory block under test. The RAM block under test
*  must not overlap with memory that is not supposed to be overwritten for e.g. stack. This kind of
*  situation must be tested at the startup.  Because of RAM destructible behavior, it is advisable
*  to disable the interrupts before starting the test.
*
* \param type
* The type of RAM test to run.
* \param startAddr
* The pointer to the start of the RAM block to test.
* \param size
* The size of RAM block to test.
* \param buffAddr
* The pointer to the start of the buffer to use for store/restore data of RAM block to test.
* If NULL, test would be performed without store/restore being performed.
* \param buffSize
* The size of the buffer, which is used for store/restore. If buffSize is smaller than the size,
* the test internally loops through the RAM block operating on memory size
* equal to buffSize in each iteration.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/

uint8_t SelfTest_SRAM(stl_sram_test_mode_t type, uint8_t* startAddr, uint32_t size,
                      uint8_t* buffAddr, uint32_t buffSize);

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
*  This function performs the self test on Stack. It can detect stuck-at faults and direct coupling
*  faults. The RAM tests are destructible in nature, so this function copies the stack in use to
*  the provided RAM area. Once the test is completed, the stack is copied back to its original
*  location. Because of the RAM destructible behavior, it is advisable to disable
*  the interrupts before starting the test.
*
* \param stackBase
* The pointer to the Stack Base.
* \param stackSize
* The size of Stack.
* \param altStackBase
* The pointer to the start of the RAM area to use as an alternate Stack Base. RAM address
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

uint8_t SelfTest_SRAM_Stack(uint8_t* stackBase, uint32_t stackSize, uint8_t* altStackBase);

/** \} group_sram_stack_functions */

/** \} group_ram */

#endif /* SELFTEST_RAM_H */

/* [] END OF FILE */

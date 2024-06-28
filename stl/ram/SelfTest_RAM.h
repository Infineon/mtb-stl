/*******************************************************************************
* File Name: SelfTest_RAM.h
* Version 1.0.0
*
* Description:
*  This file provides constants and parameter values used for SRAM
*  self tests for CAT2(PSoC4), CAT1A, CAT1C devices.
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

/**
* \addtogroup group_ram
* \{
*
* To meet Class B requirement, SRAM test must be checked for “DC fault”. Either 
* of the 2 methods can be used:
// \verbatim
  1) March/Checkerboard : This test first duplicates data in the area to be tested
                          as this test is destructive. Then the RAM area is written
                          with  alternate “0” and “1” to memory, and verifies if the
                          writen data is right by reading back. At the end of this test
                          the original data is restored to its correct location.
                    
  2) GALPAT             : This test first duplicates data in the area to be tested
                          as this test is destructive.This test initilizes the chosen 
                          range of memory uniformly (i.e. all 0 s or all 1 s). The first
                          memory cell to be tested is then inverted and all the remaining 
                          cells are inspected to ensure that their contents are correct. 
                          After every read access to one of the remaining cells, the inverted
                          cell is also checked. This procedure is repeated for each cell 
                          in the chosen memory range. A second run is carried out with the
                          opposite initialisation. Any difference produces a failure message.
                          It can detect stuck-at faults and direct coupling faults. At the 
                          end of this test the original data is restored to its correct 
                          location in memory. 
\endverbatim
*
*\note Update CY_SRAM_BASE, CY_SRAM_SIZE, CY_STACK_SIZE according to the device being
* tested.
* 
* \note GALPAT test is time consuming.This tests can be implemented at startup procedure
* to test entire SRAM area.
* 
*
*
* \section group_ram_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_ram_macros Macros
* \defgroup group_march_functions MARCH
* \defgroup group_galpat_functions GALPAT
*/

#include "SelfTest_common.h"


#if !defined(SELFTEST_RAM_H)
    #define SELFTEST_RAM_H

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_march_functions
* \{
*/

/*******************************************************************************
* Function Name: March_Test_Stack_Init
****************************************************************************//**
*
* This function initializes the test STACK base address. 
*
*
* \param shift 
* Function set to variable "Test_Stack_Addr" address of first STACK byte for test. 
* "Test_Stack_Addr" = MARCH_GALPAT_STACK_BASE + R0. R0 uses to set shift of first byte for test.
*
*
* \return
*  Void
*
*******************************************************************************/
extern void March_Test_Stack_Init(uint8_t shift);


/*******************************************************************************
* Function Name: March_Test_Stack
****************************************************************************//**
*
*  This function perform Stack RAM self test using March method. This function
*  first duplicates data in the area to be tested as this test is destructive. 
*  This API tests Stack area with Checkerboard arithmetic which writes alternate
*  “0” and “1” to memory, and verifies if the writen data is right by reading back.
*  It can detect stuck-at faults and direct coupling faults.    
*  At the end of this test the original data is restored to its correct location
*  in memory.  
*
*
*
*
* \return
*   "1" - Test Failed                                          
*   "2" - Pass, but still testing status                        
*   "3" - pass and complete status 
*
*******************************************************************************/
extern uint8_t March_Test_Stack(void);

/*******************************************************************************
* Function Name: March_Test_Init
****************************************************************************//**
*
* This function initializes the SRAM base address. 
*
* \param shift
*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. 
*   "Test_SRAM_Addr" = MARCH_GAPAT_SRAM_BASE + R0. R0 uses to set shift of first         
*   byte for test.
*
* \return
* Void 
*
*******************************************************************************/
extern void March_Test_Init(uint8_t shift);


/*******************************************************************************
* Function Name: March_Test_SRAM
****************************************************************************//**
*
*  This function perform block of RAM self test using March method. This function
*  first duplicates data in the area to be tested as this test is destructive.
*  This API tests Stack area with Checkerboard arithmetic which writes alternate
*  “0” and “1” to memory, and verifies if the writen data is right by reading back.
*  It can detect stuck-at faults and direct coupling faults.      
*  At the end of this test the original data is restored to its correct location
*  in memory. 
*
*
*
* \return
*   "1" - Test Failed                                        
*   "2" - Pass, but still testing status                          
*   "3" - pass and complete status  
*
*******************************************************************************/
extern uint8_t March_Test_SRAM(void);

/*******************************************************************************
* Function Name: SelfTests_Init_March_SRAM_Test
****************************************************************************//**
*
*  This function calls March_Test_Init
*
* \param shift
*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. 
*   "Test_SRAM_Addr" = MARCH_SRAM_BASE + R0. R0 uses to set shift of first         
*   byte for test.
*
* \return
*  Void 
*
*******************************************************************************/
void SelfTests_Init_March_SRAM_Test(uint8_t shift);


/*******************************************************************************
* Function Name: SelfTests_SRAM_March
****************************************************************************//**
*
*  This function perform block of RAM self test using March method. This function calls
*  March_Test_SRAM.
*
*
*
* \return
*   "1" - Test Failed                                         
*   "2" - Pass, but still testing status                          
*   "3" - pass and complete status  
*
*******************************************************************************/
uint8_t SelfTests_SRAM_March(void);


/*******************************************************************************
* Function Name: SelfTests_Init_March_SRAM_Test
****************************************************************************//**
*
*  This function calls March_Test_Stack_Init
*
* \param shift
* Function set to variable "Test_Stack_Addr" address of first STACK byte for test. 
* "Test_Stack_Addr" = MARCH_STACK_BASE + R0. R0 uses to set shift of first byte for test.
*
* \return
*  Void 
*
*******************************************************************************/
void SelfTests_Init_March_Stack_Test(uint8_t shift);


/*******************************************************************************
* Function Name: SelfTests_Stack_March
****************************************************************************//**
*
*  This function perform Stack RAM self test using March method. This function
*  calls March_Test_Stack.
*
*
*
* \return
*   "1" - Test Failed                                         
*   "2" - Pass, but still testing status                       
*   "3" - pass and complete status 
*
*******************************************************************************/
uint8_t SelfTests_Stack_March(void);

/** \} group_march_functions */


/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_galpat_functions
* \{
*/
/*******************************************************************************
* Function Name: GALPAT_Test_Stack_Init
****************************************************************************//**
*
* This function initializes the test STACK base address. 
*
*
* \param shift 
* Function set to variable "Test_Stack_Addr" address of first STACK byte for test. 
* "Test_Stack_Addr" = MARCH_GALPAT_STACK_BASE + R0. R0 uses to set shift of first byte for test.
*
*
* \return
*  Void
*
*******************************************************************************/
extern void GALPAT_Test_Stack_Init(uint8_t shift);

/*******************************************************************************
* Function Name: GALPAT_Test_Stack
****************************************************************************//**
*
*  This function perform Stack RAM self test using GALPAT method. This function
*  initilizes the chosen range of memory uniformly (i.e. all 0 s or all 1 s). 
*  The first memory cell to be tested is then inverted and all the remaining 
*  cells are inspected to ensure that their contents are correct. After every 
*  read access to one of the remaining cells, the inverted cell is also checked.
*  This procedure is repeated for each cell in the chosen memory range. A second
*  run is carried out with the opposite initialisation. Any difference produces a
*  failure message.
*  It can detect stuck-at faults and direct coupling faults.    
*  At the end of this test the original data is restored to its correct location
*  in memory.  
*
*
*
*
* \return
*   "1" - Test Failed                                          
*   "2" - Pass, but still testing status                        
*   "3" - pass and complete status 
*
*******************************************************************************/
extern uint8_t GALPAT_Test_Stack(void);



/*******************************************************************************
* Function Name: GALPAT_Test_Init
****************************************************************************//**
*
* This function initializes the SRAM base address. 
*
* \param shift
*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. 
*   "Test_SRAM_Addr" = MARCH_GAPAT_SRAM_BASE + R0. R0 uses to set shift of first         
*   byte for test.
*
* \return
* Void 
*
*******************************************************************************/
extern void GALPAT_Test_Init(uint8_t shift);



/*******************************************************************************
* Function Name: GALPAT_Test_SRAM
****************************************************************************//**
*
*  This function perform block of RAM self test using GALPAT method. This function
*  first duplicates data in the area to be tested as this test is destructive.
*  This API tests Stack area with Checkerboard arithmetic which writes alternate
*  “0” and “1” to memory, and verifies if the writen data is right by reading back.
*  It can detect stuck-at faults and direct coupling faults.      
*  At the end of this test the original data is restored to its correct location
*  in memory. 
*
*
*
* \return
*   "1" - Test Failed                                        
*   "2" - Pass, but still testing status                          
*   "3" - pass and complete status  
*
*******************************************************************************/
extern uint8_t GALPAT_Test_SRAM(void);

/*******************************************************************************
* Function Name: SelfTests_Init_GALPAT_SRAM_Test
****************************************************************************//**
*
*  This function calls GALPAT_Test_Init
*
* \param shift
*   Function set to variable "Test_SRAM_Addr" address of first SRAM byte for test. 
*   "Test_SRAM_Addr" = GALPAT_SRAM_BASE + R0. R0 uses to set shift of first         
*   byte for test.
*
* \return
*  Void 
*
*******************************************************************************/
void SelfTests_Init_GALPAT_SRAM_Test(uint8_t shift);

/*******************************************************************************
* Function Name: SelfTests_SRAM_GALPAT
****************************************************************************//**
*
*  This function perform block of RAM self test using GALPAT method. This function calls
*  GALPAT_Test_SRAM.
*
*
*
* \return
*   "1" - Test Failed                                         
*   "2" - Pass, but still testing status                          
*   "3" - pass and complete status  
*
*******************************************************************************/
uint8_t SelfTests_SRAM_GALPAT(void);

/*******************************************************************************
* Function Name: SelfTests_Init_GALPAT_Stack_Test
****************************************************************************//**
*
*  This function calls GALPAT_Test_Stack_Init
*
* \param shift
* Function set to variable "Test_Stack_Addr" address of first STACK byte for test. 
* "Test_Stack_Addr" = GALPAT_STACK_BASE + R0. R0 uses to set shift of first byte for test.
*
* \return
*  Void 
*
*******************************************************************************/
void SelfTests_Init_GALPAT_Stack_Test(uint8_t shift);

/*******************************************************************************
* Function Name: SelfTests_Stack_GALPAT
****************************************************************************//**
*
*  This function perform Stack RAM self test using GALPAT method. This function
*  calls GALPAT_Test_Stack.
*
*
*
* \return
*   "1" - Test Failed                                         
*   "2" - Pass, but still testing status                       
*   "3" - pass and complete status 
*
*******************************************************************************/
uint8_t SelfTests_Stack_GALPAT(void);

/** \} group_galpat_functions */


/** \cond INTERNAL */
/***************************************
* External Variables
***************************************/
#if defined(__ICCARM__)
/* used by IAR SRAM march Assembly code */
/* variable to set start address for test Stack block */
extern volatile uint32_t test_Stack_Addr;
/* variable to set start address for test RAM block */
extern volatile uint32_t test_SRAM_Addr;
#endif
/***************************************
* Initial Parameter Constants
***************************************/
//#define CHECKERBOARD_PATTERN_55        (0x55u)
//#define CHECKERBOARD_PATTERN_AA        (0xAAu)

/* This is a duplicate define of TEST_BLOCK_SIZE defined in asm files */
/* SelfTest_SRAM_March.s */
#define TEST_BLOCK_SIZE                (0x0400u)

/* Number of blocks on which SRAM is divided */
#define NUMBERS_OF_TEST_CYCLES         (uint16)(CY_SRAM_SIZE  / TEST_BLOCK_SIZE)


/** \endcond */

/** \} group_ram */

#endif /* SELFTEST_RAM_H */

/* [] END OF FILE */

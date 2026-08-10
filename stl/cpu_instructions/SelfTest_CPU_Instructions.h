/*******************************************************************************
* File Name: SelfTest_CPU_Instructions.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values used
*  for CPU instruction self tests.
*
*******************************************************************************
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
*******************************************************************************/


/**
 * \defgroup group_cpu_instructions CPU instructions (CPU Instructions STL module)
 * \{
 *
 * \brief
 *  The CPU instruction self-test verifies correct operation of core CPU/ALU instructions,
 *  such as arithmetic, logic, shift, and bit manipulation instructions. The test executes
 *  a sequence of representative instructions and checks their results against expected values.
 *  If any instruction produces an unexpected result, the test fails.
 *
 *
 * \section group_cpu_instructions_more_information More Information
 *
 * The self-test covers the following CPU instructions:
 * - ADD, SUB: Arithmetic addition and subtraction
 * - AND, ORR, EOR: Bitwise logical operations (AND, OR, XOR)
 * - MOV, MVN: Move and bitwise NOT
 * - LSL, LSR, ASR: Logical/arithmetic shift left/right
 * - CMP, CMN: Compare and compare negative
 * - ADC, SBC: Add with carry, subtract with carry
 * - ROR: Rotate right
 * - BIC: Bit clear
 * - REV: Byte order reversal
 * - CLZ: Count leading zeros
 *
 * \note The CLZ (Count Leading Zeros) instruction is not supported in Thumb code; only Thumb-2 cores support it.
 *       The test for CLZ is only executed if the core supports it.
 *
 * \defgroup group_cpu_instructions_functions Functions
 */

#if !defined(SELFTEST_CPU_INSTRUCTIONS_H)
#define SELFTEST_CPU_INSTRUCTIONS_H
#include "SelfTest_common.h"
#include <cycfg.h>

/***************************************
* Function Prototypes
***************************************/

/**
 * \addtogroup group_cpu_instructions_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_CPU_Instructions
****************************************************************************//**
*
*  This function performs the checkerboard test for all CPU instructions.
*
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTest_CPU_Instructions(void);

/** \} group_cpu_instructions */
/** \cond INTERNAL */
/***************************************
* Function Prototypes
***************************************/
uint8_t SelfTest_CPU_Instructions_GCC(void);
uint8_t SelfTest_CPU_Instructions_IAR(void);
uint8_t SelfTest_CPU_Instructions_ARM(void);
/** \endcond */

#endif /* End SELFTEST_CPU_INSTRUCTIONS_H */

/** \} group_cpu_instructions */

/* [] END OF FILE */

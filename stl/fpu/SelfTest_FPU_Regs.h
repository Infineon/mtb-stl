/*******************************************************************************
* File Name: SelfTest_FPU_Regs.h
* Version 1.0.0
*
* Description:
* This file provides constants and parameter values used for FPU register self
* tests for CAT1A and CAT1C devices.
*
*
* Hardware Dependency:
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
* \addtogroup group_fpu
* \{
*
* The FPU registers test detects stuck-at faults in the FPU by using the checkerboard test.
*
* \section group_fpu_more_information More Information
*
* This test ensures that the bits in the registers are not stuck at value '0' or '1'. It is a destructive test that performs the following major tasks:
// \verbatim
 1) The registers are tested by performing a write/read/compare test sequence using a checkerboard pattern (0x5555 5555, then 0xaaaa aaaaa). 
    These binary sequences are valid floating point values.
 2) The test returns an error code if the returned values do not match.
\endverbatim
*
* \section group_fpu_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_fpu_macros Macros
* \defgroup group_fpu_functions Functions
*/

#include "SelfTest_common.h"

#if (CY_CPU_CORTEX_M4 || CY_CPU_CORTEX_M7)

#if !defined(SELFTEST_FPU_ASM_H)
#define SELFTEST_FPU_ASM_H

#include "cy_pdl.h"

/** \cond INTERNAL */
/***************************************
* Function Prototypes
***************************************/
#if defined(__GNUC__)
uint8_t SelfTest_FPU_Regs_GCC(void);
#elif defined(__ICCARM__)
uint8_t SelfTest_FPU_Regs_IAR(void);
#elif defined(__CC_ARM)
uint8_t SelfTest_FPU_Regs_MDK(void);
#endif /* End (__GNUC__) ||  (__CC_ARM) */
/** \endcond */


/**
* \addtogroup group_fpu_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_FPU_Registers
****************************************************************************//**
*
* This function performs checkerboard test for all FPU registers (S0-S31).
* \note
* Only applicable for CAT1A and CAT1C devices.
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
*
*******************************************************************************/
uint8_t SelfTest_FPU_Registers(void);
/** \} group_fpu_functions */

#endif 

/** \} group_fpu */

#endif /* End SELFTEST_CPU_H */
/* [] END OF FILE */

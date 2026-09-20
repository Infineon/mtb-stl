/*******************************************************************************
* File Name: SelfTest_FPU_Regs.h
*
* Description:
* This file provides constants and parameter values used for FPU register self
* tests.
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
 * \defgroup group_fpu FPU (FPU STL module)
 * \{
 *
 * The FPU registers test detects stuck-at faults in the FPU by using the checkerboard test.
 *
 * \section group_fpu_more_information More Information
 *
 * This test ensures that the bits in the registers are not stuck at value '0' or '1'. The
 * test performs the following major tasks:
 *
 *      1) The registers are tested by performing a write/read/compare test sequence using a
 *         checkerboard pattern (0x5555 5555, then 0xaaaa aaaaa). These binary sequences are
 *         valid floating point values.
 *      2) The test returns an error code if the returned values do not match.
 *
 *
 * \defgroup group_fpu_functions Functions
 */

#if !defined(SELFTEST_FPU_REGS_H)
#define SELFTEST_FPU_REGS_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if ((defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) || defined (CY_DOXYGEN))

/** \cond INTERNAL */
/***************************************
* Function Prototypes
***************************************/
#if defined(__ARMCC_VERSION)
uint8_t SelfTest_FPU_Regs_ARM(void);
#elif defined(__GNUC__)
uint8_t SelfTest_FPU_Regs_GCC(void);
#elif defined(__ICCARM__)
uint8_t SelfTest_FPU_Regs_IAR(void);
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
* This function performs the checkerboard test for all FPU registers (S0-S31).
* \note
* Applicable only for PSOC 61 Programmable Line, PSOC 62 Performance Line, PSOC Control C3,
* XMC7000 and XMC5000 devices.
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
*******************************************************************************/
uint8_t SelfTest_FPU_Registers(void);
/** \} group_fpu_functions */

#endif /* if ((defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) || defined (CY_DOXYGEN)) */

/** \} group_fpu */

#endif /* End SELFTEST_CPU_H */
/* [] END OF FILE */

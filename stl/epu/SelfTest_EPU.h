/*******************************************************************************
* File Name: SelfTest_EPU.h
*
* Description:
*  This file provides the constants and parameter values for the EPU
*  (Event Processing Unit) self tests.
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
 * \defgroup group_epu EPU (EPU STL module)
 * \{
 *
 * The EPU (Event Processing Unit) self-test validates that the processing unit
 * signal path within the PPCA subsystem is functional. The test generates a
 * software event on each PU under test (in bypass mode), routes it through a
 * COMBO combiner to the EPU IRQ Generator, and verifies that an interrupt
 * status is asserted back on the Main CPU.
 *
 * \section group_epu_more_information More Information
 *
 * The test procedure for each PU in the specified range:
 *
 *      1) Save the original PU, COMBO, and IRQ configurations.
 *      2) Configure the PU in asynchronous bypass mode.
 *      3) Enable SW event generation on the PU.
 *      4) Configure COMBO[0] to route the PU output as EVOUT[0].
 *      5) Configure EPU_IRQ[0] to use COMBO[0] output as interrupt source.
 *      6) Clear any pending interrupt, then generate a SW event (constant input = 1).
 *      7) Verify that the interrupt status is asserted.
 *      8) Clear interrupt, stop SW event, and restore original configurations.
 *
 *
 * \defgroup group_epu_macros Macros
 * \defgroup group_epu_functions Functions
 */

#if !defined(SELFTEST_EPU_H)
    #define SELFTEST_EPU_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN))

/***************************************
* Function Prototypes
***************************************/

/**
 * \addtogroup group_epu_macros
 * \{
 */

/** PU type: Processing Unit Type 1 (full-featured: masking/blanking/debouncing/delay) */
#define EPU_PU_TYPE_T1              (0u)

/** PU type: Processing Unit Type 2 (simplified: bypass/polarity/edge) */
#define EPU_PU_TYPE_T2              (1u)

/* The PDL/device header (cyip_ppca.h) does not expose explicit count macros
 * for the PU_T1 / PU_T2 arrays, only the validation helpers
 * CY_IS_PUT1_INDEX_VALID / CY_IS_PUT2_INDEX_VALID (hard-coded to 31u/15u).
 * To make the self-test automatically track the hardware definition, the
 * maximum indices are derived from the size of the PU_Tx_CNFG[] register
 * arrays in PPCA_EPU_EPU_Type, which is generated from the SVD. */

/** Maximum PU_T1 index (derived from PPCA_EPU_EPU_Type::PU_T1_CNFG[] size) */
#define EPU_PU_T1_MAX_INDEX         \
    ((uint8_t)((sizeof(((PPCA_EPU_EPU_Type *)0)->PU_T1_CNFG) / \
                sizeof(((PPCA_EPU_EPU_Type *)0)->PU_T1_CNFG[0])) - 1u))

/** Maximum PU_T2 index (derived from PPCA_EPU_EPU_Type::PU_T2_CNFG[] size) */
#define EPU_PU_T2_MAX_INDEX         \
    ((uint8_t)((sizeof(((PPCA_EPU_EPU_Type *)0)->PU_T2_CNFG) / \
                sizeof(((PPCA_EPU_EPU_Type *)0)->PU_T2_CNFG[0])) - 1u))

/** \} group_epu_macros */

/**
 * \addtogroup group_epu_functions
 * \{
 */

/*******************************************************************************
* Function Name: SelfTest_EPU_PU
****************************************************************************//**
*
* Performs a self-test of the EPU processing unit signal path by generating
* a software event on each PU in the specified range and verifying that
* the resulting trigger arrives as an interrupt via the COMBO and IRQ path.
*
* The COMBO combiners (123 COMBO, 8 COMBO_FB) and EPU IRQ Generator are
* tested transitively through the PU signal path.
*
* \note The application must enable the PPCA_EPU IP block (e.g. via
* Cy_PPCA_EPU_Enable(PPCA_EPU)) before calling this function. The library
* does not toggle PPCA_EPU.CTRL to avoid affecting application-level power
* and clock policy.
*
* \param type
* PU sub-type to test: EPU_PU_TYPE_T1 (0) or EPU_PU_TYPE_T2 (1).
*
* \param puInitial
* First PU index to test (inclusive). <br>
* PU_T1 range: 0..31, PU_T2 range: 0..15.
*
* \param puEnd
* Last PU index to test (inclusive). <br>
* PU_T1 range: 0..31, PU_T2 range: 0..15.
*
* \return
*  \ref OK_STATUS                    (0) - Test passed <br>
*  \ref ERROR_STATUS                 (1) - HW self-test failed <br>
*  \ref ERROR_INCORRECT_USAGE_STATUS (4) - Invalid input parameters
*                                     (no HW access performed) <br>
*
*******************************************************************************/
uint8_t SelfTest_EPU_PU(uint8_t type, uint8_t puInitial, uint8_t puEnd);

/** \} group_epu_functions */

#endif /* (defined(CY_IP_MXS40PPSS) || defined(CY_DOXYGEN)) */

#endif /* !defined(SELFTEST_EPU_H) */

/** \} group_epu */
/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_IPC.h
* Version 1.0.0
*
* Description:
* This file provides constants and parameter values used for IPC self
* tests.
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
* \addtogroup group_ipc
* \{
*
* This module performs check on each free IPC channel with all free IPC interrupts.
// \verbatim
Following steps are performed:
  1) Acquire the IPC channel.
  2) Write some message.
  3) Acquire notification event to a specify IPC interrupt.
  4) Read the written data in ISR.
  5) Release an specific IPC channel from the locked state.
\endverbatim
*
* \section group_ipc_more_information More Information
*
// \verbatim
For PSoC6 following channels and interrupts are checked:
 Free Channels    : 8 - 15
 Free Interrupts : 7 - 15
 
For XMC Dual CM7 device channels and interrupts are checked:
 Free Channels   : 4 - 7
 Free Interrupts : 1 - 7
\endverbatim
*
* \section group_ipc_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_ipc_macros Macros
* \defgroup group_ipc_functions Functions
*/


#if !defined(SELFTEST_IPC_H)
#define SELFTEST_IPC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"

#if ((defined(CY_CPU_CORTEX_M4) && (CY_CPU_CORTEX_M4)) || (defined(CY_CPU_CORTEX_M7) && (CY_CPU_CORTEX_M7)) || (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)))
/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_ipc_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_IPC
****************************************************************************//**
*
* This function performs check on each free IPC channel with all free IPC interrupts.
*
*
*
*
*
* \return
*  0 - Test passed <br>
*  1 - Test failed
* 
*
*******************************************************************************/
uint8_t SelfTest_IPC(void);

/** \} group_ipc_functions */

/** \cond INTERNAL */
/***************************************
* Initial Parameter Constants
***************************************/
#define INT0_INDEX              0
#define INT1_INDEX              1
#define INT2_INDEX              2
#define INT3_INDEX              3
#define INT4_INDEX              4
#define INT5_INDEX              5
#define INT6_INDEX              6
#define INT7_INDEX              7
#define INT8_INDEX              8
#define INT9_INDEX              9
#define INT10_INDEX             10
#define INT11_INDEX             11
#define INT12_INDEX             12
#define INT13_INDEX             13
#define INT14_INDEX             14
#define INT15_INDEX             15



#define CH0_INDEX               0
#define CH1_INDEX               1
#define CH2_INDEX               2
#define CH3_INDEX               3
#define CH4_INDEX               4
#define CH5_INDEX               5
#define CH6_INDEX               6
#define CH7_INDEX               7
#define CH8_INDEX               8
#define CH9_INDEX               9
#define CH10_INDEX              10
#define CH11_INDEX              11
#define CH12_INDEX              12
#define CH13_INDEX              13
#define CH14_INDEX              14
#define CH15_INDEX              15


#define IPC_CH_0                CH0_INDEX
#define IPC_CH_1                CH1_INDEX
#define IPC_CH_2                CH2_INDEX
#define IPC_CH_3                CH3_INDEX
#define IPC_CH_4                CH4_INDEX
#define IPC_CH_5                CH5_INDEX
#define IPC_CH_6                CH6_INDEX
#define IPC_CH_7                CH7_INDEX
#define IPC_CH_8                CH8_INDEX
#define IPC_CH_9                CH9_INDEX
#define IPC_CH_10               CH10_INDEX
#define IPC_CH_11               CH11_INDEX
#define IPC_CH_12               CH12_INDEX
#define IPC_CH_13               CH13_INDEX
#define IPC_CH_14               CH14_INDEX
#define IPC_CH_15               CH15_INDEX


#define IPC_INT_0               INT0_INDEX 
#define IPC_INT_1               INT1_INDEX 
#define IPC_INT_2               INT2_INDEX 
#define IPC_INT_3               INT3_INDEX
#define IPC_INT_4               INT4_INDEX
#define IPC_INT_5               INT5_INDEX
#define IPC_INT_6               INT6_INDEX
#define IPC_INT_7               INT7_INDEX
#define IPC_INT_8               INT8_INDEX
#define IPC_INT_9               INT9_INDEX 
#define IPC_INT_10              INT10_INDEX
#define IPC_INT_11              INT11_INDEX
#define IPC_INT_12              INT12_INDEX
#define IPC_INT_13              INT13_INDEX
#define IPC_INT_14              INT14_INDEX
#define IPC_INT_15              INT15_INDEX


#define IPC_INT_NOTIFY_MASK(i)   ((uint32_t)0x1U << (uint32_t)i)

#define IPC_INT_REL_MASK(i)      ((uint32_t)0x1U << (uint32_t)i)

#define GET_IPC_CH_REL_MASK(i)   ((uint32_t)0x1U << (uint32_t)i) 

#define GET_IPC_CH_NOTIFY_MASK(i)         ((uint32_t)0x1U << (uint32_t)(i))



#define IPC_PRIORITY           (1u)      /* IPC1 interrupt priority */

#if CY_CPU_CORTEX_M33
#define IPC1_INTERRUPT          cpuss_interrupts_ipc_dpslp_0_IRQn
#define IPC2_INTERRUPT          cpuss_interrupts_ipc_dpslp_1_IRQn
#else
#define IPC1_INTERRUPT          cpuss_interrupts_ipc_1_IRQn 
#define IPC2_INTERRUPT          cpuss_interrupts_ipc_2_IRQn
#define IPC3_INTERRUPT          cpuss_interrupts_ipc_3_IRQn
#define IPC4_INTERRUPT          cpuss_interrupts_ipc_4_IRQn
#define IPC5_INTERRUPT          cpuss_interrupts_ipc_5_IRQn
#define IPC6_INTERRUPT          cpuss_interrupts_ipc_6_IRQn
#define IPC7_INTERRUPT          cpuss_interrupts_ipc_7_IRQn 
#define IPC8_INTERRUPT          cpuss_interrupts_ipc_8_IRQn
#define IPC9_INTERRUPT          cpuss_interrupts_ipc_9_IRQn
#define IPC10_INTERRUPT         cpuss_interrupts_ipc_10_IRQn
#define IPC11_INTERRUPT         cpuss_interrupts_ipc_11_IRQn
#define IPC12_INTERRUPT         cpuss_interrupts_ipc_12_IRQn
#define IPC13_INTERRUPT         cpuss_interrupts_ipc_13_IRQn
#define IPC14_INTERRUPT         cpuss_interrupts_ipc_14_IRQn
#define IPC15_INTERRUPT         cpuss_interrupts_ipc_15_IRQn
#endif


#endif

/** \endcond */
/** \} group_ipc */
#endif /* End SELFTEST_IPC_H */
/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_Motif.h
* Version 1.0.0
*
* Description:
*
*  This file provides constants and parameter values used for Motif
*  self tests.
*
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
* \addtogroup group_motif
* \{
*
* To meet Class B requirement, Motif must be tested for Multi channel pattern mode in conjunction with Hall sensor mode.
\verbatim
1.Multi channel pattern mode in conjunction with Hall sensor mode are performed using Motif module.
2.First initialize the MOTIF module in Hall sensor mode and input PWMs
3.HALL lookup table(HLUT) and MCP lookup table(MLUT) are initialized in Device Configurator.
4.Checks the difference between output modulation values from MOTIF and MCP values.
\endverbatim
*
* 
*
*
* \section group_motif_profile_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td>1.00</td>
*     <td>Initial Version.</td>
*     <td>Initial Version.</td>
*   </tr>
* </table>
*
* \defgroup group_motif_structure Data structure
* \defgroup group_motif_functions Functions
*/

#if !defined(MOTIF_H)
    #define MOTIF_H
#include "SelfTest_common.h"

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)) || defined (CY_DOXYGEN)

/***************************************
* Data Structure
***************************************/
/**
* \addtogroup group_motif_structure
* \{
*/

/** MOTIF configuration structure */

typedef struct stl_motif_tcpwm_config
{
    TCPWM_Type* Hall_0_base;  /**< Hall Input 0 TCPWM Base address */
    uint32_t    Hall_0_Num ;  /**< Hall Input 0 TCPWM counter number */
    cy_stc_tcpwm_pwm_config_t const * Hall_0_in_config;  /**< Hall Input 0 TCPWM config structure */
    TCPWM_Type* Hall_1_base;  /**< Hall Input 1 TCPWM Base address */
    uint32_t    Hall_1_Num ;  /**< Hall Input 1 TCPWM counter number */
    cy_stc_tcpwm_pwm_config_t const * Hall_1_in_config;  /**< Hall Input 1 TCPWM config structure */
    TCPWM_Type* Hall_2_base;  /**< Hall Input 2 TCPWM Base address */
    uint32_t    Hall_2_Num ;  /**< Hall Input 2 TCPWM counter number */
    cy_stc_tcpwm_pwm_config_t const * Hall_2_in_config;  /**< Hall Input 2 TCPWM config structure */
}stl_motif_tcpwm_config_t;

/** \} group_motif_structure */

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_motif_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_Motif_Init
****************************************************************************//**
*
*  This function perform self test on Motif module.
*  The MOTIF unit is a flexible and powerful component for motor control systems
*  that use Rotary Encoders, Hall Sensors as feedback loop.
*  This API initializes the MOTIF module in Hall sensor mode and
*  input PWMs which simulates HALL sensor output
*
* \return
*   void                         
*
*******************************************************************************/

void SelfTest_Motif_Init(TCPWM_MOTIF_GRP_MOTIF_Type *base ,cy_stc_tcpwm_motif_hall_sensor_config_t const *config, stl_motif_tcpwm_config_t *input_config);

/*******************************************************************************
* Function Name: SelfTest_Motif_Start
****************************************************************************//**
*
*  This function starts the Motif module and Modulation output from motif
*  is started with existing values. It reports the status of the interrupts
*  and clears the triggered interrupts.
*  It compares the defined Multi-Channel Mode Pattern with output modulation value, returns
*  the result OK_STATUS if values match or returns ERROR_STATUS
*  if the values do not match.
*
* \return
*   "0" "OK_STATUS" - Test passed                                        
*   "1" "ERROR_STATUS" - Test Failed                          
*
*******************************************************************************/

uint8_t SelfTest_Motif_Start(TCPWM_MOTIF_GRP_MOTIF_Type *base , stl_motif_tcpwm_config_t *input_config);

/** \} group_motif_functions */

#endif
/** \} group_motif */



#endif /* MOTIF_H */

/* [] END OF FILE */


/*******************************************************************************
* File Name: SelfTest_Motif.h
*
* Description:
*  This file provides constants and parameter values used for Motif
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
* \addtogroup group_motif
* \{
*
* MOTIF self test implements the Quadrature Decoder mode and validate the
* functionality.
*
* \defgroup group_motif_structure Data structure
* \defgroup group_motif_functions Functions
*/

#if !defined(MOTIF_H)
    #define MOTIF_H
#include "SelfTest_common.h"

#if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)) || defined (CY_DOXYGEN)
/***************************************
* MACROs
***************************************/
/** \cond INTERNAL */
#define EMU_SIG_NUM (3U)
/** \endcond */

/***************************************
* Data Structure
***************************************/
/** \addtogroup group_motif_structure
 * \{
 */
/** TCPWM configuration sub-structure */
typedef struct
{
    uint32_t                          idx; /**< PWM counter index */
    cy_stc_tcpwm_pwm_config_t const * cfg; /**< PWM configuration structure */

} stl_motif_tcpwm_cfg_t;

/** TCPWM counter configuration sub-structure */
typedef struct
{
    uint32_t                              idx;    /**< TCPWM counter index */
    cy_stc_tcpwm_counter_config_t const * cfg;    /**< TCPWM counter configuration structure */

} stl_motif_tcpwm_counter_cfg_t;

/** MOTIF self test configuration structure */

typedef struct
{
    /*MOTIF Module configuration structure*/
    TCPWM_MOTIF_GRP_MOTIF_Type * motif_base; /**< MOTIF base */
    cy_stc_tcpwm_motif_quaddec_config_t const * motif_config; /**< MOTIF config */

    /*TCPWM configuration to capture q-clk ticks*/
    TCPWM_Type * qclk_base;                    /**< TCPWM counter base */
    stl_motif_tcpwm_counter_cfg_t qclk;        /**< TCPWM counter configuration */

    /*TCPWM counter configuration to emulate input signals*/
    TCPWM_Type * sgen_base[EMU_SIG_NUM];       /**< PWM base */
    stl_motif_tcpwm_cfg_t sgen[EMU_SIG_NUM];   /**< PWM configuration */

    uint32_t ref_count;                        /**< Reference tcpwm count */
    uint32_t margin_count;                     /**< Allowed margin */
    uint32_t delay;                            /**< Delay */

}stl_motif_cfg_handle_t;

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
*  Initialize the Motif self test configuration. Initialization includes
*  - Generate the emulated signals for Phase-A, Phase-B and Index using TCPWM.
*  These signals are input to the MOTIF module.
*  - Configure the TCPWM counter to capture the Q-CLK resolution.
*  - Configure the MOTIF module.
* \param hPtr
* Pointer to the motif self test configuration handler.
*
*******************************************************************************/
void SelfTest_Motif_Init(stl_motif_cfg_handle_t *hPtr);

/*******************************************************************************
* Function Name: SelfTest_Motif_Start
****************************************************************************//**
*
*  This function starts the Motif and capture the count between two quadrature
*  clock edges.
*  Captured value compares with the reference value and returns
*  the result.
*
* \param hPtr
* Pointer to the motif self test configuration handler.
*
* \return
*  "0" "OK_STATUS" - Test passed <br>
*  "1" "ERROR_STATUS" - Test failed
*
*******************************************************************************/

uint8_t SelfTest_Motif_Start(stl_motif_cfg_handle_t *hPtr);


/** \} group_motif_functions */

#endif
/** \} group_motif */

#endif /* MOTIF_H */

/* [] END OF FILE */
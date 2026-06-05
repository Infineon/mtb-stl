/*******************************************************************************
* File Name: SelfTest_Motif.h
*
* Description:
*  This file provides constants and parameter values used for the Motif
*  self tests.
*
*******************************************************************************
* (c) 2020-2025, Infineon Technologies AG, or an affiliate of Infineon
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
 * \addtogroup group_motif
 * \{
 *
 * The MOTIF self test implements the Quadrature Decoder mode and validates the
 * functionality.
 *
 * \defgroup group_motif_structure Data structure
 * \defgroup group_motif_functions Functions
 */

#if !defined(MOTIF_H)
    #define MOTIF_H
#include "SelfTest_common.h"

#if (defined (CY_IP_MXS40TCPWM) || defined (CY_DOXYGEN))
/***************************************
* MACROs
***************************************/
/** \cond INTERNAL */
#define EMU_SIG_NUM (3U)
/** \endcond */

/***************************************
* Data Structure
***************************************/
/**
 * \addtogroup group_motif_structure
 * \{
 */
/** TCPWM configuration sub-structure */
typedef struct
{
    uint32_t                          idx; /**< PWM counter index */
    cy_stc_tcpwm_pwm_config_t const* cfg;  /**< PWM configuration structure */
} stl_motif_tcpwm_cfg_t;

/** TCPWM counter configuration sub-structure */
typedef struct
{
    uint32_t                              idx;    /**< TCPWM counter index */
    cy_stc_tcpwm_counter_config_t const* cfg;     /**< TCPWM counter configuration structure */
} stl_motif_tcpwm_counter_cfg_t;

/** MOTIF self test configuration structure */

typedef struct
{
    /*MOTIF Module configuration structure*/
    TCPWM_MOTIF_GRP_MOTIF_Type* motif_base;    /**< MOTIF base */
    cy_stc_tcpwm_motif_quaddec_config_t const* motif_config; /**< MOTIF config */

    /*TCPWM configuration to capture q-clk ticks*/
    TCPWM_Type* qclk_base;                     /**< TCPWM counter base */
    stl_motif_tcpwm_counter_cfg_t qclk;        /**< TCPWM counter configuration */

    /*TCPWM counter configuration to emulate input signals*/
    TCPWM_Type* sgen_base[EMU_SIG_NUM];        /**< PWM base */
    stl_motif_tcpwm_cfg_t sgen[EMU_SIG_NUM];   /**< PWM configuration */

    uint32_t ref_count;                        /**< Reference tcpwm count */
    uint32_t margin_count;                     /**< Allowed margin */
    uint32_t delay;                            /**< Delay */
} stl_motif_cfg_handle_t;

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
*  This function initializes the MOTIF self test configuration. Initialization includes
*  - Generate the emulated signals for Phase-A, Phase-B and Index using TCPWM.
*  These signals are input to the MOTIF module.
*  - Configure the TCPWM counter to capture the Q-CLK resolution.
*  - Configure the MOTIF module.
* \param hPtr
* Pointer to the motif self test configuration handler.
*
*******************************************************************************/
void SelfTest_Motif_Init(stl_motif_cfg_handle_t* hPtr);

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

uint8_t SelfTest_Motif_Start(stl_motif_cfg_handle_t* hPtr);


/** \} group_motif_functions */

#endif /* if (defined (CY_IP_MXS40TCPWM) || defined (CY_DOXYGEN)) */
/** \} group_motif */



#endif /* MOTIF_H */

/* [] END OF FILE */

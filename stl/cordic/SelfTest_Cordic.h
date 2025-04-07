/*******************************************************************************
* File Name: SelfTest_Cordic.h
*
* Description:
*  This file provides constants and parameter values used for CORDIC
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
* \addtogroup group_cordic
* \{
*
* To meet Class B requirement, Cordic must be tested for sine and cosine trignometric functions: <br>
*
*      1) Sine and Cosine trignometric functions are performed using CORDIC IP.
*      2) First Converting the angle in degree to radian and radian in Q31 format , then provided as input.
*      3) Converting the result in Q31 format to float.
*      4) Checks the difference between results from CORDIC and fixed output are within tolerance.
*
*
* \defgroup group_cordic_functions Functions
*/

#if !defined(SELFTEST_CORDIC_H)
    #define SELFTEST_CORDIC_H
#include "SelfTest_common.h"
#include "SelfTest_ErrorInjection.h"

/***************************************
* Function Prototypes
***************************************/
/**
* \addtogroup group_cordic_functions
* \{
*/

/*******************************************************************************
* Function Name: SelfTest_Cordic
****************************************************************************//**
*
* This function perform self test on Cordic IP.
*
* The CORDIC block accelerates calculation of trigonometric functions. 
* Sine and Cosine trignometric functions are performed in this API.
*
* \return
*  "0" "OK_STATUS" - Test passed <br>
*  "1" "ERROR_STATUS" - Test failed
*
*******************************************************************************/

uint8_t SelfTest_Cordic(void);

/** \} group_cordic_functions */


/** \} group_cordic */

#endif /* SELFTEST_CORDIC_H */

/* [] END OF FILE */


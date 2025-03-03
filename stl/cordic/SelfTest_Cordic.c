/*******************************************************************************
* File Name: SelfTest_Cordic.c
* Version 1.0.0
*
* Description:
*  This file provides the source code to the API for Cordic self tests.
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

#include "cy_pdl.h"
#include "SelfTest_Cordic.h"
#include "math.h"

#if defined (CY_IP_MXCORDIC)

/* Multiplier for Q format conversion */
#define Q31_MULTIPLIER (2147483648L) /* 1<<31 */
/* Macros for the conversion of formats */
#define DEG_RAD_MULTIPLIER        (3.141592654/180)
#define FLOAT_DEG_TO_RAD(x)       (x * DEG_RAD_MULTIPLIER)
#define FLOAT_DEG_TO_RAD_Q31(x)   ((CY_CORDIC_Q31_t)(x * (Q31_MULTIPLIER/180)))
#define FLOAT_TO_Q31(x)           ((CY_CORDIC_Q31_t)(x * Q31_MULTIPLIER))
#define Q31_TO_FLOAT(x)     ((float32_t)(x) / Q31_MULTIPLIER)

/* Macros for fixed inputs and outputs */
#define IN_SIN_COS      (30)
#define OUT_SIN         (0.5f)
#define OUT_COS         (0.866f)

static uint8_t sine_cordic(void);
static uint8_t cosine_cordic(void);


/*******************************************************************************
 * Function Name: sine
 *********************************************************************************
 * Summary:
 * This is the function for calculating the sine. It calculates the sine 
 * using CORDIC BLOCK. 
 * Then it compares the results from CORDIC and fixed output,
 * if difference between results are within tolerance then returns OK_STATUS or returns ERROR_STATUS.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  uint8_t 
 *
 *******************************************************************************/

static uint8_t sine_cordic(void)
{
    float32_t res;
    int32_t angle_deg = IN_SIN_COS;
    CY_CORDIC_Q31_t result_q31 = 0;
    float32_t tolerance = 0.0001f;

    #if(!ERROR_IN_CORDIC)
        /* Converting the angle in degree to radian and radian in Q31 format */
        CY_CORDIC_Q31_t  angle_q31 = FLOAT_DEG_TO_RAD_Q31(angle_deg);
    #else
        CY_CORDIC_Q31_t  angle_q31 = FLOAT_DEG_TO_RAD_Q31(60);
    #endif
    
    /* Calculating sine using CORDIC */
    result_q31 = Cy_CORDIC_Sin(MXCORDIC,angle_q31);
    
    /* Converting the result in Q31 format to float */
    res = Q31_TO_FLOAT(result_q31);
    
    /* Checks the difference between results from CORDIC and fixed output are within tolerance */
    if(fabs(res - OUT_SIN) < tolerance )
    {
        return OK_STATUS;
    }
    else return ERROR_STATUS;
}

/*******************************************************************************
 * Function Name: cosine
 *********************************************************************************
 * Summary:
 * This is the function for calculating the cosine. It calculates the cosine 
 * using CORDIC BLOCK. 
 * Then it compares the results from CORDIC and fixed output,
 * if difference between results are within tolerance then returns OK_STATUS or returns ERROR_STATUS.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  uint8_t
 *
 *******************************************************************************/

static uint8_t cosine_cordic(void)
{
    float32_t res;
    CY_CORDIC_Q31_t result_q31 = 0;
    int32_t angle_deg = IN_SIN_COS;
    float32_t tolerance = 0.0001f;

    /* Converting the angle in degree to radian and radian in Q31 format */
    CY_CORDIC_Q31_t angle_q31 = FLOAT_DEG_TO_RAD_Q31(angle_deg);

    /* Calculating sine using CORDIC */
    result_q31 = Cy_CORDIC_Cos(MXCORDIC,angle_q31);

    /* Converting the result in Q31 format to float */
    res = Q31_TO_FLOAT(result_q31);
    
    /* Checks the difference between results from CORDIC and fixed output are within tolerance */
    if(fabs(res - OUT_COS) < tolerance )
    {
        return OK_STATUS;
    }
    else return ERROR_STATUS;
}

#endif /* CY_IP_MXCORDIC */

/*******************************************************************************
* Function Name: SelfTest_Cordic
****************************************************************************//**
*
*  This function perform self test on Cordic IP.
*  The CORDIC block accelerates calculation of trigonometric functions. 
*  Sine and Cosine trignometric functions are performed in this API,then returns 
*  the result OK_STATUS after successfully performing the tests or returns ERROR_STATUS  
*  if the test fails.
*
*******************************************************************************/

uint8_t SelfTest_Cordic(void)
{
#if defined (CY_IP_MXCORDIC)

    /* Enable the CORDIC */
    Cy_CORDIC_Enable(MXCORDIC);
    
    /* Enable global interrupts */
    __enable_irq();
    if (OK_STATUS != sine_cordic()) /* Sine function */
    {
        return ERROR_STATUS;
    }
    if (OK_STATUS !=  cosine_cordic()) /* Cosine function */
    {
        return ERROR_STATUS;
    }
    return OK_STATUS;
#else
    return ERROR_STATUS;
#endif /* CY_IP_MXCORDIC */
}



/* [] END OF FILE */


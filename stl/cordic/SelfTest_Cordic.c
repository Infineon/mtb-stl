/*******************************************************************************
* File Name: SelfTest_Cordic.c
*
* Description:
*  This file provides the source code to the API for Cordic self tests.
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

#include "cy_pdl.h"
#include "SelfTest_Cordic.h"
#include "math.h"

#if defined (CY_IP_MXCORDIC)

/* Multiplier for the Q format conversion */
#define Q31_MULTIPLIER   (2147483648L)   /* 1<<31 */
#define Q31_MULTIPLIER_F (2147483648.0f) /* 1<<31 as float32_t */
/* Macros for the conversion of formats */
#define DEG_RAD_MULTIPLIER        (3.141592654/180)
#define FLOAT_DEG_TO_RAD(x)       ((x) * DEG_RAD_MULTIPLIER)
#define FLOAT_DEG_TO_RAD_Q31(x)   ((CY_CORDIC_Q31_t)((x) * (Q31_MULTIPLIER/180)))
#define FLOAT_TO_Q31(x)           ((CY_CORDIC_Q31_t)((x) * Q31_MULTIPLIER))
#define Q31_TO_FLOAT(x)     ((float32_t)(x) / Q31_MULTIPLIER_F)

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
* This function calculates the sine using CORDIC BLOCK
* and compares the results from teh CORDIC and fixed output.
* If the difference between the results is within the tolerance, the function returns OK_STATUS or ERROR_STATUS.
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

    #if (!ERROR_IN_CORDIC)
    /* Convert the angle into the degree to the radian and the radian into the Q31 format. */
    CY_CORDIC_Q31_t  angle_q31 = FLOAT_DEG_TO_RAD_Q31(angle_deg);
    #else
    CY_CORDIC_Q31_t  angle_q31 = FLOAT_DEG_TO_RAD_Q31(60);
    #endif

    /* Calculate the sine using CORDIC. */
    result_q31 = Cy_CORDIC_Sin(MXCORDIC, angle_q31);

    /* Convert the result into the Q31 format to float. */
    res = Q31_TO_FLOAT(result_q31);

    /* Check if the difference between the results from CORDIC and fixed output is within the tolerance. */
    if (fabsf(res - OUT_SIN) < tolerance)
    {
        return OK_STATUS;
    }
    else
    {
        return ERROR_STATUS;
    }
}


/*******************************************************************************
* Function Name: cosine
*********************************************************************************
* Summary:
* This function calculates the cosine using CORDIC BLOCK and compares the results
* from the CORDIC and fixed output. If the difference between the results is within
* the tolerance, the function returns OK_STATUS or ERROR_STATUS.
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

    /* Convert the angle into the degree to the radian and the radian into the Q31 format. */
    CY_CORDIC_Q31_t angle_q31 = FLOAT_DEG_TO_RAD_Q31(angle_deg);

    /* Calculate the sine using CORDIC. */
    result_q31 = Cy_CORDIC_Cos(MXCORDIC, angle_q31);

    /* Convert the result into the Q31 format to float. */
    res = Q31_TO_FLOAT(result_q31);

    /* Check if the difference between the results from the CORDIC and fixed output is within the tolerance. */
    if (fabsf(res - OUT_COS) < tolerance)
    {
        return OK_STATUS;
    }
    else
    {
        return ERROR_STATUS;
    }
}


/*******************************************************************************
* Function Name: SelfTest_Cordic
********************************************************************************
*
*  This function performs the self test on Cordic IP.
*  The CORDIC block accelerates the calculation of trigonometric functions.
*  Sine and Cosine trignometric functions are performed in this API.
*
*******************************************************************************/

uint8_t SelfTest_Cordic(void)
{
    /* Enable CORDIC */
    Cy_CORDIC_Enable(MXCORDIC);

    if (OK_STATUS != sine_cordic()) /* Sine function */
    {
        return ERROR_STATUS;
    }
    if (OK_STATUS != cosine_cordic())  /* Cosine function */
    {
        return ERROR_STATUS;
    }
    return OK_STATUS;
}


#endif /* CY_IP_MXCORDIC */

/* [] END OF FILE */

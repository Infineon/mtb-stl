/*******************************************************************************
* File Name: SelfTest_Cordic.h
*
* Description:
*  This file provides constants and parameter values used for CORDIC
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
 * \addtogroup group_cordic
 * \{
 *
 * To meet Class B requirement, Cordic must be tested for sine and cosine trignometric functions:
 * <br>
 *
 *      1) Sine and Cosine calculations are performed using CORDIC IP.
 *      2) First, converts the angle into the degree to the radian and the radian into the Q31 format, then provides
 *         as the input.
 *      3) Converts the result into the Q31 format to float.
 *      4) Checks if the difference between the results from the CORDIC and fixed output is within the tolerance.
 *
 *
 * \defgroup group_cordic_functions Functions
 */

#if !defined(SELFTEST_CORDIC_H)
    #define SELFTEST_CORDIC_H
#include "SelfTest_common.h"
#include "SelfTest_ErrorInjection.h"

#if (defined (CY_IP_MXCORDIC) || defined (CY_DOXYGEN))

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
* This function performs the self test on Cordic IP.
*
* The CORDIC block accelerates the calculation of trigonometric functions.
* Sine and Cosine are calculated in this function.
*
*
* \return
*  "0" "OK_STATUS" - Test passed <br>
*  "1" "ERROR_STATUS" - Test failed
*
*******************************************************************************/

uint8_t SelfTest_Cordic(void);

/** \} group_cordic_functions */

#endif /* if (defined (CY_IP_MXCORDIC) || defined (CY_DOXYGEN)) */

#endif /* SELFTEST_CORDIC_H */

/** \} group_cordic */

/* [] END OF FILE */

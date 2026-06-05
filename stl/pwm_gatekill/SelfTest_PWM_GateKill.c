/*******************************************************************************
* File Name: SelfTest_PWM_GateKill.c
*
*
* Description:
*  This file provides the source code for the PWM Gatekill self tests.
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

#include "SelfTest_PWM_GateKill.h"
#include "SelfTest_ErrorInjection.h"

#if (defined(CY_IP_M0S8TCPWM) || defined(CY_IP_MXTCPWM))
uint8_t SelfTest_PWM_GateKill(TCPWM_Type* base, uint32_t cntNum)
{
    uint32_t pwm_count;
    uint32_t ref_pwm_count;

    ref_pwm_count = Cy_TCPWM_Counter_GetCounter(base, cntNum);

    Cy_SysLib_Delay(10);

    pwm_count = Cy_TCPWM_Counter_GetCounter(base, cntNum);

    #if ERROR_IN_PWM_GATEKILL
    pwm_count += 10;
    #endif
    if (pwm_count == ref_pwm_count)
    {
        return OK_STATUS;
    }
    else
    {
        return ERROR_STATUS;
    }
}


#endif /* (defined(CY_IP_M0S8TCPWM) || defined(CY_IP_MXTCPWM)) */

/* [] END OF FILE */

/*******************************************************************************
* File Name: SelfTest_IPC.c
*
* Description:
*  This file provides the source code for the IPC self tests.
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

#include "cy_pdl.h"
#include "SelfTest_IPC.h"
#include "SelfTest_ErrorInjection.h"

#if (defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) || defined (CY_IP_MXIPC))

#if defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS)
#define STL_IPC_IRQ_NR CPUSS_IPC_IPC_IRQ_NR
#define STL_IPC_CH_NR  CPUSS_IPC_IPC_NR
#elif defined (CY_IP_M33SYSCPUSS)
#define STL_IPC_IRQ_NR CPUSS_IPC_IRQ_NR
#define STL_IPC_CH_NR  CPUSS_IPC_NR
#endif /* if defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) */
/*******************************************************************************
* Global Variables
*******************************************************************************/
#if defined (CY_IP_M4CPUSS)
static uint32_t stlIpc_freeChannelStart = 8U;
static uint32_t stlIpc_freeIntrStart = 7U;
#elif (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
static uint32_t stlIpc_freeChannelStart = 4U;
static uint32_t stlIpc_freeIntrStart = 1U;
#else
static uint32_t stlIpc_freeChannelStart = 0U;
static uint32_t stlIpc_freeIntrStart = 0U;
#endif /* if defined (CY_IP_M4CPUSS) */
static uint32_t stlIpc_freeChannelEnd = STL_IPC_CH_NR - 1U;
static uint32_t stlIpc_freeIntrEnd    = STL_IPC_IRQ_NR - 1U;
static volatile uint8_t stlIpc_chRx[16U];

/* Channels write message */
#if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
static uint32_t stlIpc_writeMesg0 = 0x100U;
#else
static uint32_t stlIpc_writeMesg0[2U] = { 0x0, 0x100 };
#endif

/* Channels read message */
#if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
static uint32_t stlIpc_readMesg4[2U];
static uint32_t stlIpc_readMesg5[2U];
static uint32_t stlIpc_readMesg6[2U];
static uint32_t stlIpc_readMesg7[2U];
#elif defined (CY_IP_M33SYSCPUSS)
static uint32_t stlIpc_readMesg0[2U];
static uint32_t stlIpc_readMesg1[2U];
static uint32_t stlIpc_readMesg2[2U];
static uint32_t stlIpc_readMesg3[2U];
#if (CPUSS_IPC_NR == 8U)
static uint32_t stlIpc_readMesg4[2U];
static uint32_t stlIpc_readMesg5[2U];
static uint32_t stlIpc_readMesg6[2U];
static uint32_t stlIpc_readMesg7[2U];
#endif /* if (CPUSS_IPC_NR == 8U) */
#elif defined (CY_IP_M4CPUSS)

#if (CY_IP_M4CPUSS_VERSION == 1)
static uint32_t stlIpc_readMesg8;
static uint32_t stlIpc_readMesg9;
static uint32_t stlIpc_readMesg10;
static uint32_t stlIpc_readMesg11;
static uint32_t stlIpc_readMesg12;
static uint32_t stlIpc_readMesg13;
static uint32_t stlIpc_readMesg14;
static uint32_t stlIpc_readMesg15;
#else
static uint32_t stlIpc_readMesg8[2U];
static uint32_t stlIpc_readMesg9[2U];
static uint32_t stlIpc_readMesg10[2U];
static uint32_t stlIpc_readMesg11[2U];
static uint32_t stlIpc_readMesg12[2U];
static uint32_t stlIpc_readMesg13[2U];
static uint32_t stlIpc_readMesg14[2U];
static uint32_t stlIpc_readMesg15[2U];
#endif /* if (CY_IP_M4CPUSS_VERSION == 1) */
#endif /* if defined (CY_IP_M7CPUSS) */


/*******************************************************************************
* Common code to be called in all ISR
*******************************************************************************/
static inline void common_fxn(IPC_INTR_STRUCT_Type* ipcIntrPtr, uint32 notifyMask, uint32 relMask)
{
    IPC_STRUCT_Type* ipcPtr;
    /* Check if the interrupt was a notify interrupt */
    if (0UL != notifyMask)
    {
        /* Clear the notify interrupt. */
        Cy_IPC_Drv_ClearInterrupt(ipcIntrPtr, CY_IPC_NO_NOTIFICATION, notifyMask);
        #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(4))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_4);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg4);
                stlIpc_chRx[4] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(5))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_5);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg5);
                stlIpc_chRx[5] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(6))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_6);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg6);
                stlIpc_chRx[6] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(7))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_7);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg7);
                stlIpc_chRx[7] = 1;
            }
        }
        #elif defined (CY_IP_M33SYSCPUSS)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(0))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_0);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg0);
                stlIpc_chRx[0] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(1))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_1);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg1);
                stlIpc_chRx[1] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(2))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_2);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg2);
                stlIpc_chRx[2] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(3))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_3);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg3);
                stlIpc_chRx[3] = 1;
            }
        }
        #if (CPUSS_IPC_NR == 8U)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(4))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_4);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg4);
                stlIpc_chRx[4] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(5))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_5);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg5);
                stlIpc_chRx[5] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(6))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_6);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg6);
                stlIpc_chRx[6] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(7))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_7);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg7);
                stlIpc_chRx[7] = 1;
            }
        }
        #endif /* if (CPUSS_IPC_NR == 8U) */
        #elif defined (CY_IP_M4CPUSS)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(8U))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_8);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg8 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg8);
                #endif
                stlIpc_chRx[8] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(9U))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_9);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg9 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg9);
                #endif
                stlIpc_chRx[9] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(10))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_10);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg10 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg10);
                #endif
                stlIpc_chRx[10] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(11))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_11);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg11 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg11);
                #endif
                stlIpc_chRx[11] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(12))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_12);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg12 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg12);
                #endif
                stlIpc_chRx[12] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(13))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_13);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg13 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg13);
                #endif
                stlIpc_chRx[13] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(14))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_14);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg14 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg14);
                #endif
                stlIpc_chRx[14] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(15))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_15);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                stlIpc_readMesg15 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlIpc_readMesg15);
                #endif
                stlIpc_chRx[15] = 1;
            }
        }

        #if (CY_IP_M4CPUSS_VERSION == 1)
        /* Avoid "set but never used" compilation warning */
        (void)stlIpc_readMesg8;
        (void)stlIpc_readMesg9;
        (void)stlIpc_readMesg10;
        (void)stlIpc_readMesg11;
        (void)stlIpc_readMesg12;
        (void)stlIpc_readMesg13;
        (void)stlIpc_readMesg14;
        (void)stlIpc_readMesg15;
        #endif
        #endif /* if defined (CY_IP_M7CPUSS) */
    }

    /* Check if the interrupt was a release interrupt */
    if (0UL != relMask)  /* Check for a Release interrupt */
    {
        /* Clear the release interrupt  */
        Cy_IPC_Drv_ClearInterrupt(ipcIntrPtr, relMask, CY_IPC_NO_NOTIFICATION);
    }
}


/*******************************************************************************
* ISR Config
*******************************************************************************/
#if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
/* Setup the IPC1 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig1 =
{
    .intrSrc      = ((NvicMux0_IRQn << 16) | IPC1_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC2 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig2 =
{
    .intrSrc      = ((NvicMux1_IRQn << 16) | IPC2_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC3 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig3 =
{
    .intrSrc      = ((NvicMux2_IRQn << 16) | IPC3_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC4 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig4 =
{
    .intrSrc      = ((NvicMux3_IRQn << 16) | IPC4_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC5 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig5 =
{
    .intrSrc      = ((NvicMux4_IRQn << 16) | IPC5_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC6 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig6 =
{
    .intrSrc      = ((NvicMux5_IRQn << 16) | IPC6_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC1 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig7 =
{
    .intrSrc      = ((NvicMux6_IRQn << 16) | IPC7_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};


#elif defined (CY_IP_M4CPUSS)
/* Setup the IPC7 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig7 =
{
    /*.intrSrc =*/ IPC7_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC8 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig8 =
{
    /*.intrSrc =*/ IPC8_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC9 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig9 =
{
    /*.intrSrc =*/ IPC9_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC10 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig10 =
{
    /*.intrSrc =*/ IPC10_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC11 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig11 =
{
    /*.intrSrc =*/ IPC11_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC12 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig12 =
{
    /*.intrSrc =*/ IPC12_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC13 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig13 =
{
    /*.intrSrc =*/ IPC13_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the  IPC14 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig14 =
{
    /*.intrSrc =*/ IPC14_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC15 Interrupt */
static const cy_stc_sysint_t stlIpc_intConfig15 =
{
    /*.intrSrc =*/ IPC15_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};
#elif defined (CY_IP_M33SYSCPUSS)
static const cy_stc_sysint_t stlIpc_intConfig1 =
{
    /*.intrSrc =*/ IPC1_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

static const cy_stc_sysint_t stlIpc_intConfig2 =
{
    /*.intrSrc =*/ IPC2_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};
#if (CPUSS_IPC_IRQ_NR == 4U)
static const cy_stc_sysint_t stlIpc_intConfig3 =
{
    /*.intrSrc =*/ IPC3_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

static const cy_stc_sysint_t stlIpc_intConfig4 =
{
    /*.intrSrc =*/ IPC4_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};
#endif /* if (CPUSS_IPC_IRQ_NR == 4U) */
#endif /* if defined (CY_IP_M7CPUSS) */

/*******************************************************************************
* ISR for Each IPC interrupt
*******************************************************************************/
#if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
/* ISR for IPC Interrupt Number 1 */
static void IPC_Interrupt_User_1(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_1);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 2 */
static void IPC_Interrupt_User_2(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_2);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 3 */
static void IPC_Interrupt_User_3(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_3);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 4 */
static void IPC_Interrupt_User_4(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_4);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 5 */
static void IPC_Interrupt_User_5(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_5);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 6 */
static void IPC_Interrupt_User_6(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_6);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 7 */
static void IPC_Interrupt_User_7(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_7);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


#elif defined (CY_IP_M4CPUSS)
/* ISR for IPC Interrupt Number 7 */
static void IPC_Interrupt_User_7(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_7);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 8 */
static void IPC_Interrupt_User_8(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_8);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 9 */
static void IPC_Interrupt_User_9(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_9);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 10 */
static void IPC_Interrupt_User_10(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_10);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 11 */
static void IPC_Interrupt_User_11(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_11);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 12 */
static void IPC_Interrupt_User_12(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_12);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 13 */
static void IPC_Interrupt_User_13(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_13);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 14 */
static void IPC_Interrupt_User_14(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_14);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 15 */
static void IPC_Interrupt_User_15(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_15);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


#elif defined (CY_IP_M33SYSCPUSS)
/* ISR for IPC Interrupt Number 1 */
static void IPC_Interrupt_User_1(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_0);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 2 */
static void IPC_Interrupt_User_2(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_1);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


#if (CPUSS_IPC_IRQ_NR == 4U)
/* ISR for IPC Interrupt Number 3 */
static void IPC_Interrupt_User_3(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_2);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


/* ISR for IPC Interrupt Number 4 */
static void IPC_Interrupt_User_4(void)
{
    uint32_t shadowIntr;
    IPC_INTR_STRUCT_Type* ipcIntrPtr;
    uint32 notifyMask;
    uint32 relMask;
    ipcIntrPtr = Cy_IPC_Drv_GetIntrBaseAddr(IPC_INT_3);
    shadowIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);
    notifyMask = Cy_IPC_Drv_ExtractAcquireMask(shadowIntr);
    relMask = Cy_IPC_Drv_ExtractReleaseMask(shadowIntr);
    common_fxn(ipcIntrPtr, notifyMask, relMask);
}


#endif /* if (CPUSS_IPC_IRQ_NR == 4U) */

#endif /* if defined (CY_IP_M7CPUSS) */

/*******************************************************************************
 * Function Name: SelfTest_IPC
 *******************************************************************************
 *
 * Summary:
 *  This function performs a check on each free channel with all free IPC interrupts
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 ******************************************************************************/
uint8_t SelfTest_IPC(void)
{
    /* Function result */
    uint8_t ret = OK_STATUS;
    uint32_t IPC_CH_REL_MASK = 0, IPC_CH_NOTIFY_MASK = 0;
    cy_en_ipcdrv_status_t ipcStatus = CY_IPC_DRV_ERROR;
    cy_en_sysint_status_t sysint_init_status;

    for (uint32_t i = stlIpc_freeChannelStart; i <= stlIpc_freeChannelEnd; i++)
    {
        IPC_CH_REL_MASK |= (uint32_t)(GET_IPC_CH_REL_MASK((i)));
    }
    for (uint32_t i = stlIpc_freeChannelStart; i <= stlIpc_freeChannelEnd; i++)
    {
        IPC_CH_NOTIFY_MASK |= (uint32_t)(GET_IPC_CH_NOTIFY_MASK((i)));
    }

    #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
    sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig1, IPC_Interrupt_User_1);
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig2, IPC_Interrupt_User_2);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig3, IPC_Interrupt_User_3);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig4, IPC_Interrupt_User_4);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig5, IPC_Interrupt_User_5);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig6, IPC_Interrupt_User_6);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig7, IPC_Interrupt_User_7);
    }
    #elif defined (CY_IP_M4CPUSS)
    sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig7, IPC_Interrupt_User_7);
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig8, IPC_Interrupt_User_8);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig9, IPC_Interrupt_User_9);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig10, IPC_Interrupt_User_10);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig11, IPC_Interrupt_User_11);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig12, IPC_Interrupt_User_12);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig13, IPC_Interrupt_User_13);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig14, IPC_Interrupt_User_14);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig15, IPC_Interrupt_User_15);
    }
    #elif defined (CY_IP_M33SYSCPUSS)
    sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig1, IPC_Interrupt_User_1);
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig2, IPC_Interrupt_User_2);
    }
    #if (CPUSS_IPC_IRQ_NR == 4U)
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig3, IPC_Interrupt_User_3);
    }
    if (CY_SYSINT_SUCCESS == sysint_init_status)
    {
        sysint_init_status = Cy_SysInt_Init(&stlIpc_intConfig4, IPC_Interrupt_User_4);
    }
    #endif /* if (CPUSS_IPC_IRQ_NR == 4U) */
    #endif /* if defined (CY_IP_M7CPUSS) */

    if (CY_SYSINT_SUCCESS != sysint_init_status)
    {
        ret = ERROR_STATUS;
    }

    if (OK_STATUS == ret)
    {
        #if (defined (CY_IP_M7CPUSS) || defined (CY_M4CPUSS_V2_IRQ_MUXING))
        NVIC_EnableIRQ(NvicMux0_IRQn);
        NVIC_EnableIRQ(NvicMux1_IRQn);
        NVIC_EnableIRQ(NvicMux2_IRQn);
        NVIC_EnableIRQ(NvicMux3_IRQn);
        NVIC_EnableIRQ(NvicMux4_IRQn);
        NVIC_EnableIRQ(NvicMux5_IRQn);
        NVIC_EnableIRQ(NvicMux6_IRQn);
        #elif defined (CY_IP_M4CPUSS)
        NVIC_EnableIRQ(IPC7_INTERRUPT);
        NVIC_EnableIRQ(IPC8_INTERRUPT);
        NVIC_EnableIRQ(IPC9_INTERRUPT);
        NVIC_EnableIRQ(IPC10_INTERRUPT);
        NVIC_EnableIRQ(IPC11_INTERRUPT);
        NVIC_EnableIRQ(IPC12_INTERRUPT);
        NVIC_EnableIRQ(IPC13_INTERRUPT);
        NVIC_EnableIRQ(IPC14_INTERRUPT);
        NVIC_EnableIRQ(IPC15_INTERRUPT);
        #elif defined (CY_IP_M33SYSCPUSS)
        NVIC_EnableIRQ(IPC1_INTERRUPT);
        NVIC_EnableIRQ(IPC2_INTERRUPT);
        #if (CPUSS_IPC_IRQ_NR == 4U)
        NVIC_EnableIRQ(IPC3_INTERRUPT);
        NVIC_EnableIRQ(IPC4_INTERRUPT);
        #endif /* if (CPUSS_IPC_IRQ_NR == 4U) */
        #endif /* if defined (CY_IP_M7CPUSS) */
    }

    /* Set IPC Interrupt mask */
    for (uint32_t i = stlIpc_freeIntrStart; i <= stlIpc_freeIntrEnd; i++)
    {
        Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(
                                        (uint32_t)i), IPC_CH_REL_MASK, IPC_CH_NOTIFY_MASK);
    }

    for (uint32_t channel = stlIpc_freeChannelStart; channel <= stlIpc_freeChannelEnd;
         channel++)
    {
        for (uint32_t intrr = stlIpc_freeIntrStart; intrr <= stlIpc_freeIntrEnd; intrr++)
        {
            if (OK_STATUS == ret)
            {
                ipcStatus = Cy_IPC_Drv_LockAcquire(Cy_IPC_Drv_GetIpcBaseAddress(channel));
                if (ipcStatus != CY_IPC_DRV_SUCCESS)
                {
                    ret = ERROR_STATUS;
                }
            }
            if (OK_STATUS == ret)
            {
                #if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
                Cy_IPC_Drv_WriteDataValue(Cy_IPC_Drv_GetIpcBaseAddress((uint32_t)channel), stlIpc_writeMesg0);
                #else
                Cy_IPC_Drv_WriteDDataValue(Cy_IPC_Drv_GetIpcBaseAddress(
                                               (uint32_t)channel), (uint32_t*)stlIpc_writeMesg0);
                #endif

                Cy_IPC_Drv_AcquireNotify(Cy_IPC_Drv_GetIpcBaseAddress((uint32_t)channel),
                                         (uint32_t)(IPC_INT_NOTIFY_MASK((intrr))));

                uint32_t locStatus = 0;
                uint32_t timeout = 1000000;
                do
                {
                    #if ERROR_IN_IPC
                    locStatus = 0;
                    #else
                    locStatus = stlIpc_chRx[channel];
                    #endif

                    /* If the timeout equal to 0. Ignore the timeout */
                    if (timeout > 0UL)
                    {
                        Cy_SysLib_DelayUs(1);
                        --(timeout);

                        if (0UL == timeout)
                        {
                            locStatus |= (1UL<<16UL);
                        }
                    }
                } while (0UL == locStatus);

                if (0U != (uint32_t)(locStatus & 0x10000UL))
                {
                    ret = ERROR_STATUS;
                }

                if (OK_STATUS == ret)
                {
                    stlIpc_chRx[channel] = 0;
                    ipcStatus =
                        Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress((uint32_t)channel),
                                               (uint32_t)(IPC_INT_REL_MASK((intrr))));
                    if (ipcStatus != CY_IPC_DRV_SUCCESS)
                    {
                        ret = ERROR_STATUS;
                    }
                }

                if (OK_STATUS == ret)
                {
                    #if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
                    stlIpc_writeMesg0++;
                    #else
                    stlIpc_writeMesg0[0]++;
                    stlIpc_writeMesg0[1]++;
                    #endif
                }
            }

            if (OK_STATUS != ret)
            {
                break;
            }
        }

        if (OK_STATUS != ret)
        {
            break;
        }
    }
    return ret;
}


#endif /* if (defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) || defined (CY_IP_MXIPC)) */

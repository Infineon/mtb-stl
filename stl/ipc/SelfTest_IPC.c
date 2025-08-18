/*******************************************************************************
* File Name: SelfTest_IPC.c
*
* Description:
*  This file provides the source code for the IPC self tests.
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

#include "cy_pdl.h"
#include "SelfTest_IPC.h"
#include "SelfTest_ErrorInjection.h"

#if (defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) || defined (CY_IP_MXIPC))


/*******************************************************************************
* Global Variables
*******************************************************************************/
#if defined (CY_IP_M4CPUSS)
static uint32_t free_channel_start = 8;
static uint32_t free_channel_end   = 15;
static uint32_t free_intr_start = 7;
static uint32_t free_intr_end = 15;
#elif defined (CY_IP_M7CPUSS)
static uint32_t free_channel_start = 4;
static uint32_t free_channel_end   = 7;
static uint32_t free_intr_start = 1;
static uint32_t free_intr_end = 7;
#elif defined (CY_IP_M33SYSCPUSS)
static uint32_t free_channel_start = 0;
static uint32_t free_channel_end   = 1;
static uint32_t free_intr_start = 0;
static uint32_t free_intr_end = 1;
#endif /* if defined (CY_IP_M4CPUSS) */
static volatile uint8_t ch_rx[16];

/* Channels write message */
#if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
static uint32_t writeMesg0 = 0x100U;
#else
static uint32_t writeMesg0[2] = { 0x0, 0x100 };
#endif

/* Channels read message */
#if defined (CY_IP_M7CPUSS)
static uint32_t readMesg4[2];
static uint32_t readMesg5[2];
static uint32_t readMesg6[2];
static uint32_t readMesg7[2];
#elif defined (CY_IP_M33SYSCPUSS)
static uint32_t readMesg0[2];
static uint32_t readMesg1[2];
#elif defined (CY_IP_M4CPUSS)

#if (CY_IP_M4CPUSS_VERSION == 1)
static uint32_t readMesg8;
static uint32_t readMesg9;
static uint32_t readMesg10;
static uint32_t readMesg11;
static uint32_t readMesg12;
static uint32_t readMesg13;
static uint32_t readMesg14;
static uint32_t readMesg15;
#else
static uint32_t readMesg8[2];
static uint32_t readMesg9[2];
static uint32_t readMesg10[2];
static uint32_t readMesg11[2];
static uint32_t readMesg12[2];
static uint32_t readMesg13[2];
static uint32_t readMesg14[2];
static uint32_t readMesg15[2];
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
        #if defined (CY_IP_M7CPUSS)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(4))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_4);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg4);
                ch_rx[4] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(5))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_5);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg5);
                ch_rx[5] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(6))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_6);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg6);
                ch_rx[6] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(7))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_7);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg7);
                ch_rx[7] = 1;
            }
        }
        #elif defined (CY_IP_M33SYSCPUSS)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(0))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_0);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg0);
                ch_rx[0] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(1))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_1);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg1);
                ch_rx[1] = 1;
            }
        }
        #elif defined (CY_IP_M4CPUSS)
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(8U))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_8);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg8 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg8);
                #endif
                ch_rx[8] = 1;
            }
        }
        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(9U))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_9);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg9 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg9);
                #endif
                ch_rx[9] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(10))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_10);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg10 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg10);
                #endif
                ch_rx[10] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(11))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_11);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg11 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg11);
                #endif
                ch_rx[11] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(12))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_12);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg12 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg12);
                #endif
                ch_rx[12] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(13))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_13);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg13 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg13);
                #endif
                ch_rx[13] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(14))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_14);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg14 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg14);
                #endif
                ch_rx[14] = 1;
            }
        }

        if (((notifyMask) & (GET_IPC_CH_NOTIFY_MASK(15))) != 0UL)
        {
            ipcPtr = Cy_IPC_Drv_GetIpcBaseAddress(IPC_CH_15);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                #if (CY_IP_M4CPUSS_VERSION == 1)
                readMesg15 = Cy_IPC_Drv_ReadDataValue(ipcPtr);
                #else
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, readMesg15);
                #endif
                ch_rx[15] = 1;
            }
        }

        #if (CY_IP_M4CPUSS_VERSION == 1)
        /* Avoid "set but never used" compilation warning */
        (void)readMesg8;
        (void)readMesg9;
        (void)readMesg10;
        (void)readMesg11;
        (void)readMesg12;
        (void)readMesg13;
        (void)readMesg14;
        (void)readMesg15;
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
#if defined (CY_IP_M7CPUSS)
/* Setup the IPC1 Interrupt */
static const cy_stc_sysint_t ipcIntConfig1 =
{
    .intrSrc      = ((NvicMux0_IRQn << 16) | IPC1_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC2 Interrupt */
static const cy_stc_sysint_t ipcIntConfig2 =
{
    .intrSrc      = ((NvicMux1_IRQn << 16) | IPC2_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC3 Interrupt */
static const cy_stc_sysint_t ipcIntConfig3 =
{
    .intrSrc      = ((NvicMux2_IRQn << 16) | IPC3_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC4 Interrupt */
static const cy_stc_sysint_t ipcIntConfig4 =
{
    .intrSrc      = ((NvicMux3_IRQn << 16) | IPC4_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC5 Interrupt */
static const cy_stc_sysint_t ipcIntConfig5 =
{
    .intrSrc      = ((NvicMux4_IRQn << 16) | IPC5_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC6 Interrupt */
static const cy_stc_sysint_t ipcIntConfig6 =
{
    .intrSrc      = ((NvicMux5_IRQn << 16) | IPC6_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};

/* Setup the  IPC1 Interrupt */
static const cy_stc_sysint_t ipcIntConfig7 =
{
    .intrSrc      = ((NvicMux6_IRQn << 16) | IPC7_INTERRUPT), /* Bit 0-15 of intrSrc is used to
                                                                 store the system interrupt value and
                                                                 bit 16-31 to store the CPU IRQ value */
    .intrPriority = IPC_PRIORITY
};


#elif defined (CY_IP_M4CPUSS)
/* Setup the IPC7 Interrupt */
static const cy_stc_sysint_t ipcIntConfig7 =
{
    /*.intrSrc =*/ IPC7_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC8 Interrupt */
static const cy_stc_sysint_t ipcIntConfig8 =
{
    /*.intrSrc =*/ IPC8_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC9 Interrupt */
static const cy_stc_sysint_t ipcIntConfig9 =
{
    /*.intrSrc =*/ IPC9_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC10 Interrupt */
static const cy_stc_sysint_t ipcIntConfig10 =
{
    /*.intrSrc =*/ IPC10_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC11 Interrupt */
static const cy_stc_sysint_t ipcIntConfig11 =
{
    /*.intrSrc =*/ IPC11_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC12 Interrupt */
static const cy_stc_sysint_t ipcIntConfig12 =
{
    /*.intrSrc =*/ IPC12_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC13 Interrupt */
static const cy_stc_sysint_t ipcIntConfig13 =
{
    /*.intrSrc =*/ IPC13_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the  IPC14 Interrupt */
static const cy_stc_sysint_t ipcIntConfig14 =
{
    /*.intrSrc =*/ IPC14_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

/* Setup the IPC15 Interrupt */
static const cy_stc_sysint_t ipcIntConfig15 =
{
    /*.intrSrc =*/ IPC15_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};
#elif defined (CY_IP_M33SYSCPUSS)
static const cy_stc_sysint_t ipcIntConfig1 =
{
    /*.intrSrc =*/ IPC1_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};

static const cy_stc_sysint_t ipcIntConfig2 =
{
    /*.intrSrc =*/ IPC2_INTERRUPT,
    /*.intrPriority =*/ IPC_PRIORITY
};
#endif /* if defined (CY_IP_M7CPUSS) */

/*******************************************************************************
* ISR for Each IPC interrupt
*******************************************************************************/
#if defined (CY_IP_M7CPUSS)
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
    cy_en_ipcdrv_status_t ipcStatus;
    cy_en_sysint_status_t sysint_init_status;

    for (uint32_t i = free_channel_start; i <= free_channel_end; i++)
    {
        IPC_CH_REL_MASK |= (uint32_t)(GET_IPC_CH_REL_MASK((i)));
    }
    for (uint32_t i = free_channel_start; i <= free_channel_end; i++)
    {
        IPC_CH_NOTIFY_MASK |= (uint32_t)(GET_IPC_CH_NOTIFY_MASK((i)));
    }

    #if defined (CY_IP_M7CPUSS)
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig1, IPC_Interrupt_User_1);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig2, IPC_Interrupt_User_2);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig3, IPC_Interrupt_User_3);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig4, IPC_Interrupt_User_4);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig5, IPC_Interrupt_User_5);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig6, IPC_Interrupt_User_6);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig7, IPC_Interrupt_User_7);
    #elif defined (CY_IP_M4CPUSS)
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig7, IPC_Interrupt_User_7);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig8, IPC_Interrupt_User_8);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig9, IPC_Interrupt_User_9);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig10, IPC_Interrupt_User_10);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig11, IPC_Interrupt_User_11);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig12, IPC_Interrupt_User_12);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig13, IPC_Interrupt_User_13);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig14, IPC_Interrupt_User_14);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig15, IPC_Interrupt_User_15);
    #elif defined (CY_IP_M33SYSCPUSS)
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig1, IPC_Interrupt_User_1);
    sysint_init_status = Cy_SysInt_Init(&ipcIntConfig2, IPC_Interrupt_User_2);
    #endif /* if defined (CY_IP_M7CPUSS) */
    (void)sysint_init_status;

    #if defined (CY_IP_M7CPUSS)
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
    #endif /* if defined (CY_IP_M7CPUSS) */

    /* Set IPC Interrupt mask */
    for (uint32_t i = free_intr_start; i <= free_intr_end; i++)
    {
        Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(
                                        (uint32_t)i), IPC_CH_REL_MASK, IPC_CH_NOTIFY_MASK);
    }

    for (uint32_t channel = free_channel_start; channel <= free_channel_end; channel++)
    {
        for (uint32_t intrr = free_intr_start; intrr <= free_intr_end; intrr++)
        {
            ipcStatus = Cy_IPC_Drv_LockAcquire(Cy_IPC_Drv_GetIpcBaseAddress(channel));
            if (ipcStatus != CY_IPC_DRV_SUCCESS)
            {
                return ERROR_STATUS;
            }
            #if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
            Cy_IPC_Drv_WriteDataValue(Cy_IPC_Drv_GetIpcBaseAddress((uint32_t)channel), writeMesg0);
            #else
            Cy_IPC_Drv_WriteDDataValue(Cy_IPC_Drv_GetIpcBaseAddress(
                                           (uint32_t)channel), (uint32_t*)writeMesg0);
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
                locStatus = ch_rx[channel];
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
                return ERROR_STATUS;
            }

            ch_rx[channel] = 0;
            ipcStatus =
                Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress((uint32_t)channel),
                                       (uint32_t)(IPC_INT_REL_MASK((intrr))));
            if (ipcStatus != CY_IPC_DRV_SUCCESS)
            {
                return ERROR_STATUS;
            }

            #if (defined (CY_IP_M4CPUSS) && (CY_IP_M4CPUSS_VERSION == 1))
            writeMesg0++;
            #else
            writeMesg0[0]++;
            writeMesg0[1]++;
            #endif
        }
    }
    return ret;
}


#endif /* if (defined (CY_IP_M4CPUSS) || defined (CY_IP_M7CPUSS) || defined (CY_IP_MXIPC)) */

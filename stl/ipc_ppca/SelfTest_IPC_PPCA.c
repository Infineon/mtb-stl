/*******************************************************************************
* File Name: SelfTest_IPC_PPCA.c
*
* Description:
*  This file provides the source code for the IPC self tests.
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
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
#include "SelfTest_IPC_PPCA.h"
#include "SelfTest_ErrorInjection.h"

#if defined(CY_IP_MXS40PPSS)

/* PPCA IPC block: 4 channels (0-3), 4 interrupt lines (0-3) */
#define STL_PPCA_IPC_CH_NR       CY_PPCA_IPC_CHANNELS
#define STL_PPCA_IPC_IRQ_NR      CY_PPCA_IPC_INTERRUPTS
/* ISR wait timeout: PPCA IPC interrupt latency is < 5 us on PSC3M8.
 * 1000 us gives a x200 safety margin while keeping per-pair wait time
 * under 1 ms in error scenarios */
#define STL_PPCA_WAIT_TIMEOUT_US 1000U
/* Mask poll window: minimum time to confirm an ISR does NOT fire.
 * Must exceed one interrupt latency + poll granularity (~ 5 us + 1 us). */
#define STL_PPCA_MASK_POLL_US    50U

/* NVIC interrupt sources for PPCA IPC interrupt lines 0-3 */
#define PPCA_IPC0_INTERRUPT     ppca_ipc_0_IRQn
#define PPCA_IPC1_INTERRUPT     ppca_ipc_1_IRQn
#define PPCA_IPC2_INTERRUPT     ppca_ipc_2_IRQn
#define PPCA_IPC3_INTERRUPT     ppca_ipc_3_IRQn

#define IPC_PRIORITY            (1U)

#define PPCA_IPC_MASK(i)               ((uint32_t)0x1U << (uint32_t)(i))

/* ISR observation context - one entry per PPCA IPC interrupt structure */
typedef struct
{
    volatile uint8_t  fired;                    /* ISR has fired since last reset       */
    volatile uint32_t notifyMask;               /* notify bits (normalised) at entry    */
    volatile uint32_t relMask;                  /* release bits (normalised) at entry   */
    volatile uint32_t intrRaw;                  /* raw INTR register at ISR entry       */
    volatile uint32_t intrMasked;               /* INTR_MASKED at ISR entry             */
    volatile uint32_t intrAfterClear;           /* raw INTR after ClearInterrupt        */
    volatile uint8_t  chRx[STL_PPCA_IPC_CH_NR]; /* per-channel data flag                */
    uint32_t          readData[2];              /* DATA0/DATA1 read inside ISR          */
} stlPpcaIpc_IntrCtx_t;

static stlPpcaIpc_IntrCtx_t stlPpcaIpc_ctx[STL_PPCA_IPC_IRQ_NR];

/* Customer configuration snapshot - saved before the test modifies any
 * PPCA IPC hardware state, restored unconditionally before
 * SelfTest_IPC_PPCA() returns. */
typedef struct
{
    uint32_t      intrMask[STL_PPCA_IPC_IRQ_NR]; /* PPCA IPC INTR_MASK for each struct     */
    uint32_t      nvicEnabled;                   /* bit N = NVIC enable for ppca_ipc_N_IRQn */
    uint32_t      isrVector[STL_PPCA_IPC_IRQ_NR]; /* ISR vectors stored as uint32_t */
    uint32_t      nvicPriority[STL_PPCA_IPC_IRQ_NR]; /* NVIC interrupt priorities           */
} stlPpcaIpc_SavedState_t;

static stlPpcaIpc_SavedState_t stlPpcaIpc_savedState;

/* Failure coordinates - set on first ERROR_STATUS return from PpcaTestPair */
static uint32_t stlPpcaIpc_failCh      = 0U;
static uint32_t stlPpcaIpc_failIntr    = 0U;
static uint8_t  stlPpcaIpc_failSubtest = 0U;

/* Helper: record failure coordinates and set ret - single-exit style,
 * mirroring the ret = ERROR_STATUS pattern used in SelfTest_IPC. */
#define PPCA_SET_FAIL(subtest_num) \
    do { \
        stlPpcaIpc_failCh      = ch; \
        stlPpcaIpc_failIntr    = intr; \
        stlPpcaIpc_failSubtest = (uint8_t)(subtest_num); \
        ret = ERROR_STATUS; \
    } while (false)

/* Clear ISR observation context */
static void PpcaResetCtx(uint32_t intrIdx)
{
    stlPpcaIpc_ctx[intrIdx].fired          = 0U;
    stlPpcaIpc_ctx[intrIdx].notifyMask     = 0U;
    stlPpcaIpc_ctx[intrIdx].relMask        = 0U;
    stlPpcaIpc_ctx[intrIdx].intrRaw        = 0U;
    stlPpcaIpc_ctx[intrIdx].intrMasked     = 0U;
    stlPpcaIpc_ctx[intrIdx].intrAfterClear = 0U;
    stlPpcaIpc_ctx[intrIdx].readData[0]    = 0U;
    stlPpcaIpc_ctx[intrIdx].readData[1]    = 0U;
    for (uint32_t i = 0U; i < STL_PPCA_IPC_CH_NR; i++)
    {
        stlPpcaIpc_ctx[intrIdx].chRx[i] = 0U;
    }
}


/* Clear all pending bits in one PPCA IPC interrupt structure */
static void PpcaClearIntr(uint32_t intrIdx)
{
    IPC_INTR_STRUCT_Type* p =
        (IPC_INTR_STRUCT_Type*)CY_PPCA_IPC_INTR_STRUCT_PTR(intrIdx);
    uint32_t raw  = Cy_IPC_Drv_GetInterruptStatus(p);
    uint32_t nmsk = Cy_IPC_Drv_ExtractAcquireMask(raw);
    uint32_t rmsk = Cy_IPC_Drv_ExtractReleaseMask(raw);
    Cy_IPC_Drv_ClearInterrupt(p, rmsk, nmsk);
}


/* Wait for ISR to fire */
static bool PpcaWaitIsr(uint32_t intrIdx, uint32_t timeoutUs)
{
    bool ret = false;
    #if ERROR_IN_IPC_PPCA_NOTIFY
    /* Bypass the wait - pretend ISR never fires */
    (void)intrIdx;
    (void)timeoutUs;
    #else
    uint32_t remaining = timeoutUs;
    while (remaining > 0U)
    {
        if (stlPpcaIpc_ctx[intrIdx].fired != 0U)
        {
            ret = true;
            break;
        }
        Cy_SysLib_DelayUs(1U);
        --remaining;
    }
    #endif /* ERROR_IN_IPC_PPCA_NOTIFY */
    return ret;
}


/* Shared ISR handler - called by the four dedicated stubs below.
* Records full interrupt context, reads channel data, clears interrupt,
* then records post-clear INTR to support test 7 verification. */
static void PpcaIpcIsrHandler(uint32_t intrIdx)
{
    IPC_INTR_STRUCT_Type* ipcIntrPtr = (IPC_INTR_STRUCT_Type*)CY_PPCA_IPC_INTR_STRUCT_PTR(intrIdx);

    /* Save both raw INTR and gated INTR_MASKED before touching anything */
    stlPpcaIpc_ctx[intrIdx].intrRaw    = Cy_IPC_Drv_GetInterruptStatus(ipcIntrPtr);
    stlPpcaIpc_ctx[intrIdx].intrMasked = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);

    /* Normalise to channel-index space (bits 0-3) from INTR_MASKED */
    stlPpcaIpc_ctx[intrIdx].notifyMask = Cy_IPC_Drv_ExtractAcquireMask(stlPpcaIpc_ctx[intrIdx].intrMasked);
    stlPpcaIpc_ctx[intrIdx].relMask    = Cy_IPC_Drv_ExtractReleaseMask(stlPpcaIpc_ctx[intrIdx].intrMasked);

    /* Read DATA registers for every notified channel */
    for (uint32_t ch = 0U; ch < STL_PPCA_IPC_CH_NR; ch++)
    {
        if ((stlPpcaIpc_ctx[intrIdx].notifyMask & PPCA_IPC_MASK(ch)) != 0UL)
        {
            IPC_STRUCT_Type* ipcPtr = Cy_IPC_Drv_GetPPCAIpcBaseAddress(ch);
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                Cy_IPC_Drv_ReadDDataValue(ipcPtr, stlPpcaIpc_ctx[intrIdx].readData);
                stlPpcaIpc_ctx[intrIdx].chRx[ch] = 1U;
            }
        }
    }

    /* Clear all observed (masked) interrupt bits - always done to prevent
     * infinite ISR re-entry via ARM Cortex-M tail-chaining. */
    Cy_IPC_Drv_ClearInterrupt(ipcIntrPtr, stlPpcaIpc_ctx[intrIdx].relMask, stlPpcaIpc_ctx[intrIdx].notifyMask);

    /* Record raw INTR after clear - cleared bits must not persist.
     * ERROR_IN_IPC_PPCA_CLEAR fakes intrAfterClear to simulate a sticky bit
     * without skipping the actual clear (which would cause infinite ISR loops). */
    #if ERROR_IN_IPC_PPCA_CLEAR
    stlPpcaIpc_ctx[intrIdx].intrAfterClear = stlPpcaIpc_ctx[intrIdx].intrRaw; /* inject: pretend INTR still has the bit
                                                                               */
    #else
    stlPpcaIpc_ctx[intrIdx].intrAfterClear = Cy_IPC_Drv_GetInterruptStatus(ipcIntrPtr);
    #endif /* ERROR_IN_IPC_PPCA_CLEAR */

    stlPpcaIpc_ctx[intrIdx].fired = 1U;
}


/* ISR configs for PPCA IPC interrupt lines 0-3 */
static const cy_stc_sysint_t stlPpcaIpc_intConfig0 =
{
    .intrSrc      = PPCA_IPC0_INTERRUPT,
    .intrPriority = IPC_PRIORITY
};
static const cy_stc_sysint_t stlPpcaIpc_intConfig1 =
{
    .intrSrc      = PPCA_IPC1_INTERRUPT,
    .intrPriority = IPC_PRIORITY
};
static const cy_stc_sysint_t stlPpcaIpc_intConfig2 =
{
    .intrSrc      = PPCA_IPC2_INTERRUPT,
    .intrPriority = IPC_PRIORITY
};
static const cy_stc_sysint_t stlPpcaIpc_intConfig3 =
{
    .intrSrc      = PPCA_IPC3_INTERRUPT,
    .intrPriority = IPC_PRIORITY
};


/* ISR stubs - one per PPCA IPC interrupt line */
static void PpcaIpcInterrupt0(void)
{
    PpcaIpcIsrHandler(0U);
}


static void PpcaIpcInterrupt1(void)
{
    PpcaIpcIsrHandler(1U);
}


static void PpcaIpcInterrupt2(void)
{
    PpcaIpcIsrHandler(2U);
}


static void PpcaIpcInterrupt3(void)
{
    PpcaIpcIsrHandler(3U);
}


/*******************************************************************************
* PpcaTestPair - runs the selected sub-test groups for one
* (channel, interrupt-structure) combination.
* Each group is self-contained: it acquires and releases the channel lock
* independently so groups can be freely combined or omitted.
* Single return at end - mirrors the ret / if (OK_STATUS == ret) pattern
* used in SelfTest_IPC.
*******************************************************************************/
static uint8_t PpcaTestPair(uint32_t ch, uint32_t intr, uint8_t testMask)
{
    IPC_STRUCT_Type* ipcPtr           = Cy_IPC_Drv_GetPPCAIpcBaseAddress(ch);
    IPC_INTR_STRUCT_Type* ipcIntrPtr  = (IPC_INTR_STRUCT_Type*)CY_PPCA_IPC_INTR_STRUCT_PTR(intr);
    cy_en_ipcdrv_status_t ipcStatus   = CY_IPC_DRV_SUCCESS;
    uint8_t ret                       = OK_STATUS;

    const uint32_t chNotifyMask  = PPCA_IPC_MASK(ch);
    const uint32_t chRelMask     = PPCA_IPC_MASK(ch);
    const uint32_t intrNotifyAll = (1U << STL_PPCA_IPC_CH_NR) - 1U;
    const uint32_t intrRelAll    = (1U << STL_PPCA_IPC_CH_NR) - 1U;

    /* Guarantee a clean starting state */
    if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
    {
        ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(1U);
        }
    }
    if (OK_STATUS == ret)
    {
        PpcaClearIntr(intr);
        PpcaResetCtx(intr);
        Cy_IPC_Drv_SetInterruptMask(ipcIntrPtr, intrRelAll, intrNotifyAll);
    }

    /* ------------------------------------------------------------------
     * GROUP LOCK: Acquire / lock-status attribute / Release (sub-tests 1,2,3)
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_LOCK) != 0U))
    {
        ipcStatus = Cy_IPC_Drv_LockAcquire(ipcPtr);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(1U);
        }
        if (OK_STATUS == ret)
        {
            if (!Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                PPCA_SET_FAIL(3U);
            }
        }
        if (OK_STATUS == ret)
        {
            ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
            if (CY_IPC_DRV_SUCCESS != ipcStatus)
            {
                PPCA_SET_FAIL(2U);
            }
        }
        if (OK_STATUS == ret)
        {
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                PPCA_SET_FAIL(2U);
            }
        }
    }

    /* ------------------------------------------------------------------
     * GROUP DATA: DATA0/DATA1 write/read integrity and independence
     * (sub-tests 4,5)
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_DATA) != 0U))
    {
        ipcStatus = Cy_IPC_Drv_LockAcquire(ipcPtr);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(4U);
        }
        if (OK_STATUS == ret)
        {
            uint32_t wData[2] = { 0xDEADBEEFUL, 0xCAFEBABEUL };
            uint32_t rData[2] = { 0U, 0U };
            Cy_IPC_Drv_WriteDDataValue(ipcPtr, wData);
            Cy_IPC_Drv_ReadDDataValue(ipcPtr, rData);
            #if ERROR_IN_IPC_PPCA_DATA
            rData[0] ^= 0x1U;
            #endif
            if ((rData[0] != wData[0]) || (rData[1] != wData[1]))
            {
                PPCA_SET_FAIL(4U);
            }
        }
        if (OK_STATUS == ret)
        {
            uint32_t wData[2] = { 0x55555555UL, 0xAAAAAAAAUL };
            uint32_t rData[2] = { 0U, 0U };
            Cy_IPC_Drv_WriteDDataValue(ipcPtr, wData);
            Cy_IPC_Drv_ReadDDataValue(ipcPtr, rData);
            #if ERROR_IN_IPC_PPCA_DATA
            rData[1] ^= 0x1U;
            #endif
            if ((rData[0] != wData[0]) || (rData[1] != wData[1]))
            {
                PPCA_SET_FAIL(5U);
            }
        }
        if (OK_STATUS == ret)
        {
            ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
            if (CY_IPC_DRV_SUCCESS != ipcStatus)
            {
                PPCA_SET_FAIL(13U);
            }
        }
    }

    /* ------------------------------------------------------------------
     * GROUP NOTIFY: Notify-to-self loopback + INTR clear-on-write-1
     * (sub-tests 6,7)
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_NOTIFY) != 0U))
    {
        uint32_t wData[2];
        wData[0] = 0x0U;
        wData[1] = 0x100U + ch;

        ipcStatus = Cy_IPC_Drv_LockAcquire(ipcPtr);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(6U);
        }
        if (OK_STATUS == ret)
        {
            Cy_IPC_Drv_WriteDDataValue(ipcPtr, wData);
            PpcaResetCtx(intr);
            Cy_IPC_Drv_AcquireNotify(ipcPtr, PPCA_IPC_MASK(intr));

            if (!PpcaWaitIsr(intr, STL_PPCA_WAIT_TIMEOUT_US))
            {
                PPCA_SET_FAIL(6U);
            }
        }
        if ((OK_STATUS == ret) && ((stlPpcaIpc_ctx[intr].notifyMask & chNotifyMask) == 0U))
        {
            PPCA_SET_FAIL(6U);
        }
        if ((OK_STATUS == ret) &&
            ((Cy_IPC_Drv_ExtractAcquireMask(stlPpcaIpc_ctx[intr].intrAfterClear) & chNotifyMask) != 0U))
        {
            PPCA_SET_FAIL(7U);
        }
        if (OK_STATUS == ret)
        {
            ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
            if (CY_IPC_DRV_SUCCESS != ipcStatus)
            {
                PPCA_SET_FAIL(14U);
            }
        }
    }

    /* ------------------------------------------------------------------
     * GROUP INTR: Masking / INTR_MASKED algebraic consistency / INTR_SET
     * (sub-tests 8,9,10) - no channel lock required
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_INTR) != 0U))
    {
        uint32_t rawIntr;
        uint32_t maskReg;
        uint32_t maskedIntr;

        PpcaResetCtx(intr);
        PpcaClearIntr(intr);
        Cy_IPC_Drv_SetInterruptMask(ipcIntrPtr, 0U, 0U);
        Cy_IPC_Drv_SetInterrupt(ipcIntrPtr, 0U, chNotifyMask);
        Cy_SysLib_DelayUs(STL_PPCA_MASK_POLL_US);

        rawIntr    = Cy_IPC_Drv_GetInterruptStatus(ipcIntrPtr);
        maskReg    = Cy_IPC_Drv_GetInterruptMask(ipcIntrPtr);
        maskedIntr = Cy_IPC_Drv_GetInterruptStatusMasked(ipcIntrPtr);

        if ((Cy_IPC_Drv_ExtractAcquireMask(rawIntr) & chNotifyMask) == 0U)
        {
            PPCA_SET_FAIL(8U);
        }
        if ((OK_STATUS == ret) && (maskedIntr != 0U))
        {
            PPCA_SET_FAIL(8U);
        }
        if (OK_STATUS == ret)
        {
            #if ERROR_IN_IPC_PPCA_MASK
            stlPpcaIpc_ctx[intr].fired = 1U;
            #endif
            if (stlPpcaIpc_ctx[intr].fired != 0U)
            {
                PPCA_SET_FAIL(8U);
            }
        }
        if ((OK_STATUS == ret) && (maskedIntr != (rawIntr & maskReg)))
        {
            PPCA_SET_FAIL(9U);
        }
        /* Restore mask unconditionally - any pending INTR must be unblocked
         * so the ISR fires now (success path) or is harmlessly consumed
         * and cleaned up by the next PpcaTestPair() preamble. */
        PpcaResetCtx(intr);
        Cy_IPC_Drv_SetInterruptMask(ipcIntrPtr, intrRelAll, intrNotifyAll);

        if (OK_STATUS == ret)
        {
            if (!PpcaWaitIsr(intr, STL_PPCA_WAIT_TIMEOUT_US))
            {
                PPCA_SET_FAIL(10U);
            }
        }
        if ((OK_STATUS == ret) &&
            (Cy_IPC_Drv_ExtractAcquireMask(stlPpcaIpc_ctx[intr].intrMasked) !=
             (Cy_IPC_Drv_ExtractAcquireMask(stlPpcaIpc_ctx[intr].intrRaw) & intrNotifyAll)))
        {
            PPCA_SET_FAIL(9U);
        }
    }

    /* ------------------------------------------------------------------
     * GROUP RELEASE_EVT: Release-event self-loopback (sub-test 11)
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_RELEASE_EVT) != 0U))
    {
        ipcStatus = Cy_IPC_Drv_LockAcquire(ipcPtr);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(11U);
        }
        if (OK_STATUS == ret)
        {
            PpcaResetCtx(intr);
            ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, PPCA_IPC_MASK(intr));
            if (CY_IPC_DRV_SUCCESS != ipcStatus)
            {
                PPCA_SET_FAIL(11U);
            }
        }
        if (OK_STATUS == ret)
        {
            if (!PpcaWaitIsr(intr, STL_PPCA_WAIT_TIMEOUT_US))
            {
                PPCA_SET_FAIL(11U);
            }
        }
        if (OK_STATUS == ret)
        {
            #if ERROR_IN_IPC_PPCA_RELEASE
            stlPpcaIpc_ctx[intr].relMask = 0U;
            #endif
            if ((stlPpcaIpc_ctx[intr].relMask & chRelMask) == 0U)
            {
                PPCA_SET_FAIL(11U);
            }
        }
    }

    /* ------------------------------------------------------------------
     * GROUP SILENT_REL: Silent release, no interrupt generated (sub-test 12)
     * ------------------------------------------------------------------ */
    if ((OK_STATUS == ret) && ((testMask & (uint8_t)SELFTEST_IPC_PPCA_SILENT_REL) != 0U))
    {
        ipcStatus = Cy_IPC_Drv_LockAcquire(ipcPtr);
        if (CY_IPC_DRV_SUCCESS != ipcStatus)
        {
            PPCA_SET_FAIL(12U);
        }
        if (OK_STATUS == ret)
        {
            PpcaResetCtx(intr);
            ipcStatus = Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
            if (CY_IPC_DRV_SUCCESS != ipcStatus)
            {
                PPCA_SET_FAIL(12U);
            }
        }
        if (OK_STATUS == ret)
        {
            if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
            {
                PPCA_SET_FAIL(12U);
            }
        }
        if (OK_STATUS == ret)
        {
            Cy_SysLib_DelayUs(STL_PPCA_MASK_POLL_US);
            #if ERROR_IN_IPC_PPCA_SILENT
            stlPpcaIpc_ctx[intr].fired = 1U;
            #endif
            if (stlPpcaIpc_ctx[intr].fired != 0U)
            {
                PPCA_SET_FAIL(12U);
            }
        }
    }

    return ret;
}


/* Restore the customer configuration saved at the start of
 * SelfTest_IPC_PPCA().  Called both on the normal return path and on the
 * early Cy_SysInt_Init() failure path to prevent ISR vector / INTR_MASK
 * leaks regardless of where the function exits. */
static void PpcaRestoreState(void)
{
    uint32_t i;

    /* Disable test ISRs before swapping vectors back so a pending
     * interrupt cannot dispatch to the customer ISR with stale PPCA state. */
    NVIC_DisableIRQ(PPCA_IPC0_INTERRUPT);
    NVIC_DisableIRQ(PPCA_IPC1_INTERRUPT);
    NVIC_DisableIRQ(PPCA_IPC2_INTERRUPT);
    NVIC_DisableIRQ(PPCA_IPC3_INTERRUPT);

    /* Clear any test-generated pending INTR bits now, while the NVIC is
     * silenced.  Without this, a notify/release bit left by a failing sub-test
     * could dispatch a spurious interrupt to the customer ISR the moment the
     * customer mask is written back below. */
    for (i = 0U; i < STL_PPCA_IPC_IRQ_NR; i++)
    {
        PpcaClearIntr(i);
    }

    /* Release any channel locks left from an interrupted test run */
    for (uint32_t ch = 0U; ch < STL_PPCA_IPC_CH_NR; ch++)
    {
        IPC_STRUCT_Type* ipcPtr = Cy_IPC_Drv_GetPPCAIpcBaseAddress(ch);
        if (Cy_IPC_Drv_IsLockAcquired(ipcPtr))
        {
            (void)Cy_IPC_Drv_LockRelease(ipcPtr, 0U);
        }
    }

    /* Restore customer ISR vectors, priorities and INTR_MASK values */
    for (i = 0U; i < STL_PPCA_IPC_IRQ_NR; i++)
    {
        uint32_t  irqNum = (uint32_t)PPCA_IPC0_INTERRUPT + i;
        IRQn_Type irqN = (IRQn_Type)irqNum;
        uint32_t  saved = stlPpcaIpc_savedState.intrMask[i];

        NVIC_SetVector(irqN, stlPpcaIpc_savedState.isrVector[i]);
        NVIC_SetPriority(irqN, stlPpcaIpc_savedState.nvicPriority[i]);

        /* Restore INTR_MASK: split raw register value back into
         * rel (bits 0-15) and notify (bits 16-31 -> shifted to 0-15) fields */
        Cy_IPC_Drv_SetInterruptMask(
            (IPC_INTR_STRUCT_Type*)CY_PPCA_IPC_INTR_STRUCT_PTR(i),
            Cy_IPC_Drv_ExtractReleaseMask(saved),
            Cy_IPC_Drv_ExtractAcquireMask(saved));
    }

    /* Restore NVIC enable state */
    for (i = 0U; i < STL_PPCA_IPC_IRQ_NR; i++)
    {
        if ((stlPpcaIpc_savedState.nvicEnabled & (1UL << i)) != 0U)
        {
            uint32_t irqNum = (uint32_t)PPCA_IPC0_INTERRUPT + i;
            NVIC_EnableIRQ((IRQn_Type)irqNum);
        }
    }
}


/*******************************************************************************
 * Function Name: SelfTest_IPC_PPCA
 ******************************************************************************/
uint8_t SelfTest_IPC_PPCA(uint8_t testMask)
{
    uint8_t ret = OK_STATUS;
    cy_en_sysint_status_t sysint_status;

    /* Runtime guard: the hand-unrolled ISR stubs (PpcaIpcInterrupt0..3)
     * must stay in sync with the number of PPCA IPC interrupt lines. */
    CY_ASSERT(STL_PPCA_IPC_IRQ_NR == 4U);

    /* Reject an empty selection - at least one group must run. */
    if ((testMask & (uint8_t)SELFTEST_IPC_PPCA_ALL) == 0U)
    {
        return ERROR_STATUS;
    }

    /* Save customer configuration before touching any PPCA IPC hardware.
     * Restored unconditionally before this function returns. */
    stlPpcaIpc_savedState.nvicEnabled = 0U;
    for (uint32_t i = 0U; i < STL_PPCA_IPC_IRQ_NR; i++)
    {
        uint32_t  irqNum = (uint32_t)PPCA_IPC0_INTERRUPT + i;
        IRQn_Type irqN = (IRQn_Type)irqNum;
        stlPpcaIpc_savedState.intrMask[i]     =
            Cy_IPC_Drv_GetInterruptMask((IPC_INTR_STRUCT_Type*)CY_PPCA_IPC_INTR_STRUCT_PTR(i));
        stlPpcaIpc_savedState.isrVector[i]    = NVIC_GetVector(irqN);
        stlPpcaIpc_savedState.nvicPriority[i] = NVIC_GetPriority(irqN);
        if (NVIC_GetEnableIRQ(irqN) != 0U)
        {
            stlPpcaIpc_savedState.nvicEnabled |= (1UL << i);
        }
    }

    sysint_status = Cy_SysInt_Init(&stlPpcaIpc_intConfig0, PpcaIpcInterrupt0);
    if (CY_SYSINT_SUCCESS == sysint_status)
    {
        sysint_status = Cy_SysInt_Init(&stlPpcaIpc_intConfig1, PpcaIpcInterrupt1);
    }
    if (CY_SYSINT_SUCCESS == sysint_status)
    {
        sysint_status = Cy_SysInt_Init(&stlPpcaIpc_intConfig2, PpcaIpcInterrupt2);
    }
    if (CY_SYSINT_SUCCESS == sysint_status)
    {
        sysint_status = Cy_SysInt_Init(&stlPpcaIpc_intConfig3, PpcaIpcInterrupt3);
    }
    if (CY_SYSINT_SUCCESS != sysint_status)
    {
        PpcaRestoreState();
        return ERROR_STATUS;
    }

    NVIC_EnableIRQ(PPCA_IPC0_INTERRUPT);
    NVIC_EnableIRQ(PPCA_IPC1_INTERRUPT);
    NVIC_EnableIRQ(PPCA_IPC2_INTERRUPT);
    NVIC_EnableIRQ(PPCA_IPC3_INTERRUPT);

    for (uint32_t ch = 0U; ch < STL_PPCA_IPC_CH_NR; ch++)
    {
        for (uint32_t intr = 0U; intr < STL_PPCA_IPC_IRQ_NR; intr++)
        {
            ret = PpcaTestPair(ch, intr, testMask);
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

    PpcaRestoreState();

    return ret;
}


uint32_t SelfTest_IPC_PPCA_GetFailChannel(void)
{
    return stlPpcaIpc_failCh;
}


uint32_t SelfTest_IPC_PPCA_GetFailIntr(void)
{
    return stlPpcaIpc_failIntr;
}


uint8_t SelfTest_IPC_PPCA_GetFailSubtest(void)
{
    return stlPpcaIpc_failSubtest;
}


#endif /* defined(CY_IP_MXS40PPSS) */

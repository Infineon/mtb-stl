/*******************************************************************************
* File Name: SelfTest_ECC.h
*
* Description:
*  This file provides function prototypes, constants, and parameter values
*  used for ECC self tests.
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
/**
 * \defgroup group_ecc ECC (ECC STL module)
 * \{
 *
 * ECC hardware can correct single-bit (correctable) memory errors and report
 * double-bit (non-correctable) memory errors through SysFault.
 *
 * The STL ECC self-test APIs validate the non-correctable fault-reporting path
 * by injecting ECC corruption and verifying the expected SysFault source.
 *
 * Test sequence used by the APIs:
 * - Configure SysFault mask/interrupt for ECC sources.
 * - Inject ECC corruption into the target memory.
 * - Read corrupted location to trigger ECC check.
 * - Poll fault source and compare with expected non-correctable source.
 *
 * \section group_ecc_more_information More Information
 *
 * API applicability:
 *
 * <table>
 *   <tr><th>API</th><th>Device Family</th><th>Memory under test</th><th>Address requirement</th></tr>
 *   <tr><td>\ref SelfTest_ECC</td><td>XMC</td><td>Flash</td><td>Internal test address is used</td></tr>
 *   <tr><td>\ref SelfTest_ECC_Flash</td><td>PSOC Control</td><td>Flash</td><td>Flash row-aligned address</td></tr>
 *   <tr><td>\ref SelfTest_ECC_Ram</td><td>PSOC Control</td><td>RAM</td><td>Valid SRAM address</td></tr>
 * </table>
 *
 *
 * \defgroup group_ecc_macros Macros
 * \defgroup group_ecc_enums Enumerated Types
 * \defgroup group_ecc_data_structures Data Structures
 * \defgroup group_ecc_functions Functions
 */
#if !defined(SELFTEST_ECC_H)
    #define SELFTEST_ECC_H

#include "cy_pdl.h"
#include "SelfTest_common.h"
#include "SelfTest_ErrorInjection.h"

/**
 * \addtogroup group_ecc_macros
 * \{
 */

/** Legacy one-shot destructive ECC self-tests. This is the default mode. */
#define STL_ECC_MODE_ONE_SHOT                  (0u)

/** Application-managed runtime Flash and RAM ECC configuration, verification,
 *  evidence capture/decoding, and provoke support. */
#define STL_ECC_MODE_RUNTIME     (1u)

/** Selects which ECC implementation is compiled. Override from the application
 *  Makefile, for example DEFINES+=SELFTEST_ECC_MODE=STL_ECC_MODE_RUNTIME. */
#if !defined(SELFTEST_ECC_MODE)
    #define SELFTEST_ECC_MODE            STL_ECC_MODE_ONE_SHOT
#endif

/** \} group_ecc_macros */

#if ((SELFTEST_ECC_MODE != STL_ECC_MODE_ONE_SHOT) && \
    (SELFTEST_ECC_MODE != STL_ECC_MODE_RUNTIME))
    #error "SELFTEST_ECC_MODE must be STL_ECC_MODE_ONE_SHOT or STL_ECC_MODE_RUNTIME"
#endif

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME)
    #if !(defined(SELFTEST_PSC3_FAMILY) && defined(CY_IP_MXS40FLASHC) && \
    defined(CY_IP_MXSRAMC) && defined(CY_IP_MXFAULT))
        #error "STL_ECC_MODE_RUNTIME is supported only on PSOC Control C3 devices"
    #endif
#endif

#if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT) || defined (CY_DOXYGEN))
#include "cy_sysfault.h"

/** \cond INTERNAL */
#define CY_FLASH_SIZE_ROW          512U
#if defined(CY_IP_M7CPUSS)
#define CY_FLASH_ADDR              ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY     0x14
#define CY_ECC_C_ERROR_PARITY      0x5D
#define CY_MEMORY_DATA             0x5A5A5A5A
#define CY_ECC_C_FAULT             CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT            CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif

#if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT)
#define CY_FLASH_ADDR              ((CY_FLASH_SM_SBM_BASE + CY_FLASH_SM_SBM_SIZE) - CY_FLASH_SIZE_ROW)
#define CY_ECC_NC_ERROR_PARITY     0x14
#define CY_ECC_C_ERROR_PARITY      0x5D
#define CY_MEMORY_DATA             0x5A5A5A5A
#define CY_ECC_C_FAULT             CY_SYSFAULT_FLASHC_MAIN_C_ECC
#define CY_ECC_NC_FAULT            CY_SYSFAULT_FLASHC_MAIN_NC_ECC
#endif /* if defined(CY_IP_M4CPUSS) && defined(CY_IP_MXFLASHC_VERSION_ECT) */

#if defined(CY_IP_M33SYSCPUSS)
#define CY_FLASH_ADDR              (CY_FLASH_BASE + (CY_FLASH_SIZE/2))
#define CY_ECC_NC_ERROR_PARITY     0xE1
#define CY_ECC_C_ERROR_PARITY      0x16
#define CY_MEMORY_DATA             0x12341234
#define CY_ECC_C_FAULT             CPUSS_FLASHC_MAIN_C_ECC_MMIO
#define CY_ECC_NC_FAULT            CPUSS_FLASHC_MAIN_NC_ECC_MMIO
#define CY_ECC_C_RAM_FAULT         CPUSS_RAMC0_C_ECC_MMIO
#define CY_ECC_NC_RAM_FAULT        CPUSS_RAMC0_NC_ECC_MMIO
#if (CY_IP_MXSRAMC_INSTANCES == 2U)
#define CY_ECC_C_RAM1_FAULT         CPUSS_RAMC1_C_ECC_MMIO
#define CY_ECC_NC_RAM1_FAULT        CPUSS_RAMC1_NC_ECC_MMIO
#endif
#define CY_RAM_ADDR                (CY_SRAM_BASE + (CY_SRAM_SIZE/2))
#define CY_RAM_ECC_NC_ERROR_PARITY 0x039
#define CY_RAM_ECC_C_ERROR_PARITY  0x070
#define CY_RAM_MEMORY_DATA         0x5A5A5A5A
#define CY_RAM_CORRUPT_DATA        0x7A7A7A7A
#endif /* if (defined(CY_CPU_CORTEX_M33) && (CY_CPU_CORTEX_M33)) */

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME)
/* Flash ECC word geometry: 128-bit data word plus 9-bit SECDED parity. */
#define STL_ECC_FLASH_WORD_SHIFT          (4u)
#define STL_ECC_FLASH_DATA0_WORD_ADDR_Msk (0x007FFFFFu)
#define STL_ECC_FLASH_DATA1_SYNDROME_Msk  (0x000001FFu)

/* RAM ECC word geometry: 32-bit data word plus 7-bit SECDED parity. */
#define STL_ECC_RAM_GRANULE_BYTES          (4u)
#if defined(CY_IP_MXSRAMC_INSTANCES)
    #define STL_ECC_RAM_INSTANCES           (CY_IP_MXSRAMC_INSTANCES)
#else
    #define STL_ECC_RAM_INSTANCES           (1u)
#endif

/** \cond INTERNAL */
#define STL_ECC_RAM_DATA1_SYNDROME_Msk      (0x0000007Fu)
#define STL_ECC_RAM_PROVOKE_DATA            (0x5A5A5A5AuL)
#define STL_ECC_RAM_PROVOKE_FLIP_C          (0x00000001uL)
#define STL_ECC_RAM_PROVOKE_FLIP_NC         (0x00000003uL)
/** \endcond */

/* FAULT_STRUCT DATA0 holds a module-internal offset, so the bus alias the
 * application executes from is not reported by hardware and can be overridden. */
#if !defined(STL_ECC_FLASH_MAIN_BASE)
    #define STL_ECC_FLASH_MAIN_BASE       (CY_FLASH_BASE)
#endif
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */
/** \endcond */

/***************************************
*       Enumerations
***************************************/
/** \addtogroup group_ecc_enums
 * \{
 */
#if (SELFTEST_ECC_MODE == STL_ECC_MODE_ONE_SHOT)
/** Error injection mode */
typedef enum
{
    CY_ECC_NC_ERROR = 1 /**< Non correctable error injection.*/
} cy_en_ecc_error_mode_t;
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_ONE_SHOT) */

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) || defined(CY_DOXYGEN)
/** Classification of a decoded Flash ECC fault record. */
typedef enum
{
    STL_ECC_FLASH_EVENT_NONE            = 0, /**< Not a Flash ECC source, or the record is not valid. */
    STL_ECC_FLASH_EVENT_CORRECTABLE     = 1, /**< FLASHC main_c: correctable-error threshold crossing. */
    STL_ECC_FLASH_EVENT_NON_CORRECTABLE = 2  /**< FLASHC main_nc: non-correctable Flash ECC error. */
} stl_ecc_flash_event_kind_t;

/** Classification of a decoded RAM ECC fault record. */
typedef enum
{
    STL_ECC_RAM_EVENT_NONE            = 0, /**< Not a RAM ECC source, or the record is not valid. */
    STL_ECC_RAM_EVENT_CORRECTABLE     = 1, /**< RAMCn_C_ECC: correctable RAM ECC error. */
    STL_ECC_RAM_EVENT_NON_CORRECTABLE = 2  /**< RAMCn_NC_ECC: non-correctable RAM ECC error. */
} stl_ecc_ram_event_kind_t;

/** Error class requested from the destructive RAM ECC provoke API. */
typedef enum
{
    STL_ECC_RAM_INJECT_CORRECTABLE     = 0, /**< One data-bit mismatch. */
    STL_ECC_RAM_INJECT_NON_CORRECTABLE = 1  /**< Two data-bit mismatch. */
} stl_ecc_ram_inject_mode_t;
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */
/** \}group_ecc_enums */

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) || defined(CY_DOXYGEN)
/***************************************
*       Data Structures
***************************************/
/** \addtogroup group_ecc_data_structures
 * \{
 */
/** Flash ECC runtime configuration contract. */
typedef struct
{
    /** Required configuration: FLASH_CTL.ECC_EN target state. Must be true. */
    bool eccEnable;
    /** Required configuration: FLASH_CTL.RBUS_ERR_SILENT target state. Must be false. */
    bool rbusErrSilent;
    /** When true, FM_CTL.ANA_CTL0.ECC_ENC_DIS is additionally verified to be 0.
     *  The recommended value is true, which requires the caller to run in the
     *  Secure Main CM33 context: the Flash Macro register block is protection
     *  context restricted. An image that cannot read it must pass false,
     *  otherwise this read-only check itself raises a fault. */
    bool checkFlashMacroEncoder;
} stl_ecc_flash_cfg_t;

/** Hardware evidence captured for one FAULT_STRUCT0 record. */
typedef struct
{
    uint32_t faultSource; /**< Fault source index reported by Cy_SysFault_GetErrorSource(). */
    bool     faultValid;  /**< Fault record valid status at capture time. */
    uint32_t data[4];     /**< FAULT_STRUCT0 DATA0 to DATA3 as captured. */
    uint32_t flashCtl;    /**< FLASHC.FLASH_CTL evidence at capture time. */
    uint32_t eccLogir;    /**< FLASHC.ECC_LOGIR evidence at capture time. */
} stl_ecc_flash_snapshot_t;

/** Decoded Flash ECC event. */
typedef struct
{
    stl_ecc_flash_event_kind_t kind;         /**< Event classification. */
    uint32_t                   wordAddress;  /**< DATA0 word address; valid only when kind is not NONE. */
    uint32_t                   offset;       /**< Byte offset of the ECC word in the Flash main region. */
    uint32_t                   address;      /**< STL_ECC_FLASH_MAIN_BASE + offset; the bus alias is not
                                              *   reported by hardware and is a software assumption. */
    uint16_t                   syndrome;     /**< DATA1[8:0] SECDED syndrome. */
    uint16_t                   ecc1Cnt;      /**< ECC_LOGIR.ECC1CNT evidence copied from the snapshot. */
    uint16_t                   eccThreshold; /**< ECC_LOGIR.ECCTHRESHOLD evidence copied from the snapshot. */
} stl_ecc_flash_event_t;

/** Flash ECC event counters. The storage belongs to the application. */
typedef struct
{
    uint32_t              correctable;    /**< Correctable-threshold events counted. Saturates. */
    uint32_t              nonCorrectable; /**< Non-correctable events counted. Saturates. */
    uint32_t              other;          /**< Records of other fault sources counted. Saturates. */
    stl_ecc_flash_event_t last;           /**< Last Flash ECC event; not updated for other sources. */
} stl_ecc_flash_counters_t;

/** RAM ECC runtime configuration contract for one RAMC instance. */
typedef struct
{
    bool eccEnable;    /**< RAMC.ECC_CTL.EN target state. Must be true. */
    bool autoCorrect;  /**< RAMC.ECC_CTL.AUTO_CORRECT target state. Must be true. */
    bool checkEnable;  /**< RAMC.ECC_CTL.CHECK_EN target state. */
} stl_ecc_ram_cfg_t;

/** Hardware evidence captured for one FAULT_STRUCT0 record. */
typedef struct
{
    uint32_t faultSource;                       /**< Source index from Cy_SysFault_GetErrorSource(). */
    bool     faultValid;                        /**< Fault record valid status at capture time. */
    uint32_t data[4];                           /**< FAULT_STRUCT0 DATA0 to DATA3 as captured. */
    uint32_t eccCtl[STL_ECC_RAM_INSTANCES];     /**< RAMCn.ECC_CTL evidence at capture time. */
    uint32_t eccStatus0[STL_ECC_RAM_INSTANCES]; /**< RAMCn.ECC_STATUS0 evidence at capture time. */
    uint32_t eccStatus1[STL_ECC_RAM_INSTANCES]; /**< RAMCn.ECC_STATUS1 evidence at capture time. */
} stl_ecc_ram_snapshot_t;

/** Decoded RAM ECC event. */
typedef struct
{
    stl_ecc_ram_event_kind_t kind;     /**< Event classification. */
    uint32_t                 ramcIdx;  /**< Reporting RAMC instance. */
    uint32_t                 address;  /**< DATA0 as reported, including the bus alias. */
    uint16_t                 syndrome; /**< DATA1[6:0] SECDED syndrome. */
} stl_ecc_ram_event_t;

/** RAM ECC event counters. The storage belongs to the application. */
typedef struct
{
    uint32_t            correctable;    /**< Correctable events counted. Saturates. */
    uint32_t            nonCorrectable; /**< Non-correctable events counted. Saturates. */
    uint32_t            other;          /**< Records of other fault sources counted. Saturates. */
    stl_ecc_ram_event_t last;           /**< Last RAM ECC event; not updated for other sources. */
} stl_ecc_ram_counters_t;
/** \}group_ecc_data_structures */
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */

/***************************************
* Function Prototypes
***************************************/
/**
 * \addtogroup group_ecc_functions
 * \{
 */
#if (SELFTEST_ECC_MODE == STL_ECC_MODE_ONE_SHOT) || defined(CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTest_ECC
****************************************************************************//**
*
* Performs ECC hardware self-test for Flash memory on XMC devices.
*
* The self-test configures SysFault for both correctable and
* non-correctable Flash ECC sources, then injects ECC corruption
* into the internal flash test location and reads that location to
* validate that the non-correctable source is reported.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable Flash ECC source.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it erases and programs the internal flash
* test row. Any previous content at the test address is permanently overwritten.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API is applicable only to XMC devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC(cy_en_ecc_error_mode_t eccErrorMode);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash
****************************************************************************//**
*
* Performs ECC hardware self-test for Flash memory on PSOC Control devices.
*
* The self-test configures SysFault for Flash ECC sources, programs
* conflicting flash data/ECC information to inject corruption, re-enables ECC check,
* and reads the provided flash row to trigger ECC fault reporting.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable Flash ECC source.
*
* \param addr
* Aligned flash row address.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it erases and reprograms the flash row at
* \p addr. Any previous content at that address is permanently overwritten.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API is applicable only to PSOC Control devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);


/*******************************************************************************
* Function Name: SelfTest_ECC_Ram
****************************************************************************//**
*
* Performs ECC hardware self-test for RAM memory on PSOC Control devices.
*
* The self-test configures SysFault for RAM ECC sources, writes baseline
* data with ECC enabled, corrupts data with ECC disabled, re-enables ECC check,
* and reads memory to trigger ECC fault reporting.
*
* On devices with multiple RAMC instances, the API selects the RAMC instance
* based on \p addr and accepts the non-correctable source from the corresponding
* RAMC block.
*
* The API passes when the observed SysFault source matches the expected
* non-correctable RAM ECC source.
*
* \param addr
* RAM address.
*
* \param eccErrorMode
* Error injection mode \ref cy_en_ecc_error_mode_t
*
* \return
*  \ref OK_STATUS (0) - Test passed <br>
*  \ref ERROR_STATUS (1) - Test failed <br>
*
* \note This API is destructive: it overwrites the RAM location at \p addr
* with test data. The caller must ensure \p addr does not hold live application
* data during the test.
* It also changes ECC SysFault configuration and intentionally injects an ECC
* fault. Do not call it while application fault handling cannot tolerate this
* controlled fault injection sequence.
*
* \note
* This API assumes that RAM address range is continuous and does not have gaps
* between address ranges of different RAM controllers
*
* \note
* This API is applicable only to devices that have RAM ECC.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram(uint32_t addr, cy_en_ecc_error_mode_t eccErrorMode);
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_ONE_SHOT) */

#if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) || defined(CY_DOXYGEN)
/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_ConfigVerify
****************************************************************************//**
*
* Verifies that the Flash ECC configuration still matches the expected contract.
*
* Checks that FLASH_CTL.ECC_EN is set, FLASH_CTL.RBUS_ERR_SILENT is cleared,
* and that ECC error injection is neither enabled nor latched. When
* \ref stl_ecc_flash_cfg_t::checkFlashMacroEncoder is true, the Flash Macro
* encoder disable bit is additionally verified to be cleared.
*
* This API performs no register write, so it is intended for periodic runtime
* invocation as a configuration integrity diagnostic. It does not read
* ECC_LOGIR: threshold and counter state are application-owned monitoring
* policy, not a configuration invariant.
*
* \param expected
* Expected configuration. ECC must be enabled and RBUS error silence must be
* disabled.
*
* \return
*  \ref OK_STATUS (0) - Configuration matches <br>
*  \ref ERROR_STATUS (1) - Configuration does not match <br>
*  \ref ERROR_BAD_PARAM (9) - Invalid or unsupported request <br>
*
* \note Reading the Flash Macro register block requires the Secure Main CM33
* context. Pass \ref stl_ecc_flash_cfg_t::checkFlashMacroEncoder as false from
* any image that does not have that access.
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_ConfigVerify(const stl_ecc_flash_cfg_t* expected);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_SnapshotCapture
****************************************************************************//**
*
* Captures the fault record and Flash ECC register evidence for one event.
*
* Intended to be called from the application fault handler before the record is
* acknowledged. The API is read-only: it performs no register write, does not
* clear the fault status or interrupt, does not touch the NVIC, and keeps no
* internal state. Applications with their own fault framework may fill
* \ref stl_ecc_flash_snapshot_t directly instead of calling this function.
*
* \param snapshot
* Destination for the captured evidence.
*
* \return
*  \ref OK_STATUS (0) - Snapshot captured <br>
*  \ref ERROR_BAD_PARAM (9) - snapshot is NULL <br>
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_SnapshotCapture(stl_ecc_flash_snapshot_t* snapshot);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_Decode
****************************************************************************//**
*
* Decodes a captured fault record into a Flash ECC event.
*
* This is a pure function: it reads and writes no register, performs no
* acknowledgement, keeps no internal state, and never blocks. It is reentrant
* and can be called from any context.
*
* A record that belongs to another fault source, or that is not valid, is a
* successfully decoded result reported as \ref STL_ECC_FLASH_EVENT_NONE, so the
* application drain loop needs no error handling for foreign sources.
*
* \param snapshot
* Evidence captured for one fault record.
*
* \param event
* Destination for the decoded event. Always fully written on success.
*
* \return
*  \ref OK_STATUS (0) - Snapshot decoded <br>
*  \ref ERROR_BAD_PARAM (9) - snapshot or event is NULL <br>
*
* \note \ref stl_ecc_flash_event_t::ecc1Cnt and
* \ref stl_ecc_flash_event_t::eccThreshold are reported as evidence only. The
* hardware counter saturates at the threshold, so it is not a lifetime count of
* corrected errors.
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_Decode(const stl_ecc_flash_snapshot_t* snapshot,
                                  stl_ecc_flash_event_t* event);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_Accumulate
****************************************************************************//**
*
* Adds one decoded event to a set of application-owned counters.
*
* This is the runtime monitoring accumulator. It reads and writes no register,
* keeps no internal state, and never blocks. All storage belongs to the caller,
* so an application can keep several independent counter sets, reset them at
* will, and place them in memory that survives a reset.
*
* An event of kind \ref STL_ECC_FLASH_EVENT_NONE increments
* \ref stl_ecc_flash_counters_t::other and leaves
* \ref stl_ecc_flash_counters_t::last untouched, so a drain loop can pass every
* record it sees without filtering foreign fault sources first.
*
* \param counters
* Counter set to update. Ignored when NULL.
*
* \param event
* Decoded event to account for. Ignored when NULL.
*
* \note The counters saturate at their maximum value instead of wrapping, so a
* long-running system never reports fewer events than it observed.
*
* \note The caller owns concurrency. A counter set updated from an interrupt
* handler must not be updated concurrently from another context.
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
void SelfTest_ECC_Flash_Accumulate(stl_ecc_flash_counters_t* counters,
                                   const stl_ecc_flash_event_t* event);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_ProvokeNc
****************************************************************************//**
*
* Provokes a real non-correctable Flash ECC error in one reserved row.
*
* This is the destructive stimulus of the application-managed self test. It
* disables Flash ECC, erases the row, programs a first pattern, programs a
* second pattern into the same row without an intervening erase, re-enables
* Flash ECC and reads the row. The stored data no longer matches the stored
* parity, so the read reports a non-correctable event.
*
* The row content is destroyed. The caller must supply a row that is reserved
* for this purpose and that holds no code or data of the application, and must
* call \ref SelfTest_ECC_Flash_RestoreRow afterwards.
*
* With the baseline configuration of this module the resulting bus error is
* precise and escalates to a CPU exception, so this function does not return to
* its caller unless the application fault handler resumes execution. The
* application owns that policy; the STL installs no handler, configures no fault
* structure, touches no NVIC state and provides no recovery. Cleanup of the row
* is therefore a separate entry point that stays reachable after a reset.
*
* A normal return means either that the application fault handler resumed
* execution, or that no error was reported at all. The two cases are
* distinguished by the evidence the application captured with
* \ref SelfTest_ECC_Flash_SnapshotCapture, not by this return value.
*
* Flash ECC is enabled on return. The rest of the configuration verified by
* \ref SelfTest_ECC_Flash_ConfigVerify is not restored by this function.
*
* \param addr
* Row-aligned address of the reserved row.
*
* \return
*  \ref OK_STATUS (0) - Stimulus applied and the row was read <br>
*  \ref ERROR_STATUS (1) - Flash initialization, erase or first program failed <br>
*
* \note Requires write access to the Flash macro. On a device with a protected
* Secure Main region this means the Secure Main CM33 image.
*
* \note There is no correctable counterpart. Storing a single-bit mismatch needs
* a Flash macro page-latch parity sequence that the PDL does not expose.
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_ProvokeNc(uint32_t addr);

/*******************************************************************************
* Function Name: SelfTest_ECC_Flash_RestoreRow
****************************************************************************//**
*
* Erases the reserved row and programs it with consistent data and parity.
*
* Removes the mismatch stored by \ref SelfTest_ECC_Flash_ProvokeNc so that the
* row can be read again with ECC enabled. Separate from the stimulus because the
* stimulus does not return on a device that escalates the bus error, so cleanup
* has to be reachable from the next boot.
*
* Flash ECC is left in the state the caller had; the application should restore
* its runtime configuration and call \ref SelfTest_ECC_Flash_ConfigVerify.
*
* \param addr
* Row-aligned address of the reserved row.
*
* \return
*  \ref OK_STATUS (0) - Row restored <br>
*  \ref ERROR_STATUS (1) - Flash initialization, erase or program failed <br>
*
* \note This API is applicable only to PSOC Control C3 devices.
*
*******************************************************************************/
uint8_t SelfTest_ECC_Flash_RestoreRow(uint32_t addr);

/*******************************************************************************
* Function Name: SelfTest_ECC_Ram_ConfigVerify
****************************************************************************//**
*
* Verifies the RAM ECC runtime configuration for one RAMC instance without
* writing any hardware register.
*
* \param ramc
* RAMC controller to verify. The pointer must identify a supported main-SRAM
* RAMC instance.
*
* \param expected
* Expected configuration. ECC and automatic correction must be enabled.
*
* \return
*  \ref OK_STATUS (0) - Configuration matches <br>
*  \ref ERROR_STATUS (1) - Configuration does not match <br>
*  \ref ERROR_BAD_PARAM (9) - Invalid request <br>
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram_ConfigVerify(RAMC_Type* ramc, const stl_ecc_ram_cfg_t* expected);

/*******************************************************************************
* Function Name: SelfTest_ECC_Ram_SnapshotCapture
****************************************************************************//**
*
* Captures one FAULT_STRUCT0 record and RAM ECC register evidence. This API is
* read-only; acknowledgement and fault handling remain application-owned.
*
* \param snapshot
* Destination for the captured evidence.
*
* \return
*  \ref OK_STATUS (0) - Snapshot captured <br>
*  \ref ERROR_BAD_PARAM (9) - snapshot is NULL <br>
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram_SnapshotCapture(stl_ecc_ram_snapshot_t* snapshot);

/*******************************************************************************
* Function Name: SelfTest_ECC_Ram_Decode
****************************************************************************//**
*
* Decodes captured RAM ECC evidence. This is a pure function and reports
* unrelated fault sources as \ref STL_ECC_RAM_EVENT_NONE.
*
* \param snapshot
* Captured fault and RAMC evidence.
*
* \param event
* Destination for the decoded RAM ECC event.
*
* \return
*  \ref OK_STATUS (0) - Snapshot decoded <br>
*  \ref ERROR_BAD_PARAM (9) - snapshot or event is NULL <br>
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram_Decode(const stl_ecc_ram_snapshot_t* snapshot,
                                stl_ecc_ram_event_t* event);

/*******************************************************************************
* Function Name: SelfTest_ECC_Ram_Accumulate
****************************************************************************//**
*
* Adds a decoded RAM ECC event to caller-owned saturating counters.
*
* \param counters
* Counter set to update. Ignored when NULL.
*
* \param event
* Decoded event to account for. Ignored when NULL.
*
*******************************************************************************/
void SelfTest_ECC_Ram_Accumulate(stl_ecc_ram_counters_t* counters,
                                 const stl_ecc_ram_event_t* event);

/*******************************************************************************
* Function Name: SelfTest_ECC_Ram_Provoke
****************************************************************************//**
*
* Destructively provokes a RAM ECC event in an application-owned sacrificial
* word, then repairs the word and restores the RAMC configuration. The caller
* supplies the RAMC controller and must ensure the address belongs to it.
*
* The library does not claim the memory is protected while ECC_CTL is 0. The
* caller must prevent DMA and other bus masters from storing to that RAMC while
* the generation-off window is active.
*
* \param ramc
* RAMC controller that owns \p wordAddr. The pointer must identify a supported
* main-SRAM RAMC instance.
*
* \param wordAddr
* 32-bit-aligned address of the application-owned sacrificial word.
*
* \param mode
* Requested correctable or non-correctable ECC error class.
*
* \return
*  \ref OK_STATUS (0) - Stimulus applied and word repaired <br>
*  \ref ERROR_BAD_PARAM (9) - Invalid RAMC pointer, address, or mode <br>
*
*******************************************************************************/
uint8_t SelfTest_ECC_Ram_Provoke(RAMC_Type* ramc,
                                 uint32_t wordAddr,
                                 stl_ecc_ram_inject_mode_t mode);
#endif /* if (SELFTEST_ECC_MODE == STL_ECC_MODE_RUNTIME) */
/** \}group_ecc_functions */

/** \}group_ecc */

#endif /* if (defined (CY_IP_MXS40FAULT) || defined (CY_IP_MXFAULT) || defined (CY_DOXYGEN)) */
#endif /* SELFTEST_ECC_H */

/* [] END OF FILE */

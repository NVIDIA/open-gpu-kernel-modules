
#ifndef _G_KERNEL_GSP_NVOC_H_
#define _G_KERNEL_GSP_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include "g_kernel_gsp_nvoc.h"

#ifndef KERNEL_GSP_H
#define KERNEL_GSP_H

/*!
 * This file provides definitions for all KernelGsp data structures
 * and interfaces.  KernelGsp is responsible for initiating the boot
 * of RM on the GSP core (GSP-RM) and helps facilitate communication
 * between Kernel RM and GSP-RM.
 */

#include "core/core.h"
#include "core/bin_data.h"
#include "gpu/eng_state.h"
#include "gpu/intr/intr_service.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gsp/gsp_static_config.h"
#include "gpu/gsp/gsp_init_args.h"
#include "gpu/gsp/gsp_fw_heap.h"
#include "nv-firmware.h"
#include "nv_sriov_defines.h"
#include "rmRiscvUcode.h"

#include "libos_init_args.h"
#include "gsp_fw_wpr_meta.h"
#include "gsp_fw_sr_meta.h"
#include "liblogdecode.h"

/*!
 * Forward declarations
 */
typedef struct SimAccessBuffer SimAccessBuffer;
typedef struct GSP_FMC_BOOT_PARAMS GSP_FMC_BOOT_PARAMS;

/*!
 * Structure for VBIOS image for early FRTS.
 */
typedef struct KernelGspVbiosImg
{
    NvU8 *pImage;
    NvU32 biosSize;
    NvU32 expansionRomOffset;
} KernelGspVbiosImg;

/*!
 * Variant of KernelGspFlcnUcode representing a non-Boot-from-HS ucode that
 * loads directly without the generic falcon bootloader.
 */
typedef struct KernelGspFlcnUcodeBootDirect
{
    NvU8 *pImage;
    NvU32 size;

    NvU32 imemSize;
    NvU32 imemNsSize;
    NvU32 imemNsPa;
    NvU32 imemSecSize;
    NvU32 imemSecPa;

    NvU32 dataOffset;
    NvU32 dmemSize;
    NvU32 dmemPa;
} KernelGspFlcnUcodeBootDirect;

/*!
 * Variant of KernelGspFlcnUcode representing a non-Boot-from-HS ucode that
 * loads via the generic falcon bootloader.
 */
typedef struct KernelGspFlcnUcodeBootWithLoader
{
    MEMORY_DESCRIPTOR *pCodeMemDesc;
    MEMORY_DESCRIPTOR *pDataMemDesc;

    NvU32 codeOffset;
    NvU32 imemSize;
    NvU32 imemNsSize;
    NvU32 imemNsPa;
    NvU32 imemSecSize;
    NvU32 imemSecPa;
    NvU32 codeEntry;

    NvU32 dataOffset;
    NvU32 dmemSize;
    NvU32 dmemPa;

    // Extra fields used for falcon ucodes from VBIOS
    NvU32 interfaceOffset;
} KernelGspFlcnUcodeBootWithLoader;

/*!
 * Variant of KernelGspFlcnUcode representing a Boot-from-HS ucode.
 */
typedef struct KernelGspFlcnUcodeBootFromHs
{
    MEMORY_DESCRIPTOR *pUcodeMemDesc;
    NvU32 size;

    NvU32 codeOffset;
    NvU32 imemSize;
    NvU32 imemPa;
    NvU32 imemVa;

    NvU32 dataOffset;
    NvU32 dmemSize;
    NvU32 dmemPa;
    NvU32 dmemVa;

    NvU32 hsSigDmemAddr;
    NvU32 ucodeId;
    NvU32 engineIdMask;

    // Extra fields used for falcon ucodes from VBIOS
    NvU32 *pSignatures;
    NvU32 signaturesTotalSize;  // size of buffer pointed by pSignatures
    NvU32 sigSize;  // size of one signature
    NvU32 sigCount;

    NvU32 vbiosSigVersions;
    NvU32 interfaceOffset;
} KernelGspFlcnUcodeBootFromHs;

/*!
 * Type of KernelGspFlcnUcode. Used as tag in tagged union KernelGspFlcnUcode.
 * Affects how the ucode is loaded/booted.
 */
typedef enum KernelGspFlcnUcodeBootType
{
    KGSP_FLCN_UCODE_BOOT_DIRECT,
    KGSP_FLCN_UCODE_BOOT_WITH_LOADER,
    KGSP_FLCN_UCODE_BOOT_FROM_HS
} KernelGspFlcnUcodeBootType;

/*!
 * RPC processing trigger
 */
typedef enum KernelGspRpcEventHandlerContext
{
    KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL, // called after issuing an RPC
    KGSP_RPC_EVENT_HANDLER_CONTEXT_POLL_BOOTUP, // called from kgspWaitForRmInitDone
    KGSP_RPC_EVENT_HANDLER_CONTEXT_INTERRUPT // called in bottom-half interrupt path
} KernelGspRpcEventHandlerContext;

/*!
 * Tagged union of falcon ucode variants used by early FRTS and GSP-RM boot.
 */
typedef struct KernelGspFlcnUcode
{
    KernelGspFlcnUcodeBootType bootType;
    union
    {
        KernelGspFlcnUcodeBootDirect ucodeBootDirect;
        KernelGspFlcnUcodeBootWithLoader ucodeBootWithLoader;
        KernelGspFlcnUcodeBootFromHs ucodeBootFromHs;
    };
} KernelGspFlcnUcode;

/*!
 * Structure for used for executing a FWSEC command
 */
typedef struct KernelGspPreparedFwsecCmd
{
    KernelGspFlcnUcode *pFwsecUcode;
    NvU32 cmd;
    NvU64 frtsOffset;
} KernelGspPreparedFwsecCmd;

/*!
 * GSP-RM source when running in Emulated/Simulated RISCV environment is
 * extremely slow, so we need a factor (X) to scale timeouts by.
 */
#define GSP_SCALE_TIMEOUT_EMU_SIM  2500

/*!
 * Size of libos init arguments packet.
 */
#define LIBOS_INIT_ARGUMENTS_SIZE       0x1000

/*!
 * Structure for passing GSP-RM firmware data
 */
typedef struct GSP_FIRMWARE
{
    const void *pBuf;           // buffer holding the firmware (ucode)
    NvU32       size;           // size of the firmware
    const void *pImageData;     // points to the GSP FW image start inside the pBuf buffer
    NvU64       imageSize;      // GSP FW image size inside the pBuf buffer
    const void *pSignatureData; // points to the GSP FW signature start inside the pBuf buffer
    NvU64       signatureSize;  // GSP FW signature size inside the pBuf buffer
    const void *pLogElf;        // firmware logging section and symbol information to decode logs
    NvU32       logElfSize;     // size of the gsp log elf binary
} GSP_FIRMWARE;

/*!
 * Known ELF section names (or name prefixes) of gsp_*.bin or gsp_log_*.bin.
 */
#define GSP_VERSION_SECTION_NAME           ".fwversion"
#define GSP_IMAGE_SECTION_NAME             ".fwimage"
#define GSP_LOGGING_SECTION_NAME           ".fwlogging"
#define GSP_SIGNATURE_SECTION_NAME_PREFIX  ".fwsignature_"
#define GSP_CC_SIGNATURE_SECTION_NAME_PREFIX  ".fwsignature_cc_"

/*!
 * GSP Notify op infra. Used by UVM in HCC mode.
 */
#define GSP_NOTIFY_OP_RESERVED_OPCODE                                 0
// Request fault buffer flush.
#define GSP_NOTIFY_OP_FLUSH_REPLAYABLE_FAULT_BUFFER_OPCODE            1
#define GSP_NOTIFY_OP_FLUSH_REPLAYABLE_FAULT_BUFFER_VALID_ARGC        1
#define GSP_NOTIFY_OP_FLUSH_REPLAYABLE_FAULT_BUFFER_FLUSH_MODE_ARGIDX 0
// Fault on prefetch toggle.
#define GSP_NOTIFY_OP_TOGGLE_FAULT_ON_PREFETCH_OPCODE                 2
#define GSP_NOTIFY_OP_TOGGLE_FAULT_ON_PREFETCH_VALID_ARGC             1
#define GSP_NOTIFY_OP_TOGGLE_FAULT_ON_PREFETCH_EN_ARGIDX              0
// Always keep this as the last defined value
#define GSP_NOTIFY_OP_OPCODE_MAX                                      3
#define GSP_NOTIFY_OP_NO_ARGUMENTS                                    0
#define GSP_NOTIFY_OP_MAX_ARGUMENT_COUNT                              1
typedef struct NotifyOpSharedSurface
{
    NvU32 inUse;                                    // 0 - signals free, 1 - signals busy
                                                    // An atomic swap is issued in a loop over this field from the
                                                    // KernelRM side to synchronize access to the shared notify op resource.
                                                    // Once the operation finishes the exiting thread flips the value back to 0.
    NvU32 seqNum;                                   // Read by KernelRM; Written by GSP. Provides synchronization so the
                                                    // requester knows when the operation is finished by GSP.
    NvU32 opCode;                                   // Written by KernelRM; Read by GSP. Specifies the operation to be performed.
    NvU32 status;                                   // Read by KernelRM; Written by GSP. Specifies the status of the operation.
                                                    // Becomes valid for the current operation after seqNum is incremented.
    NvU32 argc;                                     // Written by KernelRM; Read by GSP. Specifies the number of arguments.
    NvU32 args[GSP_NOTIFY_OP_MAX_ARGUMENT_COUNT];   // Written by KernelRM; Read by GSP. Contains a list of NvU32 args used
                                                    // by the operation.
} NotifyOpSharedSurface;

/*!
 * Index into libosLogDecode array.
 */
enum
{
    LOGIDX_INIT,
    LOGIDX_INTR,
    LOGIDX_RM,
    LOGIDX_KERNEL,
    LOGIDX_SIZE
};

/*!
 * LIBOS task logging.
 */
typedef struct
{
    /* Memory for task logging */
    MEMORY_DESCRIPTOR                  *pTaskLogDescriptor;
    NvU64                              *pTaskLogBuffer;
    NvP64                               pTaskLogMappingPriv;
    NvU64                               id8;
} RM_LIBOS_LOG_MEM;

/*!
 * KernelGsp object definition
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif

struct MESSAGE_QUEUE_COLLECTION;



struct KernelGsp {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct KernelFalcon __nvoc_base_KernelFalcon;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct CrashCatEngine *__nvoc_pbase_CrashCatEngine;    // crashcatEngine super^3
    struct KernelCrashCatEngine *__nvoc_pbase_KernelCrashCatEngine;    // kcrashcatEngine super^2
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;    // kflcn super
    struct KernelGsp *__nvoc_pbase_KernelGsp;    // kgsp

    // Vtable with 82 per-object function pointers
    NV_STATUS (*__kgspConstructEngine__)(struct OBJGPU *, struct KernelGsp * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgspStateInitLocked__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kgspRegisterIntrService__)(struct OBJGPU *, struct KernelGsp * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__kgspServiceInterrupt__)(struct OBJGPU *, struct KernelGsp * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    void (*__kgspConfigureFalcon__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NvBool (*__kgspIsDebugModeEnabled__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kgspAllocBootArgs__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    void (*__kgspFreeBootArgs__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    void (*__kgspProgramLibosBootArgsAddr__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgspSetCmdQueueHead__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kgspPrepareForBootstrap__)(struct OBJGPU *, struct KernelGsp * /*this*/, GSP_FIRMWARE *);  // halified (3 hals) body
    NV_STATUS (*__kgspBootstrap__)(struct OBJGPU *, struct KernelGsp * /*this*/, GSP_FIRMWARE *);  // halified (3 hals) body
    void (*__kgspGetGspRmBootUcodeStorage__)(struct OBJGPU *, struct KernelGsp * /*this*/, BINDATA_STORAGE **, BINDATA_STORAGE **);  // halified (5 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveGspRmBoot__)(struct KernelGsp * /*this*/);  // halified (7 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveConcatenatedFMCDesc__)(struct KernelGsp * /*this*/);  // halified (3 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveConcatenatedFMC__)(struct KernelGsp * /*this*/);  // halified (3 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveGspRmFmcGfwDebugSigned__)(struct KernelGsp * /*this*/);  // halified (4 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveGspRmFmcGfwProdSigned__)(struct KernelGsp * /*this*/);  // halified (4 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__)(struct KernelGsp * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kgspCalculateFbLayout__)(struct OBJGPU *, struct KernelGsp * /*this*/, GSP_FIRMWARE *);  // halified (3 hals) body
    NvU32 (*__kgspGetNonWprHeapSize__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteSequencerCommand__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32, NvU32 *, NvU32);  // halified (3 hals) body
    NvU32 (*__kgspReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kgspResetHw__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual halified (4 hals) override (kflcn) base (kflcn) body
    NvBool (*__kgspHealthCheck__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NvU32 (*__kgspService__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NvBool (*__kgspIsWpr2Up__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NvU32 (*__kgspGetFrtsSize__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NvU64 (*__kgspGetPrescrubbedTopFbSize__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__kgspExtractVbiosFromRom__)(struct OBJGPU *, struct KernelGsp * /*this*/, KernelGspVbiosImg **);  // halified (3 hals) body
    NV_STATUS (*__kgspPrepareForFwsecFrts__)(struct OBJGPU *, struct KernelGsp * /*this*/, KernelGspFlcnUcode *, const NvU64, KernelGspPreparedFwsecCmd *);  // halified (3 hals) body
    NV_STATUS (*__kgspPrepareForFwsecSb__)(struct OBJGPU *, struct KernelGsp * /*this*/, KernelGspFlcnUcode *, KernelGspPreparedFwsecCmd *);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteFwsec__)(struct OBJGPU *, struct KernelGsp * /*this*/, KernelGspPreparedFwsecCmd *);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteScrubberIfNeeded__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteBooterLoad__)(struct OBJGPU *, struct KernelGsp * /*this*/, const NvU64);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteBooterUnloadIfNeeded__)(struct OBJGPU *, struct KernelGsp * /*this*/, const NvU64);  // halified (3 hals) body
    NV_STATUS (*__kgspExecuteHsFalcon__)(struct OBJGPU *, struct KernelGsp * /*this*/, KernelGspFlcnUcode *, struct KernelFalcon *, NvU32 *, NvU32 *);  // halified (4 hals) body
    NV_STATUS (*__kgspWaitForProcessorSuspend__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgspSavePowerMgmtState__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgspRestorePowerMgmtState__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    void (*__kgspFreeSuspendResumeData__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kgspWaitForGfwBootOk__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveBooterLoadUcode__)(struct KernelGsp * /*this*/);  // halified (7 hals) body
    const BINDATA_ARCHIVE * (*__kgspGetBinArchiveBooterUnloadUcode__)(struct KernelGsp * /*this*/);  // halified (7 hals) body
    NvU64 (*__kgspGetMinWprHeapSizeMB__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NvU64 (*__kgspGetMaxWprHeapSizeMB__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NvU32 (*__kgspGetFwHeapParamOsCarveoutSize__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kgspInitVgpuPartitionLogging__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32, NvU64, NvU64, NvU64, NvU64, NvU64, NvU64);  // halified (3 hals) body
    NV_STATUS (*__kgspFreeVgpuPartitionLogging__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // halified (3 hals) body
    const char * (*__kgspGetSignatureSectionNamePrefix__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__kgspSetupGspFmcArgs__)(struct OBJGPU *, struct KernelGsp * /*this*/, GSP_FIRMWARE *);  // halified (3 hals) body
    void (*__kgspReadEmem__)(struct KernelGsp * /*this*/, NvU64, NvU64, void *);  // virtual halified (2 hals) override (kcrashcatEngine) base (kflcn) body
    NV_STATUS (*__kgspIssueNotifyOp__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32, NvU32 *, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kgspCheckGspRmCcCleanup__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // halified (3 hals) body
    void (*__kgspInitMissing__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePreInitLocked__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStateInitUnlocked__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePreLoad__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStateLoad__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePostLoad__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePreUnload__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStateUnload__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspStatePostUnload__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kgspStateDestroy__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgspIsPresent__)(struct OBJGPU *, struct KernelGsp * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgspClearInterrupt__)(struct OBJGPU *, struct KernelGsp * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NV_STATUS (*__kgspServiceNotificationInterrupt__)(struct OBJGPU *, struct KernelGsp * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NvU32 (*__kgspRegRead__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    void (*__kgspRegWrite__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32, NvU32);  // virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    NvU32 (*__kgspMaskDmemAddr__)(struct OBJGPU *, struct KernelGsp * /*this*/, NvU32);  // virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    NvBool (*__kgspConfigured__)(struct KernelGsp * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspUnload__)(struct KernelGsp * /*this*/);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspVprintf__)(struct KernelGsp * /*this*/, NvBool, const char *, va_list);  // virtual inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__kgspPriRead__)(struct KernelGsp * /*this*/, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspPriWrite__)(struct KernelGsp * /*this*/, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void * (*__kgspMapBufferDescriptor__)(struct KernelGsp * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspUnmapBufferDescriptor__)(struct KernelGsp * /*this*/, CrashCatBufferDescriptor *);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspSyncBufferDescriptor__)(struct KernelGsp * /*this*/, CrashCatBufferDescriptor *, NvU32, NvU32);  // virtual inherited (kcrashcatEngine) base (kflcn)
    void (*__kgspReadDmem__)(struct KernelGsp * /*this*/, NvU32, NvU32, void *);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    const NvU32 * (*__kgspGetScratchOffsets__)(struct KernelGsp * /*this*/, NV_CRASHCAT_SCRATCH_GROUP_ID);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    NvU32 (*__kgspGetWFL0Offset__)(struct KernelGsp * /*this*/);  // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)

    // Data members
    struct MESSAGE_QUEUE_COLLECTION *pMQCollection;
    struct OBJRPC *pRpc;
    char vbiosVersionStr[16];
    KernelGspFlcnUcode *pFwsecUcode;
    KernelGspFlcnUcode *pScrubberUcode;
    KernelGspFlcnUcode *pBooterLoadUcode;
    KernelGspFlcnUcode *pBooterUnloadUcode;
    MEMORY_DESCRIPTOR *pWprMetaDescriptor;
    GspFwWprMeta *pWprMeta;
    NvP64 pWprMetaMappingPriv;
    KernelGspPreparedFwsecCmd *pPreparedFwsecCmd;
    MEMORY_DESCRIPTOR *pSRMetaDescriptor;
    MEMORY_DESCRIPTOR *pSRRadix3Descriptor;
    MEMORY_DESCRIPTOR *pGspFmcArgumentsDescriptor;
    GSP_FMC_BOOT_PARAMS *pGspFmcArgumentsCached;
    NvP64 pGspFmcArgumentsMappingPriv;
    MEMORY_DESCRIPTOR *pLibosInitArgumentsDescriptor;
    LibosMemoryRegionInitArgument *pLibosInitArgumentsCached;
    NvP64 pLibosInitArgumentsMappingPriv;
    MEMORY_DESCRIPTOR *pGspArgumentsDescriptor;
    GSP_ARGUMENTS_CACHED *pGspArgumentsCached;
    NvP64 pGspArgumentsMappingPriv;
    MEMORY_DESCRIPTOR *pGspRmBootUcodeMemdesc;
    NvP64 pGspRmBootUcodeMemdescPriv;
    NvU32 gspRmBootUcodeSize;
    NvU8 *pGspRmBootUcodeImage;
    RM_RISCV_UCODE_DESC *pGspRmBootUcodeDesc;
    MEMORY_DESCRIPTOR *pGspUCodeRadix3Descriptor;
    MEMORY_DESCRIPTOR *pSignatureMemdesc;
    LIBOS_LOG_DECODE logDecode;
    LIBOS_LOG_DECODE logDecodeVgpuPartition[32];
    RM_LIBOS_LOG_MEM rmLibosLogMem[4];
    RM_LIBOS_LOG_MEM gspPluginInitTaskLogMem[32];
    RM_LIBOS_LOG_MEM gspPluginVgpuTaskLogMem[32];
    RM_LIBOS_LOG_MEM libosKernelLogMem[32];
    NvBool bHasVgpuLogs;
    void *pLogElf;
    NvU64 logElfDataSize;
    PORT_MUTEX *pNvlogFlushMtx;
    NvBool bLibosLogsPollingEnabled;
    NvU8 bootAttempts;
    NvBool bInInit;
    NvBool bInLockdown;
    NvBool bPollingForRpcResponse;
    NvBool bFatalError;
    MEMORY_DESCRIPTOR *pMemDesc_simAccessBuf;
    SimAccessBuffer *pSimAccessBuf;
    NvP64 pSimAccessBufPriv;
    MEMORY_DESCRIPTOR *pNotifyOpSurfMemDesc;
    NotifyOpSharedSurface *pNotifyOpSurf;
    NvP64 pNotifyOpSurfPriv;
    MEMORY_DESCRIPTOR *pProfilerSamplesMD;
    void *pProfilerSamplesMDPriv;
    void *pProfilerSamples;
    GspStaticConfigInfo gspStaticInfo;
    NvBool bPartitionedFmc;
    NvBool bScrubberUcodeSupported;
    NvU32 fwHeapParamBaseSize;
    NvBool bBootGspRmWithBoostClocks;
    NvU8 ememPort;
};

#ifndef __NVOC_CLASS_KernelGsp_TYPEDEF__
#define __NVOC_CLASS_KernelGsp_TYPEDEF__
typedef struct KernelGsp KernelGsp;
#endif /* __NVOC_CLASS_KernelGsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGsp
#define __nvoc_class_id_KernelGsp 0x311d4e
#endif /* __nvoc_class_id_KernelGsp */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;

#define __staticCast_KernelGsp(pThis) \
    ((pThis)->__nvoc_pbase_KernelGsp)

#ifdef __nvoc_kernel_gsp_h_disabled
#define __dynamicCast_KernelGsp(pThis) ((KernelGsp*)NULL)
#else //__nvoc_kernel_gsp_h_disabled
#define __dynamicCast_KernelGsp(pThis) \
    ((KernelGsp*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGsp)))
#endif //__nvoc_kernel_gsp_h_disabled

// Property macros
#define PDB_PROP_KGSP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGSP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp**, Dynamic*, NvU32);
#define __objCreate_KernelGsp(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGsp((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kgspConstructEngine_FNPTR(pKernelGsp) pKernelGsp->__kgspConstructEngine__
#define kgspConstructEngine(pGpu, pKernelGsp, arg3) kgspConstructEngine_DISPATCH(pGpu, pKernelGsp, arg3)
#define kgspStateInitLocked_FNPTR(pKernelGsp) pKernelGsp->__kgspStateInitLocked__
#define kgspStateInitLocked(pGpu, pKernelGsp) kgspStateInitLocked_DISPATCH(pGpu, pKernelGsp)
#define kgspRegisterIntrService_FNPTR(pKernelGsp) pKernelGsp->__kgspRegisterIntrService__
#define kgspRegisterIntrService(pGpu, pKernelGsp, pRecords) kgspRegisterIntrService_DISPATCH(pGpu, pKernelGsp, pRecords)
#define kgspServiceInterrupt_FNPTR(pKernelGsp) pKernelGsp->__kgspServiceInterrupt__
#define kgspServiceInterrupt(pGpu, pKernelGsp, pParams) kgspServiceInterrupt_DISPATCH(pGpu, pKernelGsp, pParams)
#define kgspConfigureFalcon_FNPTR(pKernelGsp) pKernelGsp->__kgspConfigureFalcon__
#define kgspConfigureFalcon(pGpu, pKernelGsp) kgspConfigureFalcon_DISPATCH(pGpu, pKernelGsp)
#define kgspConfigureFalcon_HAL(pGpu, pKernelGsp) kgspConfigureFalcon_DISPATCH(pGpu, pKernelGsp)
#define kgspIsDebugModeEnabled_FNPTR(pKernelGsp) pKernelGsp->__kgspIsDebugModeEnabled__
#define kgspIsDebugModeEnabled(pGpu, pKernelGsp) kgspIsDebugModeEnabled_DISPATCH(pGpu, pKernelGsp)
#define kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp) kgspIsDebugModeEnabled_DISPATCH(pGpu, pKernelGsp)
#define kgspAllocBootArgs_FNPTR(pKernelGsp) pKernelGsp->__kgspAllocBootArgs__
#define kgspAllocBootArgs(pGpu, pKernelGsp) kgspAllocBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspAllocBootArgs_HAL(pGpu, pKernelGsp) kgspAllocBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeBootArgs_FNPTR(pKernelGsp) pKernelGsp->__kgspFreeBootArgs__
#define kgspFreeBootArgs(pGpu, pKernelGsp) kgspFreeBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeBootArgs_HAL(pGpu, pKernelGsp) kgspFreeBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspProgramLibosBootArgsAddr_FNPTR(pKernelGsp) pKernelGsp->__kgspProgramLibosBootArgsAddr__
#define kgspProgramLibosBootArgsAddr(pGpu, pKernelGsp) kgspProgramLibosBootArgsAddr_DISPATCH(pGpu, pKernelGsp)
#define kgspProgramLibosBootArgsAddr_HAL(pGpu, pKernelGsp) kgspProgramLibosBootArgsAddr_DISPATCH(pGpu, pKernelGsp)
#define kgspSetCmdQueueHead_FNPTR(pKernelGsp) pKernelGsp->__kgspSetCmdQueueHead__
#define kgspSetCmdQueueHead(pGpu, pKernelGsp, queueIdx, value) kgspSetCmdQueueHead_DISPATCH(pGpu, pKernelGsp, queueIdx, value)
#define kgspSetCmdQueueHead_HAL(pGpu, pKernelGsp, queueIdx, value) kgspSetCmdQueueHead_DISPATCH(pGpu, pKernelGsp, queueIdx, value)
#define kgspPrepareForBootstrap_FNPTR(pKernelGsp) pKernelGsp->__kgspPrepareForBootstrap__
#define kgspPrepareForBootstrap(pGpu, pKernelGsp, pGspFw) kgspPrepareForBootstrap_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspPrepareForBootstrap_HAL(pGpu, pKernelGsp, pGspFw) kgspPrepareForBootstrap_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspBootstrap_FNPTR(pKernelGsp) pKernelGsp->__kgspBootstrap__
#define kgspBootstrap(pGpu, pKernelGsp, pGspFw) kgspBootstrap_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspBootstrap_HAL(pGpu, pKernelGsp, pGspFw) kgspBootstrap_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspGetGspRmBootUcodeStorage_FNPTR(pKernelGsp) pKernelGsp->__kgspGetGspRmBootUcodeStorage__
#define kgspGetGspRmBootUcodeStorage(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc) kgspGetGspRmBootUcodeStorage_DISPATCH(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc)
#define kgspGetGspRmBootUcodeStorage_HAL(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc) kgspGetGspRmBootUcodeStorage_DISPATCH(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc)
#define kgspGetBinArchiveGspRmBoot_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveGspRmBoot__
#define kgspGetBinArchiveGspRmBoot(pKernelGsp) kgspGetBinArchiveGspRmBoot_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmBoot_HAL(pKernelGsp) kgspGetBinArchiveGspRmBoot_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveConcatenatedFMCDesc_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveConcatenatedFMCDesc__
#define kgspGetBinArchiveConcatenatedFMCDesc(pKernelGsp) kgspGetBinArchiveConcatenatedFMCDesc_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveConcatenatedFMCDesc_HAL(pKernelGsp) kgspGetBinArchiveConcatenatedFMCDesc_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveConcatenatedFMC_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveConcatenatedFMC__
#define kgspGetBinArchiveConcatenatedFMC(pKernelGsp) kgspGetBinArchiveConcatenatedFMC_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveConcatenatedFMC_HAL(pKernelGsp) kgspGetBinArchiveConcatenatedFMC_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwDebugSigned_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__
#define kgspGetBinArchiveGspRmFmcGfwDebugSigned(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwDebugSigned_HAL(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwProdSigned_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwProdSigned__
#define kgspGetBinArchiveGspRmFmcGfwProdSigned(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwProdSigned_HAL(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmCcFmcGfwProdSigned_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__
#define kgspGetBinArchiveGspRmCcFmcGfwProdSigned(pKernelGsp) kgspGetBinArchiveGspRmCcFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmCcFmcGfwProdSigned_HAL(pKernelGsp) kgspGetBinArchiveGspRmCcFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspCalculateFbLayout_FNPTR(pKernelGsp) pKernelGsp->__kgspCalculateFbLayout__
#define kgspCalculateFbLayout(pGpu, pKernelGsp, pGspFw) kgspCalculateFbLayout_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspCalculateFbLayout_HAL(pGpu, pKernelGsp, pGspFw) kgspCalculateFbLayout_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspGetNonWprHeapSize_FNPTR(pKernelGsp) pKernelGsp->__kgspGetNonWprHeapSize__
#define kgspGetNonWprHeapSize(pGpu, pKernelGsp) kgspGetNonWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetNonWprHeapSize_HAL(pGpu, pKernelGsp) kgspGetNonWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteSequencerCommand_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteSequencerCommand__
#define kgspExecuteSequencerCommand(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize) kgspExecuteSequencerCommand_DISPATCH(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize)
#define kgspExecuteSequencerCommand_HAL(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize) kgspExecuteSequencerCommand_DISPATCH(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize)
#define kgspReadUcodeFuseVersion_FNPTR(pKernelGsp) pKernelGsp->__kgspReadUcodeFuseVersion__
#define kgspReadUcodeFuseVersion(pGpu, pKernelGsp, ucodeId) kgspReadUcodeFuseVersion_DISPATCH(pGpu, pKernelGsp, ucodeId)
#define kgspReadUcodeFuseVersion_HAL(pGpu, pKernelGsp, ucodeId) kgspReadUcodeFuseVersion_DISPATCH(pGpu, pKernelGsp, ucodeId)
#define kgspResetHw_FNPTR(pKernelGsp) pKernelGsp->__kgspResetHw__
#define kgspResetHw(pGpu, pKernelGsp) kgspResetHw_DISPATCH(pGpu, pKernelGsp)
#define kgspResetHw_HAL(pGpu, pKernelGsp) kgspResetHw_DISPATCH(pGpu, pKernelGsp)
#define kgspHealthCheck_FNPTR(pKernelGsp) pKernelGsp->__kgspHealthCheck__
#define kgspHealthCheck(pGpu, pKernelGsp) kgspHealthCheck_DISPATCH(pGpu, pKernelGsp)
#define kgspHealthCheck_HAL(pGpu, pKernelGsp) kgspHealthCheck_DISPATCH(pGpu, pKernelGsp)
#define kgspService_FNPTR(pKernelGsp) pKernelGsp->__kgspService__
#define kgspService(pGpu, pKernelGsp) kgspService_DISPATCH(pGpu, pKernelGsp)
#define kgspService_HAL(pGpu, pKernelGsp) kgspService_DISPATCH(pGpu, pKernelGsp)
#define kgspIsWpr2Up_FNPTR(pKernelGsp) pKernelGsp->__kgspIsWpr2Up__
#define kgspIsWpr2Up(pGpu, pKernelGsp) kgspIsWpr2Up_DISPATCH(pGpu, pKernelGsp)
#define kgspIsWpr2Up_HAL(pGpu, pKernelGsp) kgspIsWpr2Up_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFrtsSize_FNPTR(pKernelGsp) pKernelGsp->__kgspGetFrtsSize__
#define kgspGetFrtsSize(pGpu, pKernelGsp) kgspGetFrtsSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFrtsSize_HAL(pGpu, pKernelGsp) kgspGetFrtsSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetPrescrubbedTopFbSize_FNPTR(pKernelGsp) pKernelGsp->__kgspGetPrescrubbedTopFbSize__
#define kgspGetPrescrubbedTopFbSize(pGpu, pKernelGsp) kgspGetPrescrubbedTopFbSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetPrescrubbedTopFbSize_HAL(pGpu, pKernelGsp) kgspGetPrescrubbedTopFbSize_DISPATCH(pGpu, pKernelGsp)
#define kgspExtractVbiosFromRom_FNPTR(pKernelGsp) pKernelGsp->__kgspExtractVbiosFromRom__
#define kgspExtractVbiosFromRom(pGpu, pKernelGsp, ppVbiosImg) kgspExtractVbiosFromRom_DISPATCH(pGpu, pKernelGsp, ppVbiosImg)
#define kgspExtractVbiosFromRom_HAL(pGpu, pKernelGsp, ppVbiosImg) kgspExtractVbiosFromRom_DISPATCH(pGpu, pKernelGsp, ppVbiosImg)
#define kgspPrepareForFwsecFrts_FNPTR(pKernelGsp) pKernelGsp->__kgspPrepareForFwsecFrts__
#define kgspPrepareForFwsecFrts(pGpu, pKernelGsp, pFwsecUcode, frtsOffset, preparedCmd) kgspPrepareForFwsecFrts_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, frtsOffset, preparedCmd)
#define kgspPrepareForFwsecFrts_HAL(pGpu, pKernelGsp, pFwsecUcode, frtsOffset, preparedCmd) kgspPrepareForFwsecFrts_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, frtsOffset, preparedCmd)
#define kgspPrepareForFwsecSb_FNPTR(pKernelGsp) pKernelGsp->__kgspPrepareForFwsecSb__
#define kgspPrepareForFwsecSb(pGpu, pKernelGsp, pFwsecUcode, preparedCmd) kgspPrepareForFwsecSb_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, preparedCmd)
#define kgspPrepareForFwsecSb_HAL(pGpu, pKernelGsp, pFwsecUcode, preparedCmd) kgspPrepareForFwsecSb_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, preparedCmd)
#define kgspExecuteFwsec_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteFwsec__
#define kgspExecuteFwsec(pGpu, pKernelGsp, preparedCmd) kgspExecuteFwsec_DISPATCH(pGpu, pKernelGsp, preparedCmd)
#define kgspExecuteFwsec_HAL(pGpu, pKernelGsp, preparedCmd) kgspExecuteFwsec_DISPATCH(pGpu, pKernelGsp, preparedCmd)
#define kgspExecuteScrubberIfNeeded_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteScrubberIfNeeded__
#define kgspExecuteScrubberIfNeeded(pGpu, pKernelGsp) kgspExecuteScrubberIfNeeded_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteScrubberIfNeeded_HAL(pGpu, pKernelGsp) kgspExecuteScrubberIfNeeded_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteBooterLoad_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteBooterLoad__
#define kgspExecuteBooterLoad(pGpu, pKernelGsp, sysmemAddrOfData) kgspExecuteBooterLoad_DISPATCH(pGpu, pKernelGsp, sysmemAddrOfData)
#define kgspExecuteBooterLoad_HAL(pGpu, pKernelGsp, sysmemAddrOfData) kgspExecuteBooterLoad_DISPATCH(pGpu, pKernelGsp, sysmemAddrOfData)
#define kgspExecuteBooterUnloadIfNeeded_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteBooterUnloadIfNeeded__
#define kgspExecuteBooterUnloadIfNeeded(pGpu, pKernelGsp, sysmemAddrOfSuspendResumeData) kgspExecuteBooterUnloadIfNeeded_DISPATCH(pGpu, pKernelGsp, sysmemAddrOfSuspendResumeData)
#define kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp, sysmemAddrOfSuspendResumeData) kgspExecuteBooterUnloadIfNeeded_DISPATCH(pGpu, pKernelGsp, sysmemAddrOfSuspendResumeData)
#define kgspExecuteHsFalcon_FNPTR(pKernelGsp) pKernelGsp->__kgspExecuteHsFalcon__
#define kgspExecuteHsFalcon(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1) kgspExecuteHsFalcon_DISPATCH(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1)
#define kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1) kgspExecuteHsFalcon_DISPATCH(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1)
#define kgspWaitForProcessorSuspend_FNPTR(pKernelGsp) pKernelGsp->__kgspWaitForProcessorSuspend__
#define kgspWaitForProcessorSuspend(pGpu, pKernelGsp) kgspWaitForProcessorSuspend_DISPATCH(pGpu, pKernelGsp)
#define kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp) kgspWaitForProcessorSuspend_DISPATCH(pGpu, pKernelGsp)
#define kgspSavePowerMgmtState_FNPTR(pKernelGsp) pKernelGsp->__kgspSavePowerMgmtState__
#define kgspSavePowerMgmtState(pGpu, pKernelGsp) kgspSavePowerMgmtState_DISPATCH(pGpu, pKernelGsp)
#define kgspSavePowerMgmtState_HAL(pGpu, pKernelGsp) kgspSavePowerMgmtState_DISPATCH(pGpu, pKernelGsp)
#define kgspRestorePowerMgmtState_FNPTR(pKernelGsp) pKernelGsp->__kgspRestorePowerMgmtState__
#define kgspRestorePowerMgmtState(pGpu, pKernelGsp) kgspRestorePowerMgmtState_DISPATCH(pGpu, pKernelGsp)
#define kgspRestorePowerMgmtState_HAL(pGpu, pKernelGsp) kgspRestorePowerMgmtState_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeSuspendResumeData_FNPTR(pKernelGsp) pKernelGsp->__kgspFreeSuspendResumeData__
#define kgspFreeSuspendResumeData(pGpu, pKernelGsp) kgspFreeSuspendResumeData_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeSuspendResumeData_HAL(pGpu, pKernelGsp) kgspFreeSuspendResumeData_DISPATCH(pGpu, pKernelGsp)
#define kgspWaitForGfwBootOk_FNPTR(pKernelGsp) pKernelGsp->__kgspWaitForGfwBootOk__
#define kgspWaitForGfwBootOk(pGpu, pKernelGsp) kgspWaitForGfwBootOk_DISPATCH(pGpu, pKernelGsp)
#define kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp) kgspWaitForGfwBootOk_DISPATCH(pGpu, pKernelGsp)
#define kgspGetBinArchiveBooterLoadUcode_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveBooterLoadUcode__
#define kgspGetBinArchiveBooterLoadUcode(pKernelGsp) kgspGetBinArchiveBooterLoadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterLoadUcode_HAL(pKernelGsp) kgspGetBinArchiveBooterLoadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterUnloadUcode_FNPTR(pKernelGsp) pKernelGsp->__kgspGetBinArchiveBooterUnloadUcode__
#define kgspGetBinArchiveBooterUnloadUcode(pKernelGsp) kgspGetBinArchiveBooterUnloadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterUnloadUcode_HAL(pKernelGsp) kgspGetBinArchiveBooterUnloadUcode_DISPATCH(pKernelGsp)
#define kgspGetMinWprHeapSizeMB_FNPTR(pKernelGsp) pKernelGsp->__kgspGetMinWprHeapSizeMB__
#define kgspGetMinWprHeapSizeMB(pGpu, pKernelGsp) kgspGetMinWprHeapSizeMB_DISPATCH(pGpu, pKernelGsp)
#define kgspGetMinWprHeapSizeMB_HAL(pGpu, pKernelGsp) kgspGetMinWprHeapSizeMB_DISPATCH(pGpu, pKernelGsp)
#define kgspGetMaxWprHeapSizeMB_FNPTR(pKernelGsp) pKernelGsp->__kgspGetMaxWprHeapSizeMB__
#define kgspGetMaxWprHeapSizeMB(pGpu, pKernelGsp) kgspGetMaxWprHeapSizeMB_DISPATCH(pGpu, pKernelGsp)
#define kgspGetMaxWprHeapSizeMB_HAL(pGpu, pKernelGsp) kgspGetMaxWprHeapSizeMB_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFwHeapParamOsCarveoutSize_FNPTR(pKernelGsp) pKernelGsp->__kgspGetFwHeapParamOsCarveoutSize__
#define kgspGetFwHeapParamOsCarveoutSize(pGpu, pKernelGsp) kgspGetFwHeapParamOsCarveoutSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFwHeapParamOsCarveoutSize_HAL(pGpu, pKernelGsp) kgspGetFwHeapParamOsCarveoutSize_DISPATCH(pGpu, pKernelGsp)
#define kgspInitVgpuPartitionLogging_FNPTR(pKernelGsp) pKernelGsp->__kgspInitVgpuPartitionLogging__
#define kgspInitVgpuPartitionLogging(pGpu, pKernelGsp, gfid, initTaskLogBUffOffset, initTaskLogBUffSize, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, kernelLogBuffOffset, kernelLogBuffSize) kgspInitVgpuPartitionLogging_DISPATCH(pGpu, pKernelGsp, gfid, initTaskLogBUffOffset, initTaskLogBUffSize, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, kernelLogBuffOffset, kernelLogBuffSize)
#define kgspInitVgpuPartitionLogging_HAL(pGpu, pKernelGsp, gfid, initTaskLogBUffOffset, initTaskLogBUffSize, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, kernelLogBuffOffset, kernelLogBuffSize) kgspInitVgpuPartitionLogging_DISPATCH(pGpu, pKernelGsp, gfid, initTaskLogBUffOffset, initTaskLogBUffSize, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, kernelLogBuffOffset, kernelLogBuffSize)
#define kgspFreeVgpuPartitionLogging_FNPTR(pKernelGsp) pKernelGsp->__kgspFreeVgpuPartitionLogging__
#define kgspFreeVgpuPartitionLogging(pGpu, pKernelGsp, gfid) kgspFreeVgpuPartitionLogging_DISPATCH(pGpu, pKernelGsp, gfid)
#define kgspFreeVgpuPartitionLogging_HAL(pGpu, pKernelGsp, gfid) kgspFreeVgpuPartitionLogging_DISPATCH(pGpu, pKernelGsp, gfid)
#define kgspGetSignatureSectionNamePrefix_FNPTR(pKernelGsp) pKernelGsp->__kgspGetSignatureSectionNamePrefix__
#define kgspGetSignatureSectionNamePrefix(pGpu, pKernelGsp) kgspGetSignatureSectionNamePrefix_DISPATCH(pGpu, pKernelGsp)
#define kgspGetSignatureSectionNamePrefix_HAL(pGpu, pKernelGsp) kgspGetSignatureSectionNamePrefix_DISPATCH(pGpu, pKernelGsp)
#define kgspSetupGspFmcArgs_FNPTR(pKernelGsp) pKernelGsp->__kgspSetupGspFmcArgs__
#define kgspSetupGspFmcArgs(pGpu, pKernelGsp, pGspFw) kgspSetupGspFmcArgs_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspSetupGspFmcArgs_HAL(pGpu, pKernelGsp, pGspFw) kgspSetupGspFmcArgs_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspReadEmem_FNPTR(pKernelGsp) pKernelGsp->__kgspReadEmem__
#define kgspReadEmem(pKernelGsp, offset, size, pBuf) kgspReadEmem_DISPATCH(pKernelGsp, offset, size, pBuf)
#define kgspReadEmem_HAL(pKernelGsp, offset, size, pBuf) kgspReadEmem_DISPATCH(pKernelGsp, offset, size, pBuf)
#define kgspIssueNotifyOp_FNPTR(pKernelGsp) pKernelGsp->__kgspIssueNotifyOp__
#define kgspIssueNotifyOp(pGpu, pKernelGsp, opCode, pArgs, argc) kgspIssueNotifyOp_DISPATCH(pGpu, pKernelGsp, opCode, pArgs, argc)
#define kgspIssueNotifyOp_HAL(pGpu, pKernelGsp, opCode, pArgs, argc) kgspIssueNotifyOp_DISPATCH(pGpu, pKernelGsp, opCode, pArgs, argc)
#define kgspCheckGspRmCcCleanup_FNPTR(pKernelGsp) pKernelGsp->__kgspCheckGspRmCcCleanup__
#define kgspCheckGspRmCcCleanup(pGpu, pKernelGsp) kgspCheckGspRmCcCleanup_DISPATCH(pGpu, pKernelGsp)
#define kgspCheckGspRmCcCleanup_HAL(pGpu, pKernelGsp) kgspCheckGspRmCcCleanup_DISPATCH(pGpu, pKernelGsp)
#define kgspInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kgspInitMissing(pGpu, pEngstate) kgspInitMissing_DISPATCH(pGpu, pEngstate)
#define kgspStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kgspStatePreInitLocked(pGpu, pEngstate) kgspStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgspStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kgspStatePreInitUnlocked(pGpu, pEngstate) kgspStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgspStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kgspStateInitUnlocked(pGpu, pEngstate) kgspStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgspStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kgspStatePreLoad(pGpu, pEngstate, arg3) kgspStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateLoad__
#define kgspStateLoad(pGpu, pEngstate, arg3) kgspStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define kgspStatePostLoad(pGpu, pEngstate, arg3) kgspStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kgspStatePreUnload(pGpu, pEngstate, arg3) kgspStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateUnload__
#define kgspStateUnload(pGpu, pEngstate, arg3) kgspStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kgspStatePostUnload(pGpu, pEngstate, arg3) kgspStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__
#define kgspStateDestroy(pGpu, pEngstate) kgspStateDestroy_DISPATCH(pGpu, pEngstate)
#define kgspIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kgspIsPresent(pGpu, pEngstate) kgspIsPresent_DISPATCH(pGpu, pEngstate)
#define kgspClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservClearInterrupt__
#define kgspClearInterrupt(pGpu, pIntrService, pParams) kgspClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kgspServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__
#define kgspServiceNotificationInterrupt(pGpu, pIntrService, pParams) kgspServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kgspRegRead_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegRead__
#define kgspRegRead(pGpu, pKernelFlcn, offset) kgspRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kgspRegRead_HAL(pGpu, pKernelFlcn, offset) kgspRegRead_DISPATCH(pGpu, pKernelFlcn, offset)
#define kgspRegWrite_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnRegWrite__
#define kgspRegWrite(pGpu, pKernelFlcn, offset, data) kgspRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kgspRegWrite_HAL(pGpu, pKernelFlcn, offset, data) kgspRegWrite_DISPATCH(pGpu, pKernelFlcn, offset, data)
#define kgspMaskDmemAddr_FNPTR(pKernelFlcn) pKernelFlcn->__nvoc_base_KernelFalcon.__kflcnMaskDmemAddr__
#define kgspMaskDmemAddr(pGpu, pKernelFlcn, addr) kgspMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kgspMaskDmemAddr_HAL(pGpu, pKernelFlcn, addr) kgspMaskDmemAddr_DISPATCH(pGpu, pKernelFlcn, addr)
#define kgspConfigured_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineConfigured__
#define kgspConfigured(arg_this) kgspConfigured_DISPATCH(arg_this)
#define kgspUnload_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineUnload__
#define kgspUnload(arg_this) kgspUnload_DISPATCH(arg_this)
#define kgspVprintf_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineVprintf__
#define kgspVprintf(arg_this, bReportStart, fmt, args) kgspVprintf_DISPATCH(arg_this, bReportStart, fmt, args)
#define kgspPriRead_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEnginePriRead__
#define kgspPriRead(arg_this, offset) kgspPriRead_DISPATCH(arg_this, offset)
#define kgspPriWrite_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEnginePriWrite__
#define kgspPriWrite(arg_this, offset, data) kgspPriWrite_DISPATCH(arg_this, offset, data)
#define kgspMapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineMapBufferDescriptor__
#define kgspMapBufferDescriptor(arg_this, pBufDesc) kgspMapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kgspUnmapBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineUnmapBufferDescriptor__
#define kgspUnmapBufferDescriptor(arg_this, pBufDesc) kgspUnmapBufferDescriptor_DISPATCH(arg_this, pBufDesc)
#define kgspSyncBufferDescriptor_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineSyncBufferDescriptor__
#define kgspSyncBufferDescriptor(arg_this, pBufDesc, offset, size) kgspSyncBufferDescriptor_DISPATCH(arg_this, pBufDesc, offset, size)
#define kgspReadDmem_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineReadDmem__
#define kgspReadDmem(arg_this, offset, size, pBuf) kgspReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kgspReadDmem_HAL(arg_this, offset, size, pBuf) kgspReadDmem_DISPATCH(arg_this, offset, size, pBuf)
#define kgspGetScratchOffsets_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineGetScratchOffsets__
#define kgspGetScratchOffsets(arg_this, scratchGroupId) kgspGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kgspGetScratchOffsets_HAL(arg_this, scratchGroupId) kgspGetScratchOffsets_DISPATCH(arg_this, scratchGroupId)
#define kgspGetWFL0Offset_FNPTR(arg_this) arg_this->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineGetWFL0Offset__
#define kgspGetWFL0Offset(arg_this) kgspGetWFL0Offset_DISPATCH(arg_this)
#define kgspGetWFL0Offset_HAL(arg_this) kgspGetWFL0Offset_DISPATCH(arg_this)

// Dispatch functions
static inline NV_STATUS kgspConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, ENGDESCRIPTOR arg3) {
    return pKernelGsp->__kgspConstructEngine__(pGpu, pKernelGsp, arg3);
}

static inline NV_STATUS kgspStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspStateInitLocked__(pGpu, pKernelGsp);
}

static inline void kgspRegisterIntrService_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceRecord pRecords[175]) {
    pKernelGsp->__kgspRegisterIntrService__(pGpu, pKernelGsp, pRecords);
}

static inline NvU32 kgspServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelGsp->__kgspServiceInterrupt__(pGpu, pKernelGsp, pParams);
}

static inline void kgspConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspConfigureFalcon__(pGpu, pKernelGsp);
}

static inline NvBool kgspIsDebugModeEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspIsDebugModeEnabled__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspAllocBootArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspAllocBootArgs__(pGpu, pKernelGsp);
}

static inline void kgspFreeBootArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspFreeBootArgs__(pGpu, pKernelGsp);
}

static inline void kgspProgramLibosBootArgsAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspProgramLibosBootArgsAddr__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspSetCmdQueueHead_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 queueIdx, NvU32 value) {
    return pKernelGsp->__kgspSetCmdQueueHead__(pGpu, pKernelGsp, queueIdx, value);
}

static inline NV_STATUS kgspPrepareForBootstrap_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspPrepareForBootstrap__(pGpu, pKernelGsp, pGspFw);
}

static inline NV_STATUS kgspBootstrap_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspBootstrap__(pGpu, pKernelGsp, pGspFw);
}

static inline void kgspGetGspRmBootUcodeStorage_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc) {
    pKernelGsp->__kgspGetGspRmBootUcodeStorage__(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveGspRmBoot_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmBoot__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveConcatenatedFMCDesc_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveConcatenatedFMCDesc__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveConcatenatedFMC_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveConcatenatedFMC__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwProdSigned__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveGspRmCcFmcGfwProdSigned_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__(pKernelGsp);
}

static inline NV_STATUS kgspCalculateFbLayout_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspCalculateFbLayout__(pGpu, pKernelGsp, pGspFw);
}

static inline NvU32 kgspGetNonWprHeapSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetNonWprHeapSize__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspExecuteSequencerCommand_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize) {
    return pKernelGsp->__kgspExecuteSequencerCommand__(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize);
}

static inline NvU32 kgspReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId) {
    return pKernelGsp->__kgspReadUcodeFuseVersion__(pGpu, pKernelGsp, ucodeId);
}

static inline NV_STATUS kgspResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspResetHw__(pGpu, pKernelGsp);
}

static inline NvBool kgspHealthCheck_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspHealthCheck__(pGpu, pKernelGsp);
}

static inline NvU32 kgspService_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspService__(pGpu, pKernelGsp);
}

static inline NvBool kgspIsWpr2Up_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspIsWpr2Up__(pGpu, pKernelGsp);
}

static inline NvU32 kgspGetFrtsSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetFrtsSize__(pGpu, pKernelGsp);
}

static inline NvU64 kgspGetPrescrubbedTopFbSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetPrescrubbedTopFbSize__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspExtractVbiosFromRom_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg) {
    return pKernelGsp->__kgspExtractVbiosFromRom__(pGpu, pKernelGsp, ppVbiosImg);
}

static inline NV_STATUS kgspPrepareForFwsecFrts_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset, KernelGspPreparedFwsecCmd *preparedCmd) {
    return pKernelGsp->__kgspPrepareForFwsecFrts__(pGpu, pKernelGsp, pFwsecUcode, frtsOffset, preparedCmd);
}

static inline NV_STATUS kgspPrepareForFwsecSb_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, KernelGspPreparedFwsecCmd *preparedCmd) {
    return pKernelGsp->__kgspPrepareForFwsecSb__(pGpu, pKernelGsp, pFwsecUcode, preparedCmd);
}

static inline NV_STATUS kgspExecuteFwsec_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspPreparedFwsecCmd *preparedCmd) {
    return pKernelGsp->__kgspExecuteFwsec__(pGpu, pKernelGsp, preparedCmd);
}

static inline NV_STATUS kgspExecuteScrubberIfNeeded_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspExecuteScrubberIfNeeded__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspExecuteBooterLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfData) {
    return pKernelGsp->__kgspExecuteBooterLoad__(pGpu, pKernelGsp, sysmemAddrOfData);
}

static inline NV_STATUS kgspExecuteBooterUnloadIfNeeded_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfSuspendResumeData) {
    return pKernelGsp->__kgspExecuteBooterUnloadIfNeeded__(pGpu, pKernelGsp, sysmemAddrOfSuspendResumeData);
}

static inline NV_STATUS kgspExecuteHsFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1) {
    return pKernelGsp->__kgspExecuteHsFalcon__(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1);
}

static inline NV_STATUS kgspWaitForProcessorSuspend_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspWaitForProcessorSuspend__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspSavePowerMgmtState_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspSavePowerMgmtState__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspRestorePowerMgmtState_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspRestorePowerMgmtState__(pGpu, pKernelGsp);
}

static inline void kgspFreeSuspendResumeData_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspFreeSuspendResumeData__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspWaitForGfwBootOk_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspWaitForGfwBootOk__(pGpu, pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveBooterLoadUcode_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveBooterLoadUcode__(pKernelGsp);
}

static inline const BINDATA_ARCHIVE * kgspGetBinArchiveBooterUnloadUcode_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveBooterUnloadUcode__(pKernelGsp);
}

static inline NvU64 kgspGetMinWprHeapSizeMB_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetMinWprHeapSizeMB__(pGpu, pKernelGsp);
}

static inline NvU64 kgspGetMaxWprHeapSizeMB_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetMaxWprHeapSizeMB__(pGpu, pKernelGsp);
}

static inline NvU32 kgspGetFwHeapParamOsCarveoutSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetFwHeapParamOsCarveoutSize__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspInitVgpuPartitionLogging_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid, NvU64 initTaskLogBUffOffset, NvU64 initTaskLogBUffSize, NvU64 vgpuTaskLogBUffOffset, NvU64 vgpuTaskLogBuffSize, NvU64 kernelLogBuffOffset, NvU64 kernelLogBuffSize) {
    return pKernelGsp->__kgspInitVgpuPartitionLogging__(pGpu, pKernelGsp, gfid, initTaskLogBUffOffset, initTaskLogBUffSize, vgpuTaskLogBUffOffset, vgpuTaskLogBuffSize, kernelLogBuffOffset, kernelLogBuffSize);
}

static inline NV_STATUS kgspFreeVgpuPartitionLogging_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid) {
    return pKernelGsp->__kgspFreeVgpuPartitionLogging__(pGpu, pKernelGsp, gfid);
}

static inline const char * kgspGetSignatureSectionNamePrefix_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetSignatureSectionNamePrefix__(pGpu, pKernelGsp);
}

static inline NV_STATUS kgspSetupGspFmcArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspSetupGspFmcArgs__(pGpu, pKernelGsp, pGspFw);
}

static inline void kgspReadEmem_DISPATCH(struct KernelGsp *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf) {
    pKernelGsp->__kgspReadEmem__(pKernelGsp, offset, size, pBuf);
}

static inline NV_STATUS kgspIssueNotifyOp_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pArgs, NvU32 argc) {
    return pKernelGsp->__kgspIssueNotifyOp__(pGpu, pKernelGsp, opCode, pArgs, argc);
}

static inline NV_STATUS kgspCheckGspRmCcCleanup_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspCheckGspRmCcCleanup__(pGpu, pKernelGsp);
}

static inline void kgspInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    pEngstate->__kgspInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return pEngstate->__kgspStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kgspStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    pEngstate->__kgspStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kgspIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspIsPresent__(pGpu, pEngstate);
}

static inline NvBool kgspClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__kgspClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kgspServiceNotificationInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__kgspServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 kgspRegRead_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset) {
    return pKernelFlcn->__kgspRegRead__(pGpu, pKernelFlcn, offset);
}

static inline void kgspRegWrite_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset, NvU32 data) {
    pKernelFlcn->__kgspRegWrite__(pGpu, pKernelFlcn, offset, data);
}

static inline NvU32 kgspMaskDmemAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 addr) {
    return pKernelFlcn->__kgspMaskDmemAddr__(pGpu, pKernelFlcn, addr);
}

static inline NvBool kgspConfigured_DISPATCH(struct KernelGsp *arg_this) {
    return arg_this->__kgspConfigured__(arg_this);
}

static inline void kgspUnload_DISPATCH(struct KernelGsp *arg_this) {
    arg_this->__kgspUnload__(arg_this);
}

static inline void kgspVprintf_DISPATCH(struct KernelGsp *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    arg_this->__kgspVprintf__(arg_this, bReportStart, fmt, args);
}

static inline NvU32 kgspPriRead_DISPATCH(struct KernelGsp *arg_this, NvU32 offset) {
    return arg_this->__kgspPriRead__(arg_this, offset);
}

static inline void kgspPriWrite_DISPATCH(struct KernelGsp *arg_this, NvU32 offset, NvU32 data) {
    arg_this->__kgspPriWrite__(arg_this, offset, data);
}

static inline void * kgspMapBufferDescriptor_DISPATCH(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return arg_this->__kgspMapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kgspUnmapBufferDescriptor_DISPATCH(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    arg_this->__kgspUnmapBufferDescriptor__(arg_this, pBufDesc);
}

static inline void kgspSyncBufferDescriptor_DISPATCH(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    arg_this->__kgspSyncBufferDescriptor__(arg_this, pBufDesc, offset, size);
}

static inline void kgspReadDmem_DISPATCH(struct KernelGsp *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    arg_this->__kgspReadDmem__(arg_this, offset, size, pBuf);
}

static inline const NvU32 * kgspGetScratchOffsets_DISPATCH(struct KernelGsp *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return arg_this->__kgspGetScratchOffsets__(arg_this, scratchGroupId);
}

static inline NvU32 kgspGetWFL0Offset_DISPATCH(struct KernelGsp *arg_this) {
    return arg_this->__kgspGetWFL0Offset__(arg_this);
}

NV_STATUS kgspConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, ENGDESCRIPTOR arg3);

NV_STATUS kgspStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

void kgspRegisterIntrService_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceRecord pRecords[175]);

NvU32 kgspServiceInterrupt_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceServiceInterruptArguments *pParams);

void kgspConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

void kgspConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspConfigureFalcon_f2d351(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_PRECOMP(0);
}

NvBool kgspIsDebugModeEnabled_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NvBool kgspIsDebugModeEnabled_GA100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvBool kgspIsDebugModeEnabled_108313(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NV_STATUS kgspAllocBootArgs_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspAllocBootArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspAllocBootArgs_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kgspFreeBootArgs_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

void kgspFreeBootArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspFreeBootArgs_f2d351(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_PRECOMP(0);
}

void kgspProgramLibosBootArgsAddr_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspProgramLibosBootArgsAddr_f2d351(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_PRECOMP(0);
}

NV_STATUS kgspSetCmdQueueHead_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 queueIdx, NvU32 value);

static inline NV_STATUS kgspSetCmdQueueHead_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 queueIdx, NvU32 value) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspPrepareForBootstrap_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspPrepareForBootstrap_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspPrepareForBootstrap_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspBootstrap_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspBootstrap_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspBootstrap_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kgspGetGspRmBootUcodeStorage_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

void kgspGetGspRmBootUcodeStorage_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

void kgspGetGspRmBootUcodeStorage_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

void kgspGetGspRmBootUcodeStorage_GB100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

static inline void kgspGetGspRmBootUcodeStorage_f2d351(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc) {
    NV_ASSERT_PRECOMP(0);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GH100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_AD102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GB100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveConcatenatedFMCDesc_GH100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveConcatenatedFMCDesc_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveConcatenatedFMC_GH100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveConcatenatedFMC_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_GB100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GH100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

NV_STATUS kgspCalculateFbLayout_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspCalculateFbLayout_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspCalculateFbLayout_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NvU32 kgspGetNonWprHeapSize_ed6b8b(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 1048576;
}

static inline NvU32 kgspGetNonWprHeapSize_d505ea(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 2097152;
}

static inline NvU32 kgspGetNonWprHeapSize_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteSequencerCommand_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize);

NV_STATUS kgspExecuteSequencerCommand_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize);

static inline NV_STATUS kgspExecuteSequencerCommand_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NvU32 kgspReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId) {
    return 0;
}

NvU32 kgspReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId);

static inline NvU32 kgspReadUcodeFuseVersion_474d46(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NV_STATUS kgspResetHw_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspResetHw_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspResetHw_GB100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspResetHw_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NvBool kgspHealthCheck_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvBool kgspHealthCheck_108313(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NvU32 kgspService_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvU32 kgspService_474d46(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvBool kgspIsWpr2Up_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NvBool kgspIsWpr2Up_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvBool kgspIsWpr2Up_108313(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NvU32 kgspGetFrtsSize_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvU32 kgspGetFrtsSize_4a4dee(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 0;
}

static inline NvU32 kgspGetFrtsSize_474d46(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU64 kgspGetPrescrubbedTopFbSize_e1e623(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 256 * 1024 * 1024;
}

static inline NvU64 kgspGetPrescrubbedTopFbSize_604eb7(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (+18446744073709551615ULL);
}

static inline NvU64 kgspGetPrescrubbedTopFbSize_474d46(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NV_STATUS kgspExtractVbiosFromRom_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg);

static inline NV_STATUS kgspExtractVbiosFromRom_395e98(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgspExtractVbiosFromRom_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspPrepareForFwsecFrts_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset, KernelGspPreparedFwsecCmd *preparedCmd);

static inline NV_STATUS kgspPrepareForFwsecFrts_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset, KernelGspPreparedFwsecCmd *preparedCmd) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspPrepareForFwsecSb_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, KernelGspPreparedFwsecCmd *preparedCmd);

static inline NV_STATUS kgspPrepareForFwsecSb_395e98(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, KernelGspPreparedFwsecCmd *preparedCmd) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgspPrepareForFwsecSb_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, KernelGspPreparedFwsecCmd *preparedCmd) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteFwsec_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspPreparedFwsecCmd *preparedCmd);

static inline NV_STATUS kgspExecuteFwsec_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspPreparedFwsecCmd *preparedCmd) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteScrubberIfNeeded_AD102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspExecuteScrubberIfNeeded_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteBooterLoad_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfData);

static inline NV_STATUS kgspExecuteBooterLoad_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfData) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteBooterUnloadIfNeeded_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfSuspendResumeData);

static inline NV_STATUS kgspExecuteBooterUnloadIfNeeded_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 sysmemAddrOfSuspendResumeData) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspExecuteHsFalcon_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1);

NV_STATUS kgspExecuteHsFalcon_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1);

static inline NV_STATUS kgspExecuteHsFalcon_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspWaitForProcessorSuspend_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspWaitForProcessorSuspend_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspSavePowerMgmtState_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspSavePowerMgmtState_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspRestorePowerMgmtState_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspRestorePowerMgmtState_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kgspFreeSuspendResumeData_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspFreeSuspendResumeData_f2d351(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_PRECOMP(0);
}

NV_STATUS kgspWaitForGfwBootOk_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspWaitForGfwBootOk_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspWaitForGfwBootOk_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_TU116(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_AD102(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_TU116(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_AD102(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline NvU64 kgspGetMinWprHeapSizeMB_7185bf(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (64U);
}

static inline NvU64 kgspGetMinWprHeapSizeMB_cc88c3(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pGpu->bVgpuGspPluginOffloadEnabled ? (565U) : (86U);
}

static inline NvU64 kgspGetMinWprHeapSizeMB_b2b553(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 0;
}

static inline NvU64 kgspGetMaxWprHeapSizeMB_ad4e6a(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (256U);
}

static inline NvU64 kgspGetMaxWprHeapSizeMB_55728f(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pGpu->bVgpuGspPluginOffloadEnabled ? (1040U) : (278U);
}

static inline NvU64 kgspGetMaxWprHeapSizeMB_b2b553(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 0;
}

static inline NvU32 kgspGetFwHeapParamOsCarveoutSize_397f70(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (0 << 20);
}

static inline NvU32 kgspGetFwHeapParamOsCarveoutSize_4b5307(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pGpu->bVgpuGspPluginOffloadEnabled ? (36 << 20) : (22 << 20);
}

static inline NvU32 kgspGetFwHeapParamOsCarveoutSize_b2b553(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 0;
}

static inline NV_STATUS kgspInitVgpuPartitionLogging_395e98(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid, NvU64 initTaskLogBUffOffset, NvU64 initTaskLogBUffSize, NvU64 vgpuTaskLogBUffOffset, NvU64 vgpuTaskLogBuffSize, NvU64 kernelLogBuffOffset, NvU64 kernelLogBuffSize) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgspInitVgpuPartitionLogging_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid, NvU64 initTaskLogBUffOffset, NvU64 initTaskLogBUffSize, NvU64 vgpuTaskLogBUffOffset, NvU64 vgpuTaskLogBuffSize, NvU64 kernelLogBuffOffset, NvU64 kernelLogBuffSize);

static inline NV_STATUS kgspFreeVgpuPartitionLogging_395e98(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kgspFreeVgpuPartitionLogging_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 gfid);

const char *kgspGetSignatureSectionNamePrefix_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

const char *kgspGetSignatureSectionNamePrefix_GB100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline const char *kgspGetSignatureSectionNamePrefix_789efb(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return ".fwsignature_";
}

static inline const char *kgspGetSignatureSectionNamePrefix_80f438(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

NV_STATUS kgspSetupGspFmcArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspSetupGspFmcArgs_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kgspReadEmem_TU102(struct KernelGsp *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf);

static inline void kgspReadEmem_366c4c(struct KernelGsp *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf) {
    NV_ASSERT(0);
}

NV_STATUS kgspIssueNotifyOp_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pArgs, NvU32 argc);

static inline NV_STATUS kgspIssueNotifyOp_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pArgs, NvU32 argc) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kgspCheckGspRmCcCleanup_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspCheckGspRmCcCleanup_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

void kgspDestruct_IMPL(struct KernelGsp *pKernelGsp);

#define __nvoc_kgspDestruct(pKernelGsp) kgspDestruct_IMPL(pKernelGsp)
void kgspPopulateGspRmInitArgs_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_SR_INIT_ARGUMENTS *pGspSrInitArgs);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspPopulateGspRmInitArgs(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_SR_INIT_ARGUMENTS *pGspSrInitArgs) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, pGspSrInitArgs) kgspPopulateGspRmInitArgs_IMPL(pGpu, pKernelGsp, pGspSrInitArgs)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspInitRm_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspInitRm(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspInitRm(pGpu, pKernelGsp, pGspFw) kgspInitRm_IMPL(pGpu, pKernelGsp, pGspFw)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspCreateRadix3_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, MEMORY_DESCRIPTOR **ppMemdescRadix3, MEMORY_DESCRIPTOR *pMemdescData, const void *pData, NvU64 sizeOfData);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspCreateRadix3(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, MEMORY_DESCRIPTOR **ppMemdescRadix3, MEMORY_DESCRIPTOR *pMemdescData, const void *pData, NvU64 sizeOfData) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspCreateRadix3(pGpu, pKernelGsp, ppMemdescRadix3, pMemdescData, pData, sizeOfData) kgspCreateRadix3_IMPL(pGpu, pKernelGsp, ppMemdescRadix3, pMemdescData, pData, sizeOfData)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspUnloadRm_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspUnloadRm(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspUnloadRm(pGpu, pKernelGsp) kgspUnloadRm_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspPrepareBootBinaryImage_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspPrepareBootBinaryImage(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspPrepareBootBinaryImage(pGpu, pKernelGsp) kgspPrepareBootBinaryImage_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

NvU64 kgspGetFwHeapSize_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU64 posteriorFbSize);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NvU64 kgspGetFwHeapSize(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU64 posteriorFbSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return 0;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspGetFwHeapSize(pGpu, pKernelGsp, posteriorFbSize) kgspGetFwHeapSize_IMPL(pGpu, pKernelGsp, posteriorFbSize)
#endif //__nvoc_kernel_gsp_h_disabled

NvU64 kgspGetWprEndMargin_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NvU64 kgspGetWprEndMargin(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return 0;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspGetWprEndMargin(pGpu, pKernelGsp) kgspGetWprEndMargin_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

void kgspSetupLibosInitArgs_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspSetupLibosInitArgs(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspSetupLibosInitArgs(pGpu, pKernelGsp) kgspSetupLibosInitArgs_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspQueueAsyncInitRpcs_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspQueueAsyncInitRpcs(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspQueueAsyncInitRpcs(pGpu, pKernelGsp) kgspQueueAsyncInitRpcs_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

void kgspRpcRecvEvents_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspRpcRecvEvents(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspRpcRecvEvents(pGpu, pKernelGsp) kgspRpcRecvEvents_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspWaitForRmInitDone_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspWaitForRmInitDone(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspWaitForRmInitDone(pGpu, pKernelGsp) kgspWaitForRmInitDone_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspStartLogPolling_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspStartLogPolling(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspStartLogPolling(pGpu, pKernelGsp) kgspStartLogPolling_IMPL(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

void kgspDumpGspLogs_IMPL(struct KernelGsp *pKernelGsp, NvBool arg2);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspDumpGspLogs(struct KernelGsp *pKernelGsp, NvBool arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspDumpGspLogs(pKernelGsp, arg2) kgspDumpGspLogs_IMPL(pKernelGsp, arg2)
#endif //__nvoc_kernel_gsp_h_disabled

void kgspDumpGspLogsUnlocked_IMPL(struct KernelGsp *pKernelGsp, NvBool arg2);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspDumpGspLogsUnlocked(struct KernelGsp *pKernelGsp, NvBool arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspDumpGspLogsUnlocked(pKernelGsp, arg2) kgspDumpGspLogsUnlocked_IMPL(pKernelGsp, arg2)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspExecuteSequencerBuffer_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, void *pRunCpuSeqParams);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspExecuteSequencerBuffer(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, void *pRunCpuSeqParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspExecuteSequencerBuffer(pGpu, pKernelGsp, pRunCpuSeqParams) kgspExecuteSequencerBuffer_IMPL(pGpu, pKernelGsp, pRunCpuSeqParams)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspParseFwsecUcodeFromVbiosImg_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const KernelGspVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode, NvU64 *pVbiosVersionCombined);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspParseFwsecUcodeFromVbiosImg(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const KernelGspVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode, NvU64 *pVbiosVersionCombined) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspParseFwsecUcodeFromVbiosImg(pGpu, pKernelGsp, pVbiosImg, ppFwsecUcode, pVbiosVersionCombined) kgspParseFwsecUcodeFromVbiosImg_IMPL(pGpu, pKernelGsp, pVbiosImg, ppFwsecUcode, pVbiosVersionCombined)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspAllocateScrubberUcodeImage_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppScrubberUcode);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspAllocateScrubberUcodeImage(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppScrubberUcode) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspAllocateScrubberUcodeImage(pGpu, pKernelGsp, ppScrubberUcode) kgspAllocateScrubberUcodeImage_IMPL(pGpu, pKernelGsp, ppScrubberUcode)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspAllocateBooterLoadUcodeImage_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppBooterLoadUcode);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspAllocateBooterLoadUcodeImage(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppBooterLoadUcode) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspAllocateBooterLoadUcodeImage(pGpu, pKernelGsp, ppBooterLoadUcode) kgspAllocateBooterLoadUcodeImage_IMPL(pGpu, pKernelGsp, ppBooterLoadUcode)
#endif //__nvoc_kernel_gsp_h_disabled

NV_STATUS kgspAllocateBooterUnloadUcodeImage_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppBooterUnloadUcode);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspAllocateBooterUnloadUcodeImage(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode **ppBooterUnloadUcode) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspAllocateBooterUnloadUcodeImage(pGpu, pKernelGsp, ppBooterUnloadUcode) kgspAllocateBooterUnloadUcodeImage_IMPL(pGpu, pKernelGsp, ppBooterUnloadUcode)
#endif //__nvoc_kernel_gsp_h_disabled

void kgspRcAndNotifyAllUserChannels_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 exceptType);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspRcAndNotifyAllUserChannels(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 exceptType) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspRcAndNotifyAllUserChannels(pGpu, pKernelGsp, exceptType) kgspRcAndNotifyAllUserChannels_IMPL(pGpu, pKernelGsp, exceptType)
#endif //__nvoc_kernel_gsp_h_disabled

#undef PRIVATE_FIELD


NV_STATUS rpcRmApiControl_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                              NvU32 cmd, void *pParamStructPtr, NvU32 paramsSize);
NV_STATUS rpcRmApiAlloc_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                            NvHandle hObject, NvU32 hClass, void *pAllocParams, NvU32 allocParamsSize);
NV_STATUS rpcRmApiDupObject_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hParent, NvHandle *phObject,
                                NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags);
NV_STATUS rpcRmApiFree_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hObject);

/* Free a KernelGspVbiosImg structure */
void kgspFreeVbiosImg(KernelGspVbiosImg *pVbiosImg);
/* Free a KernelGspFlcnUcode structure */
void kgspFreeFlcnUcode(KernelGspFlcnUcode *pFlcnUcode);

void kgspLogRpcDebugInfo(struct OBJGPU *pGpu, OBJRPC *pRpc, NvU32 errorNum, NvBool bPollingForRpcResponse);

#endif // KERNEL_GSP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GSP_NVOC_H_

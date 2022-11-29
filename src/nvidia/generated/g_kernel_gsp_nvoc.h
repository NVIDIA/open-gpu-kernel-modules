#ifndef _G_KERNEL_GSP_NVOC_H_
#define _G_KERNEL_GSP_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nv-firmware.h"
#include "rmRiscvUcode.h"

#include "libos_init_args.h"
#include "gsp_fw_wpr_meta.h"
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

/*!
 * Index into libosLogDecode array.
 */
enum
{
    LOGIDX_INIT,
    LOGIDX_VGPU,
    LOGIDX_RM,
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
#ifdef NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelGsp {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    struct KernelGsp *__nvoc_pbase_KernelGsp;
    NV_STATUS (*__kgspConstructEngine__)(struct OBJGPU *, struct KernelGsp *, ENGDESCRIPTOR);
    void (*__kgspRegisterIntrService__)(struct OBJGPU *, struct KernelGsp *, IntrServiceRecord *);
    NvU32 (*__kgspServiceInterrupt__)(struct OBJGPU *, struct KernelGsp *, IntrServiceServiceInterruptArguments *);
    void (*__kgspConfigureFalcon__)(struct OBJGPU *, struct KernelGsp *);
    NvBool (*__kgspIsDebugModeEnabled__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspAllocBootArgs__)(struct OBJGPU *, struct KernelGsp *);
    void (*__kgspFreeBootArgs__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspBootstrapRiscvOSEarly__)(struct OBJGPU *, struct KernelGsp *, GSP_FIRMWARE *);
    void (*__kgspGetGspRmBootUcodeStorage__)(struct OBJGPU *, struct KernelGsp *, BINDATA_STORAGE **, BINDATA_STORAGE **);
    const BINDATA_ARCHIVE *(*__kgspGetBinArchiveGspRmBoot__)(struct KernelGsp *);
    const BINDATA_ARCHIVE *(*__kgspGetBinArchiveGspRmFmcGfwDebugSigned__)(struct KernelGsp *);
    const BINDATA_ARCHIVE *(*__kgspGetBinArchiveGspRmFmcGfwProdSigned__)(struct KernelGsp *);
    NV_STATUS (*__kgspCalculateFbLayout__)(struct OBJGPU *, struct KernelGsp *, GSP_FIRMWARE *);
    NvU32 (*__kgspGetNonWprHeapSize__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspExecuteSequencerCommand__)(struct OBJGPU *, struct KernelGsp *, NvU32, NvU32 *, NvU32);
    NvU32 (*__kgspReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspResetHw__)(struct OBJGPU *, struct KernelGsp *);
    NvBool (*__kgspIsEngineInReset__)(struct OBJGPU *, struct KernelGsp *);
    NvU32 (*__kgspGetFrtsSize__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspExtractVbiosFromRom__)(struct OBJGPU *, struct KernelGsp *, KernelGspVbiosImg **);
    NV_STATUS (*__kgspExecuteFwsecFrts__)(struct OBJGPU *, struct KernelGsp *, KernelGspFlcnUcode *, const NvU64);
    NV_STATUS (*__kgspExecuteFwsecSb__)(struct OBJGPU *, struct KernelGsp *, KernelGspFlcnUcode *);
    NV_STATUS (*__kgspExecuteBooterLoad__)(struct OBJGPU *, struct KernelGsp *, const NvU64);
    NV_STATUS (*__kgspExecuteBooterUnloadIfNeeded__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspExecuteHsFalcon__)(struct OBJGPU *, struct KernelGsp *, KernelGspFlcnUcode *, struct KernelFalcon *, NvU32 *, NvU32 *);
    NV_STATUS (*__kgspWaitForGfwBootOk__)(struct OBJGPU *, struct KernelGsp *);
    const BINDATA_ARCHIVE *(*__kgspGetBinArchiveBooterLoadUcode__)(struct KernelGsp *);
    const BINDATA_ARCHIVE *(*__kgspGetBinArchiveBooterUnloadUcode__)(struct KernelGsp *);
    NvU64 (*__kgspGetWprHeapSize__)(struct OBJGPU *, struct KernelGsp *);
    const char *(*__kgspGetSignatureSectionNamePrefix__)(struct OBJGPU *, struct KernelGsp *);
    NV_STATUS (*__kgspSetupGspFmcArgs__)(struct OBJGPU *, struct KernelGsp *, GSP_FIRMWARE *);
    void (*__kgspStateDestroy__)(POBJGPU, struct KernelGsp *);
    void (*__kgspFreeTunableState__)(POBJGPU, struct KernelGsp *, void *);
    NV_STATUS (*__kgspCompareTunableState__)(POBJGPU, struct KernelGsp *, void *, void *);
    NvBool (*__kgspClearInterrupt__)(struct OBJGPU *, struct KernelGsp *, IntrServiceClearInterruptArguments *);
    NvBool (*__kgspIsPresent__)(POBJGPU, struct KernelGsp *);
    NV_STATUS (*__kgspReconcileTunableState__)(POBJGPU, struct KernelGsp *, void *);
    NV_STATUS (*__kgspStateLoad__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspStateUnload__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspServiceNotificationInterrupt__)(struct OBJGPU *, struct KernelGsp *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__kgspStateInitLocked__)(POBJGPU, struct KernelGsp *);
    NV_STATUS (*__kgspStatePreLoad__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspStatePostUnload__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspStatePreUnload__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspGetTunableState__)(POBJGPU, struct KernelGsp *, void *);
    NV_STATUS (*__kgspStateInitUnlocked__)(POBJGPU, struct KernelGsp *);
    void (*__kgspInitMissing__)(POBJGPU, struct KernelGsp *);
    NV_STATUS (*__kgspStatePreInitLocked__)(POBJGPU, struct KernelGsp *);
    NV_STATUS (*__kgspStatePreInitUnlocked__)(POBJGPU, struct KernelGsp *);
    NV_STATUS (*__kgspStatePostLoad__)(POBJGPU, struct KernelGsp *, NvU32);
    NV_STATUS (*__kgspAllocTunableState__)(POBJGPU, struct KernelGsp *, void **);
    NV_STATUS (*__kgspSetTunableState__)(POBJGPU, struct KernelGsp *, void *);
    struct OBJRPC *pRpc;
    KernelGspFlcnUcode *pFwsecUcode;
    KernelGspFlcnUcode *pBooterLoadUcode;
    KernelGspFlcnUcode *pBooterUnloadUcode;
    MEMORY_DESCRIPTOR *pWprMetaDescriptor;
    GspFwWprMeta *pWprMeta;
    NvP64 pWprMetaMappingPriv;
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
    RM_LIBOS_LOG_MEM rmLibosLogMem[3];
    void *pLogElf;
    NvBool bLibosLogsPollingEnabled;
    NvBool bInInit;
    NvBool bPollingForRpcResponse;
    NvBool bXid119Printed;
    MEMORY_DESCRIPTOR *pMemDesc_simAccessBuf;
    SimAccessBuffer *pSimAccessBuf;
    NvP64 pSimAccessBufPriv;
    GspStaticConfigInfo gspStaticInfo;
    NvBool bPartitionedFmc;
};

#ifndef __NVOC_CLASS_KernelGsp_TYPEDEF__
#define __NVOC_CLASS_KernelGsp_TYPEDEF__
typedef struct KernelGsp KernelGsp;
#endif /* __NVOC_CLASS_KernelGsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGsp
#define __nvoc_class_id_KernelGsp 0x311d4e
#endif /* __nvoc_class_id_KernelGsp */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;

#define __staticCast_KernelGsp(pThis) \
    ((pThis)->__nvoc_pbase_KernelGsp)

#ifdef __nvoc_kernel_gsp_h_disabled
#define __dynamicCast_KernelGsp(pThis) ((KernelGsp*)NULL)
#else //__nvoc_kernel_gsp_h_disabled
#define __dynamicCast_KernelGsp(pThis) \
    ((KernelGsp*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGsp)))
#endif //__nvoc_kernel_gsp_h_disabled

#define PDB_PROP_KGSP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGSP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp**, Dynamic*, NvU32);
#define __objCreate_KernelGsp(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGsp((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kgspConstructEngine(pGpu, pKernelGsp, arg0) kgspConstructEngine_DISPATCH(pGpu, pKernelGsp, arg0)
#define kgspRegisterIntrService(pGpu, pKernelGsp, pRecords) kgspRegisterIntrService_DISPATCH(pGpu, pKernelGsp, pRecords)
#define kgspServiceInterrupt(pGpu, pKernelGsp, pParams) kgspServiceInterrupt_DISPATCH(pGpu, pKernelGsp, pParams)
#define kgspConfigureFalcon(pGpu, pKernelGsp) kgspConfigureFalcon_DISPATCH(pGpu, pKernelGsp)
#define kgspConfigureFalcon_HAL(pGpu, pKernelGsp) kgspConfigureFalcon_DISPATCH(pGpu, pKernelGsp)
#define kgspIsDebugModeEnabled(pGpu, pKernelGsp) kgspIsDebugModeEnabled_DISPATCH(pGpu, pKernelGsp)
#define kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp) kgspIsDebugModeEnabled_DISPATCH(pGpu, pKernelGsp)
#define kgspAllocBootArgs(pGpu, pKernelGsp) kgspAllocBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspAllocBootArgs_HAL(pGpu, pKernelGsp) kgspAllocBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeBootArgs(pGpu, pKernelGsp) kgspFreeBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspFreeBootArgs_HAL(pGpu, pKernelGsp) kgspFreeBootArgs_DISPATCH(pGpu, pKernelGsp)
#define kgspBootstrapRiscvOSEarly(pGpu, pKernelGsp, pGspFw) kgspBootstrapRiscvOSEarly_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspBootstrapRiscvOSEarly_HAL(pGpu, pKernelGsp, pGspFw) kgspBootstrapRiscvOSEarly_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspGetGspRmBootUcodeStorage(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc) kgspGetGspRmBootUcodeStorage_DISPATCH(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc)
#define kgspGetGspRmBootUcodeStorage_HAL(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc) kgspGetGspRmBootUcodeStorage_DISPATCH(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc)
#define kgspGetBinArchiveGspRmBoot(pKernelGsp) kgspGetBinArchiveGspRmBoot_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmBoot_HAL(pKernelGsp) kgspGetBinArchiveGspRmBoot_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwDebugSigned(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwDebugSigned_HAL(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwProdSigned(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveGspRmFmcGfwProdSigned_HAL(pKernelGsp) kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(pKernelGsp)
#define kgspCalculateFbLayout(pGpu, pKernelGsp, pGspFw) kgspCalculateFbLayout_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspCalculateFbLayout_HAL(pGpu, pKernelGsp, pGspFw) kgspCalculateFbLayout_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspGetNonWprHeapSize(pGpu, pKernelGsp) kgspGetNonWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetNonWprHeapSize_HAL(pGpu, pKernelGsp) kgspGetNonWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteSequencerCommand(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize) kgspExecuteSequencerCommand_DISPATCH(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize)
#define kgspExecuteSequencerCommand_HAL(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize) kgspExecuteSequencerCommand_DISPATCH(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize)
#define kgspReadUcodeFuseVersion(pGpu, pKernelGsp, ucodeId) kgspReadUcodeFuseVersion_DISPATCH(pGpu, pKernelGsp, ucodeId)
#define kgspReadUcodeFuseVersion_HAL(pGpu, pKernelGsp, ucodeId) kgspReadUcodeFuseVersion_DISPATCH(pGpu, pKernelGsp, ucodeId)
#define kgspResetHw(pGpu, pKernelGsp) kgspResetHw_DISPATCH(pGpu, pKernelGsp)
#define kgspResetHw_HAL(pGpu, pKernelGsp) kgspResetHw_DISPATCH(pGpu, pKernelGsp)
#define kgspIsEngineInReset(pGpu, pKernelGsp) kgspIsEngineInReset_DISPATCH(pGpu, pKernelGsp)
#define kgspIsEngineInReset_HAL(pGpu, pKernelGsp) kgspIsEngineInReset_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFrtsSize(pGpu, pKernelGsp) kgspGetFrtsSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetFrtsSize_HAL(pGpu, pKernelGsp) kgspGetFrtsSize_DISPATCH(pGpu, pKernelGsp)
#define kgspExtractVbiosFromRom(pGpu, pKernelGsp, ppVbiosImg) kgspExtractVbiosFromRom_DISPATCH(pGpu, pKernelGsp, ppVbiosImg)
#define kgspExtractVbiosFromRom_HAL(pGpu, pKernelGsp, ppVbiosImg) kgspExtractVbiosFromRom_DISPATCH(pGpu, pKernelGsp, ppVbiosImg)
#define kgspExecuteFwsecFrts(pGpu, pKernelGsp, pFwsecUcode, frtsOffset) kgspExecuteFwsecFrts_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, frtsOffset)
#define kgspExecuteFwsecFrts_HAL(pGpu, pKernelGsp, pFwsecUcode, frtsOffset) kgspExecuteFwsecFrts_DISPATCH(pGpu, pKernelGsp, pFwsecUcode, frtsOffset)
#define kgspExecuteFwsecSb(pGpu, pKernelGsp, pFwsecUcode) kgspExecuteFwsecSb_DISPATCH(pGpu, pKernelGsp, pFwsecUcode)
#define kgspExecuteFwsecSb_HAL(pGpu, pKernelGsp, pFwsecUcode) kgspExecuteFwsecSb_DISPATCH(pGpu, pKernelGsp, pFwsecUcode)
#define kgspExecuteBooterLoad(pGpu, pKernelGsp, gspFwWprMetaOffset) kgspExecuteBooterLoad_DISPATCH(pGpu, pKernelGsp, gspFwWprMetaOffset)
#define kgspExecuteBooterLoad_HAL(pGpu, pKernelGsp, gspFwWprMetaOffset) kgspExecuteBooterLoad_DISPATCH(pGpu, pKernelGsp, gspFwWprMetaOffset)
#define kgspExecuteBooterUnloadIfNeeded(pGpu, pKernelGsp) kgspExecuteBooterUnloadIfNeeded_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp) kgspExecuteBooterUnloadIfNeeded_DISPATCH(pGpu, pKernelGsp)
#define kgspExecuteHsFalcon(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1) kgspExecuteHsFalcon_DISPATCH(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1)
#define kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1) kgspExecuteHsFalcon_DISPATCH(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1)
#define kgspWaitForGfwBootOk(pGpu, pKernelGsp) kgspWaitForGfwBootOk_DISPATCH(pGpu, pKernelGsp)
#define kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp) kgspWaitForGfwBootOk_DISPATCH(pGpu, pKernelGsp)
#define kgspGetBinArchiveBooterLoadUcode(pKernelGsp) kgspGetBinArchiveBooterLoadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterLoadUcode_HAL(pKernelGsp) kgspGetBinArchiveBooterLoadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterUnloadUcode(pKernelGsp) kgspGetBinArchiveBooterUnloadUcode_DISPATCH(pKernelGsp)
#define kgspGetBinArchiveBooterUnloadUcode_HAL(pKernelGsp) kgspGetBinArchiveBooterUnloadUcode_DISPATCH(pKernelGsp)
#define kgspGetWprHeapSize(pGpu, pKernelGsp) kgspGetWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetWprHeapSize_HAL(pGpu, pKernelGsp) kgspGetWprHeapSize_DISPATCH(pGpu, pKernelGsp)
#define kgspGetSignatureSectionNamePrefix(pGpu, pKernelGsp) kgspGetSignatureSectionNamePrefix_DISPATCH(pGpu, pKernelGsp)
#define kgspGetSignatureSectionNamePrefix_HAL(pGpu, pKernelGsp) kgspGetSignatureSectionNamePrefix_DISPATCH(pGpu, pKernelGsp)
#define kgspSetupGspFmcArgs(pGpu, pKernelGsp, pGspFw) kgspSetupGspFmcArgs_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspSetupGspFmcArgs_HAL(pGpu, pKernelGsp, pGspFw) kgspSetupGspFmcArgs_DISPATCH(pGpu, pKernelGsp, pGspFw)
#define kgspStateDestroy(pGpu, pEngstate) kgspStateDestroy_DISPATCH(pGpu, pEngstate)
#define kgspFreeTunableState(pGpu, pEngstate, pTunableState) kgspFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgspCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) kgspCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define kgspClearInterrupt(pGpu, pIntrService, pParams) kgspClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kgspIsPresent(pGpu, pEngstate) kgspIsPresent_DISPATCH(pGpu, pEngstate)
#define kgspReconcileTunableState(pGpu, pEngstate, pTunableState) kgspReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgspStateLoad(pGpu, pEngstate, arg0) kgspStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgspStateUnload(pGpu, pEngstate, arg0) kgspStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgspServiceNotificationInterrupt(pGpu, pIntrService, pParams) kgspServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kgspStateInitLocked(pGpu, pEngstate) kgspStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kgspStatePreLoad(pGpu, pEngstate, arg0) kgspStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgspStatePostUnload(pGpu, pEngstate, arg0) kgspStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgspStatePreUnload(pGpu, pEngstate, arg0) kgspStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgspGetTunableState(pGpu, pEngstate, pTunableState) kgspGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgspStateInitUnlocked(pGpu, pEngstate) kgspStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgspInitMissing(pGpu, pEngstate) kgspInitMissing_DISPATCH(pGpu, pEngstate)
#define kgspStatePreInitLocked(pGpu, pEngstate) kgspStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgspStatePreInitUnlocked(pGpu, pEngstate) kgspStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgspStatePostLoad(pGpu, pEngstate, arg0) kgspStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgspAllocTunableState(pGpu, pEngstate, ppTunableState) kgspAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define kgspSetTunableState(pGpu, pEngstate, pTunableState) kgspSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
void kgspProgramLibosBootArgsAddr_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);


#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspProgramLibosBootArgsAddr(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspProgramLibosBootArgsAddr(pGpu, pKernelGsp) kgspProgramLibosBootArgsAddr_TU102(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

#define kgspProgramLibosBootArgsAddr_HAL(pGpu, pKernelGsp) kgspProgramLibosBootArgsAddr(pGpu, pKernelGsp)

NV_STATUS kgspSetCmdQueueHead_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 queueIdx, NvU32 value);


#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspSetCmdQueueHead(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 queueIdx, NvU32 value) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspSetCmdQueueHead(pGpu, pKernelGsp, queueIdx, value) kgspSetCmdQueueHead_TU102(pGpu, pKernelGsp, queueIdx, value)
#endif //__nvoc_kernel_gsp_h_disabled

#define kgspSetCmdQueueHead_HAL(pGpu, pKernelGsp, queueIdx, value) kgspSetCmdQueueHead(pGpu, pKernelGsp, queueIdx, value)

void kgspHealthCheck_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);


#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspHealthCheck(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspHealthCheck(pGpu, pKernelGsp) kgspHealthCheck_TU102(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

#define kgspHealthCheck_HAL(pGpu, pKernelGsp) kgspHealthCheck(pGpu, pKernelGsp)

NvU32 kgspService_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);


#ifdef __nvoc_kernel_gsp_h_disabled
static inline NvU32 kgspService(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return 0;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspService(pGpu, pKernelGsp) kgspService_TU102(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

#define kgspService_HAL(pGpu, pKernelGsp) kgspService(pGpu, pKernelGsp)

NV_STATUS kgspWaitForProcessorSuspend_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);


#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspWaitForProcessorSuspend(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspWaitForProcessorSuspend(pGpu, pKernelGsp) kgspWaitForProcessorSuspend_TU102(pGpu, pKernelGsp)
#endif //__nvoc_kernel_gsp_h_disabled

#define kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp) kgspWaitForProcessorSuspend(pGpu, pKernelGsp)

NV_STATUS kgspConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, ENGDESCRIPTOR arg0);

static inline NV_STATUS kgspConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, ENGDESCRIPTOR arg0) {
    return pKernelGsp->__kgspConstructEngine__(pGpu, pKernelGsp, arg0);
}

void kgspRegisterIntrService_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceRecord pRecords[163]);

static inline void kgspRegisterIntrService_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceRecord pRecords[163]) {
    pKernelGsp->__kgspRegisterIntrService__(pGpu, pKernelGsp, pRecords);
}

NvU32 kgspServiceInterrupt_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceServiceInterruptArguments *pParams);

static inline NvU32 kgspServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelGsp->__kgspServiceInterrupt__(pGpu, pKernelGsp, pParams);
}

void kgspConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

void kgspConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspConfigureFalcon__(pGpu, pKernelGsp);
}

NvBool kgspIsDebugModeEnabled_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NvBool kgspIsDebugModeEnabled_GA100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvBool kgspIsDebugModeEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspIsDebugModeEnabled__(pGpu, pKernelGsp);
}

NV_STATUS kgspAllocBootArgs_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspAllocBootArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspAllocBootArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspAllocBootArgs__(pGpu, pKernelGsp);
}

void kgspFreeBootArgs_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

void kgspFreeBootArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline void kgspFreeBootArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    pKernelGsp->__kgspFreeBootArgs__(pGpu, pKernelGsp);
}

NV_STATUS kgspBootstrapRiscvOSEarly_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspBootstrapRiscvOSEarly_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspBootstrapRiscvOSEarly_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspBootstrapRiscvOSEarly_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspBootstrapRiscvOSEarly__(pGpu, pKernelGsp, pGspFw);
}

void kgspGetGspRmBootUcodeStorage_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

void kgspGetGspRmBootUcodeStorage_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

void kgspGetGspRmBootUcodeStorage_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc);

static inline void kgspGetGspRmBootUcodeStorage_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, BINDATA_STORAGE **ppBinStorageImage, BINDATA_STORAGE **ppBinStorageDesc) {
    pKernelGsp->__kgspGetGspRmBootUcodeStorage__(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_GH100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_AD102(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmBoot_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmBoot__(pKernelGsp);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwDebugSigned_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__(pKernelGsp);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveGspRmFmcGfwProdSigned_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveGspRmFmcGfwProdSigned__(pKernelGsp);
}

NV_STATUS kgspCalculateFbLayout_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

NV_STATUS kgspCalculateFbLayout_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspCalculateFbLayout_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspCalculateFbLayout__(pGpu, pKernelGsp, pGspFw);
}

static inline NvU32 kgspGetNonWprHeapSize_ed6b8b(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 1048576;
}

static inline NvU32 kgspGetNonWprHeapSize_d505ea(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 2097152;
}

static inline NvU32 kgspGetNonWprHeapSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetNonWprHeapSize__(pGpu, pKernelGsp);
}

NV_STATUS kgspExecuteSequencerCommand_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize);

NV_STATUS kgspExecuteSequencerCommand_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize);

static inline NV_STATUS kgspExecuteSequencerCommand_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 opCode, NvU32 *pPayLoad, NvU32 payloadSize) {
    return pKernelGsp->__kgspExecuteSequencerCommand__(pGpu, pKernelGsp, opCode, pPayLoad, payloadSize);
}

static inline NvU32 kgspReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId) {
    return 0;
}

NvU32 kgspReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId);

static inline NvU32 kgspReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvU32 ucodeId) {
    return pKernelGsp->__kgspReadUcodeFuseVersion__(pGpu, pKernelGsp, ucodeId);
}

NV_STATUS kgspResetHw_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspResetHw_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspResetHw__(pGpu, pKernelGsp);
}

NvBool kgspIsEngineInReset_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvBool kgspIsEngineInReset_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspIsEngineInReset__(pGpu, pKernelGsp);
}

NvU32 kgspGetFrtsSize_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NvU32 kgspGetFrtsSize_4a4dee(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return 0;
}

static inline NvU32 kgspGetFrtsSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetFrtsSize__(pGpu, pKernelGsp);
}

NV_STATUS kgspExtractVbiosFromRom_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg);

static inline NV_STATUS kgspExtractVbiosFromRom_395e98(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgspExtractVbiosFromRom_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspVbiosImg **ppVbiosImg) {
    return pKernelGsp->__kgspExtractVbiosFromRom__(pGpu, pKernelGsp, ppVbiosImg);
}

NV_STATUS kgspExecuteFwsecFrts_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset);

static inline NV_STATUS kgspExecuteFwsecFrts_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kgspExecuteFwsecFrts_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode, const NvU64 frtsOffset) {
    return pKernelGsp->__kgspExecuteFwsecFrts__(pGpu, pKernelGsp, pFwsecUcode, frtsOffset);
}

NV_STATUS kgspExecuteFwsecSb_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode);

static inline NV_STATUS kgspExecuteFwsecSb_ac1694(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode) {
    return NV_OK;
}

static inline NV_STATUS kgspExecuteFwsecSb_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFwsecUcode) {
    return pKernelGsp->__kgspExecuteFwsecSb__(pGpu, pKernelGsp, pFwsecUcode);
}

NV_STATUS kgspExecuteBooterLoad_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 gspFwWprMetaOffset);

static inline NV_STATUS kgspExecuteBooterLoad_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 gspFwWprMetaOffset) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kgspExecuteBooterLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const NvU64 gspFwWprMetaOffset) {
    return pKernelGsp->__kgspExecuteBooterLoad__(pGpu, pKernelGsp, gspFwWprMetaOffset);
}

NV_STATUS kgspExecuteBooterUnloadIfNeeded_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspExecuteBooterUnloadIfNeeded_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kgspExecuteBooterUnloadIfNeeded_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspExecuteBooterUnloadIfNeeded__(pGpu, pKernelGsp);
}

NV_STATUS kgspExecuteHsFalcon_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1);

NV_STATUS kgspExecuteHsFalcon_GA102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1);

static inline NV_STATUS kgspExecuteHsFalcon_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kgspExecuteHsFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, KernelGspFlcnUcode *pFlcnUcode, struct KernelFalcon *pKernelFlcn, NvU32 *pMailbox0, NvU32 *pMailbox1) {
    return pKernelGsp->__kgspExecuteHsFalcon__(pGpu, pKernelGsp, pFlcnUcode, pKernelFlcn, pMailbox0, pMailbox1);
}

NV_STATUS kgspWaitForGfwBootOk_TU102(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

NV_STATUS kgspWaitForGfwBootOk_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline NV_STATUS kgspWaitForGfwBootOk_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspWaitForGfwBootOk__(pGpu, pKernelGsp);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_TU116(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_AD102(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterLoadUcode_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveBooterLoadUcode__(pKernelGsp);
}

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_TU102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_TU116(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_GA100(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_GA102(struct KernelGsp *pKernelGsp);

const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_AD102(struct KernelGsp *pKernelGsp);

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_80f438(struct KernelGsp *pKernelGsp) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}

static inline const BINDATA_ARCHIVE *kgspGetBinArchiveBooterUnloadUcode_DISPATCH(struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetBinArchiveBooterUnloadUcode__(pKernelGsp);
}

static inline NvU64 kgspGetWprHeapSize_e77d51(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (64 * 1024 * 1024);
}

static inline NvU64 kgspGetWprHeapSize_38f3bc(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return (80 * 1024 * 1024);
}

static inline NvU64 kgspGetWprHeapSize_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetWprHeapSize__(pGpu, pKernelGsp);
}

const char *kgspGetSignatureSectionNamePrefix_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

static inline const char *kgspGetSignatureSectionNamePrefix_789efb(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return ".fwsignature_";
}

static inline const char *kgspGetSignatureSectionNamePrefix_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    return pKernelGsp->__kgspGetSignatureSectionNamePrefix__(pGpu, pKernelGsp);
}

NV_STATUS kgspSetupGspFmcArgs_GH100(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

static inline NV_STATUS kgspSetupGspFmcArgs_5baef9(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kgspSetupGspFmcArgs_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    return pKernelGsp->__kgspSetupGspFmcArgs__(pGpu, pKernelGsp, pGspFw);
}

static inline void kgspStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    pEngstate->__kgspStateDestroy__(pGpu, pEngstate);
}

static inline void kgspFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    pEngstate->__kgspFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kgspCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__kgspCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline NvBool kgspClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__kgspClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvBool kgspIsPresent_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspIsPresent__(pGpu, pEngstate);
}

static inline NV_STATUS kgspReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return pEngstate->__kgspReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kgspStateLoad_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspStateUnload_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspServiceNotificationInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__kgspServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kgspStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return pEngstate->__kgspGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kgspStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kgspInitMissing_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    pEngstate->__kgspInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return pEngstate->__kgspStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return pEngstate->__kgspStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgspAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void **ppTunableState) {
    return pEngstate->__kgspAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS kgspSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return pEngstate->__kgspSetTunableState__(pGpu, pEngstate, pTunableState);
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

void kgspSetupLibosInitArgs_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspSetupLibosInitArgs(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspSetupLibosInitArgs(pGpu, pKernelGsp) kgspSetupLibosInitArgs_IMPL(pGpu, pKernelGsp)
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

NV_STATUS kgspInitLogging_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspInitLogging(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, GSP_FIRMWARE *pGspFw) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspInitLogging(pGpu, pKernelGsp, pGspFw) kgspInitLogging_IMPL(pGpu, pKernelGsp, pGspFw)
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

void kgspDumpGspLogs_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvBool arg0);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline void kgspDumpGspLogs(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspDumpGspLogs(pGpu, pKernelGsp, arg0) kgspDumpGspLogs_IMPL(pGpu, pKernelGsp, arg0)
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

NV_STATUS kgspParseFwsecUcodeFromVbiosImg_IMPL(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const KernelGspVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode);

#ifdef __nvoc_kernel_gsp_h_disabled
static inline NV_STATUS kgspParseFwsecUcodeFromVbiosImg(struct OBJGPU *pGpu, struct KernelGsp *pKernelGsp, const KernelGspVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode) {
    NV_ASSERT_FAILED_PRECOMP("KernelGsp was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_gsp_h_disabled
#define kgspParseFwsecUcodeFromVbiosImg(pGpu, pKernelGsp, pVbiosImg, ppFwsecUcode) kgspParseFwsecUcodeFromVbiosImg_IMPL(pGpu, pKernelGsp, pVbiosImg, ppFwsecUcode)
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

#undef PRIVATE_FIELD


NV_STATUS rpcRmApiControl_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                              NvU32 cmd, void *pParamStructPtr, NvU32 paramsSize);
NV_STATUS rpcRmApiAlloc_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                            NvHandle hObject, NvU32 hClass, void *pAllocParams);
NV_STATUS rpcRmApiDupObject_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hParent, NvHandle *phObject,
                                NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags);
NV_STATUS rpcRmApiFree_GSP(RM_API *pRmApi, NvHandle hClient, NvHandle hObject);

/* Free a KernelGspVbiosImg structure */
void kgspFreeVbiosImg(KernelGspVbiosImg *pVbiosImg);
/* Free a KernelGspFlcnUcode structure */
void kgspFreeFlcnUcode(KernelGspFlcnUcode *pFlcnUcode);

#endif // KERNEL_GSP_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_GSP_NVOC_H_

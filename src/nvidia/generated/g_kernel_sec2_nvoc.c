#define NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sec2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2f36c9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelSec2(KernelSec2*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_dtor_KernelSec2(KernelSec2*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2;

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelSec2 = {
    /*pClassDef=*/          &__nvoc_class_def_KernelSec2,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelSec2,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelSec2 = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_KernelSec2_KernelSec2,
        &__nvoc_rtti_KernelSec2_KernelFalcon,
        &__nvoc_rtti_KernelSec2_KernelCrashCatEngine,
        &__nvoc_rtti_KernelSec2_CrashCatEngine,
        &__nvoc_rtti_KernelSec2_IntrService,
        &__nvoc_rtti_KernelSec2_OBJENGSTATE,
        &__nvoc_rtti_KernelSec2_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2 = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelSec2),
        /*classId=*/            classId(KernelSec2),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelSec2",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelSec2,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelSec2,
    /*pExportInfo=*/        &__nvoc_export_info_KernelSec2
};

static NV_STATUS __nvoc_thunk_KernelSec2_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelSec2, ENGDESCRIPTOR arg0) {
    return ksec2ConstructEngine(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelSec2_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelSec2, IntrServiceRecord pRecords[168]) {
    ksec2RegisterIntrService(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_IntrService.offset), pRecords);
}

static NV_STATUS __nvoc_thunk_KernelSec2_intrservServiceNotificationInterrupt(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return ksec2ServiceNotificationInterrupt(arg0, (struct KernelSec2 *)(((unsigned char *)arg1) - __nvoc_rtti_KernelSec2_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelSec2_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelSec2) {
    return ksec2ResetHw(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_KernelFalcon.offset));
}

static NV_STATUS __nvoc_thunk_KernelSec2_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelSec2, NvU32 arg0) {
    return ksec2StateLoad(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_KernelCrashCatEngine_ksec2Configured(struct KernelSec2 *arg0) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_ksec2PriRead(struct KernelSec2 *arg0, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset);
}

static void __nvoc_thunk_KernelFalcon_ksec2RegWrite(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), offset, data);
}

static NvU32 __nvoc_thunk_KernelFalcon_ksec2MaskDmemAddr(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), addr);
}

static void __nvoc_thunk_OBJENGSTATE_ksec2StateDestroy(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2Vprintf(struct KernelSec2 *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

static NvBool __nvoc_thunk_IntrService_ksec2ClearInterrupt(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelSec2_IntrService.offset), pParams);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2PriWrite(struct KernelSec2 *arg0, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, data);
}

static void *__nvoc_thunk_KernelCrashCatEngine_ksec2MapBufferDescriptor(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2SyncBufferDescriptor(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

static NvU32 __nvoc_thunk_KernelFalcon_ksec2RegRead(struct OBJGPU *pGpu, struct KernelSec2 *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelSec2_KernelFalcon.offset), offset);
}

static NvBool __nvoc_thunk_OBJENGSTATE_ksec2IsPresent(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NvU32 __nvoc_thunk_IntrService_ksec2ServiceInterrupt(struct OBJGPU *pGpu, struct KernelSec2 *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelSec2_IntrService.offset), pParams);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2ReadEmem(struct KernelSec2 *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, size, pBuf);
}

static const NvU32 *__nvoc_thunk_KernelCrashCatEngine_ksec2GetScratchOffsets(struct KernelSec2 *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), scratchGroupId);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2Unload(struct KernelSec2 *arg0) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_ksec2GetWFL0Offset(struct KernelSec2 *arg0) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateInitLocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreLoad(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePostUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateInitUnlocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_ksec2InitMissing(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreInitLocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePostLoad(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2UnmapBufferDescriptor(struct KernelSec2 *arg0, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_ksec2ReadDmem(struct KernelSec2 *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelSec2_KernelCrashCatEngine.offset), offset, size, pBuf);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2 = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelSec2(KernelSec2 *pThis) {
    __nvoc_ksec2Destruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_KernelFalcon;
    __nvoc_init_dataField_KernelSec2(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelSec2_exit; // Success

__nvoc_ctor_KernelSec2_fail_KernelFalcon:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelSec2_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelSec2_fail_OBJENGSTATE:
__nvoc_ctor_KernelSec2_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelSec2_1(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // Hal function -- ksec2ConstructEngine
    pThis->__ksec2ConstructEngine__ = &ksec2ConstructEngine_IMPL;

    // Hal function -- ksec2RegisterIntrService
    pThis->__ksec2RegisterIntrService__ = &ksec2RegisterIntrService_IMPL;

    // Hal function -- ksec2ServiceNotificationInterrupt
    pThis->__ksec2ServiceNotificationInterrupt__ = &ksec2ServiceNotificationInterrupt_IMPL;

    // Hal function -- ksec2ConfigureFalcon
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_TU102;
    }
    else
    {
        pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA102;
    }

    // Hal function -- ksec2ResetHw
    pThis->__ksec2ResetHw__ = &ksec2ResetHw_TU102;

    // Hal function -- ksec2StateLoad
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__ksec2StateLoad__ = &ksec2StateLoad_GH100;
    }
    // default
    else
    {
        pThis->__ksec2StateLoad__ = &ksec2StateLoad_56cd7a;
    }

    // Hal function -- ksec2ReadUcodeFuseVersion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_b2b553;
    }
    else
    {
        pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_GA100;
    }

    // Hal function -- ksec2GetBinArchiveBlUcode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__ksec2GetBinArchiveBlUcode__ = &ksec2GetBinArchiveBlUcode_80f438;
    }
    else
    {
        pThis->__ksec2GetBinArchiveBlUcode__ = &ksec2GetBinArchiveBlUcode_TU102;
    }

    // Hal function -- ksec2GetGenericBlUcode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__ksec2GetGenericBlUcode__ = &ksec2GetGenericBlUcode_5baef9;
    }
    else
    {
        pThis->__ksec2GetGenericBlUcode__ = &ksec2GetGenericBlUcode_TU102;
    }

    // Hal function -- ksec2GetBinArchiveSecurescrubUcode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__ksec2GetBinArchiveSecurescrubUcode__ = &ksec2GetBinArchiveSecurescrubUcode_AD10X;
    }
    else
    {
        pThis->__ksec2GetBinArchiveSecurescrubUcode__ = &ksec2GetBinArchiveSecurescrubUcode_80f438;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelSec2_engstateConstructEngine;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelSec2_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_KernelSec2_intrservServiceNotificationInterrupt;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_KernelSec2_kflcnResetHw;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelSec2_engstateStateLoad;

    pThis->__ksec2Configured__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2Configured;

    pThis->__ksec2PriRead__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2PriRead;

    pThis->__ksec2RegWrite__ = &__nvoc_thunk_KernelFalcon_ksec2RegWrite;

    pThis->__ksec2MaskDmemAddr__ = &__nvoc_thunk_KernelFalcon_ksec2MaskDmemAddr;

    pThis->__ksec2StateDestroy__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateDestroy;

    pThis->__ksec2Vprintf__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2Vprintf;

    pThis->__ksec2ClearInterrupt__ = &__nvoc_thunk_IntrService_ksec2ClearInterrupt;

    pThis->__ksec2PriWrite__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2PriWrite;

    pThis->__ksec2MapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2MapBufferDescriptor;

    pThis->__ksec2SyncBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2SyncBufferDescriptor;

    pThis->__ksec2RegRead__ = &__nvoc_thunk_KernelFalcon_ksec2RegRead;

    pThis->__ksec2IsPresent__ = &__nvoc_thunk_OBJENGSTATE_ksec2IsPresent;

    pThis->__ksec2ServiceInterrupt__ = &__nvoc_thunk_IntrService_ksec2ServiceInterrupt;

    pThis->__ksec2ReadEmem__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2ReadEmem;

    pThis->__ksec2GetScratchOffsets__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2GetScratchOffsets;

    pThis->__ksec2Unload__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2Unload;

    pThis->__ksec2StateUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateUnload;

    pThis->__ksec2GetWFL0Offset__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2GetWFL0Offset;

    pThis->__ksec2StateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateInitLocked;

    pThis->__ksec2StatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreLoad;

    pThis->__ksec2StatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePostUnload;

    pThis->__ksec2StatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreUnload;

    pThis->__ksec2StateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateInitUnlocked;

    pThis->__ksec2InitMissing__ = &__nvoc_thunk_OBJENGSTATE_ksec2InitMissing;

    pThis->__ksec2StatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreInitLocked;

    pThis->__ksec2StatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked;

    pThis->__ksec2StatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePostLoad;

    pThis->__ksec2UnmapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2UnmapBufferDescriptor;

    pThis->__ksec2ReadDmem__ = &__nvoc_thunk_KernelCrashCatEngine_ksec2ReadDmem;
}

void __nvoc_init_funcTable_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelSec2_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelSec2 = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelSec2(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelSec2 *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelSec2), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelSec2));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelSec2);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_KernelSec2(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelSec2(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelSec2_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelSec2_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelSec2));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelSec2(ppThis, pParent, createFlags);

    return status;
}


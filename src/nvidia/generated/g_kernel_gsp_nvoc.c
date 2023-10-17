#define NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_gsp_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x311d4e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGsp(KernelGsp*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_dtor_KernelGsp(KernelGsp*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp;

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelGsp = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGsp,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGsp,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGsp = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGsp_KernelGsp,
        &__nvoc_rtti_KernelGsp_KernelFalcon,
        &__nvoc_rtti_KernelGsp_KernelCrashCatEngine,
        &__nvoc_rtti_KernelGsp_CrashCatEngine,
        &__nvoc_rtti_KernelGsp_IntrService,
        &__nvoc_rtti_KernelGsp_OBJENGSTATE,
        &__nvoc_rtti_KernelGsp_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGsp),
        /*classId=*/            classId(KernelGsp),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGsp",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGsp,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGsp,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGsp
};

static NV_STATUS __nvoc_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg0) {
    return kgspConstructEngine(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[168]) {
    kgspRegisterIntrService(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pRecords);
}

static NvU32 __nvoc_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return kgspServiceInterrupt(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspResetHw(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_KernelFalcon.offset));
}

static NvBool __nvoc_thunk_KernelCrashCatEngine_kgspConfigured(struct KernelGsp *arg0) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_kgspPriRead(struct KernelGsp *arg0, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset);
}

static void __nvoc_thunk_KernelFalcon_kgspRegWrite(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), offset, data);
}

static NvU32 __nvoc_thunk_KernelFalcon_kgspMaskDmemAddr(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), addr);
}

static void __nvoc_thunk_OBJENGSTATE_kgspStateDestroy(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspVprintf(struct KernelGsp *arg0, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

static NvBool __nvoc_thunk_IntrService_kgspClearInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspPriWrite(struct KernelGsp *arg0, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, data);
}

static void *__nvoc_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor(struct KernelGsp *arg0, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor(struct KernelGsp *arg0, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

static NvU32 __nvoc_thunk_KernelFalcon_kgspRegRead(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *)pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), offset);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kgspIsPresent(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspReadEmem(struct KernelGsp *arg0, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, size, pBuf);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static const NvU32 *__nvoc_thunk_KernelCrashCatEngine_kgspGetScratchOffsets(struct KernelGsp *arg0, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), scratchGroupId);
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspUnload(struct KernelGsp *arg0) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_IntrService_kgspServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static NvU32 __nvoc_thunk_KernelCrashCatEngine_kgspGetWFL0Offset(struct KernelGsp *arg0) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateInitLocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePostUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateInitUnlocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kgspInitMissing(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreInitLocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreInitUnlocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePostLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor(struct KernelGsp *arg0, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc);
}

static void __nvoc_thunk_KernelCrashCatEngine_kgspReadDmem(struct KernelGsp *arg0, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *)arg0) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, size, pBuf);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelGsp(KernelGsp *pThis) {
    __nvoc_kgspDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal field -- bIsTaskIsrQueueRequired
    pThis->bIsTaskIsrQueueRequired = ((NvBool)(0 != 0));

    // Hal field -- bPartitionedFmc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bPartitionedFmc = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bPartitionedFmc = ((NvBool)(0 != 0));
    }

    // Hal field -- bScrubberUcodeSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bScrubberUcodeSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bScrubberUcodeSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- fwHeapParamBaseSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->fwHeapParamBaseSize = (8 << 20);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->fwHeapParamBaseSize = (14 << 20);
    }

    // Hal field -- bBootGspRmWithBoostClocks
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bBootGspRmWithBoostClocks = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBootGspRmWithBoostClocks = ((NvBool)(0 != 0));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_KernelFalcon;
    __nvoc_init_dataField_KernelGsp(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGsp_exit; // Success

__nvoc_ctor_KernelGsp_fail_KernelFalcon:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelGsp_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGsp_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsp_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelGsp_1(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__kgspConstructEngine__ = &kgspConstructEngine_IMPL;

    pThis->__kgspRegisterIntrService__ = &kgspRegisterIntrService_IMPL;

    pThis->__kgspServiceInterrupt__ = &kgspServiceInterrupt_IMPL;

    // Hal function -- kgspConfigureFalcon
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_TU102;
    }
    else
    {
        pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_GA102;
    }

    // Hal function -- kgspIsDebugModeEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_TU102;
    }
    else
    {
        pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GA100;
    }

    // Hal function -- kgspAllocBootArgs
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_GH100;
    }
    else
    {
        pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_TU102;
    }

    // Hal function -- kgspFreeBootArgs
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_GH100;
    }
    else
    {
        pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_TU102;
    }

    // Hal function -- kgspBootstrapRiscvOSEarly
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspBootstrapRiscvOSEarly__ = &kgspBootstrapRiscvOSEarly_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspBootstrapRiscvOSEarly__ = &kgspBootstrapRiscvOSEarly_TU102;
    }
    else
    {
        pThis->__kgspBootstrapRiscvOSEarly__ = &kgspBootstrapRiscvOSEarly_GA102;
    }

    // Hal function -- kgspGetGspRmBootUcodeStorage
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_TU102;
    }
    else
    {
        pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GA102;
    }

    // Hal function -- kgspGetBinArchiveGspRmBoot
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA102;
    }
    else
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_AD102;
    }

    // Hal function -- kgspGetBinArchiveConcatenatedFMCDesc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GH100;
    }
    else
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_80f438;
    }

    // Hal function -- kgspGetBinArchiveConcatenatedFMC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GH100;
    }
    else
    {
        pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_80f438;
    }

    // Hal function -- kgspGetBinArchiveGspRmFmcGfwDebugSigned
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100;
    }
    // default
    else
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438;
    }

    // Hal function -- kgspGetBinArchiveGspRmFmcGfwProdSigned
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100;
    }
    // default
    else
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438;
    }

    // Hal function -- kgspGetBinArchiveGspRmCcFmcGfwProdSigned
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GH100;
    }
    // default
    else
    {
        pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438;
    }

    // Hal function -- kgspCalculateFbLayout
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_GH100;
    }
    else
    {
        pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_TU102;
    }

    // Hal function -- kgspGetNonWprHeapSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_d505ea;
    }
    else
    {
        pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_ed6b8b;
    }

    // Hal function -- kgspExecuteSequencerCommand
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_TU102;
    }
    else
    {
        pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_GA102;
    }

    // Hal function -- kgspReadUcodeFuseVersion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_b2b553;
    }
    else
    {
        pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GA100;
    }

    // Hal function -- kgspResetHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspResetHw__ = &kgspResetHw_GH100;
    }
    else
    {
        pThis->__kgspResetHw__ = &kgspResetHw_TU102;
    }

    // Hal function -- kgspIsWpr2Up
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_GH100;
    }
    else
    {
        pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_TU102;
    }

    // Hal function -- kgspGetFrtsSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_4a4dee;
    }
    else
    {
        pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_TU102;
    }

    // Hal function -- kgspGetPrescrubbedTopFbSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_604eb7;
    }
    else
    {
        pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_e1e623;
    }

    // Hal function -- kgspExtractVbiosFromRom
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_395e98;
    }
    else
    {
        pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_TU102;
    }

    // Hal function -- kgspExecuteFwsecFrts
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kgspExecuteFwsecFrts__ = &kgspExecuteFwsecFrts_5baef9;
    }
    else
    {
        pThis->__kgspExecuteFwsecFrts__ = &kgspExecuteFwsecFrts_TU102;
    }

    // Hal function -- kgspExecuteFwsecSb
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspExecuteFwsecSb__ = &kgspExecuteFwsecSb_ac1694;
    }
    else
    {
        pThis->__kgspExecuteFwsecSb__ = &kgspExecuteFwsecSb_TU102;
    }

    // Hal function -- kgspExecuteScrubberIfNeeded
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_AD102;
    }
    else
    {
        pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_5baef9;
    }

    // Hal function -- kgspExecuteBooterLoad
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_5baef9;
    }
    else
    {
        pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_TU102;
    }

    // Hal function -- kgspExecuteBooterUnloadIfNeeded
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_5baef9;
    }
    else
    {
        pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_TU102;
    }

    // Hal function -- kgspExecuteHsFalcon
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_5baef9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_TU102;
    }
    else
    {
        pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_GA102;
    }

    // Hal function -- kgspWaitForGfwBootOk
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_GH100;
    }
    else
    {
        pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_TU102;
    }

    // Hal function -- kgspGetBinArchiveBooterLoadUcode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_80f438;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU116;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA102;
    }
    else
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_AD102;
    }

    // Hal function -- kgspGetBinArchiveBooterUnloadUcode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_80f438;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU116;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA102;
    }
    else
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_AD102;
    }

    // Hal function -- kgspGetMinWprHeapSizeMB
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_7185bf;
    }
    else
    {
        pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_cc88c3;
    }

    // Hal function -- kgspGetMaxWprHeapSizeMB
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_ad4e6a;
    }
    else
    {
        pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_55728f;
    }

    // Hal function -- kgspGetFwHeapParamOsCarveoutSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_397f70;
    }
    else
    {
        pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_4b5307;
    }

    // Hal function -- kgspInitVgpuPartitionLogging
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
    }
    else
    {
        pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_IMPL;
    }

    // Hal function -- kgspFreeVgpuPartitionLogging
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
    }
    else
    {
        pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_IMPL;
    }

    // Hal function -- kgspGetSignatureSectionNamePrefix
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GH100;
    }
    // default
    else
    {
        pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_789efb;
    }

    // Hal function -- kgspSetupGspFmcArgs
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_GH100;
    }
    // default
    else
    {
        pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_5baef9;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelGsp_engstateConstructEngine;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelGsp_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_KernelGsp_intrservServiceInterrupt;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_KernelGsp_kflcnResetHw;

    pThis->__kgspConfigured__ = &__nvoc_thunk_KernelCrashCatEngine_kgspConfigured;

    pThis->__kgspPriRead__ = &__nvoc_thunk_KernelCrashCatEngine_kgspPriRead;

    pThis->__kgspRegWrite__ = &__nvoc_thunk_KernelFalcon_kgspRegWrite;

    pThis->__kgspMaskDmemAddr__ = &__nvoc_thunk_KernelFalcon_kgspMaskDmemAddr;

    pThis->__kgspStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kgspStateDestroy;

    pThis->__kgspVprintf__ = &__nvoc_thunk_KernelCrashCatEngine_kgspVprintf;

    pThis->__kgspClearInterrupt__ = &__nvoc_thunk_IntrService_kgspClearInterrupt;

    pThis->__kgspPriWrite__ = &__nvoc_thunk_KernelCrashCatEngine_kgspPriWrite;

    pThis->__kgspMapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor;

    pThis->__kgspSyncBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor;

    pThis->__kgspRegRead__ = &__nvoc_thunk_KernelFalcon_kgspRegRead;

    pThis->__kgspIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kgspIsPresent;

    pThis->__kgspReadEmem__ = &__nvoc_thunk_KernelCrashCatEngine_kgspReadEmem;

    pThis->__kgspStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStateLoad;

    pThis->__kgspGetScratchOffsets__ = &__nvoc_thunk_KernelCrashCatEngine_kgspGetScratchOffsets;

    pThis->__kgspUnload__ = &__nvoc_thunk_KernelCrashCatEngine_kgspUnload;

    pThis->__kgspStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStateUnload;

    pThis->__kgspServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_kgspServiceNotificationInterrupt;

    pThis->__kgspGetWFL0Offset__ = &__nvoc_thunk_KernelCrashCatEngine_kgspGetWFL0Offset;

    pThis->__kgspStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStateInitLocked;

    pThis->__kgspStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreLoad;

    pThis->__kgspStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePostUnload;

    pThis->__kgspStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreUnload;

    pThis->__kgspStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStateInitUnlocked;

    pThis->__kgspInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kgspInitMissing;

    pThis->__kgspStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreInitLocked;

    pThis->__kgspStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreInitUnlocked;

    pThis->__kgspStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePostLoad;

    pThis->__kgspUnmapBufferDescriptor__ = &__nvoc_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor;

    pThis->__kgspReadDmem__ = &__nvoc_thunk_KernelCrashCatEngine_kgspReadDmem;
}

void __nvoc_init_funcTable_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelGsp_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGsp = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGsp(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelGsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGsp), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelGsp));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGsp);

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

    __nvoc_init_KernelGsp(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGsp(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGsp_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGsp_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGsp));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsp(ppThis, pParent, createFlags);

    return status;
}


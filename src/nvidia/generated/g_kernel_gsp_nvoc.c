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

// 6 down-thunk(s) defined to bridge methods in KernelGsp from superclasses

// kgspConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg3) {
    return kgspConstructEngine(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelGsp_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp) {
    return kgspStateInitLocked(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspRegisterIntrService: virtual override (intrserv) base (intrserv)
static void __nvoc_down_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[175]) {
    kgspRegisterIntrService(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pRecords);
}

// kgspServiceInterrupt: virtual override (intrserv) base (intrserv)
static NvU32 __nvoc_down_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return kgspServiceInterrupt(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

// kgspResetHw: virtual halified (4 hals) override (kflcn) base (kflcn) body
static NV_STATUS __nvoc_down_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspResetHw(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_KernelFalcon.offset));
}

// kgspReadEmem: virtual halified (2 hals) override (kcrashcatEngine) base (kflcn) body
static void __nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem(struct KernelCrashCatEngine *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf) {
    kgspReadEmem((struct KernelGsp *)(((unsigned char *) pKernelGsp) - __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, size, pBuf);
}


// 28 up-thunk(s) defined to bridge methods in KernelGsp to superclasses

// kgspInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kgspInitMissing(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostLoad(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePreUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStateUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspStatePostUnload(struct OBJGPU *pGpu, struct KernelGsp *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg3);
}

// kgspStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kgspStateDestroy(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kgspIsPresent(struct OBJGPU *pGpu, struct KernelGsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

// kgspClearInterrupt: virtual inherited (intrserv) base (intrserv)
static NvBool __nvoc_up_thunk_IntrService_kgspClearInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

// kgspServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
static NV_STATUS __nvoc_up_thunk_IntrService_kgspServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

// kgspRegRead: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
static NvU32 __nvoc_up_thunk_KernelFalcon_kgspRegRead(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), offset);
}

// kgspRegWrite: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
static void __nvoc_up_thunk_KernelFalcon_kgspRegWrite(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), offset, data);
}

// kgspMaskDmemAddr: virtual halified (3 hals) inherited (kflcn) base (kflcn) body
static NvU32 __nvoc_up_thunk_KernelFalcon_kgspMaskDmemAddr(struct OBJGPU *pGpu, struct KernelGsp *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + __nvoc_rtti_KernelGsp_KernelFalcon.offset), addr);
}

// kgspConfigured: virtual inherited (kcrashcatEngine) base (kflcn)
static NvBool __nvoc_up_thunk_KernelCrashCatEngine_kgspConfigured(struct KernelGsp *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
}

// kgspUnload: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnload(struct KernelGsp *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
}

// kgspVprintf: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspVprintf(struct KernelGsp *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), bReportStart, fmt, args);
}

// kgspPriRead: virtual inherited (kcrashcatEngine) base (kflcn)
static NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspPriRead(struct KernelGsp *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset);
}

// kgspPriWrite: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspPriWrite(struct KernelGsp *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, data);
}

// kgspMapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void * __nvoc_up_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc);
}

// kgspUnmapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc);
}

// kgspSyncBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor(struct KernelGsp *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), pBufDesc, offset, size);
}

// kgspReadDmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static void __nvoc_up_thunk_KernelCrashCatEngine_kgspReadDmem(struct KernelGsp *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), offset, size, pBuf);
}

// kgspGetScratchOffsets: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kgspGetScratchOffsets(struct KernelGsp *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset), scratchGroupId);
}

// kgspGetWFL0Offset: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
static NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kgspGetWFL0Offset(struct KernelGsp *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + __nvoc_rtti_KernelGsp_KernelCrashCatEngine.offset));
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

    // Hal field -- bPartitionedFmc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->fwHeapParamBaseSize = (14 << 20);
    }

    // Hal field -- bBootGspRmWithBoostClocks
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bBootGspRmWithBoostClocks = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBootGspRmWithBoostClocks = ((NvBool)(0 != 0));
    }

    // Hal field -- ememPort
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->ememPort = 6;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->ememPort = 2;
    }
    // default
    else
    {
        pThis->ememPort = 0;
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

// Vtable initialization
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

    // kgspConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kgspConstructEngine__ = &kgspConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGsp_engstateConstructEngine;

    // kgspStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kgspStateInitLocked__ = &kgspStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelGsp_engstateStateInitLocked;

    // kgspRegisterIntrService -- virtual override (intrserv) base (intrserv)
    pThis->__kgspRegisterIntrService__ = &kgspRegisterIntrService_IMPL;
    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelGsp_intrservRegisterIntrService;

    // kgspServiceInterrupt -- virtual override (intrserv) base (intrserv)
    pThis->__kgspServiceInterrupt__ = &kgspServiceInterrupt_IMPL;
    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelGsp_intrservServiceInterrupt;

    // kgspConfigureFalcon -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_TU102;
        }
        else
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_GA102;
        }
    }

    // kgspIsDebugModeEnabled -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_108313;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_TU102;
        }
        else
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GA100;
        }
    }

    // kgspAllocBootArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_GH100;
        }
        else
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_TU102;
        }
    }

    // kgspFreeBootArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_GH100;
        }
        else
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_TU102;
        }
    }

    // kgspProgramLibosBootArgsAddr -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspProgramLibosBootArgsAddr__ = &kgspProgramLibosBootArgsAddr_f2d351;
    }
    else
    {
        pThis->__kgspProgramLibosBootArgsAddr__ = &kgspProgramLibosBootArgsAddr_TU102;
    }

    // kgspSetCmdQueueHead -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspSetCmdQueueHead__ = &kgspSetCmdQueueHead_5baef9;
    }
    else
    {
        pThis->__kgspSetCmdQueueHead__ = &kgspSetCmdQueueHead_TU102;
    }

    // kgspPrepareForBootstrap -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_GH100;
        }
        else
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_TU102;
        }
    }

    // kgspBootstrap -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspBootstrap__ = &kgspBootstrap_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_GH100;
        }
        else
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_TU102;
        }
    }

    // kgspGetGspRmBootUcodeStorage -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_TU102;
        }
        else
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GA102;
        }
    }

    // kgspGetBinArchiveGspRmBoot -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB100;
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
    }

    // kgspGetBinArchiveConcatenatedFMCDesc -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GH100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_80f438;
        }
    }

    // kgspGetBinArchiveConcatenatedFMC -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GH100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_80f438;
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwDebugSigned -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_80f438;
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwProdSigned -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_80f438;
        }
    }

    // kgspGetBinArchiveGspRmCcFmcGfwProdSigned -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GH100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_80f438;
        }
    }

    // kgspCalculateFbLayout -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_GH100;
        }
        else
        {
            pThis->__kgspCalculateFbLayout__ = &kgspCalculateFbLayout_TU102;
        }
    }

    // kgspGetNonWprHeapSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_d505ea;
        }
        else
        {
            pThis->__kgspGetNonWprHeapSize__ = &kgspGetNonWprHeapSize_ed6b8b;
        }
    }

    // kgspExecuteSequencerCommand -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_TU102;
        }
        else
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_GA102;
        }
    }

    // kgspReadUcodeFuseVersion -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_b2b553;
        }
        else
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GA100;
        }
    }

    // kgspResetHw -- virtual halified (4 hals) override (kflcn) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspResetHw__ = &kgspResetHw_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GB100;
        }
        else
        {
            pThis->__kgspResetHw__ = &kgspResetHw_TU102;
        }
    }
    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_down_thunk_KernelGsp_kflcnResetHw;

    // kgspHealthCheck -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspHealthCheck__ = &kgspHealthCheck_108313;
    }
    else
    {
        pThis->__kgspHealthCheck__ = &kgspHealthCheck_TU102;
    }

    // kgspService -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspService__ = &kgspService_474d46;
    }
    else
    {
        pThis->__kgspService__ = &kgspService_TU102;
    }

    // kgspIsWpr2Up -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_108313;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_GH100;
        }
        else
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_TU102;
        }
    }

    // kgspGetFrtsSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_4a4dee;
        }
        else
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_TU102;
        }
    }

    // kgspGetPrescrubbedTopFbSize -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_604eb7;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_474d46;
        }
        else
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_e1e623;
        }
    }

    // kgspExtractVbiosFromRom -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_395e98;
        }
        else
        {
            pThis->__kgspExtractVbiosFromRom__ = &kgspExtractVbiosFromRom_TU102;
        }
    }

    // kgspPrepareForFwsecFrts -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_5baef9;
        }
        else
        {
            pThis->__kgspPrepareForFwsecFrts__ = &kgspPrepareForFwsecFrts_TU102;
        }
    }

    // kgspPrepareForFwsecSb -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_395e98;
        }
        else
        {
            pThis->__kgspPrepareForFwsecSb__ = &kgspPrepareForFwsecSb_TU102;
        }
    }

    // kgspExecuteFwsec -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_5baef9;
        }
        else
        {
            pThis->__kgspExecuteFwsec__ = &kgspExecuteFwsec_TU102;
        }
    }

    // kgspExecuteScrubberIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_AD102;
        }
        else
        {
            pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_5baef9;
        }
    }

    // kgspExecuteBooterLoad -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_5baef9;
        }
        else
        {
            pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_TU102;
        }
    }

    // kgspExecuteBooterUnloadIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_5baef9;
        }
        else
        {
            pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_TU102;
        }
    }

    // kgspExecuteHsFalcon -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
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
    }

    // kgspWaitForProcessorSuspend -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspWaitForProcessorSuspend__ = &kgspWaitForProcessorSuspend_5baef9;
    }
    else
    {
        pThis->__kgspWaitForProcessorSuspend__ = &kgspWaitForProcessorSuspend_TU102;
    }

    // kgspSavePowerMgmtState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspSavePowerMgmtState__ = &kgspSavePowerMgmtState_5baef9;
    }
    else
    {
        pThis->__kgspSavePowerMgmtState__ = &kgspSavePowerMgmtState_TU102;
    }

    // kgspRestorePowerMgmtState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspRestorePowerMgmtState__ = &kgspRestorePowerMgmtState_5baef9;
    }
    else
    {
        pThis->__kgspRestorePowerMgmtState__ = &kgspRestorePowerMgmtState_TU102;
    }

    // kgspFreeSuspendResumeData -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeSuspendResumeData__ = &kgspFreeSuspendResumeData_f2d351;
    }
    else
    {
        pThis->__kgspFreeSuspendResumeData__ = &kgspFreeSuspendResumeData_TU102;
    }

    // kgspWaitForGfwBootOk -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_GH100;
        }
        else
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_TU102;
        }
    }

    // kgspGetBinArchiveBooterLoadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_80f438;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA102;
        }
        else
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_AD102;
        }
    }

    // kgspGetBinArchiveBooterUnloadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_80f438;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA102;
        }
        else
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_AD102;
        }
    }

    // kgspGetMinWprHeapSizeMB -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_7185bf;
        }
        else
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_cc88c3;
        }
    }

    // kgspGetMaxWprHeapSizeMB -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_ad4e6a;
        }
        else
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_55728f;
        }
    }

    // kgspGetFwHeapParamOsCarveoutSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_397f70;
        }
        else
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_4b5307;
        }
    }

    // kgspInitVgpuPartitionLogging -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
        }
        else
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_IMPL;
        }
    }

    // kgspFreeVgpuPartitionLogging -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
        }
        else
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_IMPL;
        }
    }

    // kgspGetSignatureSectionNamePrefix -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_789efb;
        }
    }

    // kgspSetupGspFmcArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_GH100;
        }
        // default
        else
        {
            pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_5baef9;
        }
    }

    // kgspReadEmem -- virtual halified (2 hals) override (kcrashcatEngine) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspReadEmem__ = &kgspReadEmem_366c4c;
    }
    else
    {
        pThis->__kgspReadEmem__ = &kgspReadEmem_TU102;
    }
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__kcrashcatEngineReadEmem__ = &__nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem;

    // kgspIssueNotifyOp -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_GH100;
        }
        // default
        else
        {
            pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_5baef9;
        }
    }

    // kgspCheckGspRmCcCleanup -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_GH100;
        }
        // default
        else
        {
            pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_5baef9;
        }
    }

    // kgspInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kgspInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgspInitMissing;

    // kgspStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitLocked;

    // kgspStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreInitUnlocked;

    // kgspStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateInitUnlocked;

    // kgspStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreLoad;

    // kgspStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateLoad;

    // kgspStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePostLoad;

    // kgspStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePreUnload;

    // kgspStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateUnload;

    // kgspStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStatePostUnload;

    // kgspStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__kgspStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kgspStateDestroy;

    // kgspIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kgspIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgspIsPresent;

    // kgspClearInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__kgspClearInterrupt__ = &__nvoc_up_thunk_IntrService_kgspClearInterrupt;

    // kgspServiceNotificationInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__kgspServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_kgspServiceNotificationInterrupt;

    // kgspRegRead -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspRegRead__ = &__nvoc_up_thunk_KernelFalcon_kgspRegRead;

    // kgspRegWrite -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspRegWrite__ = &__nvoc_up_thunk_KernelFalcon_kgspRegWrite;

    // kgspMaskDmemAddr -- virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    pThis->__kgspMaskDmemAddr__ = &__nvoc_up_thunk_KernelFalcon_kgspMaskDmemAddr;

    // kgspConfigured -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspConfigured;

    // kgspUnload -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspUnload;

    // kgspVprintf -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspVprintf;

    // kgspPriRead -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspPriRead;

    // kgspPriWrite -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspPriWrite;

    // kgspMapBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspMapBufferDescriptor;

    // kgspUnmapBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspUnmapBufferDescriptor;

    // kgspSyncBufferDescriptor -- virtual inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspSyncBufferDescriptor;

    // kgspReadDmem -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspReadDmem;

    // kgspGetScratchOffsets -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspGetScratchOffsets;

    // kgspGetWFL0Offset -- virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    pThis->__kgspGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_kgspGetWFL0Offset;
} // End __nvoc_init_funcTable_KernelGsp_1 with approximately 203 basic block(s).


// Initialize vtable(s) for 82 virtual method(s).
void __nvoc_init_funcTable_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 82 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGsp), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGsp));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGsp);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGsp_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGsp));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsp(ppThis, pParent, createFlags);

    return status;
}


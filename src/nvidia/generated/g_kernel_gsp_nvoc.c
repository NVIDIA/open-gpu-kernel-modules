#define NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_gsp_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__311d4e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

// Forward declarations for KernelGsp
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__KernelFalcon(KernelFalcon*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__KernelGsp(KernelGsp*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGsp(KernelGsp*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelGsp(KernelGsp*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGsp(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGsp;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGsp;

// Down-thunk(s) to bridge KernelGsp methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvU64 errorId, const char *fmt, va_list args);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // super^2
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super^2
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // super^2
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset);    // super
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data);    // super
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr);    // super
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp);    // this
void __nvoc_down_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[320]);    // this
NvU32 __nvoc_down_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams);    // this
NV_STATUS __nvoc_down_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp);    // this
void __nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem(struct KernelCrashCatEngine *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf);    // this

// Up-thunk(s) to bridge KernelGsp methods to ancestors (if any)

// Class-specific details for KernelGsp
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp = 
{
    .classInfo.size =               sizeof(KernelGsp),
    .classInfo.classId =            classId(KernelGsp),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelGsp",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelGsp,
    .pCastInfo =          &__nvoc_castinfo__KernelGsp,
    .pExportInfo =        &__nvoc_export_info__KernelGsp
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelGsp __nvoc_metadata__KernelGsp = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGsp,    // (kgsp) this
    .rtti.dtor      = &__nvoc_dtor_KernelGsp,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService),
    .metadata__KernelFalcon.rtti.pClassDef = &__nvoc_class_def_KernelFalcon,    // (kflcn) super
    .metadata__KernelFalcon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) super^2
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super^3
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),

    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGsp_engstateConstructEngine,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelGsp_engstateStateInitLocked,    // virtual
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelGsp_intrservRegisterIntrService,    // virtual
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelGsp_intrservServiceInterrupt,    // virtual
    .metadata__KernelFalcon.vtable.__kflcnResetHw__ = &__nvoc_down_thunk_KernelGsp_kflcnResetHw,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadEmem__ = &__nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem,    // virtual halified (singleton optimized) body
    .metadata__OBJENGSTATE.vtable.__engstateConstructApertures__ = &engstateConstructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateDestructApertures__ = &engstateDestructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineResetLog__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineResetLog,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGsp = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__KernelGsp.rtti,    // [0]: (kgsp) this
        &__nvoc_metadata__KernelGsp.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelGsp.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__KernelGsp.metadata__IntrService.rtti,    // [3]: (intrserv) super
        &__nvoc_metadata__KernelGsp.metadata__KernelFalcon.rtti,    // [4]: (kflcn) super
        &__nvoc_metadata__KernelGsp.metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti,    // [5]: (kcrashcatEngine) super^2
        &__nvoc_metadata__KernelGsp.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [6]: (crashcatEngine) super^3
    }
};

// 6 down-thunk(s) defined to bridge methods in KernelGsp from superclasses

// kgspConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg3) {
    return kgspConstructEngine_IMPL(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsp_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp) {
    return kgspStateInitLocked_IMPL(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE)));
}

// kgspRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[320]) {
    kgspRegisterIntrService_IMPL(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pRecords);
}

// kgspServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return kgspServiceInterrupt_IMPL(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService)), pParams);
}

// kgspResetHw: virtual halified (5 hals) override (kflcn) base (kflcn) body
NV_STATUS __nvoc_down_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspResetHw(pGpu, (struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon)));
}

// kgspReadEmem: virtual halified (3 hals) override (kcrashcatEngine) base (kflcn) body
void __nvoc_down_thunk_KernelGsp_kcrashcatEngineReadEmem(struct KernelCrashCatEngine *pKernelGsp, NvU64 offset, NvU64 size, void *pBuf) {
    kgspReadEmem((struct KernelGsp *)(((unsigned char *) pKernelGsp) - NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGsp = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelGsp object.
void __nvoc_kgspDestruct(KernelGsp*);
void __nvoc_dtor_OBJENGSTATE(Dynamic*);
void __nvoc_dtor_IntrService(Dynamic*);
void __nvoc_dtor_KernelFalcon(Dynamic*);
void __nvoc_dtor_KernelGsp(Dynamic* pThis) {

    KernelGsp *__nvoc_this = (KernelGsp *) pThis;

// Call destructor.
    __nvoc_kgspDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE((Dynamic *) &__nvoc_this->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService((Dynamic *) &__nvoc_this->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon((Dynamic *) &__nvoc_this->__nvoc_base_KernelFalcon);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelGsp(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal field -- bPartitionedFmc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->bPartitionedFmc = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPartitionedFmc = NV_FALSE;
    }

    // Hal field -- bScrubberUcodeSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bScrubberUcodeSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bScrubberUcodeSupported = NV_FALSE;
    }

    // Hal field -- bImemPagingSupported
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->bImemPagingSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bImemPagingSupported = NV_FALSE;
    }

    // Hal field -- fwHeapParamBaseSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->fwHeapParamBaseSize = (8 << 20);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->fwHeapParamBaseSize = (18 << 20);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->fwHeapParamBaseSize = (32 << 20);
    }
    // default
    else
    {
        pThis->fwHeapParamBaseSize = 0;
    }

    // Hal field -- fwHeapParamUcodeReclaim
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->fwHeapParamUcodeReclaim = (7 << 20);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->fwHeapParamUcodeReclaim = (55 << 20);
    }
    // default
    else
    {
        pThis->fwHeapParamUcodeReclaim = 0;
    }

    // Hal field -- bBootGspRmWithBoostClocks
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bBootGspRmWithBoostClocks = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBootGspRmWithBoostClocks = NV_FALSE;
    }

    // Hal field -- ememPort
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
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


// Construct KernelGsp object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_IntrService(IntrService *);
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_KernelFalcon;

    // Initialize data fields.
    __nvoc_init_dataField_KernelGsp(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelGsp_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelGsp_fail_KernelFalcon:
    __nvoc_dtor_IntrService((Dynamic *)&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelGsp_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE((Dynamic *)&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGsp_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsp_exit:
    return status;
}

// Vtable initialization 1/2
static void __nvoc_init_funcTable_KernelGsp_1(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kgspGetWprEndMargin -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetWprEndMargin__ = &kgspGetWprEndMargin_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            pThis->__kgspGetWprEndMargin__ = &kgspGetWprEndMargin_474d46;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetWprEndMargin__ = &kgspGetWprEndMargin_TU102;
        }
        else
        {
            pThis->__kgspGetWprEndMargin__ = &kgspGetWprEndMargin_GH100;
        }
    }

    // kgspConfigureFalcon -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_TU102;
        }
        else
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_GA102;
        }
    }

    // kgspIsDebugModeEnabled -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x05f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GA100;
        }
        else
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GB100;
        }
    }

    // kgspAllocBootArgs -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_TU102;
        }
        else
        {
            pThis->__kgspAllocBootArgs__ = &kgspAllocBootArgs_GH100;
        }
    }

    // kgspFreeBootArgs -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_TU102;
        }
        else
        {
            pThis->__kgspFreeBootArgs__ = &kgspFreeBootArgs_GH100;
        }
    }

    // kgspProgramLibosBootArgsAddr -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspProgramLibosBootArgsAddr__ = &kgspProgramLibosBootArgsAddr_TU102;
        }
    }

    // kgspPrepareForBootstrap -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_TU102;
        }
        else
        {
            pThis->__kgspPrepareForBootstrap__ = &kgspPrepareForBootstrap_GH100;
        }
    }

    // kgspBootstrap -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_TU102;
        }
        else
        {
            pThis->__kgspBootstrap__ = &kgspBootstrap_GH100;
        }
    }

    // kgspTeardown -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspTeardown__ = &kgspTeardown_TU102;
        }
        else
        {
            pThis->__kgspTeardown__ = &kgspTeardown_GH100;
        }
    }

    // kgspGetGspRmBootUcodeStorage -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GA102;
        }
        else
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GB100;
        }
    }

    // kgspGetBinArchiveGspRmBoot -- halified (12 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) )) /* ChipHal: GB10B | GB10C */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB10B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB20B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_AD102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000003eUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveConcatenatedFMCDesc -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDesc__ = &kgspGetBinArchiveConcatenatedFMCDesc_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveConcatenatedFMC -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMC__ = &kgspGetBinArchiveConcatenatedFMC_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveConcatenatedFMCDescSpdmLite -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        // default
        // NVOC_UNREACHABLE, NULL-initialized.
    }

    // kgspGetBinArchiveConcatenatedFMCSpdmLite -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        // default
        // NVOC_UNREACHABLE, NULL-initialized.
    }

    // kgspGetBinArchiveConcatenatedFMCDescTdisp -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCDescTdisp__ = &kgspGetBinArchiveConcatenatedFMCDescTdisp_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDescTdisp__ = &kgspGetBinArchiveConcatenatedFMCDescTdisp_GR100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCDescTdisp__ = &kgspGetBinArchiveConcatenatedFMCDescTdisp_80f438;
        }
    }

    // kgspGetBinArchiveConcatenatedFMCTdisp -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveConcatenatedFMCTdisp__ = &kgspGetBinArchiveConcatenatedFMCTdisp_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCTdisp__ = &kgspGetBinArchiveConcatenatedFMCTdisp_GR100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveConcatenatedFMCTdisp__ = &kgspGetBinArchiveConcatenatedFMCTdisp_80f438;
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwDebugSigned -- halified (8 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) )) /* ChipHal: GB10B | GB10C */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB10B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB20B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000003eUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwDebugSigned__ = &kgspGetBinArchiveGspRmFmcGfwDebugSigned_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveGspRmFmcGfwProdSigned -- halified (8 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) )) /* ChipHal: GB10B | GB10C */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB10B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB20B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000003eUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcGfwProdSigned_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveGspRmFmcTdispGfwProdSigned -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetBinArchiveGspRmFmcTdispGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcTdispGfwProdSigned_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmFmcTdispGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcTdispGfwProdSigned_GR100;
        }
        // default
        else
        {
            pThis->__kgspGetBinArchiveGspRmFmcTdispGfwProdSigned__ = &kgspGetBinArchiveGspRmFmcTdispGfwProdSigned_80f438;
        }
    }

    // kgspGetBinArchiveGspRmCcFmcGfwProdSigned -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
        {
            pThis->__kgspGetBinArchiveGspRmCcFmcGfwProdSigned__ = &kgspGetBinArchiveGspRmCcFmcGfwProdSigned_GB100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspPopulateWprMeta -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPopulateWprMeta__ = &kgspPopulateWprMeta_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB10B | GB10C | GB20B | GB20C */ 
        {
            pThis->__kgspPopulateWprMeta__ = &kgspPopulateWprMeta_GB10B;
        }
        else
        {
            pThis->__kgspPopulateWprMeta__ = &kgspPopulateWprMeta_GH100;
        }
    }

    // kgspGetNonWprHeapSizePortionForGsp -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetNonWprHeapSizePortionForGsp__ = &kgspGetNonWprHeapSizePortionForGsp_ac2203;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgspGetNonWprHeapSizePortionForGsp__ = &kgspGetNonWprHeapSizePortionForGsp_b5d989;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetNonWprHeapSizePortionForGsp__ = &kgspGetNonWprHeapSizePortionForGsp_ed6b8b;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
        {
            pThis->__kgspGetNonWprHeapSizePortionForGsp__ = &kgspGetNonWprHeapSizePortionForGsp_d505ea;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspReadUcodeFuseVersion -- halified (6 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_b2b553;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000603eUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x05f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GA100;
        }
        else
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GB100;
        }
    }

    // kgspResetHw -- virtual halified (5 hals) override (kflcn) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_TU102;
        }
        else
        {
            pThis->__kgspResetHw__ = &kgspResetHw_GB100;
        }
    }

    // kgspHealthCheck -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspHealthCheck__ = &kgspHealthCheck_TU102;
        }
    }

    // kgspDumpMailbox -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspDumpMailbox__ = &kgspDumpMailbox_TU102;
        }
    }

    // kgspEncodeMailboxRegisters -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspEncodeMailboxRegisters__ = &kgspEncodeMailboxRegisters_TU102;
        }
    }

    // kgspReadMailbox -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspReadMailbox__ = &kgspReadMailbox_TU102;
        }
    }

    // kgspService -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspService__ = &kgspService_TU102;
        }
    }

    // kgspCheckGspPoisonError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspCheckGspPoisonError__ = &kgspCheckGspPoisonError_GB100;
        }
        // default
        else
        {
            pThis->__kgspCheckGspPoisonError__ = &kgspCheckGspPoisonError_d69453;
        }
    }

    // kgspServiceFatalHwError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspServiceFatalHwError__ = &kgspServiceFatalHwError_GB100;
        }
        // default
        else
        {
            pThis->__kgspServiceFatalHwError__ = &kgspServiceFatalHwError_d44104;
        }
    }

    // kgspEccIsErrorPending -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspEccIsErrorPending__ = &kgspEccIsErrorPending_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspEccIsErrorPending__ = &kgspEccIsErrorPending_GB100;
        }
        // default
        else
        {
            pThis->__kgspEccIsErrorPending__ = &kgspEccIsErrorPending_d69453;
        }
    }

    // kgspCheckGpuContainmentError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspCheckGpuContainmentError__ = &kgspCheckGpuContainmentError_GB100;
        }
        // default
        else
        {
            pThis->__kgspCheckGpuContainmentError__ = &kgspCheckGpuContainmentError_ac1694;
        }
    }

    // kgspEccServiceEvent -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_GB100;
        }
        // default
        else
        {
            pThis->__kgspEccServiceEvent__ = &kgspEccServiceEvent_d44104;
        }
    }

    // kgspEccServiceUncorrError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kgspEccServiceUncorrError__ = &kgspEccServiceUncorrError_GB100;
        }
        // default
        else
        {
            pThis->__kgspEccServiceUncorrError__ = &kgspEccServiceUncorrError_d44104;
        }
    }

    // kgspEccServiceUncorrErrorIndex -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspEccServiceUncorrErrorIndex__ = &kgspEccServiceUncorrErrorIndex_GR100;
        }
        // default
        else
        {
            pThis->__kgspEccServiceUncorrErrorIndex__ = &kgspEccServiceUncorrErrorIndex_d44104;
        }
    }

    // kgspIsWpr2Up -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_TU102;
        }
        else
        {
            pThis->__kgspIsWpr2Up__ = &kgspIsWpr2Up_GB100;
        }
    }

    // kgspGetFrtsSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000400UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: GA100 | GB10B | GB10C | GB20B | GB20C | T234D | T238D | T264D */ 
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_b2b553;
        }
        else
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_TU102;
        }
    }

    // kgspPrepareScrubberImageIfNeeded -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPrepareScrubberImageIfNeeded__ = &kgspPrepareScrubberImageIfNeeded_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            pThis->__kgspPrepareScrubberImageIfNeeded__ = &kgspPrepareScrubberImageIfNeeded_5baef9;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPrepareScrubberImageIfNeeded__ = &kgspPrepareScrubberImageIfNeeded_TU102;
        }
        else
        {
            pThis->__kgspPrepareScrubberImageIfNeeded__ = &kgspPrepareScrubberImageIfNeeded_ac1694;
        }
    }

    // kgspCopyBar0Radix3Buf -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCopyBar0Radix3Buf__ = &kgspCopyBar0Radix3Buf_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspCopyBar0Radix3Buf__ = &kgspCopyBar0Radix3Buf_GH100;
        }
        else
        {
            pThis->__kgspCopyBar0Radix3Buf__ = &kgspCopyBar0Radix3Buf_5baef9;
        }
    }

    // kgspCopyFmodelBootloaderToFb -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspCopyFmodelBootloaderToFb__ = &kgspCopyFmodelBootloaderToFb_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspCopyFmodelBootloaderToFb__ = &kgspCopyFmodelBootloaderToFb_GH100;
        }
        else
        {
            pThis->__kgspCopyFmodelBootloaderToFb__ = &kgspCopyFmodelBootloaderToFb_5baef9;
        }
    }

    // kgspPopulateSrRegionsInfo -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPopulateSrRegionsInfo__ = &kgspPopulateSrRegionsInfo_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            pThis->__kgspPopulateSrRegionsInfo__ = &kgspPopulateSrRegionsInfo_d44104;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspPopulateSrRegionsInfo__ = &kgspPopulateSrRegionsInfo_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB10B | GB10C | GB20B | GB20C */ 
        {
            pThis->__kgspPopulateSrRegionsInfo__ = &kgspPopulateSrRegionsInfo_GB10B;
        }
        else
        {
            pThis->__kgspPopulateSrRegionsInfo__ = &kgspPopulateSrRegionsInfo_GH100;
        }
    }

    // kgspGetPrescrubbedTopFbSize -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_13afc5;
        }
        else
        {
            pThis->__kgspGetPrescrubbedTopFbSize__ = &kgspGetPrescrubbedTopFbSize_7250dc;
        }
    }

    // kgspIsScrubberImageSupported -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspIsScrubberImageSupported__ = &kgspIsScrubberImageSupported_d69453;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspIsScrubberImageSupported__ = &kgspIsScrubberImageSupported_e661f0;
        }
        else
        {
            pThis->__kgspIsScrubberImageSupported__ = &kgspIsScrubberImageSupported_d69453;
        }
    }

    // kgspExecuteScrubberIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteScrubberIfNeeded__ = &kgspExecuteScrubberIfNeeded_AD102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspExecuteBooterLoad -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteBooterLoad__ = &kgspExecuteBooterLoad_TU102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspExecuteBooterUnloadIfNeeded -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteBooterUnloadIfNeeded__ = &kgspExecuteBooterUnloadIfNeeded_TU102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspExecuteHsFalcon -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_GA102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspWaitForProcessorSuspend -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspWaitForProcessorSuspend__ = &kgspWaitForProcessorSuspend_TU102;
        }
    }

    // kgspPrepareSuspendResumeData -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspPrepareSuspendResumeData__ = &kgspPrepareSuspendResumeData_TU102;
        }
    }

    // kgspFreeSuspendResumeData -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspFreeSuspendResumeData__ = &kgspFreeSuspendResumeData_TU102;
        }
    }

    // kgspWaitForGfwBootOk -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000c0UL) )) /* ChipHal: GB10B | GB10C | GB20B | GB20C */ 
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_ac1694;
        }
        else
        {
            pThis->__kgspWaitForGfwBootOk__ = &kgspWaitForGfwBootOk_GH100;
        }
    }

    // kgspGetBinArchiveBooterLoadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_AD102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetBinArchiveBooterUnloadUcode -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_AD102;
        }
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetLogCount -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetLogCount__ = &kgspGetLogCount_13cd8d;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetLogCount__ = &kgspGetLogCount_895ae1;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetLogCount__ = &kgspGetLogCount_520858;
        }
        // default
        else
        {
            pThis->__kgspGetLogCount__ = &kgspGetLogCount_13cd8d;
        }
    }

    // kgspGetMinWprHeapSizeMB -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_b2b553;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_6961d8;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_IMPL;
        }
        // default
        else
        {
            pThis->__kgspGetMinWprHeapSizeMB__ = &kgspGetMinWprHeapSizeMB_b2b553;
        }
    }

    // kgspGetLibos3BaremetalMinWprHeapSizeMB -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->__kgspGetLibos3BaremetalMinWprHeapSizeMB__ = &kgspGetLibos3BaremetalMinWprHeapSizeMB_GR100;
    }
    // default
    else
    {
        pThis->__kgspGetLibos3BaremetalMinWprHeapSizeMB__ = &kgspGetLibos3BaremetalMinWprHeapSizeMB_cb6d22;
    }

    // kgspGetMaxWprHeapSizeMB -- halified (4 hals) body
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_IMPL;
        }
        // default
        else
        {
            pThis->__kgspGetMaxWprHeapSizeMB__ = &kgspGetMaxWprHeapSizeMB_b2b553;
        }
    }

    // kgspGetFwHeapParamOsCarveoutSize -- halified (4 hals) body
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_4b5307;
        }
        // default
        else
        {
            pThis->__kgspGetFwHeapParamOsCarveoutSize__ = &kgspGetFwHeapParamOsCarveoutSize_b2b553;
        }
    }

    // kgspInitVgpuPartitionLogging -- halified (4 hals) body
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_IMPL;
        }
        // default
        else
        {
            pThis->__kgspInitVgpuPartitionLogging__ = &kgspInitVgpuPartitionLogging_395e98;
        }
    }

    // kgspPreserveVgpuPartitionLogging -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_395e98;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_IMPL;
        }
        // default
        else
        {
            pThis->__kgspPreserveVgpuPartitionLogging__ = &kgspPreserveVgpuPartitionLogging_395e98;
        }
    }

    // kgspFreeVgpuPartitionLogging -- halified (4 hals) body
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
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_IMPL;
        }
        // default
        else
        {
            pThis->__kgspFreeVgpuPartitionLogging__ = &kgspFreeVgpuPartitionLogging_395e98;
        }
    }
} // End __nvoc_init_funcTable_KernelGsp_1 with approximately 262 basic block(s).

// Vtable initialization 2/2
static void __nvoc_init_funcTable_KernelGsp_2(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kgspGetLibosVersion -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetLibosVersion__ = &kgspGetLibosVersion_25c6f4;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetLibosVersion__ = &kgspGetLibosVersion_5bf0ec;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetLibosVersion__ = &kgspGetLibosVersion_61db9f;
        }
        // default
        else
        {
            pThis->__kgspGetLibosVersion__ = &kgspGetLibosVersion_25c6f4;
        }
    }

    // kgspVgpuFwHeapSize -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB202 */ 
    {
        pThis->__kgspVgpuFwHeapSize__ = &kgspVgpuFwHeapSize_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->__kgspVgpuFwHeapSize__ = &kgspVgpuFwHeapSize_ff4276;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000fdUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgspVgpuFwHeapSize__ = &kgspVgpuFwHeapSize_TU102;
    }
    // default
    else
    {
        pThis->__kgspVgpuFwHeapSize__ = &kgspVgpuFwHeapSize_70f978;
    }

    // kgspVgpuNumVgpuPartitions -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB202 */ 
    {
        pThis->__kgspVgpuNumVgpuPartitions__ = &kgspVgpuNumVgpuPartitions_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->__kgspVgpuNumVgpuPartitions__ = &kgspVgpuNumVgpuPartitions_9c861d;
    }
    // default
    else
    {
        pThis->__kgspVgpuNumVgpuPartitions__ = &kgspVgpuNumVgpuPartitions_c24270;
    }

    // kgspGetSignatureSectionNamePrefix -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetSignatureSectionNamePrefix__ = &kgspGetSignatureSectionNamePrefix_789efb;
        }
    }

    // kgspGetTdispSignatureSectionNamePrefix -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgspGetTdispSignatureSectionNamePrefix__ = &kgspGetTdispSignatureSectionNamePrefix_80f438;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
        {
            pThis->__kgspGetTdispSignatureSectionNamePrefix__ = &kgspGetTdispSignatureSectionNamePrefix_GR100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB10C | GB110 | GB112 */ 
        {
            pThis->__kgspGetTdispSignatureSectionNamePrefix__ = &kgspGetTdispSignatureSectionNamePrefix_GB100;
        }
        // default
        else
        {
            pThis->__kgspGetTdispSignatureSectionNamePrefix__ = &kgspGetTdispSignatureSectionNamePrefix_80f438;
        }
    }

    // kgspSetupGspFmcArgs -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspSetupGspFmcArgs__ = &kgspSetupGspFmcArgs_GH100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspReadEmem -- virtual halified (3 hals) override (kcrashcatEngine) base (kflcn) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspReadEmem__ = &kgspReadEmem_TU102;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspGetCrashcatSysmemBufferSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060feUL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
    {
        pThis->__kgspGetCrashcatSysmemBufferSize__ = &kgspGetCrashcatSysmemBufferSize_3eaa85;
    }
    // default
    else
    {
        pThis->__kgspGetCrashcatSysmemBufferSize__ = &kgspGetCrashcatSysmemBufferSize_b2b553;
    }

    // kgspIssueNotifyOp -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspIssueNotifyOp__ = &kgspIssueNotifyOp_GH100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspCheckGspRmCcCleanup -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfc000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspCheckGspRmCcCleanup__ = &kgspCheckGspRmCcCleanup_GH100;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspExecuteCoreResume -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0ffe0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            pThis->__kgspExecuteCoreResume__ = &kgspExecuteCoreResume_TU102;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspLoadAndExecuteGenericBootloader -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspLoadAndExecuteGenericBootloader__ = &kgspLoadAndExecuteGenericBootloader_TU102;
        }
        // default
        else
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
    }

    // kgspLoadAndExecuteHsBinary -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000e000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | T234D | T238D | T264D */ 
        {
            // NVOC_UNREACHABLE, NULL-initialized.
        }
        else
        {
            pThis->__kgspLoadAndExecuteHsBinary__ = &kgspLoadAndExecuteHsBinary_GA102;
        }
    }

    // kgspGetGspScratchAperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->__kgspGetGspScratchAperture__ = &kgspGetGspScratchAperture_GR100;
    }
    // default
    else
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }

    // kgspReleaseGspScratchAperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00006000UL) )) /* ChipHal: GR100 | GR102 */ 
    {
        pThis->__kgspReleaseGspScratchAperture__ = &kgspReleaseGspScratchAperture_GR100;
    }
    // default
    else
    {
        // NVOC_UNREACHABLE, NULL-initialized.
    }
} // End __nvoc_init_funcTable_KernelGsp_2 with approximately 46 basic block(s).


// Initialize vtable(s) for 78 virtual method(s).
void __nvoc_init_funcTable_KernelGsp(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 78 per-object function pointer(s).
    // To reduce stack pressure with some unoptimized builds, the logic is distributed among 2 functions.
    __nvoc_init_funcTable_KernelGsp_1(pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGsp_2(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NvBool kgspIsHeartbeatSupported_STATIC_DISPATCH(struct OBJGPU *pGpu) {
    ChipHal *chipHal = &staticCast(pGpu, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pGpu, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;

    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        return kgspIsHeartbeatSupported_72a2e1(pGpu);
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            return kgspIsHeartbeatSupported_d69453(pGpu);
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            return kgspIsHeartbeatSupported_e661f0(pGpu);
        }
        // default
        else
        {
            return kgspIsHeartbeatSupported_72a2e1(pGpu);
        }
    }

    NV_ASSERT_FAILED("No hal impl found for kgspIsHeartbeatSupported");

    return NV_FALSE;
}

NvBool kgspHasLibosKernelLogging_STATIC_DISPATCH(struct OBJGPU *pGpu) {
    ChipHal *chipHal = &staticCast(pGpu, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pGpu, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;

    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        return kgspHasLibosKernelLogging_72a2e1(pGpu);
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            return kgspHasLibosKernelLogging_d69453(pGpu);
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xfdf0f800UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000060ffUL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB10C | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | GR100 | GR102 */ 
        {
            return kgspHasLibosKernelLogging_e661f0(pGpu);
        }
        // default
        else
        {
            return kgspHasLibosKernelLogging_72a2e1(pGpu);
        }
    }

    NV_ASSERT_FAILED("No hal impl found for kgspHasLibosKernelLogging");

    return NV_FALSE;
}

// Initialize newly constructed object.
void __nvoc_init__KernelGsp(KernelGsp *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;    // (kflcn) super
    pThis->__nvoc_pbase_KernelGsp = pThis;    // (kgsp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init__KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pGpuhalspecowner, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__KernelFalcon.metadata__KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_base_KernelFalcon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp.metadata__KernelFalcon;    // (kflcn) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsp;    // (kgsp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGsp(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelGsp *__nvoc_pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelGsp));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelGsp));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelGsp_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelGsp_cleanup);
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelGsp_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelGsp(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelGsp(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelGsp_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelGsp_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelGsp));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_KernelGsp((KernelGsp **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}


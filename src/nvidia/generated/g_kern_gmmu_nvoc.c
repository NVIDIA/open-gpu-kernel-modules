#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_gmmu_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x29362f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGmmu;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for KernelGmmu
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__KernelGmmu(KernelGmmu*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGmmu(KernelGmmu*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGmmu(KernelGmmu*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelGmmu(KernelGmmu*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGmmu(KernelGmmu*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGmmu;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGmmu;

// Down-thunk(s) to bridge KernelGmmu methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3);    // this
void __nvoc_down_thunk_KernelGmmu_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu);    // this
void __nvoc_down_thunk_KernelGmmu_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceRecord arg3[179]);    // this
NvBool __nvoc_down_thunk_KernelGmmu_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceClearInterruptArguments *pParams);    // this
NvU32 __nvoc_down_thunk_KernelGmmu_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams);    // this
NV_STATUS __nvoc_down_thunk_KernelGmmu_intrservServiceNotificationInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

// Up-thunk(s) to bridge KernelGmmu methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kgmmuInitMissing(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreLoad(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePostUnload(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kgmmuIsPresent(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGmmu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGmmu),
        /*classId=*/            classId(KernelGmmu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGmmu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGmmu,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGmmu,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGmmu
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelGmmu __nvoc_metadata__KernelGmmu = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGmmu,    // (kgmmu) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGmmu,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService),

    .vtable.__kgmmuConstructEngine__ = &kgmmuConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGmmu_engstateConstructEngine,    // virtual
    .vtable.__kgmmuStateInitLocked__ = &kgmmuStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelGmmu_engstateStateInitLocked,    // virtual
    .vtable.__kgmmuStateLoad__ = &kgmmuStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelGmmu_engstateStateLoad,    // virtual
    .vtable.__kgmmuStateUnload__ = &kgmmuStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelGmmu_engstateStateUnload,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelGmmu_engstateStatePostLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelGmmu_engstateStatePreUnload,    // virtual
    .vtable.__kgmmuStateDestroy__ = &kgmmuStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelGmmu_engstateStateDestroy,    // virtual
    .vtable.__kgmmuRegisterIntrService__ = &kgmmuRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelGmmu_intrservRegisterIntrService,    // virtual
    .vtable.__kgmmuClearInterrupt__ = &kgmmuClearInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &__nvoc_down_thunk_KernelGmmu_intrservClearInterrupt,    // virtual
    .vtable.__kgmmuServiceInterrupt__ = &kgmmuServiceInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelGmmu_intrservServiceInterrupt,    // virtual
    .vtable.__kgmmuServiceNotificationInterrupt__ = &kgmmuServiceNotificationInterrupt_56cd7a,    // virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_KernelGmmu_intrservServiceNotificationInterrupt,    // virtual
    .vtable.__kgmmuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kgmmuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kgmmuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kgmmuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kgmmuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kgmmuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kgmmuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgmmuIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGmmu = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__KernelGmmu.rtti,    // [0]: (kgmmu) this
        &__nvoc_metadata__KernelGmmu.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelGmmu.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__KernelGmmu.metadata__IntrService.rtti,    // [3]: (intrserv) super
    }
};

// 11 down-thunk(s) defined to bridge methods in KernelGmmu from superclasses

// kgmmuConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, ENGDESCRIPTOR arg3) {
    return kgmmuConstructEngine(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu) {
    return kgmmuStateInitLocked(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3) {
    return kgmmuStateLoad(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3) {
    return kgmmuStateUnload(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3) {
    return kgmmuStatePostLoad(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStatePreUnload: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelGmmu_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg3) {
    return kgmmuStatePreUnload(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelGmmu_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu) {
    kgmmuStateDestroy(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelGmmu_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceRecord arg3[179]) {
    kgmmuRegisterIntrService(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService)), arg3);
}

// kgmmuClearInterrupt: virtual override (intrserv) base (intrserv)
NvBool __nvoc_down_thunk_KernelGmmu_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceClearInterruptArguments *pParams) {
    return kgmmuClearInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService)), pParams);
}

// kgmmuServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelGmmu_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams) {
    return kgmmuServiceInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService)), pParams);
}

// kgmmuServiceNotificationInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv) body
NV_STATUS __nvoc_down_thunk_KernelGmmu_intrservServiceNotificationInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return kgmmuServiceNotificationInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *) pKernelGmmu) - NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService)), pParams);
}


// 7 up-thunk(s) defined to bridge methods in KernelGmmu to superclasses

// kgmmuInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgmmuInitMissing(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}

// kgmmuStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePreLoad(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgmmuStatePostUnload(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgmmuIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kgmmuIsPresent(struct OBJGPU *pGpu, struct KernelGmmu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGmmu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_kgmmuDestruct(KernelGmmu*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelGmmu(KernelGmmu *pThis) {
    __nvoc_kgmmuDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGmmu(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE, NV_FALSE);

    // Hal field -- defaultBigPageSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->defaultBigPageSize = (64 * 1024);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->defaultBigPageSize = 0;
    }

    // Hal field -- bHugePageSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bHugePageSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bHugePageSupported = NV_FALSE;
    }

    // Hal field -- bPageSize512mbSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bPageSize512mbSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPageSize512mbSupported = NV_FALSE;
    }

    // Hal field -- bPageSize256gbSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->bPageSize256gbSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPageSize256gbSupported = NV_FALSE;
    }

    // Hal field -- bBug2720120WarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->bBug2720120WarEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBug2720120WarEnabled = NV_FALSE;
    }

    // Hal field -- bVaspaceInteropSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bVaspaceInteropSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bVaspaceInteropSupported = NV_FALSE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelGmmu(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGmmu_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGmmu_fail_IntrService;
    __nvoc_init_dataField_KernelGmmu(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelGmmu_exit; // Success

__nvoc_ctor_KernelGmmu_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGmmu_fail_OBJENGSTATE:
__nvoc_ctor_KernelGmmu_exit:

    return status;
}

// Vtable initialization 1/2
static void __nvoc_init_funcTable_KernelGmmu_1(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // kgmmuStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuStatePostLoad__ = &kgmmuStatePostLoad_56cd7a;
    }
    else
    {
        pThis->__kgmmuStatePostLoad__ = &kgmmuStatePostLoad_IMPL;
    }

    // kgmmuStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuStatePreUnload__ = &kgmmuStatePreUnload_56cd7a;
    }
    else
    {
        pThis->__kgmmuStatePreUnload__ = &kgmmuStatePreUnload_IMPL;
    }

    // kgmmuGetFaultBufferReservedFbSpaceSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuGetFaultBufferReservedFbSpaceSize__ = &kgmmuGetFaultBufferReservedFbSpaceSize_IMPL;
    }
    else
    {
        pThis->__kgmmuGetFaultBufferReservedFbSpaceSize__ = &kgmmuGetFaultBufferReservedFbSpaceSize_4a4dee;
    }

    // kgmmuGetMaxBigPageSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetMaxBigPageSize__ = &kgmmuGetMaxBigPageSize_474d46;
    }
    else
    {
        pThis->__kgmmuGetMaxBigPageSize__ = &kgmmuGetMaxBigPageSize_GM107;
    }

    // kgmmuGetVaspaceClass -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetVaspaceClass__ = &kgmmuGetVaspaceClass_474d46;
    }
    else
    {
        pThis->__kgmmuGetVaspaceClass__ = &kgmmuGetVaspaceClass_f515df;
    }

    // kgmmuInstBlkAtsGet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuInstBlkAtsGet__ = &kgmmuInstBlkAtsGet_f03539;
    }
    else
    {
        pThis->__kgmmuInstBlkAtsGet__ = &kgmmuInstBlkAtsGet_GV100;
    }

    // kgmmuInstBlkVaLimitGet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_GV100;
    }
    else
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_f03539;
    }

    // kgmmuInstBlkMagicValueGet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuInstBlkMagicValueGet__ = &kgmmuInstBlkMagicValueGet_GA10B;
    }
    // default
    else
    {
        pThis->__kgmmuInstBlkMagicValueGet__ = &kgmmuInstBlkMagicValueGet_46f6a7;
    }

    // kgmmuInstBlkPageDirBaseGet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuInstBlkPageDirBaseGet__ = &kgmmuInstBlkPageDirBaseGet_46f6a7;
    }
    else
    {
        pThis->__kgmmuInstBlkPageDirBaseGet__ = &kgmmuInstBlkPageDirBaseGet_GV100;
    }

    // kgmmuGetPDBAllocSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetPDBAllocSize__ = &kgmmuGetPDBAllocSize_474d46;
    }
    else
    {
        pThis->__kgmmuGetPDBAllocSize__ = &kgmmuGetPDBAllocSize_GP100;
    }

    // kgmmuGetBigPageSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetBigPageSize__ = &kgmmuGetBigPageSize_474d46;
    }
    else
    {
        pThis->__kgmmuGetBigPageSize__ = &kgmmuGetBigPageSize_GM107;
    }

    // kgmmuFmtInitPteApertures -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitPteApertures__ = &kgmmuFmtInitPteApertures_b3696a;
    }
    else
    {
        pThis->__kgmmuFmtInitPteApertures__ = &kgmmuFmtInitPteApertures_GM10X;
    }

    // kgmmuFmtInitPdeApertures -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitPdeApertures__ = &kgmmuFmtInitPdeApertures_b3696a;
    }
    else
    {
        pThis->__kgmmuFmtInitPdeApertures__ = &kgmmuFmtInitPdeApertures_GM10X;
    }

    // kgmmuInvalidateTlb -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuInvalidateTlb__ = &kgmmuInvalidateTlb_f2d351;
    }
    else
    {
        pThis->__kgmmuInvalidateTlb__ = &kgmmuInvalidateTlb_GM107;
    }

    // kgmmuCommitInvalidateTlbTest -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuCommitInvalidateTlbTest__ = &kgmmuCommitInvalidateTlbTest_5baef9;
    }
    else
    {
        pThis->__kgmmuCommitInvalidateTlbTest__ = &kgmmuCommitInvalidateTlbTest_GM107;
    }

    // kgmmuCheckPendingInvalidates -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuCheckPendingInvalidates__ = &kgmmuCheckPendingInvalidates_5baef9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgmmuCheckPendingInvalidates__ = &kgmmuCheckPendingInvalidates_TU102;
    }
    else
    {
        pThis->__kgmmuCheckPendingInvalidates__ = &kgmmuCheckPendingInvalidates_TU102;
    }

    // kgmmuCommitTlbInvalidate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuCommitTlbInvalidate__ = &kgmmuCommitTlbInvalidate_5baef9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgmmuCommitTlbInvalidate__ = &kgmmuCommitTlbInvalidate_TU102;
    }
    else
    {
        pThis->__kgmmuCommitTlbInvalidate__ = &kgmmuCommitTlbInvalidate_GB100;
    }

    // kgmmuSetPdbToInvalidate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuSetPdbToInvalidate__ = &kgmmuSetPdbToInvalidate_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgmmuSetPdbToInvalidate__ = &kgmmuSetPdbToInvalidate_TU102;
    }
    else
    {
        pThis->__kgmmuSetPdbToInvalidate__ = &kgmmuSetPdbToInvalidate_TU102;
    }

    // kgmmuSetTlbInvalidateMembarWarParameters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_TU102;
    }
    else
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_4a4dee;
    }

    // kgmmuSetTlbInvalidationScope -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_46f6a7;
    }
    else
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_GA100;
    }

    // kgmmuFmtInitPteComptagLine -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitPteComptagLine__ = &kgmmuFmtInitPteComptagLine_TU10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPteComptagLine__ = &kgmmuFmtInitPteComptagLine_b3696a;
    }

    // kgmmuFmtInitPeerPteFld -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitPeerPteFld__ = &kgmmuFmtInitPeerPteFld_TU10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPeerPteFld__ = &kgmmuFmtInitPeerPteFld_b3696a;
    }

    // kgmmuEnableComputePeerAddressing -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableComputePeerAddressing__ = &kgmmuEnableComputePeerAddressing_56cd7a;
    }
    else
    {
        pThis->__kgmmuEnableComputePeerAddressing__ = &kgmmuEnableComputePeerAddressing_IMPL;
    }

    // kgmmuFmtInitPte -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitPte__ = &kgmmuFmtInitPte_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitPte__ = &kgmmuFmtInitPte_GP10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPte__ = &kgmmuFmtInitPte_GH10X;
    }

    // kgmmuFmtInitPde -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitPde__ = &kgmmuFmtInitPde_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitPde__ = &kgmmuFmtInitPde_GP10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPde__ = &kgmmuFmtInitPde_GH10X;
    }

    // kgmmuFmtIsVersionSupported -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtIsVersionSupported__ = &kgmmuFmtIsVersionSupported_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtIsVersionSupported__ = &kgmmuFmtIsVersionSupported_GP10X;
    }
    else
    {
        pThis->__kgmmuFmtIsVersionSupported__ = &kgmmuFmtIsVersionSupported_GH10X;
    }

    // kgmmuFmtInitLevels -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GH10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GP10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GA10X;
    }
    else
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GB10X;
    }

    // kgmmuFmtInitPdeMulti -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtInitPdeMulti__ = &kgmmuFmtInitPdeMulti_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtInitPdeMulti__ = &kgmmuFmtInitPdeMulti_GP10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPdeMulti__ = &kgmmuFmtInitPdeMulti_GH10X;
    }

    // kgmmuDetermineMaxVASize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuDetermineMaxVASize__ = &kgmmuDetermineMaxVASize_b3696a;
    }
    else
    {
        pThis->__kgmmuDetermineMaxVASize__ = &kgmmuDetermineMaxVASize_GM107;
    }

    // kgmmuFmtFamiliesInit -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuFmtFamiliesInit__ = &kgmmuFmtFamiliesInit_GM200;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuFmtFamiliesInit__ = &kgmmuFmtFamiliesInit_GV100;
    }
    else
    {
        pThis->__kgmmuFmtFamiliesInit__ = &kgmmuFmtFamiliesInit_GH100;
    }

    // kgmmuTranslatePtePcfFromSw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuTranslatePtePcfFromSw__ = &kgmmuTranslatePtePcfFromSw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePtePcfFromSw__ = &kgmmuTranslatePtePcfFromSw_56cd7a;
    }

    // kgmmuTranslatePtePcfFromHw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuTranslatePtePcfFromHw__ = &kgmmuTranslatePtePcfFromHw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePtePcfFromHw__ = &kgmmuTranslatePtePcfFromHw_56cd7a;
    }

    // kgmmuTranslatePdePcfFromSw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuTranslatePdePcfFromSw__ = &kgmmuTranslatePdePcfFromSw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePdePcfFromSw__ = &kgmmuTranslatePdePcfFromSw_56cd7a;
    }

    // kgmmuTranslatePdePcfFromHw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuTranslatePdePcfFromHw__ = &kgmmuTranslatePdePcfFromHw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePdePcfFromHw__ = &kgmmuTranslatePdePcfFromHw_56cd7a;
    }

    // kgmmuGetFaultRegisterMappings -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetFaultRegisterMappings__ = &kgmmuGetFaultRegisterMappings_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuGetFaultRegisterMappings__ = &kgmmuGetFaultRegisterMappings_TU102;
    }
    else
    {
        pThis->__kgmmuGetFaultRegisterMappings__ = &kgmmuGetFaultRegisterMappings_GH100;
    }

    // kgmmuGetFaultTypeString -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetFaultTypeString__ = &kgmmuGetFaultTypeString_95626c;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgmmuGetFaultTypeString__ = &kgmmuGetFaultTypeString_GP100;
    }
    else
    {
        pThis->__kgmmuGetFaultTypeString__ = &kgmmuGetFaultTypeString_GB100;
    }

    // kgmmuChangeReplayableFaultOwnership -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuChangeReplayableFaultOwnership__ = &kgmmuChangeReplayableFaultOwnership_56cd7a;
    }
    else
    {
        pThis->__kgmmuChangeReplayableFaultOwnership__ = &kgmmuChangeReplayableFaultOwnership_GV100;
    }

    // kgmmuServiceReplayableFault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuServiceReplayableFault__ = &kgmmuServiceReplayableFault_46f6a7;
    }
    else
    {
        pThis->__kgmmuServiceReplayableFault__ = &kgmmuServiceReplayableFault_TU102;
    }

    // kgmmuIssueReplayableFaultBufferFlush -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuIssueReplayableFaultBufferFlush__ = &kgmmuIssueReplayableFaultBufferFlush_GH100;
    }
    else
    {
        pThis->__kgmmuIssueReplayableFaultBufferFlush__ = &kgmmuIssueReplayableFaultBufferFlush_46f6a7;
    }

    // kgmmuToggleFaultOnPrefetch -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuToggleFaultOnPrefetch__ = &kgmmuToggleFaultOnPrefetch_GH100;
    }
    else
    {
        pThis->__kgmmuToggleFaultOnPrefetch__ = &kgmmuToggleFaultOnPrefetch_46f6a7;
    }

    // kgmmuReportFaultBufferOverflow -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuReportFaultBufferOverflow__ = &kgmmuReportFaultBufferOverflow_46f6a7;
    }
    else
    {
        pThis->__kgmmuReportFaultBufferOverflow__ = &kgmmuReportFaultBufferOverflow_GV100;
    }

    // kgmmuReadFaultBufferGetPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuReadFaultBufferGetPtr__ = &kgmmuReadFaultBufferGetPtr_46f6a7;
    }
    else
    {
        pThis->__kgmmuReadFaultBufferGetPtr__ = &kgmmuReadFaultBufferGetPtr_TU102;
    }

    // kgmmuWriteFaultBufferGetPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuWriteFaultBufferGetPtr__ = &kgmmuWriteFaultBufferGetPtr_46f6a7;
    }
    else
    {
        pThis->__kgmmuWriteFaultBufferGetPtr__ = &kgmmuWriteFaultBufferGetPtr_TU102;
    }

    // kgmmuReadFaultBufferPutPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuReadFaultBufferPutPtr__ = &kgmmuReadFaultBufferPutPtr_46f6a7;
    }
    else
    {
        pThis->__kgmmuReadFaultBufferPutPtr__ = &kgmmuReadFaultBufferPutPtr_TU102;
    }

    // kgmmuReadMmuFaultBufferSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuReadMmuFaultBufferSize__ = &kgmmuReadMmuFaultBufferSize_a547a8;
    }
    else
    {
        pThis->__kgmmuReadMmuFaultBufferSize__ = &kgmmuReadMmuFaultBufferSize_TU102;
    }

    // kgmmuReadMmuFaultStatus -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuReadMmuFaultStatus__ = &kgmmuReadMmuFaultStatus_a547a8;
    }
    else
    {
        pThis->__kgmmuReadMmuFaultStatus__ = &kgmmuReadMmuFaultStatus_TU102;
    }

    // kgmmuWriteMmuFaultStatus -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuWriteMmuFaultStatus__ = &kgmmuWriteMmuFaultStatus_f2d351;
    }
    else
    {
        pThis->__kgmmuWriteMmuFaultStatus__ = &kgmmuWriteMmuFaultStatus_TU102;
    }

    // kgmmuIsNonReplayableFaultPending -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuIsNonReplayableFaultPending__ = &kgmmuIsNonReplayableFaultPending_3dd2c9;
    }
    else
    {
        pThis->__kgmmuIsNonReplayableFaultPending__ = &kgmmuIsNonReplayableFaultPending_TU102;
    }

    // kgmmuClientShadowFaultBufferAlloc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuClientShadowFaultBufferAlloc__ = &kgmmuClientShadowFaultBufferAlloc_56cd7a;
    }
    else
    {
        pThis->__kgmmuClientShadowFaultBufferAlloc__ = &kgmmuClientShadowFaultBufferAlloc_GV100;
    }

    // kgmmuClientShadowFaultBufferFree -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuClientShadowFaultBufferFree__ = &kgmmuClientShadowFaultBufferFree_56cd7a;
    }
    else
    {
        pThis->__kgmmuClientShadowFaultBufferFree__ = &kgmmuClientShadowFaultBufferFree_GV100;
    }

    // kgmmuFaultBufferAllocSharedMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuFaultBufferAllocSharedMemory__ = &kgmmuFaultBufferAllocSharedMemory_GH100;
    }
    else
    {
        pThis->__kgmmuFaultBufferAllocSharedMemory__ = &kgmmuFaultBufferAllocSharedMemory_56cd7a;
    }

    // kgmmuFaultBufferFreeSharedMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuFaultBufferFreeSharedMemory__ = &kgmmuFaultBufferFreeSharedMemory_GH100;
    }
    else
    {
        pThis->__kgmmuFaultBufferFreeSharedMemory__ = &kgmmuFaultBufferFreeSharedMemory_b3696a;
    }

    // kgmmuSetupWarForBug2720120 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_GA100;
    }
    else
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_56cd7a;
    }

    // kgmmuTestAccessCounterWriteNak -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | T234D | T264D */ 
    {
        pThis->__kgmmuTestAccessCounterWriteNak__ = &kgmmuTestAccessCounterWriteNak_3dd2c9;
    }
    else
    {
        pThis->__kgmmuTestAccessCounterWriteNak__ = &kgmmuTestAccessCounterWriteNak_TU102;
    }

    // kgmmuCheckAccessCounterBar2FaultServicingState -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuCheckAccessCounterBar2FaultServicingState__ = &kgmmuCheckAccessCounterBar2FaultServicingState_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__kgmmuCheckAccessCounterBar2FaultServicingState__ = &kgmmuCheckAccessCounterBar2FaultServicingState_GB100;
    }
    else
    {
        pThis->__kgmmuCheckAccessCounterBar2FaultServicingState__ = &kgmmuCheckAccessCounterBar2FaultServicingState_GV100;
    }

    // kgmmuGetGraphicsEngineId -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuGetGraphicsEngineId__ = &kgmmuGetGraphicsEngineId_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuGetGraphicsEngineId__ = &kgmmuGetGraphicsEngineId_GH100;
    }
    // default
    else
    {
        pThis->__kgmmuGetGraphicsEngineId__ = &kgmmuGetGraphicsEngineId_4a4dee;
    }

    // kgmmuEnableNvlinkComputePeerAddressing -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuEnableNvlinkComputePeerAddressing__ = &kgmmuEnableNvlinkComputePeerAddressing_5baef9;
    }
    else
    {
        pThis->__kgmmuEnableNvlinkComputePeerAddressing__ = &kgmmuEnableNvlinkComputePeerAddressing_GV100;
    }

    // kgmmuClearNonReplayableFaultIntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuClearNonReplayableFaultIntr__ = &kgmmuClearNonReplayableFaultIntr_b3696a;
    }
    else
    {
        pThis->__kgmmuClearNonReplayableFaultIntr__ = &kgmmuClearNonReplayableFaultIntr_TU102;
    }

    // kgmmuClearReplayableFaultIntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuClearReplayableFaultIntr__ = &kgmmuClearReplayableFaultIntr_b3696a;
    }
    else
    {
        pThis->__kgmmuClearReplayableFaultIntr__ = &kgmmuClearReplayableFaultIntr_TU102;
    }

    // kgmmuReadShadowBufPutIndex -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuReadShadowBufPutIndex__ = &kgmmuReadShadowBufPutIndex_GH100;
    }
    // default
    else
    {
        pThis->__kgmmuReadShadowBufPutIndex__ = &kgmmuReadShadowBufPutIndex_474d46;
    }

    // kgmmuPrintFaultInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuPrintFaultInfo__ = &kgmmuPrintFaultInfo_b3696a;
    }
    else
    {
        pThis->__kgmmuPrintFaultInfo__ = &kgmmuPrintFaultInfo_TU102;
    }

    // kgmmuIsFaultEngineBar1 -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuIsFaultEngineBar1__ = &kgmmuIsFaultEngineBar1_86b752;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuIsFaultEngineBar1__ = &kgmmuIsFaultEngineBar1_TU102;
    }
    else
    {
        pThis->__kgmmuIsFaultEngineBar1__ = &kgmmuIsFaultEngineBar1_GH100;
    }

    // kgmmuIsFaultEngineBar2 -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuIsFaultEngineBar2__ = &kgmmuIsFaultEngineBar2_86b752;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuIsFaultEngineBar2__ = &kgmmuIsFaultEngineBar2_TU102;
    }
    else
    {
        pThis->__kgmmuIsFaultEngineBar2__ = &kgmmuIsFaultEngineBar2_GH100;
    }

    // kgmmuIsFaultEnginePhysical -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuIsFaultEnginePhysical__ = &kgmmuIsFaultEnginePhysical_86b752;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuIsFaultEnginePhysical__ = &kgmmuIsFaultEnginePhysical_GV100;
    }
    else
    {
        pThis->__kgmmuIsFaultEnginePhysical__ = &kgmmuIsFaultEnginePhysical_GH100;
    }

    // kgmmuCopyMmuFaults -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuCopyMmuFaults__ = &kgmmuCopyMmuFaults_56cd7a;
        }
        else
        {
            pThis->__kgmmuCopyMmuFaults__ = &kgmmuCopyMmuFaults_GV100;
        }
    }
    else
    {
        pThis->__kgmmuCopyMmuFaults__ = &kgmmuCopyMmuFaults_92bfc3;
    }

    // kgmmuParseFaultPacket -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuParseFaultPacket__ = &kgmmuParseFaultPacket_56cd7a;
        }
        else
        {
            pThis->__kgmmuParseFaultPacket__ = &kgmmuParseFaultPacket_GV100;
        }
    }
    else
    {
        pThis->__kgmmuParseFaultPacket__ = &kgmmuParseFaultPacket_92bfc3;
    }

    // kgmmuFaultBufferClearPackets -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferClearPackets__ = &kgmmuFaultBufferClearPackets_b3696a;
        }
        else
        {
            pThis->__kgmmuFaultBufferClearPackets__ = &kgmmuFaultBufferClearPackets_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferClearPackets__ = &kgmmuFaultBufferClearPackets_f2d351;
    }

    // kgmmuFaultBufferGetFault -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferGetFault__ = &kgmmuFaultBufferGetFault_fa6e19;
        }
        else
        {
            pThis->__kgmmuFaultBufferGetFault__ = &kgmmuFaultBufferGetFault_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferGetFault__ = &kgmmuFaultBufferGetFault_dc3e6c;
    }

    // kgmmuCopyFaultPacketToClientShadowBuffer -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_4a4dee;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_GV100;
        }
        else
        {
            pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_GH100;
        }
    }
    else
    {
        pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_13cd8d;
    }

    // kgmmuIsReplayableShadowFaultBufferFull -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_GH100;
        }
        // default
        else
        {
            pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_3dd2c9;
        }
    }
    else
    {
        pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_72a2e1;
    }

    // kgmmuReadClientShadowBufPutIndex -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_GB100;
        }
        // default
        else
        {
            pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_4a4dee;
        }
    }
    else
    {
        pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_13cd8d;
    }

    // kgmmuWriteClientShadowBufPutIndex -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_GB100;
        }
        // default
        else
        {
            pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_b3696a;
        }
    }
    else
    {
        pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_f2d351;
    }

    // kgmmuInitCeMmuFaultIdRange -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuInitCeMmuFaultIdRange__ = &kgmmuInitCeMmuFaultIdRange_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kgmmuInitCeMmuFaultIdRange__ = &kgmmuInitCeMmuFaultIdRange_GA100;
    }
    // default
    else
    {
        pThis->__kgmmuInitCeMmuFaultIdRange__ = &kgmmuInitCeMmuFaultIdRange_56cd7a;
    }

    // kgmmuFaultBufferMap -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferMap__ = &kgmmuFaultBufferMap_IMPL;
    }
    else
    {
        pThis->__kgmmuFaultBufferMap__ = &kgmmuFaultBufferMap_92bfc3;
    }

    // kgmmuFaultBufferUnmap -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferUnmap__ = &kgmmuFaultBufferUnmap_IMPL;
    }
    else
    {
        pThis->__kgmmuFaultBufferUnmap__ = &kgmmuFaultBufferUnmap_92bfc3;
    }

    // kgmmuFaultBufferInit -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferInit__ = &kgmmuFaultBufferInit_56cd7a;
        }
        else
        {
            pThis->__kgmmuFaultBufferInit__ = &kgmmuFaultBufferInit_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferInit__ = &kgmmuFaultBufferInit_56cd7a;
    }

    // kgmmuFaultBufferDestroy -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferDestroy__ = &kgmmuFaultBufferDestroy_56cd7a;
        }
        else
        {
            pThis->__kgmmuFaultBufferDestroy__ = &kgmmuFaultBufferDestroy_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferDestroy__ = &kgmmuFaultBufferDestroy_56cd7a;
    }

    // kgmmuFaultBufferLoad -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferLoad__ = &kgmmuFaultBufferLoad_ac1694;
        }
        else
        {
            pThis->__kgmmuFaultBufferLoad__ = &kgmmuFaultBufferLoad_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferLoad__ = &kgmmuFaultBufferLoad_ac1694;
    }

    // kgmmuFaultBufferUnload -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultBufferUnload__ = &kgmmuFaultBufferUnload_ac1694;
        }
        else
        {
            pThis->__kgmmuFaultBufferUnload__ = &kgmmuFaultBufferUnload_GV100;
        }
    }
    else
    {
        pThis->__kgmmuFaultBufferUnload__ = &kgmmuFaultBufferUnload_ac1694;
    }

    // kgmmuEnableFaultBuffer -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuEnableFaultBuffer__ = &kgmmuEnableFaultBuffer_395e98;
        }
        else
        {
            pThis->__kgmmuEnableFaultBuffer__ = &kgmmuEnableFaultBuffer_GV100;
        }
    }
    else
    {
        pThis->__kgmmuEnableFaultBuffer__ = &kgmmuEnableFaultBuffer_395e98;
    }

    // kgmmuDisableFaultBuffer -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuDisableFaultBuffer__ = &kgmmuDisableFaultBuffer_46f6a7;
        }
        else
        {
            pThis->__kgmmuDisableFaultBuffer__ = &kgmmuDisableFaultBuffer_GV100;
        }
    }
    else
    {
        pThis->__kgmmuDisableFaultBuffer__ = &kgmmuDisableFaultBuffer_92bfc3;
    }

    // kgmmuSetAndGetDefaultFaultBufferSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuSetAndGetDefaultFaultBufferSize__ = &kgmmuSetAndGetDefaultFaultBufferSize_4a4dee;
        }
        else
        {
            pThis->__kgmmuSetAndGetDefaultFaultBufferSize__ = &kgmmuSetAndGetDefaultFaultBufferSize_TU102;
        }
    }
    else
    {
        pThis->__kgmmuSetAndGetDefaultFaultBufferSize__ = &kgmmuSetAndGetDefaultFaultBufferSize_13cd8d;
    }

    // kgmmuReadMmuFaultInstHiLo -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuReadMmuFaultInstHiLo__ = &kgmmuReadMmuFaultInstHiLo_f2d351;
        }
        else
        {
            pThis->__kgmmuReadMmuFaultInstHiLo__ = &kgmmuReadMmuFaultInstHiLo_TU102;
        }
    }
    else
    {
        pThis->__kgmmuReadMmuFaultInstHiLo__ = &kgmmuReadMmuFaultInstHiLo_f2d351;
    }

    // kgmmuReadMmuFaultAddrHiLo -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuReadMmuFaultAddrHiLo__ = &kgmmuReadMmuFaultAddrHiLo_f2d351;
        }
        else
        {
            pThis->__kgmmuReadMmuFaultAddrHiLo__ = &kgmmuReadMmuFaultAddrHiLo_TU102;
        }
    }
    else
    {
        pThis->__kgmmuReadMmuFaultAddrHiLo__ = &kgmmuReadMmuFaultAddrHiLo_f2d351;
    }

    // kgmmuReadMmuFaultInfo -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuReadMmuFaultInfo__ = &kgmmuReadMmuFaultInfo_a547a8;
        }
        else
        {
            pThis->__kgmmuReadMmuFaultInfo__ = &kgmmuReadMmuFaultInfo_TU102;
        }
    }
    else
    {
        pThis->__kgmmuReadMmuFaultInfo__ = &kgmmuReadMmuFaultInfo_a547a8;
    }

    // kgmmuWriteMmuFaultBufferSize -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuWriteMmuFaultBufferSize__ = &kgmmuWriteMmuFaultBufferSize_f2d351;
        }
        else
        {
            pThis->__kgmmuWriteMmuFaultBufferSize__ = &kgmmuWriteMmuFaultBufferSize_TU102;
        }
    }
    else
    {
        pThis->__kgmmuWriteMmuFaultBufferSize__ = &kgmmuWriteMmuFaultBufferSize_f2d351;
    }

    // kgmmuWriteMmuFaultBufferHiLo -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuWriteMmuFaultBufferHiLo__ = &kgmmuWriteMmuFaultBufferHiLo_f2d351;
        }
        else
        {
            pThis->__kgmmuWriteMmuFaultBufferHiLo__ = &kgmmuWriteMmuFaultBufferHiLo_TU102;
        }
    }
    else
    {
        pThis->__kgmmuWriteMmuFaultBufferHiLo__ = &kgmmuWriteMmuFaultBufferHiLo_f2d351;
    }

    // kgmmuEnableMmuFaultInterrupts -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableMmuFaultInterrupts__ = &kgmmuEnableMmuFaultInterrupts_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableMmuFaultInterrupts__ = &kgmmuEnableMmuFaultInterrupts_92bfc3;
    }

    // kgmmuDisableMmuFaultInterrupts -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuDisableMmuFaultInterrupts__ = &kgmmuDisableMmuFaultInterrupts_46f6a7;
    }
    else
    {
        pThis->__kgmmuDisableMmuFaultInterrupts__ = &kgmmuDisableMmuFaultInterrupts_92bfc3;
    }

    // kgmmuEnableMmuFaultOverflowIntr -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableMmuFaultOverflowIntr__ = &kgmmuEnableMmuFaultOverflowIntr_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableMmuFaultOverflowIntr__ = &kgmmuEnableMmuFaultOverflowIntr_92bfc3;
    }

    // kgmmuSignExtendFaultAddress -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_f2d351;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_GV100;
        }
        else
        {
            pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_GH100;
        }
    }
    else
    {
        pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_f2d351;
    }

    // kgmmuGetFaultType -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_GV100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_GB100;
        }
        // default
        else
        {
            pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_46f6a7;
        }
    }
    else
    {
        pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_92bfc3;
    }

    // kgmmuIsP2PUnboundInstFault -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
        {
            pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_3dd2c9;
        }
        else
        {
            pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_GA100;
        }
    }
    else
    {
        pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_92bfc3;
    }

    // kgmmuServiceVfPriFaults -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceVfPriFaults__ = &kgmmuServiceVfPriFaults_IMPL;
    }
    else
    {
        pThis->__kgmmuServiceVfPriFaults__ = &kgmmuServiceVfPriFaults_92bfc3;
    }

    // kgmmuTestVidmemAccessBitBufferError -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuTestVidmemAccessBitBufferError__ = &kgmmuTestVidmemAccessBitBufferError_3dd2c9;
    }
    else
    {
        pThis->__kgmmuTestVidmemAccessBitBufferError__ = &kgmmuTestVidmemAccessBitBufferError_72a2e1;
    }

    // kgmmuDisableVidmemAccessBitBuf -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuDisableVidmemAccessBitBuf__ = &kgmmuDisableVidmemAccessBitBuf_b3696a;
    }
    else
    {
        pThis->__kgmmuDisableVidmemAccessBitBuf__ = &kgmmuDisableVidmemAccessBitBuf_e426af;
    }

    // kgmmuEnableVidmemAccessBitBuf -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableVidmemAccessBitBuf__ = &kgmmuEnableVidmemAccessBitBuf_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableVidmemAccessBitBuf__ = &kgmmuEnableVidmemAccessBitBuf_92bfc3;
    }

    // kgmmuClearAccessCounterWriteNak -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuClearAccessCounterWriteNak__ = &kgmmuClearAccessCounterWriteNak_b3696a;
    }
    else
    {
        pThis->__kgmmuClearAccessCounterWriteNak__ = &kgmmuClearAccessCounterWriteNak_e426af;
    }

    // kgmmuServiceMthdBuffFaultInBar2Fault -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceMthdBuffFaultInBar2Fault__ = &kgmmuServiceMthdBuffFaultInBar2Fault_56cd7a;
    }
    else
    {
        pThis->__kgmmuServiceMthdBuffFaultInBar2Fault__ = &kgmmuServiceMthdBuffFaultInBar2Fault_92bfc3;
    }

    // kgmmuFaultCancelTargeted -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultCancelTargeted__ = &kgmmuFaultCancelTargeted_VF;
    }
    else
    {
        pThis->__kgmmuFaultCancelTargeted__ = &kgmmuFaultCancelTargeted_92bfc3;
    }

    // kgmmuFaultCancelIssueInvalidate -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kgmmuFaultCancelIssueInvalidate__ = &kgmmuFaultCancelIssueInvalidate_46f6a7;
        }
        else
        {
            pThis->__kgmmuFaultCancelIssueInvalidate__ = &kgmmuFaultCancelIssueInvalidate_GP100;
        }
    }
    else
    {
        pThis->__kgmmuFaultCancelIssueInvalidate__ = &kgmmuFaultCancelIssueInvalidate_92bfc3;
    }

    // kgmmuServiceNonReplayableFault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuServiceNonReplayableFault__ = &kgmmuServiceNonReplayableFault_46f6a7;
    }
    else
    {
        pThis->__kgmmuServiceNonReplayableFault__ = &kgmmuServiceNonReplayableFault_GV100;
    }

    // kgmmuHandleNonReplayableFaultPacket -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuHandleNonReplayableFaultPacket__ = &kgmmuHandleNonReplayableFaultPacket_46f6a7;
    }
    else
    {
        pThis->__kgmmuHandleNonReplayableFaultPacket__ = &kgmmuHandleNonReplayableFaultPacket_GV100;
    }

    // kgmmuNotifyNonReplayableFault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuNotifyNonReplayableFault__ = &kgmmuNotifyNonReplayableFault_46f6a7;
    }
    else
    {
        pThis->__kgmmuNotifyNonReplayableFault__ = &kgmmuNotifyNonReplayableFault_GV100;
    }

    // kgmmuServiceMmuFault -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuServiceMmuFault__ = &kgmmuServiceMmuFault_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuServiceMmuFault__ = &kgmmuServiceMmuFault_GV100;
    }
    else
    {
        pThis->__kgmmuServiceMmuFault__ = &kgmmuServiceMmuFault_GA100;
    }

    // kgmmuGetFaultInfoFromFaultPckt -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuGetFaultInfoFromFaultPckt__ = &kgmmuGetFaultInfoFromFaultPckt_4a4dee;
    }
    else
    {
        pThis->__kgmmuGetFaultInfoFromFaultPckt__ = &kgmmuGetFaultInfoFromFaultPckt_GV100;
    }

    // kgmmuServicePriFaults -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuServicePriFaults__ = &kgmmuServicePriFaults_46f6a7;
    }
    else
    {
        pThis->__kgmmuServicePriFaults__ = &kgmmuServicePriFaults_GV100;
    }
} // End __nvoc_init_funcTable_KernelGmmu_1 with approximately 263 basic block(s).

// Vtable initialization 2/2
static void __nvoc_init_funcTable_KernelGmmu_2(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // kgmmuServiceUnboundInstBlockFault -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceUnboundInstBlockFault__ = &kgmmuServiceUnboundInstBlockFault_56cd7a;
    }
    else
    {
        pThis->__kgmmuServiceUnboundInstBlockFault__ = &kgmmuServiceUnboundInstBlockFault_92bfc3;
    }

    // kgmmuCheckAndDecideBigPageSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kgmmuCheckAndDecideBigPageSize__ = &kgmmuCheckAndDecideBigPageSize_56cd7a;
    }
    else
    {
        pThis->__kgmmuCheckAndDecideBigPageSize__ = &kgmmuCheckAndDecideBigPageSize_GP100;
    }

    // kgmmuGetEccCounts -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000420UL) )) /* ChipHal: TU102 | GA100 | GH100 */ 
    {
        pThis->__kgmmuGetEccCounts__ = &kgmmuGetEccCounts_TU102;
    }
    // default
    else
    {
        pThis->__kgmmuGetEccCounts__ = &kgmmuGetEccCounts_4a4dee;
    }

    // kgmmuCreateFakeSparseTables -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuCreateFakeSparseTables__ = &kgmmuCreateFakeSparseTables_GH100;
    }
    // default
    else
    {
        pThis->__kgmmuCreateFakeSparseTables__ = &kgmmuCreateFakeSparseTables_56cd7a;
    }

    // kgmmuGetFakeSparseEntry -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuGetFakeSparseEntry__ = &kgmmuGetFakeSparseEntry_GH100;
    }
    // default
    else
    {
        pThis->__kgmmuGetFakeSparseEntry__ = &kgmmuGetFakeSparseEntry_fa6e19;
    }
} // End __nvoc_init_funcTable_KernelGmmu_2 with approximately 10 basic block(s).


// Initialize vtable(s) for 128 virtual method(s).
void __nvoc_init_funcTable_KernelGmmu(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 112 per-object function pointer(s).
    // To reduce stack pressure with some unoptimized builds, the logic is distributed among 2 functions.
    __nvoc_init_funcTable_KernelGmmu_1(pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGmmu_2(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGmmu(KernelGmmu *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_KernelGmmu = pThis;    // (kgmmu) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGmmu.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGmmu.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGmmu.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGmmu;    // (kgmmu) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGmmu(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGmmu *pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGmmu), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGmmu));

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

    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelGmmu(pThis, pGpuhalspecowner, pRmhalspecowner);
    status = __nvoc_ctor_KernelGmmu(pThis, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGmmu_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGmmu_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGmmu));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGmmu(ppThis, pParent, createFlags);

    return status;
}


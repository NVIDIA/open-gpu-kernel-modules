#define NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_graphics_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xea3fa9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphics;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for KernelGraphics
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__KernelGraphics(KernelGraphics*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGraphics(KernelGraphics*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGraphics(KernelGraphics*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelGraphics(KernelGraphics*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGraphics(KernelGraphics*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphics;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphics;

// Down-thunk(s) to bridge KernelGraphics methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
void __nvoc_down_thunk_KernelGraphics_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NvBool __nvoc_down_thunk_KernelGraphics_engstateIsPresent(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStatePostLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
void __nvoc_down_thunk_KernelGraphics_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]);    // this
NV_STATUS __nvoc_down_thunk_KernelGraphics_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3);    // this
NvBool __nvoc_down_thunk_KernelGraphics_intrservClearInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceClearInterruptArguments *arg3);    // this
NvU32 __nvoc_down_thunk_KernelGraphics_intrservServiceInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceInterruptArguments *arg3);    // this

// Up-thunk(s) to bridge KernelGraphics methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kgraphicsInitMissing(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreLoad(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePostUnload(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphics = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGraphics),
        /*classId=*/            classId(KernelGraphics),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGraphics",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGraphics,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGraphics,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGraphics
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelGraphics __nvoc_metadata__KernelGraphics = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGraphics,    // (kgraphics) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphics,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(KernelGraphics, __nvoc_base_IntrService),

    .vtable.__kgraphicsConstructEngine__ = &kgraphicsConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGraphics_engstateConstructEngine,    // virtual
    .vtable.__kgraphicsStateInitLocked__ = &kgraphicsStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelGraphics_engstateStateInitLocked,    // virtual
    .vtable.__kgraphicsStateLoad__ = &kgraphicsStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelGraphics_engstateStateLoad,    // virtual
    .vtable.__kgraphicsStatePreUnload__ = &kgraphicsStatePreUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelGraphics_engstateStatePreUnload,    // virtual
    .vtable.__kgraphicsStateUnload__ = &kgraphicsStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelGraphics_engstateStateUnload,    // virtual
    .vtable.__kgraphicsStateDestroy__ = &kgraphicsStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelGraphics_engstateStateDestroy,    // virtual
    .vtable.__kgraphicsIsPresent__ = &kgraphicsIsPresent_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &__nvoc_down_thunk_KernelGraphics_engstateIsPresent,    // virtual
    .vtable.__kgraphicsStatePostLoad__ = &kgraphicsStatePostLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelGraphics_engstateStatePostLoad,    // virtual
    .vtable.__kgraphicsRegisterIntrService__ = &kgraphicsRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelGraphics_intrservRegisterIntrService,    // virtual
    .vtable.__kgraphicsServiceNotificationInterrupt__ = &kgraphicsServiceNotificationInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_KernelGraphics_intrservServiceNotificationInterrupt,    // virtual
    .vtable.__kgraphicsClearInterrupt__ = &kgraphicsClearInterrupt_GP100,    // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &__nvoc_down_thunk_KernelGraphics_intrservClearInterrupt,    // virtual
    .vtable.__kgraphicsServiceInterrupt__ = &kgraphicsServiceInterrupt_GP100,    // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelGraphics_intrservServiceInterrupt,    // virtual
    .vtable.__kgraphicsInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kgraphicsStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kgraphicsStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kgraphicsStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kgraphicsStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kgraphicsStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphics = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__KernelGraphics.rtti,    // [0]: (kgraphics) this
        &__nvoc_metadata__KernelGraphics.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelGraphics.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__KernelGraphics.metadata__IntrService.rtti,    // [3]: (intrserv) super
    }
};

// 12 down-thunk(s) defined to bridge methods in KernelGraphics from superclasses

// kgraphicsConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return kgraphicsConstructEngine(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgraphicsStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return kgraphicsStateInitLocked(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kgraphicsStateLoad(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), flags);
}

// kgraphicsStatePreUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kgraphicsStatePreUnload(pGpu, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), flags);
}

// kgraphicsStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kgraphicsStateUnload(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), flags);
}

// kgraphicsStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelGraphics_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    kgraphicsStateDestroy(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsIsPresent: virtual override (engstate) base (engstate)
NvBool __nvoc_down_thunk_KernelGraphics_engstateIsPresent(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return kgraphicsIsPresent(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStatePostLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphics_engstateStatePostLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kgraphicsStatePostLoad(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), flags);
}

// kgraphicsRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelGraphics_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]) {
    kgraphicsRegisterIntrService(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_IntrService)), arg3);
}

// kgraphicsServiceNotificationInterrupt: virtual override (intrserv) base (intrserv)
NV_STATUS __nvoc_down_thunk_KernelGraphics_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return kgraphicsServiceNotificationInterrupt(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_IntrService)), arg3);
}

// kgraphicsClearInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv)
NvBool __nvoc_down_thunk_KernelGraphics_intrservClearInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceClearInterruptArguments *arg3) {
    return kgraphicsClearInterrupt(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_IntrService)), arg3);
}

// kgraphicsServiceInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelGraphics_intrservServiceInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceInterruptArguments *arg3) {
    return kgraphicsServiceInterrupt(arg1, (struct KernelGraphics *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphics, __nvoc_base_IntrService)), arg3);
}


// 6 up-thunk(s) defined to bridge methods in KernelGraphics to superclasses

// kgraphicsInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgraphicsInitMissing(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)));
}

// kgraphicsStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePreLoad(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgraphicsStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgraphicsStatePostUnload(struct OBJGPU *pGpu, struct KernelGraphics *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphics, __nvoc_base_OBJENGSTATE)), arg3);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphics = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelGraphics(KernelGraphics *pThis) {
    __nvoc_kgraphicsDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGraphics(KernelGraphics *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bCtxswLoggingSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bCtxswLoggingSupported = NV_TRUE;
    }

    pThis->bOverrideContextBuffersToGpuCached = NV_FALSE;

    pThis->bPeFiroBufferEnabled = NV_FALSE;

    // Hal field -- bDeferContextInit
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ 
    {
        pThis->bDeferContextInit = NV_FALSE;
    }

    // Hal field -- bPerSubcontextContextHeaderSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bPerSubcontextContextHeaderSupported = NV_TRUE;
    }

    // Hal field -- bSetContextBuffersGPUPrivileged
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bSetContextBuffersGPUPrivileged = NV_TRUE;
    }

    // Hal field -- bUcodeSupportsPrivAccessMap
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bUcodeSupportsPrivAccessMap = NV_TRUE;
    }

    // Hal field -- bRtvCbSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bRtvCbSupported = NV_TRUE;
    }

    // Hal field -- bFecsRecordUcodeSeqnoSupported
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->bFecsRecordUcodeSeqnoSupported = NV_FALSE;
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
        {
            pThis->bFecsRecordUcodeSeqnoSupported = NV_TRUE;
        }
        // default
        else
        {
            pThis->bFecsRecordUcodeSeqnoSupported = NV_FALSE;
        }
    }

    // Hal field -- bBug4208224WAREnabled
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->bBug4208224WAREnabled = NV_FALSE;
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->bBug4208224WAREnabled = NV_TRUE;
        }
        // default
        else
        {
            pThis->bBug4208224WAREnabled = NV_FALSE;
        }
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelGraphics(KernelGraphics *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphics_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphics_fail_IntrService;
    __nvoc_init_dataField_KernelGraphics(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGraphics_exit; // Success

__nvoc_ctor_KernelGraphics_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGraphics_fail_OBJENGSTATE:
__nvoc_ctor_KernelGraphics_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGraphics_1(KernelGraphics *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kgraphicsCreateBug4208224Channel -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
    {
        pThis->__kgraphicsCreateBug4208224Channel__ = &kgraphicsCreateBug4208224Channel_TU102;
    }
    // default
    else
    {
        pThis->__kgraphicsCreateBug4208224Channel__ = &kgraphicsCreateBug4208224Channel_56cd7a;
    }

    // kgraphicsInitializeBug4208224WAR -- halified (3 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgraphicsInitializeBug4208224WAR__ = &kgraphicsInitializeBug4208224WAR_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgraphicsInitializeBug4208224WAR__ = &kgraphicsInitializeBug4208224WAR_TU102;
        }
        // default
        else
        {
            pThis->__kgraphicsInitializeBug4208224WAR__ = &kgraphicsInitializeBug4208224WAR_56cd7a;
        }
    }

    // kgraphicsIsBug4208224WARNeeded -- halified (3 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgraphicsIsBug4208224WARNeeded__ = &kgraphicsIsBug4208224WARNeeded_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgraphicsIsBug4208224WARNeeded__ = &kgraphicsIsBug4208224WARNeeded_TU102;
        }
        // default
        else
        {
            pThis->__kgraphicsIsBug4208224WARNeeded__ = &kgraphicsIsBug4208224WARNeeded_3dd2c9;
        }
    }

    // kgraphicsLoadStaticInfo -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgraphicsLoadStaticInfo__ = &kgraphicsLoadStaticInfo_VF;
    }
    else
    {
        pThis->__kgraphicsLoadStaticInfo__ = &kgraphicsLoadStaticInfo_KERNEL;
    }

    // kgraphicsIsUnrestrictedAccessMapSupported -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgraphicsIsUnrestrictedAccessMapSupported__ = &kgraphicsIsUnrestrictedAccessMapSupported_3dd2c9;
    }
    // default
    else
    {
        pThis->__kgraphicsIsUnrestrictedAccessMapSupported__ = &kgraphicsIsUnrestrictedAccessMapSupported_PF;
    }

    // kgraphicsGetFecsTraceRdOffset -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgraphicsGetFecsTraceRdOffset__ = &kgraphicsGetFecsTraceRdOffset_474d46;
    }
    else
    {
        pThis->__kgraphicsGetFecsTraceRdOffset__ = &kgraphicsGetFecsTraceRdOffset_GA100;
    }

    // kgraphicsSetFecsTraceRdOffset -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgraphicsSetFecsTraceRdOffset__ = &kgraphicsSetFecsTraceRdOffset_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgraphicsSetFecsTraceRdOffset__ = &kgraphicsSetFecsTraceRdOffset_GA100;
    }
    else
    {
        pThis->__kgraphicsSetFecsTraceRdOffset__ = &kgraphicsSetFecsTraceRdOffset_GB100;
    }

    // kgraphicsSetFecsTraceWrOffset -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgraphicsSetFecsTraceWrOffset__ = &kgraphicsSetFecsTraceWrOffset_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgraphicsSetFecsTraceWrOffset__ = &kgraphicsSetFecsTraceWrOffset_GA100;
    }
    else
    {
        pThis->__kgraphicsSetFecsTraceWrOffset__ = &kgraphicsSetFecsTraceWrOffset_GB100;
    }

    // kgraphicsSetFecsTraceHwEnable -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgraphicsSetFecsTraceHwEnable__ = &kgraphicsSetFecsTraceHwEnable_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgraphicsSetFecsTraceHwEnable__ = &kgraphicsSetFecsTraceHwEnable_GA100;
    }
    else
    {
        pThis->__kgraphicsSetFecsTraceHwEnable__ = &kgraphicsSetFecsTraceHwEnable_GB100;
    }

    // kgraphicsIsCtxswLoggingEnabled -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgraphicsIsCtxswLoggingEnabled__ = &kgraphicsIsCtxswLoggingEnabled_920139;
    }
    else
    {
        pThis->__kgraphicsIsCtxswLoggingEnabled__ = &kgraphicsIsCtxswLoggingEnabled_FWCLIENT;
    }
} // End __nvoc_init_funcTable_KernelGraphics_1 with approximately 25 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_KernelGraphics(KernelGraphics *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 10 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGraphics_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGraphics(KernelGraphics *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_KernelGraphics = pThis;    // (kgraphics) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphics.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphics.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphics.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphics;    // (kgraphics) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGraphics(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGraphics(KernelGraphics **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGraphics *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGraphics), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGraphics));

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

    __nvoc_init__KernelGraphics(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGraphics(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGraphics_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGraphics_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGraphics));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGraphics(KernelGraphics **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGraphics(ppThis, pParent, createFlags);

    return status;
}


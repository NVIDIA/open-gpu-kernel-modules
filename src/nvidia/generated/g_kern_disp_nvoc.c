#define NVOC_KERN_DISP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_disp_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x55952e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for KernelDisplay
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__KernelDisplay(KernelDisplay*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_KernelDisplay(KernelDisplay*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelDisplay(KernelDisplay*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_KernelDisplay(KernelDisplay*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_KernelDisplay(KernelDisplay*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelDisplay;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelDisplay;

// Down-thunk(s) to bridge KernelDisplay methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
void __nvoc_down_thunk_KernelDisplay_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags);    // this
void __nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceRecord pRecords[180]);    // this
NvU32 __nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams);    // this

// Up-thunk(s) to bridge KernelDisplay methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kdispInitMissing(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kdispIsPresent(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NvBool __nvoc_up_thunk_IntrService_kdispClearInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NV_STATUS __nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelDisplay),
        /*classId=*/            classId(KernelDisplay),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelDisplay",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelDisplay,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelDisplay,
    /*pExportInfo=*/        &__nvoc_export_info__KernelDisplay
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelDisplay __nvoc_metadata__KernelDisplay = {
    .rtti.pClassDef = &__nvoc_class_def_KernelDisplay,    // (kdisp) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelDisplay,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService),

    .vtable.__kdispConstructEngine__ = &kdispConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelDisplay_engstateConstructEngine,    // virtual
    .vtable.__kdispStatePreInitLocked__ = &kdispStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked,    // virtual
    .vtable.__kdispStateInitLocked__ = &kdispStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStateInitLocked,    // virtual
    .vtable.__kdispStateDestroy__ = &kdispStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelDisplay_engstateStateDestroy,    // virtual
    .vtable.__kdispStateLoad__ = &kdispStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelDisplay_engstateStateLoad,    // virtual
    .vtable.__kdispStateUnload__ = &kdispStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelDisplay_engstateStateUnload,    // virtual
    .vtable.__kdispRegisterIntrService__ = &kdispRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService,    // virtual
    .vtable.__kdispServiceInterrupt__ = &kdispServiceInterrupt_KERNEL,    // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt,    // virtual
    .vtable.__kdispInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kdispInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kdispStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kdispStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kdispStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kdispStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kdispStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kdispStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kdispIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kdispIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    .vtable.__kdispClearInterrupt__ = &__nvoc_up_thunk_IntrService_kdispClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .vtable.__kdispServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelDisplay = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__KernelDisplay.rtti,    // [0]: (kdisp) this
        &__nvoc_metadata__KernelDisplay.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelDisplay.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__KernelDisplay.metadata__IntrService.rtti,    // [3]: (intrserv) super
    }
};

// 8 down-thunk(s) defined to bridge methods in KernelDisplay from superclasses

// kdispConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return kdispConstructEngine(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kdispStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStatePreInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStateInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelDisplay_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    kdispStateDestroy(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateLoad(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), flags);
}

// kdispStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateUnload(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), flags);
}

// kdispRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceRecord pRecords[180]) {
    kdispRegisterIntrService(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pRecords);
}

// kdispServiceInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return kdispServiceInterrupt(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
}


// 10 up-thunk(s) defined to bridge methods in KernelDisplay to superclasses

// kdispInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kdispInitMissing(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kdispIsPresent(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_kdispClearInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
}

// kdispServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelDisplay = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_kdispDestruct(KernelDisplay*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelDisplay(KernelDisplay *pThis) {
    __nvoc_kdispDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KDISP_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, NV_TRUE);
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE, (0));

    // NVOC Property Hal field -- PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE, NV_TRUE);
    pThis->setProperty(pThis, PDB_PROP_KDISP_WINDOW_CHANNEL_ALWAYS_MAPPED, (1));

    pThis->pStaticInfo = ((void *)0);

    pThis->bWarPurgeSatellitesOnCoreFree = NV_FALSE;

    pThis->bExtdevIntrSupported = NV_FALSE;
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelDisplay_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelDisplay_fail_IntrService;
    __nvoc_init_dataField_KernelDisplay(pThis, pRmhalspecowner, pGpuhalspecowner);
    goto __nvoc_ctor_KernelDisplay_exit; // Success

__nvoc_ctor_KernelDisplay_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelDisplay_fail_OBJENGSTATE:
__nvoc_ctor_KernelDisplay_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelDisplay_1(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);

    // kdispSelectClass -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_46f6a7;
    }
    else
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_v03_00_KERNEL;
    }

    // kdispGetBaseOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispGetBaseOffset__ = &kdispGetBaseOffset_v04_02;
    }
    else
    {
        pThis->__kdispGetBaseOffset__ = &kdispGetBaseOffset_4a4dee;
    }

    // kdispGetChannelNum -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_46f6a7;
    }
    else
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_v03_00;
    }

    // kdispGetDisplayCapsBaseAndSize -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_v03_00;
    }

    // kdispGetDisplaySfUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_v03_00;
    }

    // kdispGetDisplayChannelUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_46f6a7;
    }
    else
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_v03_00;
    }

    // kdispImportImpData -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* ChipHal: GB10B | T264D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00002000UL) )) /* ChipHal: T234D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00001000UL) )) /* DispIpHal: DISPv0402 */ ))
    {
        pThis->__kdispImportImpData__ = &kdispImportImpData_IMPL;
    }
    // default
    else
    {
        pThis->__kdispImportImpData__ = &kdispImportImpData_56cd7a;
    }

    // kdispArbAndAllocDisplayBandwidth -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispArbAndAllocDisplayBandwidth__ = &kdispArbAndAllocDisplayBandwidth_v04_02;
    }
    else
    {
        pThis->__kdispArbAndAllocDisplayBandwidth__ = &kdispArbAndAllocDisplayBandwidth_46f6a7;
    }

    // kdispGetVgaWorkspaceBase -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_v04_00;
    }
    // default
    else
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_72a2e1;
    }

    // kdispInvokeDisplayModesetCallback -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispInvokeDisplayModesetCallback__ = &kdispInvokeDisplayModesetCallback_KERNEL;
    }
    else
    {
        pThis->__kdispInvokeDisplayModesetCallback__ = &kdispInvokeDisplayModesetCallback_b3696a;
    }

    // kdispReadRgLineCountAndFrameCount -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0007dc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0402 | DISPv0404 | DISPv0501 | DISPv0502 | DISPv0503 | DISPv0504 */ 
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_v03_00_KERNEL;
    }
    // default
    else
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_46f6a7;
    }

    // kdispInitBrightcStateLoad -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispInitBrightcStateLoad__ = &kdispInitBrightcStateLoad_56cd7a;
    }
    else
    {
        pThis->__kdispInitBrightcStateLoad__ = &kdispInitBrightcStateLoad_IMPL;
    }

    // kdispSetupAcpiEdid -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispSetupAcpiEdid__ = &kdispSetupAcpiEdid_56cd7a;
    }
    else
    {
        pThis->__kdispSetupAcpiEdid__ = &kdispSetupAcpiEdid_IMPL;
    }

    // kdispRestoreOriginalLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00002000UL) )) /* ChipHal: T234D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00001000UL) )) /* DispIpHal: DISPv0402 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* ChipHal: T264D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ))
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_v03_00;
    }

    // kdispComputeLsrMinTimeValue -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_56cd7a;
    }
    else
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_v02_07;
    }

    // kdispSetSwapBarrierLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00002000UL) )) /* ChipHal: T234D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00001000UL) )) /* DispIpHal: DISPv0402 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* ChipHal: T264D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ))
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_v03_00;
    }

    // kdispGetRgScanLock -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0007dc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0402 | DISPv0404 | DISPv0501 | DISPv0502 | DISPv0503 | DISPv0504 */ 
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_v02_01;
    }
    // default
    else
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_92bfc3;
    }

    // kdispDetectSliLink -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0007dc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0402 | DISPv0404 | DISPv0501 | DISPv0502 | DISPv0503 | DISPv0504 */ 
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_v04_00;
    }
    // default
    else
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_92bfc3;
    }

    // kdispReadAwakenChannelNumMask -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0007dc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0402 | DISPv0404 | DISPv0501 | DISPv0502 | DISPv0503 | DISPv0504 */ 
    {
        pThis->__kdispReadAwakenChannelNumMask__ = &kdispReadAwakenChannelNumMask_v03_00;
    }
    // default
    else
    {
        pThis->__kdispReadAwakenChannelNumMask__ = &kdispReadAwakenChannelNumMask_46f6a7;
    }

    // kdispGetPBTargetAperture -- halified (3 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* ChipHal: GB10B | T264D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ))
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_v05_01;
    }
    else if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00002000UL) )) /* ChipHal: T234D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00001000UL) )) /* DispIpHal: DISPv0402 */ ))
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_v03_00;
    }
    // default
    else
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_15a734;
    }

    // kdispAllocateSharedMem -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kdispAllocateSharedMem__ = &kdispAllocateSharedMem_46f6a7;
    }
    // default
    else
    {
        pThis->__kdispAllocateSharedMem__ = &kdispAllocateSharedMem_IMPL;
    }

    // kdispReadPendingWinSemIntr -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispReadPendingWinSemIntr__ = &kdispReadPendingWinSemIntr_3dd2c9;
    }
    else
    {
        pThis->__kdispReadPendingWinSemIntr__ = &kdispReadPendingWinSemIntr_v04_01;
    }

    // kdispHandleWinSemEvt -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispHandleWinSemEvt__ = &kdispHandleWinSemEvt_b3696a;
    }
    else
    {
        pThis->__kdispHandleWinSemEvt__ = &kdispHandleWinSemEvt_v04_01;
    }

    // kdispIntrRetrigger -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ))
    {
        pThis->__kdispIntrRetrigger__ = &kdispIntrRetrigger_v05_01;
    }
    // default
    else
    {
        pThis->__kdispIntrRetrigger__ = &kdispIntrRetrigger_b3696a;
    }

    // kdispComputeDpModeSettings -- halified (3 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00100000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_56cd7a;
    }
    else if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00002000UL) )) /* ChipHal: T234D */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00001000UL) )) /* DispIpHal: DISPv0402 */ ))
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_v02_04;
    }
    else
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_v05_01;
    }

    // kdispServiceAwakenIntr -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0007dc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0402 | DISPv0404 | DISPv0501 | DISPv0502 | DISPv0503 | DISPv0504 */ 
    {
        pThis->__kdispServiceAwakenIntr__ = &kdispServiceAwakenIntr_v03_00;
    }
    // default
    else
    {
        pThis->__kdispServiceAwakenIntr__ = &kdispServiceAwakenIntr_4a4dee;
    }

    // kdispSetChannelTrashAndAbortAccel -- halified (2 hals)
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ))
    {
        pThis->__kdispSetChannelTrashAndAbortAccel__ = &kdispSetChannelTrashAndAbortAccel_v03_00;
    }
    // default
    else
    {
        pThis->__kdispSetChannelTrashAndAbortAccel__ = &kdispSetChannelTrashAndAbortAccel_b3696a;
    }

    // kdispIsChannelIdle -- halified (2 hals)
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ))
    {
        pThis->__kdispIsChannelIdle__ = &kdispIsChannelIdle_v03_00;
    }
    // default
    else
    {
        pThis->__kdispIsChannelIdle__ = &kdispIsChannelIdle_3dd2c9;
    }

    // kdispApplyChannelConnectDisconnect -- halified (2 hals)
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ))
    {
        pThis->__kdispApplyChannelConnectDisconnect__ = &kdispApplyChannelConnectDisconnect_v03_00;
    }
    // default
    else
    {
        pThis->__kdispApplyChannelConnectDisconnect__ = &kdispApplyChannelConnectDisconnect_b3696a;
    }

    // kdispIsChannelAllocatedHw -- halified (2 hals)
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* DispIpHal: DISPv0401 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004000UL) )) /* DispIpHal: DISPv0404 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00020000UL) )) /* DispIpHal: DISPv0503 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000800UL) )) /* ChipHal: GB20C */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0504 */ ))
    {
        pThis->__kdispIsChannelAllocatedHw__ = &kdispIsChannelAllocatedHw_v03_00;
    }
    // default
    else
    {
        pThis->__kdispIsChannelAllocatedHw__ = &kdispIsChannelAllocatedHw_3dd2c9;
    }
} // End __nvoc_init_funcTable_KernelDisplay_1 with approximately 63 basic block(s).


// Initialize vtable(s) for 48 virtual method(s).
void __nvoc_init_funcTable_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 30 per-object function pointer(s).
    __nvoc_init_funcTable_KernelDisplay_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_KernelDisplay = pThis;    // (kdisp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelDisplay.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelDisplay.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__KernelDisplay.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelDisplay;    // (kdisp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelDisplay(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelDisplay(KernelDisplay **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelDisplay *pThis;
    RmHalspecOwner *pRmhalspecowner;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(KernelDisplay));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelDisplay));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelDisplay_cleanup);

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

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelDisplay_cleanup);
    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelDisplay_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelDisplay(pThis, pRmhalspecowner, pGpuhalspecowner);
    status = __nvoc_ctor_KernelDisplay(pThis, pRmhalspecowner, pGpuhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelDisplay_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelDisplay_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelDisplay));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelDisplay(KernelDisplay **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelDisplay(ppThis, pParent, createFlags);

    return status;
}


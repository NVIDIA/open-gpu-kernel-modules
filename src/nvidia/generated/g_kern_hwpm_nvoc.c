#define NVOC_KERN_HWPM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_hwpm_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xc8c00f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelHwpm
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelHwpm(KernelHwpm*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelHwpm(KernelHwpm*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelHwpm(KernelHwpm*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelHwpm(KernelHwpm*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelHwpm(KernelHwpm*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelHwpm;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHwpm;

// Down-thunk(s) to bridge KernelHwpm methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelHwpm_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm);    // this
void __nvoc_down_thunk_KernelHwpm_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm);    // this

// Up-thunk(s) to bridge KernelHwpm methods to ancestors (if any)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmConstructEngine(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, ENGDESCRIPTOR arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_khwpmInitMissing(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_khwpmIsPresent(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHwpm),
        /*classId=*/            classId(KernelHwpm),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHwpm",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHwpm,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelHwpm,
    /*pExportInfo=*/        &__nvoc_export_info__KernelHwpm
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelHwpm __nvoc_metadata__KernelHwpm = {
    .rtti.pClassDef = &__nvoc_class_def_KernelHwpm,    // (khwpm) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHwpm,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__khwpmStateInitUnlocked__ = &khwpmStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_KernelHwpm_engstateStateInitUnlocked,    // virtual
    .vtable.__khwpmStateDestroy__ = &khwpmStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelHwpm_engstateStateDestroy,    // virtual
    .vtable.__khwpmConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmConstructEngine,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
    .vtable.__khwpmInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__khwpmStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__khwpmStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__khwpmStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__khwpmStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__khwpmStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__khwpmStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__khwpmStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__khwpmStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__khwpmStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__khwpmIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelHwpm = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelHwpm.rtti,    // [0]: (khwpm) this
        &__nvoc_metadata__KernelHwpm.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelHwpm.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelHwpm from superclasses

// khwpmStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelHwpm_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm) {
    return khwpmStateInitUnlocked(pGpu, (struct KernelHwpm *)(((unsigned char *) pKernelHwpm) - NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}

// khwpmStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelHwpm_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm) {
    khwpmStateDestroy(pGpu, (struct KernelHwpm *)(((unsigned char *) pKernelHwpm) - NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}


// 12 up-thunk(s) defined to bridge methods in KernelHwpm to superclasses

// khwpmConstructEngine: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmConstructEngine(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_khwpmInitMissing(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}

// khwpmStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}

// khwpmStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}

// khwpmStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}

// khwpmStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)), arg3);
}

// khwpmIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_khwpmIsPresent(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHwpm = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelHwpm(KernelHwpm *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KHWPM_HES_CWD_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_HES_CWD_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_HES_CWD_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KHWPM_EXTENDED_BUFFER_ENABLED
    pThis->setProperty(pThis, PDB_PROP_KHWPM_EXTENDED_BUFFER_ENABLED, NV_FALSE);

    // NVOC Property Hal field -- PDB_PROP_KHWPM_EXTENDED_BUFFER_SUPPORTED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_EXTENDED_BUFFER_SUPPORTED, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelHwpm_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelHwpm(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelHwpm_exit; // Success

__nvoc_ctor_KernelHwpm_fail_OBJENGSTATE:
__nvoc_ctor_KernelHwpm_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelHwpm_1(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // khwpmGetCblockInfo -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GM107;
    }
    else
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GB100;
    }
} // End __nvoc_init_funcTable_KernelHwpm_1 with approximately 5 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelHwpm_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelHwpm = pThis;    // (khwpm) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHwpm.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHwpm.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelHwpm;    // (khwpm) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelHwpm(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelHwpm(KernelHwpm **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHwpm *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelHwpm), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHwpm));

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

    __nvoc_init__KernelHwpm(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelHwpm(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHwpm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelHwpm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHwpm));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHwpm(KernelHwpm **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHwpm(ppThis, pParent, createFlags);

    return status;
}


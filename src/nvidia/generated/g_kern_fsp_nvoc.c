#define NVOC_KERN_FSP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_fsp_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x87fb96 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFsp;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelFsp
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelFsp(KernelFsp*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelFsp(KernelFsp*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelFsp(KernelFsp*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelFsp(KernelFsp*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelFsp(KernelFsp*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelFsp;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFsp;

// Down-thunk(s) to bridge KernelFsp methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, NvU32 flags);    // this
void __nvoc_down_thunk_KernelFsp_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp);    // this

// Up-thunk(s) to bridge KernelFsp methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kfspInitMissing(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kfspIsPresent(struct OBJGPU *pGpu, struct KernelFsp *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFsp = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelFsp),
        /*classId=*/            classId(KernelFsp),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelFsp",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelFsp,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelFsp,
    /*pExportInfo=*/        &__nvoc_export_info__KernelFsp
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelFsp __nvoc_metadata__KernelFsp = {
    .rtti.pClassDef = &__nvoc_class_def_KernelFsp,    // (kfsp) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFsp,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kfspConstructEngine__ = &kfspConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFsp_engstateConstructEngine,    // virtual
    .vtable.__kfspStateUnload__ = &kfspStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelFsp_engstateStateUnload,    // virtual
    .vtable.__kfspStateDestroy__ = &kfspStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFsp_engstateStateDestroy,    // virtual
    .vtable.__kfspSendAndReadMessageAsync__ = &kfspSendAndReadMessageAsync_IMPL,    // virtual
    .vtable.__kfspInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfspInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kfspStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kfspStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kfspStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__kfspStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kfspStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kfspStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kfspStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kfspStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kfspStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kfspIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfspIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelFsp = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelFsp.rtti,    // [0]: (kfsp) this
        &__nvoc_metadata__KernelFsp.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelFsp.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelFsp from superclasses

// kfspConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, ENGDESCRIPTOR arg3) {
    return kfspConstructEngine(pGpu, (struct KernelFsp *)(((unsigned char *) pKernelFsp) - NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, NvU32 flags) {
    return kfspStateUnload(pGpu, (struct KernelFsp *)(((unsigned char *) pKernelFsp) - NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), flags);
}

// kfspStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelFsp_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp) {
    kfspStateDestroy(pGpu, (struct KernelFsp *)(((unsigned char *) pKernelFsp) - NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}


// 11 up-thunk(s) defined to bridge methods in KernelFsp to superclasses

// kfspInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kfspInitMissing(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}

// kfspStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}

// kfspStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}

// kfspStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}

// kfspStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}

// kfspStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfspIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kfspIsPresent(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFsp = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelFsp(KernelFsp *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KFSP_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_RM_BOOT_GSP
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_RM_BOOT_GSP, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_RM_BOOT_GSP, NV_TRUE);
    }

    // Hal field -- cotPayloadSignatureSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->cotPayloadSignatureSize = 96;
    }
    // default
    else
    {
        pThis->cotPayloadSignatureSize = 384;
    }

    // Hal field -- cotPayloadPublicKeySize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->cotPayloadPublicKeySize = 97;
    }
    // default
    else
    {
        pThis->cotPayloadPublicKeySize = 384;
    }

    // Hal field -- cotPayloadVersion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->cotPayloadVersion = 2;
    }
    // default
    else
    {
        pThis->cotPayloadVersion = 1;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelFsp_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelFsp(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelFsp_exit; // Success

__nvoc_ctor_KernelFsp_fail_OBJENGSTATE:
__nvoc_ctor_KernelFsp_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelFsp_1(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kfspSendPacket -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspSendPacket__ = &kfspSendPacket_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspSendPacket__ = &kfspSendPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspSendPacket__ = &kfspSendPacket_46f6a7;
    }

    // kfspReadPacket -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspReadPacket__ = &kfspReadPacket_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspReadPacket__ = &kfspReadPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspReadPacket__ = &kfspReadPacket_46f6a7;
    }

    // kfspCanSendPacket -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspCanSendPacket__ = &kfspCanSendPacket_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCanSendPacket__ = &kfspCanSendPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspCanSendPacket__ = &kfspCanSendPacket_3dd2c9;
    }

    // kfspIsResponseAvailable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspIsResponseAvailable__ = &kfspIsResponseAvailable_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspIsResponseAvailable__ = &kfspIsResponseAvailable_GH100;
    }
    // default
    else
    {
        pThis->__kfspIsResponseAvailable__ = &kfspIsResponseAvailable_3dd2c9;
    }

    // kfspGspFmcIsEnforced -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_GH100;
    }
    // default
    else
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_3dd2c9;
    }

    // kfspPrepareBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_ac1694;
    }

    // kfspSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_ac1694;
    }

    // kfspPrepareAndSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_ac1694;
    }

    // kfspWaitForSecureBoot -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB100;
    }
    // default
    else
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_46f6a7;
    }

    // kfspGetMaxSendPacketSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspGetMaxSendPacketSize__ = &kfspGetMaxSendPacketSize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetMaxSendPacketSize__ = &kfspGetMaxSendPacketSize_b2b553;
    }

    // kfspGetMaxRecvPacketSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspGetMaxRecvPacketSize__ = &kfspGetMaxRecvPacketSize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetMaxRecvPacketSize__ = &kfspGetMaxRecvPacketSize_b2b553;
    }

    // kfspNvdmToSeid -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_GH100;
    }
    // default
    else
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_b2b553;
    }

    // kfspCreateMctpHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_b2b553;
    }

    // kfspCreateNvdmHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_b2b553;
    }

    // kfspGetPacketInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_395e98;
    }

    // kfspValidateMctpPayloadHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_395e98;
    }

    // kfspProcessNvdmMessage -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_GH100;
    }
    // default
    else
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_395e98;
    }

    // kfspProcessCommandResponse -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_GH100;
    }
    // default
    else
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_395e98;
    }

    // kfspDumpDebugState -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GH100;
    }
    // default
    else
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_b3696a;
    }

    // kfspErrorCode2NvStatusMap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspErrorCode2NvStatusMap__ = &kfspErrorCode2NvStatusMap_GH100;
    }
    // default
    else
    {
        pThis->__kfspErrorCode2NvStatusMap__ = &kfspErrorCode2NvStatusMap_395e98;
    }

    // kfspGetExtraReservedMemorySize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetExtraReservedMemorySize__ = &kfspGetExtraReservedMemorySize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetExtraReservedMemorySize__ = &kfspGetExtraReservedMemorySize_4a4dee;
    }

    // kfspWaitForGspTargetMaskReleased -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspWaitForGspTargetMaskReleased__ = &kfspWaitForGspTargetMaskReleased_GH100;
    }
    // default
    else
    {
        pThis->__kfspWaitForGspTargetMaskReleased__ = &kfspWaitForGspTargetMaskReleased_395e98;
    }

    // kfspRequiresBug3957833WAR -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspRequiresBug3957833WAR__ = &kfspRequiresBug3957833WAR_GH100;
    }
    // default
    else
    {
        pThis->__kfspRequiresBug3957833WAR__ = &kfspRequiresBug3957833WAR_3dd2c9;
    }

    // kfspFrtsSysmemLocationProgram -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_GH100;
    }
    // default
    else
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_395e98;
    }

    // kfspFrtsSysmemLocationClear -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_GH100;
    }
    // default
    else
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_d44104;
    }

    // kfspCheckForClockBoostCapability -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_GB100;
    }
    // default
    else
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_b3696a;
    }

    // kfspSendClockBoostRpc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_GB100;
    }
    // default
    else
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_56cd7a;
    }
} // End __nvoc_init_funcTable_KernelFsp_1 with approximately 63 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFsp_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelFsp = pThis;    // (kfsp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFsp.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFsp.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelFsp;    // (kfsp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelFsp(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelFsp(KernelFsp **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelFsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelFsp), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelFsp));

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

    __nvoc_init__KernelFsp(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelFsp(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelFsp_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelFsp_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelFsp));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelFsp(KernelFsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelFsp(ppThis, pParent, createFlags);

    return status;
}


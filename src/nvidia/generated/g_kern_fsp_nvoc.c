#define NVOC_KERN_FSP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_fsp_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x87fb96 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFsp;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelFsp(KernelFsp*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelFsp(KernelFsp*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFsp(KernelFsp*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelFsp(KernelFsp*, RmHalspecOwner* );
void __nvoc_dtor_KernelFsp(KernelFsp*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFsp;

static const struct NVOC_RTTI __nvoc_rtti_KernelFsp_KernelFsp = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFsp,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFsp,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFsp_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFsp_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFsp, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelFsp = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelFsp_KernelFsp,
        &__nvoc_rtti_KernelFsp_OBJENGSTATE,
        &__nvoc_rtti_KernelFsp_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelFsp,
    /*pExportInfo=*/        &__nvoc_export_info_KernelFsp
};

// Down-thunk(s) to bridge KernelFsp methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelFsp_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, NvU32 flags);    // this
void __nvoc_down_thunk_KernelFsp_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp);    // this

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


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFsp = 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
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

    // Hal field -- cotPayloadSignatureSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->cotPayloadSignatureSize = 96;
    }
    // default
    else
    {
        pThis->cotPayloadSignatureSize = 384;
    }

    // Hal field -- cotPayloadPublicKeySize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->cotPayloadPublicKeySize = 97;
    }
    // default
    else
    {
        pThis->cotPayloadPublicKeySize = 384;
    }

    // Hal field -- cotPayloadVersion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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

    // kfspSendPacket -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspSendPacket__ = &kfspSendPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspSendPacket__ = &kfspSendPacket_395e98;
    }

    // kfspReadPacket -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspReadPacket__ = &kfspReadPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspReadPacket__ = &kfspReadPacket_395e98;
    }

    // kfspCanSendPacket -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCanSendPacket__ = &kfspCanSendPacket_GH100;
    }
    // default
    else
    {
        pThis->__kfspCanSendPacket__ = &kfspCanSendPacket_d69453;
    }

    // kfspIsResponseAvailable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspIsResponseAvailable__ = &kfspIsResponseAvailable_GH100;
    }
    // default
    else
    {
        pThis->__kfspIsResponseAvailable__ = &kfspIsResponseAvailable_d69453;
    }

    // kfspGspFmcIsEnforced -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_GH100;
    }
    // default
    else
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_3dd2c9;
    }

    // kfspPrepareBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_ac1694;
    }

    // kfspSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_ac1694;
    }

    // kfspPrepareAndSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_GH100;
    }
    // default
    else
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_ac1694;
    }

    // kfspWaitForSecureBoot -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB202;
    }
    // default
    else
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_46f6a7;
    }

    // kfspGetMaxSendPacketSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspGetMaxSendPacketSize__ = &kfspGetMaxSendPacketSize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetMaxSendPacketSize__ = &kfspGetMaxSendPacketSize_b2b553;
    }

    // kfspGetMaxRecvPacketSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspGetMaxRecvPacketSize__ = &kfspGetMaxRecvPacketSize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetMaxRecvPacketSize__ = &kfspGetMaxRecvPacketSize_b2b553;
    }

    // kfspNvdmToSeid -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_GH100;
    }
    // default
    else
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_b2b553;
    }

    // kfspCreateMctpHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_b2b553;
    }

    // kfspCreateNvdmHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_b2b553;
    }

    // kfspGetPacketInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_395e98;
    }

    // kfspValidateMctpPayloadHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_GH100;
    }
    // default
    else
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_395e98;
    }

    // kfspProcessNvdmMessage -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_GH100;
    }
    // default
    else
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_395e98;
    }

    // kfspProcessCommandResponse -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_GH100;
    }
    // default
    else
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_395e98;
    }

    // kfspDumpDebugState -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GB202;
    }
    // default
    else
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_b3696a;
    }

    // kfspErrorCode2NvStatusMap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_GH100;
    }
    // default
    else
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_395e98;
    }

    // kfspFrtsSysmemLocationClear -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_GH100;
    }
    // default
    else
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_d44104;
    }

    // kfspCheckForClockBoostCapability -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_GB100;
    }
    // default
    else
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_b3696a;
    }

    // kfspSendClockBoostRpc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_GB100;
    }
    // default
    else
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_56cd7a;
    }
} // End __nvoc_init_funcTable_KernelFsp_1 with approximately 57 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelFsp vtable = {
        .__kfspConstructEngine__ = &kfspConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFsp_engstateConstructEngine,    // virtual
        .__kfspStateUnload__ = &kfspStateUnload_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelFsp_engstateStateUnload,    // virtual
        .__kfspStateDestroy__ = &kfspStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFsp_engstateStateDestroy,    // virtual
        .__kfspSendAndReadMessageAsync__ = &kfspSendAndReadMessageAsync_IMPL,    // virtual
        .__kfspInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfspInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kfspStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kfspStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kfspStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__kfspStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kfspStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kfspStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__kfspStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kfspStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kfspStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kfspIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfspIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kfsp) this

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFsp_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelFsp = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelFsp);

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

    __nvoc_init_KernelFsp(pThis, pRmhalspecowner);
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


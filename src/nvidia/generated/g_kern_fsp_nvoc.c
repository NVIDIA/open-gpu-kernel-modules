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

// 2 down-thunk(s) defined to bridge methods in KernelFsp from superclasses

// kfspConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelFsp_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, ENGDESCRIPTOR arg3) {
    return kfspConstructEngine(pGpu, (struct KernelFsp *)(((unsigned char *) pKernelFsp) - __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelFsp_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp) {
    kfspStateDestroy(pGpu, (struct KernelFsp *)(((unsigned char *) pKernelFsp) - __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}


// 12 up-thunk(s) defined to bridge methods in KernelFsp to superclasses

// kfspInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kfspInitMissing(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

// kfspStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

// kfspStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

// kfspStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

// kfspStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

// kfspStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStateUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload(struct OBJGPU *pGpu, struct KernelFsp *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg3);
}

// kfspIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kfspIsPresent(struct OBJGPU *pGpu, struct KernelFsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, ((NvBool)(0 != 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_FSP_FUSE_ERROR_CHECK_ENABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM, ((NvBool)(0 == 0)));
    }

    // Hal field -- cotPayloadSignatureSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->cotPayloadSignatureSize = 96;
    }
    // default
    else
    {
        pThis->cotPayloadSignatureSize = 384;
    }

    // Hal field -- cotPayloadPublicKeySize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->cotPayloadPublicKeySize = 97;
    }
    // default
    else
    {
        pThis->cotPayloadPublicKeySize = 384;
    }

    // Hal field -- cotPayloadVersion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
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

    // kfspConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kfspConstructEngine__ = &kfspConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFsp_engstateConstructEngine;

    // kfspStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__kfspStateDestroy__ = &kfspStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFsp_engstateStateDestroy;

    // kfspSendAndReadMessageAsync -- virtual
    pThis->__kfspSendAndReadMessageAsync__ = &kfspSendAndReadMessageAsync_IMPL;

    // kfspGspFmcIsEnforced -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_GH100;
    }
    // default
    else
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_491d52;
    }

    // kfspPrepareBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_GH100;
    }
    else
    {
        pThis->__kfspPrepareBootCommands__ = &kfspPrepareBootCommands_ac1694;
    }

    // kfspSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_GH100;
    }
    else
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_ac1694;
    }

    // kfspPrepareAndSendBootCommands -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_GH100;
    }
    else
    {
        pThis->__kfspPrepareAndSendBootCommands__ = &kfspPrepareAndSendBootCommands_ac1694;
    }

    // kfspWaitForSecureBoot -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GB100;
    }
    // default
    else
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_46f6a7;
    }

    // kfspGetRmChannelSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspGetRmChannelSize__ = &kfspGetRmChannelSize_GH100;
    }
    else
    {
        pThis->__kfspGetRmChannelSize__ = &kfspGetRmChannelSize_b2b553;
    }

    // kfspConfigEmemc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspConfigEmemc__ = &kfspConfigEmemc_GH100;
    }
    else
    {
        pThis->__kfspConfigEmemc__ = &kfspConfigEmemc_395e98;
    }

    // kfspUpdateQueueHeadTail -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspUpdateQueueHeadTail__ = &kfspUpdateQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspUpdateQueueHeadTail__ = &kfspUpdateQueueHeadTail_d44104;
    }

    // kfspGetQueueHeadTail -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspGetQueueHeadTail__ = &kfspGetQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspGetQueueHeadTail__ = &kfspGetQueueHeadTail_d44104;
    }

    // kfspUpdateMsgQueueHeadTail -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspUpdateMsgQueueHeadTail__ = &kfspUpdateMsgQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspUpdateMsgQueueHeadTail__ = &kfspUpdateMsgQueueHeadTail_d44104;
    }

    // kfspGetMsgQueueHeadTail -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspGetMsgQueueHeadTail__ = &kfspGetMsgQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspGetMsgQueueHeadTail__ = &kfspGetMsgQueueHeadTail_d44104;
    }

    // kfspNvdmToSeid -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_GH100;
    }
    else
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_b2b553;
    }

    // kfspCreateMctpHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_GH100;
    }
    else
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_b2b553;
    }

    // kfspCreateNvdmHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_GH100;
    }
    else
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_b2b553;
    }

    // kfspWriteToEmem -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspWriteToEmem__ = &kfspWriteToEmem_GH100;
    }
    else
    {
        pThis->__kfspWriteToEmem__ = &kfspWriteToEmem_395e98;
    }

    // kfspReadFromEmem -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspReadFromEmem__ = &kfspReadFromEmem_GH100;
    }
    else
    {
        pThis->__kfspReadFromEmem__ = &kfspReadFromEmem_395e98;
    }

    // kfspGetPacketInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_GH100;
    }
    else
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_395e98;
    }

    // kfspValidateMctpPayloadHeader -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_GH100;
    }
    else
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_395e98;
    }

    // kfspProcessNvdmMessage -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_GH100;
    }
    else
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_395e98;
    }

    // kfspProcessCommandResponse -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_GH100;
    }
    else
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_395e98;
    }

    // kfspDumpDebugState -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GH100;
    }
    // default
    else
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_b3696a;
    }

    // kfspErrorCode2NvStatusMap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspErrorCode2NvStatusMap__ = &kfspErrorCode2NvStatusMap_GH100;
    }
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspWaitForGspTargetMaskReleased__ = &kfspWaitForGspTargetMaskReleased_GH100;
    }
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
        pThis->__kfspRequiresBug3957833WAR__ = &kfspRequiresBug3957833WAR_491d52;
    }

    // kfspFrtsSysmemLocationProgram -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_GH100;
    }
    else
    {
        pThis->__kfspFrtsSysmemLocationProgram__ = &kfspFrtsSysmemLocationProgram_395e98;
    }

    // kfspFrtsSysmemLocationClear -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_GH100;
    }
    else
    {
        pThis->__kfspFrtsSysmemLocationClear__ = &kfspFrtsSysmemLocationClear_d44104;
    }

    // kfspCheckForClockBoostCapability -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_GB100;
    }
    // default
    else
    {
        pThis->__kfspCheckForClockBoostCapability__ = &kfspCheckForClockBoostCapability_b3696a;
    }

    // kfspSendClockBoostRpc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_GB100;
    }
    // default
    else
    {
        pThis->__kfspSendClockBoostRpc__ = &kfspSendClockBoostRpc_56cd7a;
    }

    // kfspInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kfspInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfspInitMissing;

    // kfspStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitLocked;

    // kfspStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreInitUnlocked;

    // kfspStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitLocked;

    // kfspStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateInitUnlocked;

    // kfspStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreLoad;

    // kfspStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateLoad;

    // kfspStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostLoad;

    // kfspStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePreUnload;

    // kfspStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStateUnload;

    // kfspStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kfspStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfspStatePostUnload;

    // kfspIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kfspIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfspIsPresent;
} // End __nvoc_init_funcTable_KernelFsp_1 with approximately 76 basic block(s).


// Initialize vtable(s) for 44 virtual method(s).
void __nvoc_init_funcTable_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 44 per-object function pointer(s).
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


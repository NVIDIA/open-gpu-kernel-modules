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

static NV_STATUS __nvoc_thunk_KernelFsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp, ENGDESCRIPTOR arg0) {
    return kfspConstructEngine(pGpu, (struct KernelFsp *)(((unsigned char *)pKernelFsp) - __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelFsp_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFsp) {
    kfspStateDestroy(pGpu, (struct KernelFsp *)(((unsigned char *)pKernelFsp) - __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStateLoad(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStateUnload(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStateInitLocked(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePreLoad(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePostUnload(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePreUnload(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStateInitUnlocked(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kfspInitMissing(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePreInitLocked(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePreInitUnlocked(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfspStatePostLoad(POBJGPU pGpu, struct KernelFsp *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kfspIsPresent(POBJGPU pGpu, struct KernelFsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFsp_OBJENGSTATE.offset));
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, ((NvBool)(0 != 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_IS_MISSING, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFSP_DISABLE_FRTS_SYSMEM, ((NvBool)(0 == 0)));
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

    pThis->__kfspConstructEngine__ = &kfspConstructEngine_IMPL;

    pThis->__kfspStateDestroy__ = &kfspStateDestroy_IMPL;

    pThis->__kfspSecureReset__ = &kfspSecureReset_IMPL;

    pThis->__kfspSendPacket__ = &kfspSendPacket_IMPL;

    pThis->__kfspSendAndReadMessage__ = &kfspSendAndReadMessage_IMPL;

    pThis->__kfspIsQueueEmpty__ = &kfspIsQueueEmpty_IMPL;

    pThis->__kfspPollForQueueEmpty__ = &kfspPollForQueueEmpty_IMPL;

    pThis->__kfspIsMsgQueueEmpty__ = &kfspIsMsgQueueEmpty_IMPL;

    pThis->__kfspPollForResponse__ = &kfspPollForResponse_IMPL;

    // Hal function -- kfspGspFmcIsEnforced
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_GH100;
    }
    // default
    else
    {
        pThis->__kfspGspFmcIsEnforced__ = &kfspGspFmcIsEnforced_491d52;
    }

    // Hal function -- kfspSendBootCommands
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_GH100;
    }
    else
    {
        pThis->__kfspSendBootCommands__ = &kfspSendBootCommands_ac1694;
    }

    // Hal function -- kfspWaitForSecureBoot
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_GH100;
    }
    else
    {
        pThis->__kfspWaitForSecureBoot__ = &kfspWaitForSecureBoot_395e98;
    }

    // Hal function -- kfspGetRmChannelSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetRmChannelSize__ = &kfspGetRmChannelSize_GH100;
    }
    else
    {
        pThis->__kfspGetRmChannelSize__ = &kfspGetRmChannelSize_b2b553;
    }

    // Hal function -- kfspConfigEmemc
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspConfigEmemc__ = &kfspConfigEmemc_GH100;
    }
    else
    {
        pThis->__kfspConfigEmemc__ = &kfspConfigEmemc_395e98;
    }

    // Hal function -- kfspUpdateQueueHeadTail
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspUpdateQueueHeadTail__ = &kfspUpdateQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspUpdateQueueHeadTail__ = &kfspUpdateQueueHeadTail_d44104;
    }

    // Hal function -- kfspGetQueueHeadTail
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetQueueHeadTail__ = &kfspGetQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspGetQueueHeadTail__ = &kfspGetQueueHeadTail_d44104;
    }

    // Hal function -- kfspUpdateMsgQueueHeadTail
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspUpdateMsgQueueHeadTail__ = &kfspUpdateMsgQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspUpdateMsgQueueHeadTail__ = &kfspUpdateMsgQueueHeadTail_d44104;
    }

    // Hal function -- kfspGetMsgQueueHeadTail
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetMsgQueueHeadTail__ = &kfspGetMsgQueueHeadTail_GH100;
    }
    else
    {
        pThis->__kfspGetMsgQueueHeadTail__ = &kfspGetMsgQueueHeadTail_d44104;
    }

    // Hal function -- kfspNvdmToSeid
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_GH100;
    }
    else
    {
        pThis->__kfspNvdmToSeid__ = &kfspNvdmToSeid_b2b553;
    }

    // Hal function -- kfspCreateMctpHeader
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_GH100;
    }
    else
    {
        pThis->__kfspCreateMctpHeader__ = &kfspCreateMctpHeader_b2b553;
    }

    // Hal function -- kfspCreateNvdmHeader
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_GH100;
    }
    else
    {
        pThis->__kfspCreateNvdmHeader__ = &kfspCreateNvdmHeader_b2b553;
    }

    // Hal function -- kfspWriteToEmem
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWriteToEmem__ = &kfspWriteToEmem_GH100;
    }
    else
    {
        pThis->__kfspWriteToEmem__ = &kfspWriteToEmem_395e98;
    }

    // Hal function -- kfspReadFromEmem
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspReadFromEmem__ = &kfspReadFromEmem_GH100;
    }
    else
    {
        pThis->__kfspReadFromEmem__ = &kfspReadFromEmem_395e98;
    }

    // Hal function -- kfspGetPacketInfo
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_GH100;
    }
    else
    {
        pThis->__kfspGetPacketInfo__ = &kfspGetPacketInfo_395e98;
    }

    // Hal function -- kfspValidateMctpPayloadHeader
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_GH100;
    }
    else
    {
        pThis->__kfspValidateMctpPayloadHeader__ = &kfspValidateMctpPayloadHeader_395e98;
    }

    // Hal function -- kfspProcessNvdmMessage
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_GH100;
    }
    else
    {
        pThis->__kfspProcessNvdmMessage__ = &kfspProcessNvdmMessage_395e98;
    }

    // Hal function -- kfspProcessCommandResponse
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_GH100;
    }
    else
    {
        pThis->__kfspProcessCommandResponse__ = &kfspProcessCommandResponse_395e98;
    }

    // Hal function -- kfspDumpDebugState
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_GH100;
    }
    else
    {
        pThis->__kfspDumpDebugState__ = &kfspDumpDebugState_d44104;
    }

    // Hal function -- kfspErrorCode2NvStatusMap
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspErrorCode2NvStatusMap__ = &kfspErrorCode2NvStatusMap_GH100;
    }
    else
    {
        pThis->__kfspErrorCode2NvStatusMap__ = &kfspErrorCode2NvStatusMap_395e98;
    }

    // Hal function -- kfspGetExtraReservedMemorySize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspGetExtraReservedMemorySize__ = &kfspGetExtraReservedMemorySize_GH100;
    }
    // default
    else
    {
        pThis->__kfspGetExtraReservedMemorySize__ = &kfspGetExtraReservedMemorySize_4a4dee;
    }

    // Hal function -- kfspCheckGspSecureScratch
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspCheckGspSecureScratch__ = &kfspCheckGspSecureScratch_GH100;
    }
    // default
    else
    {
        pThis->__kfspCheckGspSecureScratch__ = &kfspCheckGspSecureScratch_491d52;
    }

    // Hal function -- kfspWaitForGspTargetMaskReleased
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspWaitForGspTargetMaskReleased__ = &kfspWaitForGspTargetMaskReleased_GH100;
    }
    else
    {
        pThis->__kfspWaitForGspTargetMaskReleased__ = &kfspWaitForGspTargetMaskReleased_395e98;
    }

    // Hal function -- kfspRequiresBug3957833WAR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfspRequiresBug3957833WAR__ = &kfspRequiresBug3957833WAR_GH100;
    }
    // default
    else
    {
        pThis->__kfspRequiresBug3957833WAR__ = &kfspRequiresBug3957833WAR_491d52;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelFsp_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelFsp_engstateStateDestroy;

    pThis->__kfspStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kfspStateLoad;

    pThis->__kfspStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kfspStateUnload;

    pThis->__kfspStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kfspStateInitLocked;

    pThis->__kfspStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePreLoad;

    pThis->__kfspStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePostUnload;

    pThis->__kfspStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePreUnload;

    pThis->__kfspStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kfspStateInitUnlocked;

    pThis->__kfspInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kfspInitMissing;

    pThis->__kfspStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePreInitLocked;

    pThis->__kfspStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePreInitUnlocked;

    pThis->__kfspStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kfspStatePostLoad;

    pThis->__kfspIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kfspIsPresent;
}

void __nvoc_init_funcTable_KernelFsp(KernelFsp *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_KernelFsp(KernelFsp **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelFsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelFsp), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelFsp));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelFsp);

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

    __nvoc_init_KernelFsp(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelFsp(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelFsp_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelFsp_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelFsp));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelFsp(KernelFsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelFsp(ppThis, pParent, createFlags);

    return status;
}


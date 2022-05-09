#define NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sec2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2f36c9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelSec2(KernelSec2*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelSec2(KernelSec2*, RmHalspecOwner* );
void __nvoc_dtor_KernelSec2(KernelSec2*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2;

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelSec2 = {
    /*pClassDef=*/          &__nvoc_class_def_KernelSec2,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelSec2,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSec2_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSec2, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelSec2 = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_KernelSec2_KernelSec2,
        &__nvoc_rtti_KernelSec2_KernelFalcon,
        &__nvoc_rtti_KernelSec2_OBJENGSTATE,
        &__nvoc_rtti_KernelSec2_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2 = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelSec2),
        /*classId=*/            classId(KernelSec2),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelSec2",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelSec2,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelSec2,
    /*pExportInfo=*/        &__nvoc_export_info_KernelSec2
};

static NV_STATUS __nvoc_thunk_KernelSec2_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelSec2, ENGDESCRIPTOR arg0) {
    return ksec2ConstructEngine(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelSec2_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelSec2) {
    return ksec2ResetHw(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_KernelFalcon.offset));
}

static NvBool __nvoc_thunk_KernelSec2_kflcnIsEngineInReset(struct OBJGPU *pGpu, struct KernelFalcon *pKernelSec2) {
    return ksec2IsEngineInReset(pGpu, (struct KernelSec2 *)(((unsigned char *)pKernelSec2) - __nvoc_rtti_KernelSec2_KernelFalcon.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2ReconcileTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateLoad(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateInitLocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreLoad(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePostUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_ksec2StateDestroy(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreUnload(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StateInitUnlocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_ksec2InitMissing(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreInitLocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2GetTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2CompareTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_ksec2FreeTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2StatePostLoad(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2AllocTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_ksec2SetTunableState(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_ksec2IsPresent(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelSec2_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSec2 = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelSec2(KernelSec2 *pThis) {
    __nvoc_ksec2Destruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_OBJENGSTATE;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelSec2_fail_KernelFalcon;
    __nvoc_init_dataField_KernelSec2(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelSec2_exit; // Success

__nvoc_ctor_KernelSec2_fail_KernelFalcon:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelSec2_fail_OBJENGSTATE:
__nvoc_ctor_KernelSec2_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelSec2_1(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // Hal function -- ksec2ConstructEngine
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__ksec2ConstructEngine__ = &ksec2ConstructEngine_IMPL;
    }
    else if (0)
    {
    }

    // Hal function -- ksec2ConfigureFalcon
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__ksec2ConfigureFalcon__ = &ksec2ConfigureFalcon_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- ksec2ResetHw
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__ksec2ResetHw__ = &ksec2ResetHw_TU102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- ksec2IsEngineInReset
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__ksec2IsEngineInReset__ = &ksec2IsEngineInReset_TU102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- ksec2ReadUcodeFuseVersion
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_b2b553;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__ksec2ReadUcodeFuseVersion__ = &ksec2ReadUcodeFuseVersion_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelSec2_engstateConstructEngine;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_KernelSec2_kflcnResetHw;

    pThis->__nvoc_base_KernelFalcon.__kflcnIsEngineInReset__ = &__nvoc_thunk_KernelSec2_kflcnIsEngineInReset;

    pThis->__ksec2ReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2ReconcileTunableState;

    pThis->__ksec2StateLoad__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateLoad;

    pThis->__ksec2StateUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateUnload;

    pThis->__ksec2StateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateInitLocked;

    pThis->__ksec2StatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreLoad;

    pThis->__ksec2StatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePostUnload;

    pThis->__ksec2StateDestroy__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateDestroy;

    pThis->__ksec2StatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreUnload;

    pThis->__ksec2StateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StateInitUnlocked;

    pThis->__ksec2InitMissing__ = &__nvoc_thunk_OBJENGSTATE_ksec2InitMissing;

    pThis->__ksec2StatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreInitLocked;

    pThis->__ksec2StatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePreInitUnlocked;

    pThis->__ksec2GetTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2GetTunableState;

    pThis->__ksec2CompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2CompareTunableState;

    pThis->__ksec2FreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2FreeTunableState;

    pThis->__ksec2StatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_ksec2StatePostLoad;

    pThis->__ksec2AllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2AllocTunableState;

    pThis->__ksec2SetTunableState__ = &__nvoc_thunk_OBJENGSTATE_ksec2SetTunableState;

    pThis->__ksec2IsPresent__ = &__nvoc_thunk_OBJENGSTATE_ksec2IsPresent;
}

void __nvoc_init_funcTable_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelSec2_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*, RmHalspecOwner* );
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelSec2(KernelSec2 *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelSec2 = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelSec2(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelSec2 *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelSec2));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelSec2));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelSec2);

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

    __nvoc_init_KernelSec2(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelSec2(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelSec2_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelSec2_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2 **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelSec2(ppThis, pParent, createFlags);

    return status;
}


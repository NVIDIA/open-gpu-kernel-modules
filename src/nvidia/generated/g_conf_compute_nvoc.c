#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_conf_compute_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9798cc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner* );
void __nvoc_init_funcTable_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner* );
void __nvoc_init_dataField_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner* );
void __nvoc_dtor_ConfidentialCompute(ConfidentialCompute*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ConfidentialCompute;

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialCompute_ConfidentialCompute = {
    /*pClassDef=*/          &__nvoc_class_def_ConfidentialCompute,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConfidentialCompute,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialCompute_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialCompute_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ConfidentialCompute = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_ConfidentialCompute_ConfidentialCompute,
        &__nvoc_rtti_ConfidentialCompute_OBJENGSTATE,
        &__nvoc_rtti_ConfidentialCompute_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ConfidentialCompute),
        /*classId=*/            classId(ConfidentialCompute),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ConfidentialCompute",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ConfidentialCompute,
    /*pCastInfo=*/          &__nvoc_castinfo_ConfidentialCompute,
    /*pExportInfo=*/        &__nvoc_export_info_ConfidentialCompute
};

// 5 down-thunk(s) defined to bridge methods in ConfidentialCompute from superclasses

// confComputeConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, ENGDESCRIPTOR engDesc) {
    return confComputeConstructEngine(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), engDesc);
}

// confComputeStatePreInitLocked: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute) {
    return confComputeStatePreInitLocked(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute) {
    return confComputeStateInitLocked(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags) {
    return confComputeStatePostLoad(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), flags);
}

// confComputeStatePreUnload: virtual halified (2 hals) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags) {
    return confComputeStatePreUnload(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), flags);
}


// 9 up-thunk(s) defined to bridge methods in ConfidentialCompute to superclasses

// confComputeInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_confComputeInitMissing(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), arg3);
}

// confComputeStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), arg3);
}

// confComputeStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), arg3);
}

// confComputeStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePostUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset), arg3);
}

// confComputeStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_confComputeStateDestroy(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}

// confComputeIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_confComputeIsPresent(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_ConfidentialCompute_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ConfidentialCompute = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_ConfidentialCompute(ConfidentialCompute *pThis) {
    __nvoc_confComputeDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_CONFCOMPUTE_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_IS_MISSING, ((NvBool)(0 == 0)));
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_IS_MISSING, ((NvBool)(0 != 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED, ((NvBool)(0 == 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED, ((NvBool)(0 != 0)));
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, ((NvBool)(0 != 0)));
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialCompute_fail_OBJENGSTATE;
    __nvoc_init_dataField_ConfidentialCompute(pThis, pRmhalspecowner);
    goto __nvoc_ctor_ConfidentialCompute_exit; // Success

__nvoc_ctor_ConfidentialCompute_fail_OBJENGSTATE:
__nvoc_ctor_ConfidentialCompute_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ConfidentialCompute_1(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // confComputeConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__confComputeConstructEngine__ = &confComputeConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_ConfidentialCompute_engstateConstructEngine;

    // confComputeDestruct -- halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeDestruct__ = &confComputeDestruct_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeDestruct__ = &confComputeDestruct_b3696a;
    }

    // confComputeStatePreInitLocked -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__confComputeStatePreInitLocked__ = &confComputeStatePreInitLocked_56cd7a;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePreInitLocked;

    // confComputeStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__confComputeStateInitLocked__ = &confComputeStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStateInitLocked;

    // confComputeStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeStatePostLoad__ = &confComputeStatePostLoad_IMPL;
    }
    // default
    else
    {
        pThis->__confComputeStatePostLoad__ = &confComputeStatePostLoad_56cd7a;
    }
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePostLoad;

    // confComputeStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeStatePreUnload__ = &confComputeStatePreUnload_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeStatePreUnload__ = &confComputeStatePreUnload_56cd7a;
    }
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePreUnload;

    // confComputeSetErrorState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeSetErrorState__ = &confComputeSetErrorState_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeSetErrorState__ = &confComputeSetErrorState_b3696a;
    }

    // confComputeKeyStoreRetrieveViaChannel -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreRetrieveViaChannel__ = &confComputeKeyStoreRetrieveViaChannel_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreRetrieveViaChannel__ = &confComputeKeyStoreRetrieveViaChannel_46f6a7;
    }

    // confComputeKeyStoreRetrieveViaKeyId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreRetrieveViaKeyId__ = &confComputeKeyStoreRetrieveViaKeyId_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreRetrieveViaKeyId__ = &confComputeKeyStoreRetrieveViaKeyId_46f6a7;
    }

    // confComputeDeriveSecrets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeDeriveSecrets__ = &confComputeDeriveSecrets_GH100;
    }
    // default
    else
    {
        pThis->__confComputeDeriveSecrets__ = &confComputeDeriveSecrets_46f6a7;
    }

    // confComputeUpdateSecrets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeUpdateSecrets__ = &confComputeUpdateSecrets_GH100;
    }
    // default
    else
    {
        pThis->__confComputeUpdateSecrets__ = &confComputeUpdateSecrets_46f6a7;
    }

    // confComputeIsSpdmEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeIsSpdmEnabled__ = &confComputeIsSpdmEnabled_cbe027;
    }
    // default
    else
    {
        pThis->__confComputeIsSpdmEnabled__ = &confComputeIsSpdmEnabled_491d52;
    }

    // confComputeGetEngineIdFromKeySpace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeGetEngineIdFromKeySpace__ = &confComputeGetEngineIdFromKeySpace_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGetEngineIdFromKeySpace__ = &confComputeGetEngineIdFromKeySpace_78ac8b;
    }

    // confComputeGlobalKeyIsKernelPriv -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeGlobalKeyIsKernelPriv__ = &confComputeGlobalKeyIsKernelPriv_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGlobalKeyIsKernelPriv__ = &confComputeGlobalKeyIsKernelPriv_491d52;
    }

    // confComputeGlobalKeyIsUvmKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeGlobalKeyIsUvmKey__ = &confComputeGlobalKeyIsUvmKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGlobalKeyIsUvmKey__ = &confComputeGlobalKeyIsUvmKey_491d52;
    }

    // confComputeGetKeyPairByChannel -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeGetKeyPairByChannel__ = &confComputeGetKeyPairByChannel_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGetKeyPairByChannel__ = &confComputeGetKeyPairByChannel_46f6a7;
    }

    // confComputeTriggerKeyRotation -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_GH100;
        }
        // default
        else
        {
            pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_56cd7a;
        }
    }

    // confComputeGetKeyPairForKeySpace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeGetKeyPairForKeySpace__ = &confComputeGetKeyPairForKeySpace_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGetKeyPairForKeySpace__ = &confComputeGetKeyPairForKeySpace_b3696a;
    }

    // confComputeEnableKeyRotationCallback -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_GH100;
        }
        // default
        else
        {
            pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_56cd7a;
        }
    }

    // confComputeEnableKeyRotationSupport -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_GH100;
        }
        // default
        else
        {
            pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_56cd7a;
        }
    }

    // confComputeEnableInternalKeyRotationSupport -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_GH100;
        }
        // default
        else
        {
            pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_56cd7a;
        }
    }

    // confComputeIsDebugModeEnabled -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeIsDebugModeEnabled__ = &confComputeIsDebugModeEnabled_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__confComputeIsDebugModeEnabled__ = &confComputeIsDebugModeEnabled_cbe027;
    }
    // default
    else
    {
        pThis->__confComputeIsDebugModeEnabled__ = &confComputeIsDebugModeEnabled_491d52;
    }

    // confComputeIsGpuCcCapable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_cbe027;
    }
    // default
    else
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_491d52;
    }

    // confComputeEstablishSpdmSessionAndKeys -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeEstablishSpdmSessionAndKeys__ = &confComputeEstablishSpdmSessionAndKeys_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeEstablishSpdmSessionAndKeys__ = &confComputeEstablishSpdmSessionAndKeys_46f6a7;
    }

    // confComputeKeyStoreDepositIvMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreDepositIvMask__ = &confComputeKeyStoreDepositIvMask_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDepositIvMask__ = &confComputeKeyStoreDepositIvMask_b3696a;
    }

    // confComputeKeyStoreUpdateKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreUpdateKey__ = &confComputeKeyStoreUpdateKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreUpdateKey__ = &confComputeKeyStoreUpdateKey_46f6a7;
    }

    // confComputeKeyStoreIsValidGlobalKeyId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreIsValidGlobalKeyId__ = &confComputeKeyStoreIsValidGlobalKeyId_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreIsValidGlobalKeyId__ = &confComputeKeyStoreIsValidGlobalKeyId_491d52;
    }

    // confComputeKeyStoreInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreInit__ = &confComputeKeyStoreInit_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreInit__ = &confComputeKeyStoreInit_46f6a7;
    }

    // confComputeKeyStoreDeinit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreDeinit__ = &confComputeKeyStoreDeinit_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDeinit__ = &confComputeKeyStoreDeinit_b3696a;
    }

    // confComputeKeyStoreGetExportMasterKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreGetExportMasterKey__ = &confComputeKeyStoreGetExportMasterKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreGetExportMasterKey__ = &confComputeKeyStoreGetExportMasterKey_fa6e19;
    }

    // confComputeKeyStoreDeriveKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreDeriveKey__ = &confComputeKeyStoreDeriveKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDeriveKey__ = &confComputeKeyStoreDeriveKey_46f6a7;
    }

    // confComputeKeyStoreClearExportMasterKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__confComputeKeyStoreClearExportMasterKey__ = &confComputeKeyStoreClearExportMasterKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreClearExportMasterKey__ = &confComputeKeyStoreClearExportMasterKey_b3696a;
    }

    // confComputeInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeInitMissing;

    // confComputeStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePreInitUnlocked;

    // confComputeStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateInitUnlocked;

    // confComputeStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePreLoad;

    // confComputeStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateLoad;

    // confComputeStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateUnload;

    // confComputeStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePostUnload;

    // confComputeStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateDestroy;

    // confComputeIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__confComputeIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeIsPresent;
} // End __nvoc_init_funcTable_ConfidentialCompute_1 with approximately 81 basic block(s).


// Initialize vtable(s) for 41 virtual method(s).
void __nvoc_init_funcTable_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 41 per-object function pointer(s).
    __nvoc_init_funcTable_ConfidentialCompute_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ConfidentialCompute = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_ConfidentialCompute(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ConfidentialCompute(ConfidentialCompute **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ConfidentialCompute *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConfidentialCompute), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ConfidentialCompute));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ConfidentialCompute);

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

    __nvoc_init_ConfidentialCompute(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ConfidentialCompute(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_ConfidentialCompute_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConfidentialCompute_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConfidentialCompute));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ConfidentialCompute(ConfidentialCompute **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_ConfidentialCompute(ppThis, pParent, createFlags);

    return status;
}


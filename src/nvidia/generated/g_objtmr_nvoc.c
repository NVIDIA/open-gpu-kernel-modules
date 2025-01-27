#define NVOC_OBJTMR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_objtmr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9ddede = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTMR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJTMR(OBJTMR*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_dtor_OBJTMR(OBJTMR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTMR;

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_OBJTMR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTMR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJTMR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJTMR = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_OBJTMR_OBJTMR,
        &__nvoc_rtti_OBJTMR_IntrService,
        &__nvoc_rtti_OBJTMR_OBJENGSTATE,
        &__nvoc_rtti_OBJTMR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTMR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJTMR),
        /*classId=*/            classId(OBJTMR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJTMR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJTMR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJTMR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJTMR
};

// Down-thunk(s) to bridge OBJTMR methods from ancestors (if any)
void __nvoc_down_thunk_OBJTMR_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceRecord pRecords[177]);    // this
NvBool __nvoc_down_thunk_OBJTMR_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceClearInterruptArguments *pParams);    // this
NvU32 __nvoc_down_thunk_OBJTMR_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceServiceInterruptArguments *pParams);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg3);    // this
void __nvoc_down_thunk_OBJTMR_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pTmr);    // this

// 10 down-thunk(s) defined to bridge methods in OBJTMR from superclasses

// tmrRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_OBJTMR_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceRecord pRecords[177]) {
    tmrRegisterIntrService(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService)), pRecords);
}

// tmrClearInterrupt: virtual override (intrserv) base (intrserv)
NvBool __nvoc_down_thunk_OBJTMR_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceClearInterruptArguments *pParams) {
    return tmrClearInterrupt(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService)), pParams);
}

// tmrServiceInterrupt: virtual halified (3 hals) override (intrserv) base (intrserv) body
NvU32 __nvoc_down_thunk_OBJTMR_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceServiceInterruptArguments *pParams) {
    return tmrServiceInterrupt(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService)), pParams);
}

// tmrConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, ENGDESCRIPTOR arg3) {
    return tmrConstructEngine(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    return tmrStatePreInitLocked(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    return tmrStateInitLocked(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    return tmrStateInitUnlocked(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg3) {
    return tmrStateLoad(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJTMR_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg3) {
    return tmrStateUnload(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_OBJTMR_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    tmrStateDestroy(pGpu, (struct OBJTMR *)(((unsigned char *) pTmr) - NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge OBJTMR methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_tmrInitMissing(struct OBJGPU *pGpu, struct OBJTMR *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJTMR *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreLoad(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePostLoad(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreUnload(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePostUnload(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_tmrIsPresent(struct OBJGPU *pGpu, struct OBJTMR *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_IntrService_tmrServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJTMR *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

// 8 up-thunk(s) defined to bridge methods in OBJTMR to superclasses

// tmrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_tmrInitMissing(struct OBJGPU *pGpu, struct OBJTMR *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJTMR *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreLoad(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePostLoad(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePreUnload(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_tmrStatePostUnload(struct OBJGPU *pGpu, struct OBJTMR *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)), arg3);
}

// tmrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_tmrIsPresent(struct OBJGPU *pGpu, struct OBJTMR *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE)));
}

// tmrServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_tmrServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJTMR *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTMR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_OBJTMR(OBJTMR *pThis) {
    __nvoc_tmrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS, NV_TRUE);
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        // default
        {
            pThis->setProperty(pThis, PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS, NV_FALSE);
        }
    }
    pThis->setProperty(pThis, PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS, (0));
    pThis->setProperty(pThis, PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS, (0));

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_TMR_WAR_FOR_BUG_4679970_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_WAR_FOR_BUG_4679970_DEF, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_WAR_FOR_BUG_4679970_DEF, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJTMR_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_OBJTMR_fail_IntrService;
    __nvoc_init_dataField_OBJTMR(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJTMR_exit; // Success

__nvoc_ctor_OBJTMR_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_OBJTMR_fail_OBJENGSTATE:
__nvoc_ctor_OBJTMR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJTMR_1(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // tmrDelay -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__tmrDelay__ = &tmrDelay_PTIMER;
    }
    else
    {
        pThis->__tmrDelay__ = &tmrDelay_OSTIMER;
    }

    // tmrServiceInterrupt -- virtual halified (3 hals) override (intrserv) base (intrserv) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_TU102;
        }
        else
        {
            pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_GA100;
        }
    }
    else
    {
        pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_56cd7a;
    }

    // tmrSetCurrentTime -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__tmrSetCurrentTime__ = &tmrSetCurrentTime_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__tmrSetCurrentTime__ = &tmrSetCurrentTime_GH100;
        }
        else
        {
            pThis->__tmrSetCurrentTime__ = &tmrSetCurrentTime_GV100;
        }
    }

    // tmrGetTimeEx -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__tmrGetTimeEx__ = &tmrGetTimeEx_GM107;
    }
    else
    {
        pThis->__tmrGetTimeEx__ = &tmrGetTimeEx_GH100;
    }

    // tmrSetCountdownIntrDisable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__tmrSetCountdownIntrDisable__ = &tmrSetCountdownIntrDisable_GM200;
    }
    else
    {
        pThis->__tmrSetCountdownIntrDisable__ = &tmrSetCountdownIntrDisable_56cd7a;
    }

    // tmrSetCountdown -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__tmrSetCountdown__ = &tmrSetCountdown_TU102;
    }
    else
    {
        pThis->__tmrSetCountdown__ = &tmrSetCountdown_GH100;
    }

    // tmrGrTickFreqChange -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__tmrGrTickFreqChange__ = &tmrGrTickFreqChange_GM107;
    }
    else
    {
        pThis->__tmrGrTickFreqChange__ = &tmrGrTickFreqChange_46f6a7;
    }

    // tmrGetGpuPtimerOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__tmrGetGpuPtimerOffset__ = &tmrGetGpuPtimerOffset_TU102;
    }
    else
    {
        pThis->__tmrGetGpuPtimerOffset__ = &tmrGetGpuPtimerOffset_GA100;
    }
} // End __nvoc_init_funcTable_OBJTMR_1 with approximately 18 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__OBJTMR vtable = {
        .__tmrRegisterIntrService__ = &tmrRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_OBJTMR_intrservRegisterIntrService,    // virtual
        .__tmrClearInterrupt__ = &tmrClearInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &__nvoc_down_thunk_OBJTMR_intrservClearInterrupt,    // virtual
        .IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_OBJTMR_intrservServiceInterrupt,    // virtual
        .__tmrConstructEngine__ = &tmrConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_OBJTMR_engstateConstructEngine,    // virtual
        .__tmrStatePreInitLocked__ = &tmrStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_OBJTMR_engstateStatePreInitLocked,    // virtual
        .__tmrStateInitLocked__ = &tmrStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_OBJTMR_engstateStateInitLocked,    // virtual
        .__tmrStateInitUnlocked__ = &tmrStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_OBJTMR_engstateStateInitUnlocked,    // virtual
        .__tmrStateLoad__ = &tmrStateLoad_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_OBJTMR_engstateStateLoad,    // virtual
        .__tmrStateUnload__ = &tmrStateUnload_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_OBJTMR_engstateStateUnload,    // virtual
        .__tmrStateDestroy__ = &tmrStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_OBJTMR_engstateStateDestroy,    // virtual
        .__tmrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_tmrInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__tmrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_tmrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__tmrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_tmrStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__tmrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_tmrStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__tmrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_tmrStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__tmrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_tmrStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__tmrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_tmrIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
        .__tmrServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_tmrServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_vtable = &vtable;    // (tmr) this

    // Initialize vtable(s) with 8 per-object function pointer(s).
    __nvoc_init_funcTable_OBJTMR_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJTMR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_OBJTMR(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJTMR(OBJTMR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJTMR *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJTMR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJTMR));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJTMR);

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

    __nvoc_init_OBJTMR(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJTMR(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJTMR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJTMR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJTMR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJTMR(OBJTMR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJTMR(ppThis, pParent, createFlags);

    return status;
}


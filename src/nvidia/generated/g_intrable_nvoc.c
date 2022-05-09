#define NVOC_INTRABLE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_intrable_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x31ccb7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJINTRABLE;

void __nvoc_init_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
void __nvoc_dtor_OBJINTRABLE(OBJINTRABLE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJINTRABLE;

static const struct NVOC_RTTI __nvoc_rtti_OBJINTRABLE_OBJINTRABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJINTRABLE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJINTRABLE,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJINTRABLE = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_OBJINTRABLE_OBJINTRABLE,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJINTRABLE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJINTRABLE),
        /*classId=*/            classId(OBJINTRABLE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJINTRABLE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJINTRABLE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJINTRABLE
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJINTRABLE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJINTRABLE(OBJINTRABLE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJINTRABLE(OBJINTRABLE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_ctor_OBJINTRABLE(OBJINTRABLE *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_OBJINTRABLE(pThis, pRmhalspecowner);

    status = __nvoc_intrableConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJINTRABLE_fail__init;
    goto __nvoc_ctor_OBJINTRABLE_exit; // Success

__nvoc_ctor_OBJINTRABLE_fail__init:
__nvoc_ctor_OBJINTRABLE_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJINTRABLE_1(OBJINTRABLE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__intrableGetNotificationIntrVector__ = &intrableGetNotificationIntrVector_IMPL;

    pThis->__intrableGetPhysicalIntrVectors__ = &intrableGetPhysicalIntrVectors_IMPL;

    pThis->__intrableGetKernelIntrVectors__ = &intrableGetKernelIntrVectors_IMPL;

    pThis->__intrableSetNotificationIntrVector__ = &intrableSetNotificationIntrVector_IMPL;
}

void __nvoc_init_funcTable_OBJINTRABLE(OBJINTRABLE *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_OBJINTRABLE_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJINTRABLE(OBJINTRABLE *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJINTRABLE = pThis;
    __nvoc_init_funcTable_OBJINTRABLE(pThis, pRmhalspecowner);
}


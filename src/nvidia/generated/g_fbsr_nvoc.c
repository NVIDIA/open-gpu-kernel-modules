#define NVOC_FBSR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_fbsr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa30fe6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJFBSR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJFBSR(OBJFBSR*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJFBSR(OBJFBSR*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJFBSR(OBJFBSR*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJFBSR(OBJFBSR*, RmHalspecOwner* );
void __nvoc_dtor_OBJFBSR(OBJFBSR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJFBSR;

static const struct NVOC_RTTI __nvoc_rtti_OBJFBSR_OBJFBSR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJFBSR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJFBSR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJFBSR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJFBSR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJFBSR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJFBSR_OBJFBSR,
        &__nvoc_rtti_OBJFBSR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJFBSR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJFBSR),
        /*classId=*/            classId(OBJFBSR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJFBSR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJFBSR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJFBSR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJFBSR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJFBSR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJFBSR(OBJFBSR *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJFBSR_fail_Object;
    __nvoc_init_dataField_OBJFBSR(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJFBSR_exit; // Success

__nvoc_ctor_OBJFBSR_fail_Object:
__nvoc_ctor_OBJFBSR_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJFBSR_1(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal function -- fbsrBegin
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x100003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GH100 */ 
    {
        pThis->__fbsrBegin__ = &fbsrBegin_GM107;
    }
    else
    {
        pThis->__fbsrBegin__ = &fbsrBegin_GA100;
    }

    // Hal function -- fbsrEnd
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x100003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GH100 */ 
    {
        pThis->__fbsrEnd__ = &fbsrEnd_GM107;
    }
    else
    {
        pThis->__fbsrEnd__ = &fbsrEnd_GA100;
    }

    // Hal function -- fbsrSendMemsysProgramRawCompressionMode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__fbsrSendMemsysProgramRawCompressionMode__ = &fbsrSendMemsysProgramRawCompressionMode_GA100;
    }
    // default
    else
    {
        pThis->__fbsrSendMemsysProgramRawCompressionMode__ = &fbsrSendMemsysProgramRawCompressionMode_56cd7a;
    }
}

void __nvoc_init_funcTable_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_OBJFBSR_1(pThis, pRmhalspecowner);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJFBSR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJFBSR(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJFBSR(OBJFBSR **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJFBSR *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJFBSR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJFBSR));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJFBSR);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_OBJFBSR(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJFBSR(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJFBSR_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJFBSR_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJFBSR));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJFBSR(OBJFBSR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJFBSR(ppThis, pParent, createFlags);

    return status;
}


#define NVOC_SPDM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_spdm_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x261ee8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_Spdm(Spdm*, RmHalspecOwner* );
void __nvoc_init_funcTable_Spdm(Spdm*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Spdm(Spdm*, RmHalspecOwner* );
void __nvoc_init_dataField_Spdm(Spdm*, RmHalspecOwner* );
void __nvoc_dtor_Spdm(Spdm*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Spdm;

static const struct NVOC_RTTI __nvoc_rtti_Spdm_Spdm = {
    /*pClassDef=*/          &__nvoc_class_def_Spdm,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Spdm,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Spdm_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Spdm, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Spdm = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_Spdm_Spdm,
        &__nvoc_rtti_Spdm_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Spdm),
        /*classId=*/            classId(Spdm),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Spdm",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Spdm,
    /*pCastInfo=*/          &__nvoc_castinfo_Spdm,
    /*pExportInfo=*/        &__nvoc_export_info_Spdm
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_Spdm = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_Spdm(Spdm *pThis) {
    __nvoc_spdmDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_Spdm_fail_Object;
    __nvoc_init_dataField_Spdm(pThis, pRmhalspecowner);

    status = __nvoc_spdmConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_Spdm_fail__init;
    goto __nvoc_ctor_Spdm_exit; // Success

__nvoc_ctor_Spdm_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_Spdm_fail_Object:
__nvoc_ctor_Spdm_exit:

    return status;
}

static void __nvoc_init_funcTable_Spdm_1(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // Hal function -- spdmGetCertChains
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_46f6a7;
    }

    // Hal function -- spdmGetAttestationReport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_46f6a7;
    }

    // Hal function -- spdmDeviceInit
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_46f6a7;
    }

    // Hal function -- spdmDeviceDeinit
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_46f6a7;
    }

    // Hal function -- spdmDeviceSecuredSessionSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_46f6a7;
    }

    // Hal function -- spdmCheckConnection
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_GH100;
    }
    // default
    else
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_46f6a7;
    }

    // Hal function -- spdmMessageProcess
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_GH100;
    }
    // default
    else
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_46f6a7;
    }

    // Hal function -- spdmGetCertificates
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_46f6a7;
    }
}

void __nvoc_init_funcTable_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_Spdm_1(pThis, pRmhalspecowner);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Spdm = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_Spdm(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Spdm(Spdm **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    Spdm *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Spdm), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(Spdm));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Spdm);

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

    __nvoc_init_Spdm(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Spdm(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_Spdm_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Spdm_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Spdm));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Spdm(Spdm **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Spdm(ppThis, pParent, createFlags);

    return status;
}


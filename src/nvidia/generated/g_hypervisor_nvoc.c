#define NVOC_HYPERVISOR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_hypervisor_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x33c1ba = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_init_funcTable_OBJHYPERVISOR(OBJHYPERVISOR*);
NV_STATUS __nvoc_ctor_OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_init_dataField_OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_dtor_OBJHYPERVISOR(OBJHYPERVISOR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHYPERVISOR;

static const struct NVOC_RTTI __nvoc_rtti_OBJHYPERVISOR_OBJHYPERVISOR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJHYPERVISOR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHYPERVISOR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJHYPERVISOR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJHYPERVISOR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJHYPERVISOR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJHYPERVISOR_OBJHYPERVISOR,
        &__nvoc_rtti_OBJHYPERVISOR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJHYPERVISOR),
        /*classId=*/            classId(OBJHYPERVISOR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJHYPERVISOR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJHYPERVISOR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJHYPERVISOR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJHYPERVISOR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHYPERVISOR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    __nvoc_hypervisorDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJHYPERVISOR_fail_Object;
    __nvoc_init_dataField_OBJHYPERVISOR(pThis);

    status = __nvoc_hypervisorConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJHYPERVISOR_fail__init;
    goto __nvoc_ctor_OBJHYPERVISOR_exit; // Success

__nvoc_ctor_OBJHYPERVISOR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJHYPERVISOR_fail_Object:
__nvoc_ctor_OBJHYPERVISOR_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJHYPERVISOR_1(OBJHYPERVISOR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    __nvoc_init_funcTable_OBJHYPERVISOR_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    pThis->__nvoc_pbase_OBJHYPERVISOR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJHYPERVISOR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJHYPERVISOR(OBJHYPERVISOR **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJHYPERVISOR *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJHYPERVISOR));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJHYPERVISOR));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJHYPERVISOR);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJHYPERVISOR(pThis);
    status = __nvoc_ctor_OBJHYPERVISOR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJHYPERVISOR_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJHYPERVISOR_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJHYPERVISOR(OBJHYPERVISOR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJHYPERVISOR(ppThis, pParent, createFlags);

    return status;
}


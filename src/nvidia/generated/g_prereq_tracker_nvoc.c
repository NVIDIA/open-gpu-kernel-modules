#define NVOC_PREREQ_TRACKER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_prereq_tracker_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x0e171b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PrereqTracker;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_PrereqTracker(PrereqTracker*);
void __nvoc_init_funcTable_PrereqTracker(PrereqTracker*);
NV_STATUS __nvoc_ctor_PrereqTracker(PrereqTracker*, struct OBJGPU * arg_pParent);
void __nvoc_init_dataField_PrereqTracker(PrereqTracker*);
void __nvoc_dtor_PrereqTracker(PrereqTracker*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_PrereqTracker;

static const struct NVOC_RTTI __nvoc_rtti_PrereqTracker_PrereqTracker = {
    /*pClassDef=*/          &__nvoc_class_def_PrereqTracker,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PrereqTracker,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_PrereqTracker_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PrereqTracker, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_PrereqTracker = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_PrereqTracker_PrereqTracker,
        &__nvoc_rtti_PrereqTracker_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_PrereqTracker = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(PrereqTracker),
        /*classId=*/            classId(PrereqTracker),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "PrereqTracker",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_PrereqTracker,
    /*pCastInfo=*/          &__nvoc_castinfo_PrereqTracker,
    /*pExportInfo=*/        &__nvoc_export_info_PrereqTracker
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_PrereqTracker = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_PrereqTracker(PrereqTracker *pThis) {
    __nvoc_prereqDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_PrereqTracker(PrereqTracker *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_PrereqTracker(PrereqTracker *pThis, struct OBJGPU * arg_pParent) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_PrereqTracker_fail_Object;
    __nvoc_init_dataField_PrereqTracker(pThis);

    status = __nvoc_prereqConstruct(pThis, arg_pParent);
    if (status != NV_OK) goto __nvoc_ctor_PrereqTracker_fail__init;
    goto __nvoc_ctor_PrereqTracker_exit; // Success

__nvoc_ctor_PrereqTracker_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_PrereqTracker_fail_Object:
__nvoc_ctor_PrereqTracker_exit:

    return status;
}

static void __nvoc_init_funcTable_PrereqTracker_1(PrereqTracker *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_PrereqTracker(PrereqTracker *pThis) {
    __nvoc_init_funcTable_PrereqTracker_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_PrereqTracker(PrereqTracker *pThis) {
    pThis->__nvoc_pbase_PrereqTracker = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_PrereqTracker(pThis);
}

NV_STATUS __nvoc_objCreate_PrereqTracker(PrereqTracker **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pParent) {
    NV_STATUS status;
    Object *pParentObj;
    PrereqTracker *pThis;

    pThis = portMemAllocNonPaged(sizeof(PrereqTracker));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(PrereqTracker));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_PrereqTracker);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_PrereqTracker(pThis);
    status = __nvoc_ctor_PrereqTracker(pThis, arg_pParent);
    if (status != NV_OK) goto __nvoc_objCreate_PrereqTracker_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_PrereqTracker_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PrereqTracker(PrereqTracker **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pParent = va_arg(args, struct OBJGPU *);

    status = __nvoc_objCreate_PrereqTracker(ppThis, pParent, createFlags, arg_pParent);

    return status;
}


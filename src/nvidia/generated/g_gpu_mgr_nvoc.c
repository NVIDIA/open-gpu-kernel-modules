#define NVOC_GPU_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xcf1b25 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMGR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJGPUMGR(OBJGPUMGR*);
void __nvoc_init_funcTable_OBJGPUMGR(OBJGPUMGR*);
NV_STATUS __nvoc_ctor_OBJGPUMGR(OBJGPUMGR*);
void __nvoc_init_dataField_OBJGPUMGR(OBJGPUMGR*);
void __nvoc_dtor_OBJGPUMGR(OBJGPUMGR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMGR;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMGR_OBJGPUMGR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPUMGR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUMGR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMGR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUMGR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPUMGR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPUMGR_OBJGPUMGR,
        &__nvoc_rtti_OBJGPUMGR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMGR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPUMGR),
        /*classId=*/            classId(OBJGPUMGR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPUMGR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUMGR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPUMGR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPUMGR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMGR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGPUMGR(OBJGPUMGR *pThis) {
    __nvoc_gpumgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPUMGR(OBJGPUMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJGPUMGR(OBJGPUMGR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMGR_fail_Object;
    __nvoc_init_dataField_OBJGPUMGR(pThis);

    status = __nvoc_gpumgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMGR_fail__init;
    goto __nvoc_ctor_OBJGPUMGR_exit; // Success

__nvoc_ctor_OBJGPUMGR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGPUMGR_fail_Object:
__nvoc_ctor_OBJGPUMGR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGPUMGR_1(OBJGPUMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGPUMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGPUMGR(OBJGPUMGR *pThis) {
    __nvoc_init_funcTable_OBJGPUMGR_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJGPUMGR(OBJGPUMGR *pThis) {
    pThis->__nvoc_pbase_OBJGPUMGR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJGPUMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPUMGR(OBJGPUMGR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGPUMGR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGPUMGR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGPUMGR));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPUMGR);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJGPUMGR(pThis);
    status = __nvoc_ctor_OBJGPUMGR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPUMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGPUMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGPUMGR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMGR(OBJGPUMGR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGPUMGR(ppThis, pParent, createFlags);

    return status;
}


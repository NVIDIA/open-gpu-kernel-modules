#define NVOC_HAL_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_hal_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xbf26de = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHALMGR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJHALMGR(OBJHALMGR*);
void __nvoc_init_funcTable_OBJHALMGR(OBJHALMGR*);
NV_STATUS __nvoc_ctor_OBJHALMGR(OBJHALMGR*);
void __nvoc_init_dataField_OBJHALMGR(OBJHALMGR*);
void __nvoc_dtor_OBJHALMGR(OBJHALMGR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHALMGR;

static const struct NVOC_RTTI __nvoc_rtti_OBJHALMGR_OBJHALMGR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJHALMGR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHALMGR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJHALMGR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJHALMGR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJHALMGR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJHALMGR_OBJHALMGR,
        &__nvoc_rtti_OBJHALMGR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHALMGR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJHALMGR),
        /*classId=*/            classId(OBJHALMGR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJHALMGR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJHALMGR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJHALMGR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJHALMGR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJHALMGR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJHALMGR(OBJHALMGR *pThis) {
    __nvoc_halmgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJHALMGR(OBJHALMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJHALMGR(OBJHALMGR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJHALMGR_fail_Object;
    __nvoc_init_dataField_OBJHALMGR(pThis);

    status = __nvoc_halmgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJHALMGR_fail__init;
    goto __nvoc_ctor_OBJHALMGR_exit; // Success

__nvoc_ctor_OBJHALMGR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJHALMGR_fail_Object:
__nvoc_ctor_OBJHALMGR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJHALMGR_1(OBJHALMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJHALMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJHALMGR(OBJHALMGR *pThis) {
    __nvoc_init_funcTable_OBJHALMGR_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJHALMGR(OBJHALMGR *pThis) {
    pThis->__nvoc_pbase_OBJHALMGR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJHALMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJHALMGR(OBJHALMGR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJHALMGR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJHALMGR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJHALMGR));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJHALMGR);

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

    __nvoc_init_OBJHALMGR(pThis);
    status = __nvoc_ctor_OBJHALMGR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJHALMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJHALMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJHALMGR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJHALMGR(OBJHALMGR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJHALMGR(ppThis, pParent, createFlags);

    return status;
}


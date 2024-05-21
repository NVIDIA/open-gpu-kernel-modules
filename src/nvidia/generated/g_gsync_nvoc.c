#define NVOC_GSYNC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gsync_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd07fd0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR*);
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGSYNCMGR;

static const struct NVOC_RTTI __nvoc_rtti_OBJGSYNCMGR_OBJGSYNCMGR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGSYNCMGR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGSYNCMGR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGSYNCMGR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGSYNCMGR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGSYNCMGR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGSYNCMGR_OBJGSYNCMGR,
        &__nvoc_rtti_OBJGSYNCMGR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGSYNCMGR),
        /*classId=*/            classId(OBJGSYNCMGR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGSYNCMGR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGSYNCMGR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGSYNCMGR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGSYNCMGR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGSYNCMGR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    __nvoc_gsyncmgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail_Object;
    __nvoc_init_dataField_OBJGSYNCMGR(pThis);

    status = __nvoc_gsyncmgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail__init;
    goto __nvoc_ctor_OBJGSYNCMGR_exit; // Success

__nvoc_ctor_OBJGSYNCMGR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGSYNCMGR_fail_Object:
__nvoc_ctor_OBJGSYNCMGR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGSYNCMGR_1(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGSYNCMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    __nvoc_init_funcTable_OBJGSYNCMGR_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    pThis->__nvoc_pbase_OBJGSYNCMGR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJGSYNCMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGSYNCMGR(OBJGSYNCMGR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGSYNCMGR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGSYNCMGR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGSYNCMGR));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGSYNCMGR);

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

    __nvoc_init_OBJGSYNCMGR(pThis);
    status = __nvoc_ctor_OBJGSYNCMGR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGSYNCMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGSYNCMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGSYNCMGR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGSYNCMGR(OBJGSYNCMGR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGSYNCMGR(ppThis, pParent, createFlags);

    return status;
}


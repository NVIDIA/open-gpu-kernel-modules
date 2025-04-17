#define NVOC_HAL_MGR_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_hal_mgr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xbf26de = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHALMGR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJHALMGR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJHALMGR(OBJHALMGR*);
void __nvoc_init_funcTable_OBJHALMGR(OBJHALMGR*);
NV_STATUS __nvoc_ctor_OBJHALMGR(OBJHALMGR*);
void __nvoc_init_dataField_OBJHALMGR(OBJHALMGR*);
void __nvoc_dtor_OBJHALMGR(OBJHALMGR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJHALMGR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHALMGR;

// Down-thunk(s) to bridge OBJHALMGR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJHALMGR methods to ancestors (if any)

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
    /*pCastInfo=*/          &__nvoc_castinfo__OBJHALMGR,
    /*pExportInfo=*/        &__nvoc_export_info__OBJHALMGR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJHALMGR __nvoc_metadata__OBJHALMGR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJHALMGR,    // (halmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHALMGR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJHALMGR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJHALMGR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJHALMGR.rtti,    // [0]: (halmgr) this
        &__nvoc_metadata__OBJHALMGR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHALMGR = 
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

// Initialize newly constructed object.
void __nvoc_init__OBJHALMGR(OBJHALMGR *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJHALMGR = pThis;    // (halmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJHALMGR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJHALMGR;    // (halmgr) this

    // Initialize per-object vtables.
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

    __nvoc_init__OBJHALMGR(pThis);
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


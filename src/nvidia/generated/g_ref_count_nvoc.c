#define NVOC_REF_COUNT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_ref_count_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xf89281 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJREFCNT;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJREFCNT
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJREFCNT(OBJREFCNT*);
void __nvoc_init_funcTable_OBJREFCNT(OBJREFCNT*);
NV_STATUS __nvoc_ctor_OBJREFCNT(OBJREFCNT*, Dynamic *arg_pParent, NvU32 arg_tag, RefcntStateChangeCallback *arg_pStateChangeCallback, RefcntResetCallback *arg_pResetCallback);
void __nvoc_init_dataField_OBJREFCNT(OBJREFCNT*);
void __nvoc_dtor_OBJREFCNT(OBJREFCNT*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJREFCNT;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJREFCNT;

// Down-thunk(s) to bridge OBJREFCNT methods from ancestors (if any)

// Up-thunk(s) to bridge OBJREFCNT methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJREFCNT = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJREFCNT),
        /*classId=*/            classId(OBJREFCNT),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJREFCNT",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJREFCNT,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJREFCNT,
    /*pExportInfo=*/        &__nvoc_export_info__OBJREFCNT
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJREFCNT __nvoc_metadata__OBJREFCNT = {
    .rtti.pClassDef = &__nvoc_class_def_OBJREFCNT,    // (refcnt) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJREFCNT,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJREFCNT, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJREFCNT = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJREFCNT.rtti,    // [0]: (refcnt) this
        &__nvoc_metadata__OBJREFCNT.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJREFCNT = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJREFCNT(OBJREFCNT *pThis) {
    __nvoc_refcntDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJREFCNT(OBJREFCNT *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJREFCNT(OBJREFCNT *pThis, Dynamic * arg_pParent, NvU32 arg_tag, RefcntStateChangeCallback * arg_pStateChangeCallback, RefcntResetCallback * arg_pResetCallback) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJREFCNT_fail_Object;
    __nvoc_init_dataField_OBJREFCNT(pThis);

    status = __nvoc_refcntConstruct(pThis, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback);
    if (status != NV_OK) goto __nvoc_ctor_OBJREFCNT_fail__init;
    goto __nvoc_ctor_OBJREFCNT_exit; // Success

__nvoc_ctor_OBJREFCNT_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJREFCNT_fail_Object:
__nvoc_ctor_OBJREFCNT_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJREFCNT_1(OBJREFCNT *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJREFCNT_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJREFCNT(OBJREFCNT *pThis) {
    __nvoc_init_funcTable_OBJREFCNT_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJREFCNT(OBJREFCNT *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJREFCNT = pThis;    // (refcnt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJREFCNT.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJREFCNT;    // (refcnt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJREFCNT(pThis);
}

NV_STATUS __nvoc_objCreate_OBJREFCNT(OBJREFCNT **ppThis, Dynamic *pParent, NvU32 createFlags, Dynamic * arg_pParent, NvU32 arg_tag, RefcntStateChangeCallback * arg_pStateChangeCallback, RefcntResetCallback * arg_pResetCallback)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJREFCNT *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJREFCNT), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJREFCNT));

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

    __nvoc_init__OBJREFCNT(pThis);
    status = __nvoc_ctor_OBJREFCNT(pThis, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback);
    if (status != NV_OK) goto __nvoc_objCreate_OBJREFCNT_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJREFCNT_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJREFCNT));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJREFCNT(OBJREFCNT **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    Dynamic * arg_pParent = va_arg(args, Dynamic *);
    NvU32 arg_tag = va_arg(args, NvU32);
    RefcntStateChangeCallback * arg_pStateChangeCallback = va_arg(args, RefcntStateChangeCallback *);
    RefcntResetCallback * arg_pResetCallback = va_arg(args, RefcntResetCallback *);

    status = __nvoc_objCreate_OBJREFCNT(ppThis, pParent, createFlags, arg_pParent, arg_tag, arg_pStateChangeCallback, arg_pResetCallback);

    return status;
}


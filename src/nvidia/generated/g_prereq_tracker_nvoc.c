#define NVOC_PREREQ_TRACKER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_prereq_tracker_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x0e171b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PrereqTracker;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for PrereqTracker
void __nvoc_init__Object(Object*);
void __nvoc_init__PrereqTracker(PrereqTracker*);
void __nvoc_init_funcTable_PrereqTracker(PrereqTracker*);
NV_STATUS __nvoc_ctor_PrereqTracker(PrereqTracker*, struct OBJGPU *arg_pParent);
void __nvoc_init_dataField_PrereqTracker(PrereqTracker*);
void __nvoc_dtor_PrereqTracker(PrereqTracker*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PrereqTracker;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PrereqTracker;

// Down-thunk(s) to bridge PrereqTracker methods from ancestors (if any)

// Up-thunk(s) to bridge PrereqTracker methods to ancestors (if any)

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
    /*pCastInfo=*/          &__nvoc_castinfo__PrereqTracker,
    /*pExportInfo=*/        &__nvoc_export_info__PrereqTracker
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PrereqTracker __nvoc_metadata__PrereqTracker = {
    .rtti.pClassDef = &__nvoc_class_def_PrereqTracker,    // (prereq) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PrereqTracker,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(PrereqTracker, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PrereqTracker = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__PrereqTracker.rtti,    // [0]: (prereq) this
        &__nvoc_metadata__PrereqTracker.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PrereqTracker = 
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

// Vtable initialization
static void __nvoc_init_funcTable_PrereqTracker_1(PrereqTracker *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PrereqTracker_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PrereqTracker(PrereqTracker *pThis) {
    __nvoc_init_funcTable_PrereqTracker_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PrereqTracker(PrereqTracker *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_PrereqTracker = pThis;    // (prereq) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PrereqTracker.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PrereqTracker;    // (prereq) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PrereqTracker(pThis);
}

NV_STATUS __nvoc_objCreate_PrereqTracker(PrereqTracker **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pParent)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    PrereqTracker *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PrereqTracker), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(PrereqTracker));

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

    __nvoc_init__PrereqTracker(pThis);
    status = __nvoc_ctor_PrereqTracker(pThis, arg_pParent);
    if (status != NV_OK) goto __nvoc_objCreate_PrereqTracker_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PrereqTracker_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PrereqTracker));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PrereqTracker(PrereqTracker **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pParent = va_arg(args, struct OBJGPU *);

    status = __nvoc_objCreate_PrereqTracker(ppThis, pParent, createFlags, arg_pParent);

    return status;
}


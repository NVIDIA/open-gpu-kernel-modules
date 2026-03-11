#define NVOC_HEAP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_heap_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__556e9a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Heap;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for Heap
void __nvoc_init__Object(Object*);
void __nvoc_init__Heap(Heap*);
void __nvoc_init_funcTable_Heap(Heap*);
NV_STATUS __nvoc_ctor_Heap(Heap*);
void __nvoc_init_dataField_Heap(Heap*);
void __nvoc_dtor_Heap(Heap*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Heap;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Heap;

// Down-thunk(s) to bridge Heap methods from ancestors (if any)

// Up-thunk(s) to bridge Heap methods to ancestors (if any)

// Class-specific details for Heap
const struct NVOC_CLASS_DEF __nvoc_class_def_Heap = 
{
    .classInfo.size =               sizeof(Heap),
    .classInfo.classId =            classId(Heap),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Heap",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Heap,
    .pCastInfo =          &__nvoc_castinfo__Heap,
    .pExportInfo =        &__nvoc_export_info__Heap
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__Heap __nvoc_metadata__Heap = {
    .rtti.pClassDef = &__nvoc_class_def_Heap,    // (heap) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Heap,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(Heap, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Heap = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__Heap.rtti,    // [0]: (heap) this
        &__nvoc_metadata__Heap.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Heap = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Heap object.
void __nvoc_heapDestruct(Heap*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_Heap(Heap* pThis) {

// Call destructor.
    __nvoc_heapDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_Heap(Heap *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct Heap object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_Heap(Heap *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_Heap_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_Heap(pThis);
    goto __nvoc_ctor_Heap_exit; // Success

    // Unwind on error.
__nvoc_ctor_Heap_fail_Object:
__nvoc_ctor_Heap_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Heap_1(Heap *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Heap_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Heap(Heap *pThis) {
    __nvoc_init_funcTable_Heap_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Heap(Heap *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_Heap = pThis;    // (heap) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Heap.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Heap;    // (heap) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Heap(pThis);
}

NV_STATUS __nvoc_objCreate_Heap(Heap **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Heap *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Heap));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Heap));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Heap(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_Heap(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Heap_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Heap_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Heap));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Heap(Heap **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_Heap(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}


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
char __nvoc_class_id_uniqueness_check__0x556e9a = 1;
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

const struct NVOC_CLASS_DEF __nvoc_class_def_Heap = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Heap),
        /*classId=*/            classId(Heap),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Heap",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Heap,
    /*pCastInfo=*/          &__nvoc_castinfo__Heap,
    /*pExportInfo=*/        &__nvoc_export_info__Heap
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_Heap(Heap *pThis) {
    __nvoc_heapDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Heap(Heap *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_Heap(Heap *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_Heap_fail_Object;
    __nvoc_init_dataField_Heap(pThis);
    goto __nvoc_ctor_Heap_exit; // Success

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

NV_STATUS __nvoc_objCreate_Heap(Heap **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Heap *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Heap), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Heap));

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

    __nvoc_init__Heap(pThis);
    status = __nvoc_ctor_Heap(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_Heap_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Heap_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Heap));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Heap(Heap **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Heap(ppThis, pParent, createFlags);

    return status;
}


#define NVOC_OBJECT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_object_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x497031 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for Object
void __nvoc_init__Object(Object*);
void __nvoc_init_funcTable_Object(Object*);
NV_STATUS __nvoc_ctor_Object(Object*);
void __nvoc_init_dataField_Object(Object*);
void __nvoc_dtor_Object(Object*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Object;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Object;

// Down-thunk(s) to bridge Object methods from ancestors (if any)

// Up-thunk(s) to bridge Object methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_Object = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Object),
        /*classId=*/            classId(Object),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Object",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Object,
    /*pCastInfo=*/          &__nvoc_castinfo__Object,
    /*pExportInfo=*/        &__nvoc_export_info__Object
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__Object __nvoc_metadata__Object = {
    .rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Object,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Object = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__Object.rtti,    // [0]: (obj) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Object = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Object(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_Object(pThis);
    goto __nvoc_ctor_Object_exit; // Success

__nvoc_ctor_Object_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Object_1(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Object_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Object(Object *pThis) {
    __nvoc_init_funcTable_Object_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Object(Object *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = pThis;    // (obj) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Object;    // (obj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Object(pThis);
}

NV_STATUS __nvoc_objCreate_Object(Object **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Object *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Object), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Object));

    pThis->createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, pThis);
    }
    else
    {
        pThis->pParent = NULL;
    }

    __nvoc_init__Object(pThis);
    status = __nvoc_ctor_Object(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_Object_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Object_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, pThis);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Object));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Object(Object **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Object(ppThis, pParent, createFlags);

    return status;
}


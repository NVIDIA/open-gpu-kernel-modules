#define NVOC_GPU_DB_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_db_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xcdd250 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDb;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for GpuDb
void __nvoc_init__Object(Object*);
void __nvoc_init__GpuDb(GpuDb*);
void __nvoc_init_funcTable_GpuDb(GpuDb*);
NV_STATUS __nvoc_ctor_GpuDb(GpuDb*);
void __nvoc_init_dataField_GpuDb(GpuDb*);
void __nvoc_dtor_GpuDb(GpuDb*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuDb;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuDb;

// Down-thunk(s) to bridge GpuDb methods from ancestors (if any)

// Up-thunk(s) to bridge GpuDb methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDb = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuDb),
        /*classId=*/            classId(GpuDb),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuDb",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuDb,
    /*pCastInfo=*/          &__nvoc_castinfo__GpuDb,
    /*pExportInfo=*/        &__nvoc_export_info__GpuDb
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuDb __nvoc_metadata__GpuDb = {
    .rtti.pClassDef = &__nvoc_class_def_GpuDb,    // (gpudb) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuDb,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(GpuDb, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuDb = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__GpuDb.rtti,    // [0]: (gpudb) this
        &__nvoc_metadata__GpuDb.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuDb = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GpuDb(GpuDb *pThis) {
    __nvoc_gpudbDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuDb(GpuDb *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_GpuDb(GpuDb *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GpuDb_fail_Object;
    __nvoc_init_dataField_GpuDb(pThis);

    status = __nvoc_gpudbConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_GpuDb_fail__init;
    goto __nvoc_ctor_GpuDb_exit; // Success

__nvoc_ctor_GpuDb_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_GpuDb_fail_Object:
__nvoc_ctor_GpuDb_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuDb_1(GpuDb *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuDb_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuDb(GpuDb *pThis) {
    __nvoc_init_funcTable_GpuDb_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuDb(GpuDb *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_GpuDb = pThis;    // (gpudb) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDb.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuDb;    // (gpudb) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuDb(pThis);
}

NV_STATUS __nvoc_objCreate_GpuDb(GpuDb **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GpuDb *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GpuDb), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GpuDb));

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

    __nvoc_init__GpuDb(pThis);
    status = __nvoc_ctor_GpuDb(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_GpuDb_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GpuDb_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GpuDb));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuDb(GpuDb **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_GpuDb(ppThis, pParent, createFlags);

    return status;
}


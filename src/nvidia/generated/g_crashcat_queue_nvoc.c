#define NVOC_CRASHCAT_QUEUE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_crashcat_queue_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xbaa900 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatQueue;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CrashCatQueue
void __nvoc_init__Object(Object*);
void __nvoc_init__CrashCatQueue(CrashCatQueue*, CrashCatWayfinder *pCrashcatWayfinder);
void __nvoc_init_funcTable_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder *pCrashcatWayfinder);
NV_STATUS __nvoc_ctor_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder *pCrashcatWayfinder, CrashCatQueueConfig *arg_pQueueConfig);
void __nvoc_init_dataField_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder *pCrashcatWayfinder);
void __nvoc_dtor_CrashCatQueue(CrashCatQueue*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatQueue;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatQueue;

// Down-thunk(s) to bridge CrashCatQueue methods from ancestors (if any)

// Up-thunk(s) to bridge CrashCatQueue methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatQueue = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CrashCatQueue),
        /*classId=*/            classId(CrashCatQueue),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CrashCatQueue",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CrashCatQueue,
    /*pCastInfo=*/          &__nvoc_castinfo__CrashCatQueue,
    /*pExportInfo=*/        &__nvoc_export_info__CrashCatQueue
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CrashCatQueue __nvoc_metadata__CrashCatQueue = {
    .rtti.pClassDef = &__nvoc_class_def_CrashCatQueue,    // (crashcatQueue) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatQueue,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(CrashCatQueue, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatQueue = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__CrashCatQueue.rtti,    // [0]: (crashcatQueue) this
        &__nvoc_metadata__CrashCatQueue.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatQueue = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CrashCatQueue(CrashCatQueue *pThis) {
    __nvoc_crashcatQueueDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CrashCatQueue(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder) {
    CrashCatWayfinderHal *wayfinderHal = &pCrashcatWayfinder->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pCrashcatWayfinder);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_CrashCatQueue(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder, CrashCatQueueConfig * arg_pQueueConfig) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatQueue_fail_Object;
    __nvoc_init_dataField_CrashCatQueue(pThis, pCrashcatWayfinder);

    status = __nvoc_crashcatQueueConstruct(pThis, arg_pQueueConfig);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatQueue_fail__init;
    goto __nvoc_ctor_CrashCatQueue_exit; // Success

__nvoc_ctor_CrashCatQueue_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_CrashCatQueue_fail_Object:
__nvoc_ctor_CrashCatQueue_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CrashCatQueue_1(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder) {
    CrashCatWayfinderHal *wayfinderHal = &pCrashcatWayfinder->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pCrashcatWayfinder);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
} // End __nvoc_init_funcTable_CrashCatQueue_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CrashCatQueue(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder) {
    __nvoc_init_funcTable_CrashCatQueue_1(pThis, pCrashcatWayfinder);
}

// Initialize newly constructed object.
void __nvoc_init__CrashCatQueue(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_CrashCatQueue = pThis;    // (crashcatQueue) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatQueue.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatQueue;    // (crashcatQueue) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CrashCatQueue(pThis, pCrashcatWayfinder);
}

NV_STATUS __nvoc_objCreate_CrashCatQueue(CrashCatQueue **ppThis, Dynamic *pParent, NvU32 createFlags, CrashCatQueueConfig * arg_pQueueConfig)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    CrashCatQueue *pThis;
    CrashCatWayfinder *pCrashcatWayfinder;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CrashCatQueue), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(CrashCatQueue));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    if ((pCrashcatWayfinder = dynamicCast(pParent, CrashCatWayfinder)) == NULL)
        pCrashcatWayfinder = objFindAncestorOfType(CrashCatWayfinder, pParent);
    NV_ASSERT_OR_RETURN(pCrashcatWayfinder != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__CrashCatQueue(pThis, pCrashcatWayfinder);
    status = __nvoc_ctor_CrashCatQueue(pThis, pCrashcatWayfinder, arg_pQueueConfig);
    if (status != NV_OK) goto __nvoc_objCreate_CrashCatQueue_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CrashCatQueue_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CrashCatQueue));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CrashCatQueue(CrashCatQueue **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CrashCatQueueConfig * arg_pQueueConfig = va_arg(args, CrashCatQueueConfig *);

    status = __nvoc_objCreate_CrashCatQueue(ppThis, pParent, createFlags, arg_pQueueConfig);

    return status;
}


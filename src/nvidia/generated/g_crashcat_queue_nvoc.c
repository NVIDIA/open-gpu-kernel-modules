#define NVOC_CRASHCAT_QUEUE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_crashcat_queue_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xbaa900 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatQueue;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder* );
void __nvoc_init_funcTable_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder* );
NV_STATUS __nvoc_ctor_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder* , CrashCatQueueConfig * arg_pQueueConfig);
void __nvoc_init_dataField_CrashCatQueue(CrashCatQueue*, CrashCatWayfinder* );
void __nvoc_dtor_CrashCatQueue(CrashCatQueue*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatQueue;

static const struct NVOC_RTTI __nvoc_rtti_CrashCatQueue_CrashCatQueue = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatQueue,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatQueue,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_CrashCatQueue_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CrashCatQueue, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CrashCatQueue = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_CrashCatQueue_CrashCatQueue,
        &__nvoc_rtti_CrashCatQueue_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_CrashCatQueue,
    /*pExportInfo=*/        &__nvoc_export_info_CrashCatQueue
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatQueue = 
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

void __nvoc_init_Object(Object*);
void __nvoc_init_CrashCatQueue(CrashCatQueue *pThis, CrashCatWayfinder *pCrashcatWayfinder) {
    pThis->__nvoc_pbase_CrashCatQueue = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_CrashCatQueue);

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

    __nvoc_init_CrashCatQueue(pThis, pCrashcatWayfinder);
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


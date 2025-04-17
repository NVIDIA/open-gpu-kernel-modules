#define NVOC_PLATFORM_REQUEST_HANDLER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_platform_request_handler_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x641a7f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PlatformRequestHandler;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for PlatformRequestHandler
void __nvoc_init__Object(Object*);
void __nvoc_init__PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_init_funcTable_PlatformRequestHandler(PlatformRequestHandler*);
NV_STATUS __nvoc_ctor_PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_init_dataField_PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_dtor_PlatformRequestHandler(PlatformRequestHandler*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PlatformRequestHandler;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PlatformRequestHandler;

// Down-thunk(s) to bridge PlatformRequestHandler methods from ancestors (if any)

// Up-thunk(s) to bridge PlatformRequestHandler methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_PlatformRequestHandler = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(PlatformRequestHandler),
        /*classId=*/            classId(PlatformRequestHandler),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "PlatformRequestHandler",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_PlatformRequestHandler,
    /*pCastInfo=*/          &__nvoc_castinfo__PlatformRequestHandler,
    /*pExportInfo=*/        &__nvoc_export_info__PlatformRequestHandler
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PlatformRequestHandler __nvoc_metadata__PlatformRequestHandler = {
    .rtti.pClassDef = &__nvoc_class_def_PlatformRequestHandler,    // (pfmreqhndlr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PlatformRequestHandler,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(PlatformRequestHandler, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PlatformRequestHandler = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__PlatformRequestHandler.rtti,    // [0]: (pfmreqhndlr) this
        &__nvoc_metadata__PlatformRequestHandler.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PlatformRequestHandler = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_PlatformRequestHandler(PlatformRequestHandler *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_PlatformRequestHandler(PlatformRequestHandler *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_PlatformRequestHandler(PlatformRequestHandler *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_PlatformRequestHandler_fail_Object;
    __nvoc_init_dataField_PlatformRequestHandler(pThis);

    status = __nvoc_pfmreqhndlrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_PlatformRequestHandler_fail__init;
    goto __nvoc_ctor_PlatformRequestHandler_exit; // Success

__nvoc_ctor_PlatformRequestHandler_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_PlatformRequestHandler_fail_Object:
__nvoc_ctor_PlatformRequestHandler_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PlatformRequestHandler_1(PlatformRequestHandler *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PlatformRequestHandler_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PlatformRequestHandler(PlatformRequestHandler *pThis) {
    __nvoc_init_funcTable_PlatformRequestHandler_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PlatformRequestHandler(PlatformRequestHandler *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_PlatformRequestHandler = pThis;    // (pfmreqhndlr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PlatformRequestHandler.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PlatformRequestHandler;    // (pfmreqhndlr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PlatformRequestHandler(pThis);
}

NV_STATUS __nvoc_objCreate_PlatformRequestHandler(PlatformRequestHandler **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    PlatformRequestHandler *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PlatformRequestHandler), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(PlatformRequestHandler));

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

    __nvoc_init__PlatformRequestHandler(pThis);
    status = __nvoc_ctor_PlatformRequestHandler(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_PlatformRequestHandler_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PlatformRequestHandler_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PlatformRequestHandler));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PlatformRequestHandler(PlatformRequestHandler **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_PlatformRequestHandler(ppThis, pParent, createFlags);

    return status;
}


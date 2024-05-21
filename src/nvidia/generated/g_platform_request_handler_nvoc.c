#define NVOC_PLATFORM_REQUEST_HANDLER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_platform_request_handler_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x641a7f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PlatformRequestHandler;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_init_funcTable_PlatformRequestHandler(PlatformRequestHandler*);
NV_STATUS __nvoc_ctor_PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_init_dataField_PlatformRequestHandler(PlatformRequestHandler*);
void __nvoc_dtor_PlatformRequestHandler(PlatformRequestHandler*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_PlatformRequestHandler;

static const struct NVOC_RTTI __nvoc_rtti_PlatformRequestHandler_PlatformRequestHandler = {
    /*pClassDef=*/          &__nvoc_class_def_PlatformRequestHandler,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PlatformRequestHandler,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_PlatformRequestHandler_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PlatformRequestHandler, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_PlatformRequestHandler = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_PlatformRequestHandler_PlatformRequestHandler,
        &__nvoc_rtti_PlatformRequestHandler_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_PlatformRequestHandler,
    /*pExportInfo=*/        &__nvoc_export_info_PlatformRequestHandler
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_PlatformRequestHandler = 
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

void __nvoc_init_Object(Object*);
void __nvoc_init_PlatformRequestHandler(PlatformRequestHandler *pThis) {
    pThis->__nvoc_pbase_PlatformRequestHandler = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_PlatformRequestHandler);

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

    __nvoc_init_PlatformRequestHandler(pThis);
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


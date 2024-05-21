#define NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_rs_server_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x830542 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_funcTable_RsShared(RsShared*);
NV_STATUS __nvoc_ctor_RsShared(RsShared*);
void __nvoc_init_dataField_RsShared(RsShared*);
void __nvoc_dtor_RsShared(RsShared*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RsShared;

static const struct NVOC_RTTI __nvoc_rtti_RsShared_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsShared,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RsShared_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsShared, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RsShared = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_RsShared_RsShared,
        &__nvoc_rtti_RsShared_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsShared),
        /*classId=*/            classId(RsShared),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsShared",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsShared,
    /*pCastInfo=*/          &__nvoc_castinfo_RsShared,
    /*pExportInfo=*/        &__nvoc_export_info_RsShared
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RsShared = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsShared(RsShared *pThis) {
    __nvoc_shrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsShared(RsShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RsShared(RsShared *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail_Object;
    __nvoc_init_dataField_RsShared(pThis);

    status = __nvoc_shrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail__init;
    goto __nvoc_ctor_RsShared_exit; // Success

__nvoc_ctor_RsShared_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_RsShared_fail_Object:
__nvoc_ctor_RsShared_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsShared_1(RsShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsShared_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RsShared(RsShared *pThis) {
    __nvoc_init_funcTable_RsShared_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RsShared(RsShared *pThis) {
    pThis->__nvoc_pbase_RsShared = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_RsShared(pThis);
}

NV_STATUS __nvoc_objCreate_RsShared(RsShared **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsShared *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsShared), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsShared));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsShared);

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

    __nvoc_init_RsShared(pThis);
    status = __nvoc_ctor_RsShared(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_RsShared_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsShared_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsShared));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsShared(RsShared **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_RsShared(ppThis, pParent, createFlags);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x830d90 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_RsSession(RsSession*);
void __nvoc_init_funcTable_RsSession(RsSession*);
NV_STATUS __nvoc_ctor_RsSession(RsSession*);
void __nvoc_init_dataField_RsSession(RsSession*);
void __nvoc_dtor_RsSession(RsSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RsSession;

static const struct NVOC_RTTI __nvoc_rtti_RsSession_RsSession = {
    /*pClassDef=*/          &__nvoc_class_def_RsSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RsSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsSession, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RsSession_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsSession, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RsSession = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_RsSession_RsSession,
        &__nvoc_rtti_RsSession_RsShared,
        &__nvoc_rtti_RsSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsSession),
        /*classId=*/            classId(RsSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsSession,
    /*pCastInfo=*/          &__nvoc_castinfo_RsSession,
    /*pExportInfo=*/        &__nvoc_export_info_RsSession
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RsSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_RsSession(RsSession *pThis) {
    __nvoc_sessionDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsSession(RsSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_RsSession(RsSession *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail_RsShared;
    __nvoc_init_dataField_RsSession(pThis);

    status = __nvoc_sessionConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail__init;
    goto __nvoc_ctor_RsSession_exit; // Success

__nvoc_ctor_RsSession_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_RsSession_fail_RsShared:
__nvoc_ctor_RsSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsSession_1(RsSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // sessionRemoveDependant -- virtual
    pThis->__sessionRemoveDependant__ = &sessionRemoveDependant_IMPL;

    // sessionRemoveDependency -- virtual
    pThis->__sessionRemoveDependency__ = &sessionRemoveDependency_IMPL;
} // End __nvoc_init_funcTable_RsSession_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 2 virtual method(s).
void __nvoc_init_funcTable_RsSession(RsSession *pThis) {

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_RsSession_1(pThis);
}

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_RsSession(RsSession *pThis) {
    pThis->__nvoc_pbase_RsSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
    __nvoc_init_funcTable_RsSession(pThis);
}

NV_STATUS __nvoc_objCreate_RsSession(RsSession **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsSession);

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RsSession(pThis);
    status = __nvoc_ctor_RsSession(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_RsSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsSession(RsSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_RsSession(ppThis, pParent, createFlags);

    return status;
}


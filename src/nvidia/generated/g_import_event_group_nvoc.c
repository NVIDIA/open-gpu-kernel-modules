#define NVOC_IMPORT_EVENT_GROUP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_import_event_group_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__18e040 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ImportEventGroup;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventGroup;

// Forward declarations for ImportEventGroup
void __nvoc_init__EventGroup(EventGroup*);
void __nvoc_init__ImportEventGroup(ImportEventGroup*);
void __nvoc_init_funcTable_ImportEventGroup(ImportEventGroup*);
NV_STATUS __nvoc_ctor_ImportEventGroup(ImportEventGroup*, struct EventBus *pEventBus);
void __nvoc_init_dataField_ImportEventGroup(ImportEventGroup*);
void __nvoc_dtor_ImportEventGroup(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ImportEventGroup;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ImportEventGroup;

// Down-thunk(s) to bridge ImportEventGroup methods from ancestors (if any)

// Up-thunk(s) to bridge ImportEventGroup methods to ancestors (if any)

// Class-specific details for ImportEventGroup
const struct NVOC_CLASS_DEF __nvoc_class_def_ImportEventGroup = 
{
    .classInfo.size =               sizeof(ImportEventGroup),
    .classInfo.classId =            classId(ImportEventGroup),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ImportEventGroup",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ImportEventGroup,
    .pCastInfo =          &__nvoc_castinfo__ImportEventGroup,
    .pExportInfo =        &__nvoc_export_info__ImportEventGroup
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ImportEventGroup __nvoc_metadata__ImportEventGroup = {
    .rtti.pClassDef = &__nvoc_class_def_ImportEventGroup,    // (impevtgrp) this
    .rtti.dtor      = &__nvoc_dtor_ImportEventGroup,
    .rtti.offset    = 0,
    .metadata__EventGroup.rtti.pClassDef = &__nvoc_class_def_EventGroup,    // (eventgroup) super
    .metadata__EventGroup.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__EventGroup.rtti.offset    = NV_OFFSETOF(ImportEventGroup, __nvoc_base_EventGroup),
    .metadata__EventGroup.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__EventGroup.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__EventGroup.metadata__Object.rtti.offset    = NV_OFFSETOF(ImportEventGroup, __nvoc_base_EventGroup.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ImportEventGroup = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__ImportEventGroup.rtti,    // [0]: (impevtgrp) this
        &__nvoc_metadata__ImportEventGroup.metadata__EventGroup.rtti,    // [1]: (eventgroup) super
        &__nvoc_metadata__ImportEventGroup.metadata__EventGroup.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ImportEventGroup = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ImportEventGroup object.
void __nvoc_impevtgrpDestruct(ImportEventGroup*);
void __nvoc_dtor_EventGroup(Dynamic*);
void __nvoc_dtor_ImportEventGroup(Dynamic* pThis) {

    ImportEventGroup *__nvoc_this = (ImportEventGroup *) pThis;

// Call destructor.
    __nvoc_impevtgrpDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_EventGroup((Dynamic *) &__nvoc_this->__nvoc_base_EventGroup);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ImportEventGroup(ImportEventGroup *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ImportEventGroup object.
NV_STATUS __nvoc_ctor_EventGroup(EventGroup *pEventGroup, struct EventBus *pEventBus);
NV_STATUS __nvoc_ctor_ImportEventGroup(ImportEventGroup *pGroup, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_EventGroup(&pGroup->__nvoc_base_EventGroup, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_ImportEventGroup_fail_EventGroup;

    // Initialize data fields.
    __nvoc_init_dataField_ImportEventGroup(pGroup);

    // Call the constructor for this class.
    status = __nvoc_impevtgrpConstruct(pGroup, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_ImportEventGroup_fail__init;
    goto __nvoc_ctor_ImportEventGroup_exit; // Success

    // Unwind on error.
__nvoc_ctor_ImportEventGroup_fail__init:
    __nvoc_dtor_EventGroup((Dynamic *)&pGroup->__nvoc_base_EventGroup);
__nvoc_ctor_ImportEventGroup_fail_EventGroup:
__nvoc_ctor_ImportEventGroup_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ImportEventGroup_1(ImportEventGroup *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ImportEventGroup_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_ImportEventGroup(ImportEventGroup *pThis) {
    __nvoc_init_funcTable_ImportEventGroup_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ImportEventGroup(ImportEventGroup *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_EventGroup.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_EventGroup = &pThis->__nvoc_base_EventGroup;    // (eventgroup) super
    pThis->__nvoc_pbase_ImportEventGroup = pThis;    // (impevtgrp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__EventGroup(&pThis->__nvoc_base_EventGroup);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_EventGroup.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ImportEventGroup.metadata__EventGroup.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_EventGroup.__nvoc_metadata_ptr = &__nvoc_metadata__ImportEventGroup.metadata__EventGroup;    // (eventgroup) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ImportEventGroup;    // (impevtgrp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ImportEventGroup(pThis);
}

NV_STATUS __nvoc_objCreate_ImportEventGroup(ImportEventGroup **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ImportEventGroup *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ImportEventGroup));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ImportEventGroup));

    __nvoc_pThis->__nvoc_base_EventGroup.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_EventGroup.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_EventGroup.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ImportEventGroup(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ImportEventGroup(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ImportEventGroup_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ImportEventGroup_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_EventGroup.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ImportEventGroup));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ImportEventGroup(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_ImportEventGroup((ImportEventGroup **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#define NVOC_HYPERVISOR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_hypervisor_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x33c1ba = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJHYPERVISOR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_init_funcTable_OBJHYPERVISOR(OBJHYPERVISOR*);
NV_STATUS __nvoc_ctor_OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_init_dataField_OBJHYPERVISOR(OBJHYPERVISOR*);
void __nvoc_dtor_OBJHYPERVISOR(OBJHYPERVISOR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJHYPERVISOR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHYPERVISOR;

// Down-thunk(s) to bridge OBJHYPERVISOR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJHYPERVISOR methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJHYPERVISOR),
        /*classId=*/            classId(OBJHYPERVISOR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJHYPERVISOR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJHYPERVISOR,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJHYPERVISOR,
    /*pExportInfo=*/        &__nvoc_export_info__OBJHYPERVISOR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJHYPERVISOR __nvoc_metadata__OBJHYPERVISOR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJHYPERVISOR,    // (hypervisor) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJHYPERVISOR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJHYPERVISOR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJHYPERVISOR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJHYPERVISOR.rtti,    // [0]: (hypervisor) this
        &__nvoc_metadata__OBJHYPERVISOR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJHYPERVISOR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    __nvoc_hypervisorDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJHYPERVISOR_fail_Object;
    __nvoc_init_dataField_OBJHYPERVISOR(pThis);

    status = __nvoc_hypervisorConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJHYPERVISOR_fail__init;
    goto __nvoc_ctor_OBJHYPERVISOR_exit; // Success

__nvoc_ctor_OBJHYPERVISOR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJHYPERVISOR_fail_Object:
__nvoc_ctor_OBJHYPERVISOR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJHYPERVISOR_1(OBJHYPERVISOR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJHYPERVISOR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJHYPERVISOR(OBJHYPERVISOR *pThis) {
    __nvoc_init_funcTable_OBJHYPERVISOR_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJHYPERVISOR(OBJHYPERVISOR *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJHYPERVISOR = pThis;    // (hypervisor) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJHYPERVISOR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJHYPERVISOR;    // (hypervisor) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJHYPERVISOR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJHYPERVISOR(OBJHYPERVISOR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJHYPERVISOR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJHYPERVISOR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJHYPERVISOR));

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

    __nvoc_init__OBJHYPERVISOR(pThis);
    status = __nvoc_ctor_OBJHYPERVISOR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJHYPERVISOR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJHYPERVISOR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJHYPERVISOR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJHYPERVISOR(OBJHYPERVISOR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJHYPERVISOR(ppThis, pParent, createFlags);

    return status;
}


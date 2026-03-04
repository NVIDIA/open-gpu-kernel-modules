#define NVOC_SYSMEM_SCRUB_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_sysmem_scrub_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x266962 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SysmemScrubber;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for SysmemScrubber
void __nvoc_init__Object(Object*);
void __nvoc_init__SysmemScrubber(SysmemScrubber*);
void __nvoc_init_funcTable_SysmemScrubber(SysmemScrubber*);
NV_STATUS __nvoc_ctor_SysmemScrubber(SysmemScrubber*, struct OBJGPU *arg_pGpu);
void __nvoc_init_dataField_SysmemScrubber(SysmemScrubber*);
void __nvoc_dtor_SysmemScrubber(SysmemScrubber*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SysmemScrubber;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SysmemScrubber;

// Down-thunk(s) to bridge SysmemScrubber methods from ancestors (if any)

// Up-thunk(s) to bridge SysmemScrubber methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_SysmemScrubber = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SysmemScrubber),
        /*classId=*/            classId(SysmemScrubber),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SysmemScrubber",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SysmemScrubber,
    /*pCastInfo=*/          &__nvoc_castinfo__SysmemScrubber,
    /*pExportInfo=*/        &__nvoc_export_info__SysmemScrubber
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SysmemScrubber __nvoc_metadata__SysmemScrubber = {
    .rtti.pClassDef = &__nvoc_class_def_SysmemScrubber,    // (sysmemscrub) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SysmemScrubber,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(SysmemScrubber, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SysmemScrubber = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__SysmemScrubber.rtti,    // [0]: (sysmemscrub) this
        &__nvoc_metadata__SysmemScrubber.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SysmemScrubber = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_sysmemscrubDestruct(SysmemScrubber*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_SysmemScrubber(SysmemScrubber *pThis) {
    __nvoc_sysmemscrubDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SysmemScrubber(SysmemScrubber *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_SysmemScrubber(SysmemScrubber *pThis, struct OBJGPU * arg_pGpu) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_SysmemScrubber_fail_Object;
    __nvoc_init_dataField_SysmemScrubber(pThis);

    status = __nvoc_sysmemscrubConstruct(pThis, arg_pGpu);
    if (status != NV_OK) goto __nvoc_ctor_SysmemScrubber_fail__init;
    goto __nvoc_ctor_SysmemScrubber_exit; // Success

__nvoc_ctor_SysmemScrubber_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_SysmemScrubber_fail_Object:
__nvoc_ctor_SysmemScrubber_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SysmemScrubber_1(SysmemScrubber *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SysmemScrubber_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SysmemScrubber(SysmemScrubber *pThis) {
    __nvoc_init_funcTable_SysmemScrubber_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SysmemScrubber(SysmemScrubber *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_SysmemScrubber = pThis;    // (sysmemscrub) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SysmemScrubber.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SysmemScrubber;    // (sysmemscrub) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SysmemScrubber(pThis);
}

NV_STATUS __nvoc_objCreate_SysmemScrubber(SysmemScrubber **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU *arg_pGpu)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SysmemScrubber *pThis;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(SysmemScrubber));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SysmemScrubber));

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

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SysmemScrubber(pThis);
    status = __nvoc_ctor_SysmemScrubber(pThis, arg_pGpu);
    if (status != NV_OK) goto __nvoc_objCreate_SysmemScrubber_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SysmemScrubber_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SysmemScrubber));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SysmemScrubber(SysmemScrubber **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU *arg_pGpu = va_arg(args, struct OBJGPU *);

    status = __nvoc_objCreate_SysmemScrubber(ppThis, pParent, createFlags, arg_pGpu);

    return status;
}


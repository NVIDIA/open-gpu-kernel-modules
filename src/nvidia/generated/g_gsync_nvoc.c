#define NVOC_GSYNC_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gsync_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xd07fd0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJGSYNCMGR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR*);
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGSYNCMGR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGSYNCMGR;

// Down-thunk(s) to bridge OBJGSYNCMGR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJGSYNCMGR methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGSYNCMGR),
        /*classId=*/            classId(OBJGSYNCMGR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGSYNCMGR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGSYNCMGR,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJGSYNCMGR,
    /*pExportInfo=*/        &__nvoc_export_info__OBJGSYNCMGR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJGSYNCMGR __nvoc_metadata__OBJGSYNCMGR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGSYNCMGR,    // (gsyncmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGSYNCMGR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGSYNCMGR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGSYNCMGR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJGSYNCMGR.rtti,    // [0]: (gsyncmgr) this
        &__nvoc_metadata__OBJGSYNCMGR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGSYNCMGR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    __nvoc_gsyncmgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail_Object;
    __nvoc_init_dataField_OBJGSYNCMGR(pThis);

    status = __nvoc_gsyncmgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail__init;
    goto __nvoc_ctor_OBJGSYNCMGR_exit; // Success

__nvoc_ctor_OBJGSYNCMGR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGSYNCMGR_fail_Object:
__nvoc_ctor_OBJGSYNCMGR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGSYNCMGR_1(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGSYNCMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    __nvoc_init_funcTable_OBJGSYNCMGR_1(pThis);
}

NvBool gsyncmgrIsFirmwareGPUMismatch_STATIC_DISPATCH(struct OBJGPU *pGpu, OBJGSYNC *pGsync) {
    ChipHal *chipHal = &staticCast(pGpu, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;


    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        return gsyncmgrIsFirmwareGPUMismatch_GB100(pGpu, pGsync);
    }
    // default
    else
    {
        return gsyncmgrIsFirmwareGPUMismatch_4a4dee(pGpu, pGsync);
    }

    NV_ASSERT_FAILED("No hal impl found for gsyncmgrIsFirmwareGPUMismatch");

    return NV_FALSE;
}

// Initialize newly constructed object.
void __nvoc_init__OBJGSYNCMGR(OBJGSYNCMGR *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJGSYNCMGR = pThis;    // (gsyncmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGSYNCMGR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGSYNCMGR;    // (gsyncmgr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGSYNCMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGSYNCMGR(OBJGSYNCMGR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGSYNCMGR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGSYNCMGR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGSYNCMGR));

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

    __nvoc_init__OBJGSYNCMGR(pThis);
    status = __nvoc_ctor_OBJGSYNCMGR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGSYNCMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGSYNCMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGSYNCMGR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGSYNCMGR(OBJGSYNCMGR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGSYNCMGR(ppThis, pParent, createFlags);

    return status;
}


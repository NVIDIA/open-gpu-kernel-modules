#define NVOC_FBSR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_fbsr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xa30fe6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJFBSR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJFBSR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJFBSR(OBJFBSR*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_OBJFBSR(OBJFBSR*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_OBJFBSR(OBJFBSR*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_OBJFBSR(OBJFBSR*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_OBJFBSR(OBJFBSR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJFBSR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJFBSR;

// Down-thunk(s) to bridge OBJFBSR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJFBSR methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJFBSR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJFBSR),
        /*classId=*/            classId(OBJFBSR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJFBSR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJFBSR,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJFBSR,
    /*pExportInfo=*/        &__nvoc_export_info__OBJFBSR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJFBSR __nvoc_metadata__OBJFBSR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJFBSR,    // (fbsr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJFBSR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJFBSR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJFBSR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJFBSR.rtti,    // [0]: (fbsr) this
        &__nvoc_metadata__OBJFBSR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJFBSR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJFBSR(OBJFBSR *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJFBSR_fail_Object;
    __nvoc_init_dataField_OBJFBSR(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJFBSR_exit; // Success

__nvoc_ctor_OBJFBSR_fail_Object:
__nvoc_ctor_OBJFBSR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJFBSR_1(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // fbsrBegin -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__fbsrBegin__ = &fbsrBegin_GA100;
    }
    else
    {
        pThis->__fbsrBegin__ = &fbsrBegin_GM107;
    }

    // fbsrEnd -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__fbsrEnd__ = &fbsrEnd_GA100;
    }
    else
    {
        pThis->__fbsrEnd__ = &fbsrEnd_GM107;
    }

    // fbsrSendMemsysProgramRawCompressionMode -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__fbsrSendMemsysProgramRawCompressionMode__ = &fbsrSendMemsysProgramRawCompressionMode_GA100;
    }
    // default
    else
    {
        pThis->__fbsrSendMemsysProgramRawCompressionMode__ = &fbsrSendMemsysProgramRawCompressionMode_56cd7a;
    }
} // End __nvoc_init_funcTable_OBJFBSR_1 with approximately 6 basic block(s).


// Initialize vtable(s) for 3 virtual method(s).
void __nvoc_init_funcTable_OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_OBJFBSR_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__OBJFBSR(OBJFBSR *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJFBSR = pThis;    // (fbsr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJFBSR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJFBSR;    // (fbsr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJFBSR(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJFBSR(OBJFBSR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJFBSR *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJFBSR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJFBSR));

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

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__OBJFBSR(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJFBSR(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJFBSR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJFBSR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJFBSR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJFBSR(OBJFBSR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJFBSR(ppThis, pParent, createFlags);

    return status;
}


#define NVOC_VASPACE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_vaspace_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x6c347f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJVASPACE
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJVASPACE(OBJVASPACE*);
void __nvoc_init_funcTable_OBJVASPACE(OBJVASPACE*);
NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_dataField_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJVASPACE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJVASPACE;

// Down-thunk(s) to bridge OBJVASPACE methods from ancestors (if any)

// Up-thunk(s) to bridge OBJVASPACE methods to ancestors (if any)

// Not instantiable because it's an abstract class with following pure virtual functions:
//  vaspaceConstruct_
//  vaspaceAlloc
//  vaspaceFree
//  vaspaceApplyDefaultAlignment
//  vaspaceGetVasInfo
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJVASPACE),
        /*classId=*/            classId(OBJVASPACE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJVASPACE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJVASPACE,
    /*pExportInfo=*/        &__nvoc_export_info__OBJVASPACE
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJVASPACE __nvoc_metadata__OBJVASPACE = {
    .rtti.pClassDef = &__nvoc_class_def_OBJVASPACE,    // (vaspace) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJVASPACE,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJVASPACE, __nvoc_base_Object),

    .vtable.__vaspaceConstruct___ = NULL,    // pure virtual
    .vtable.__vaspaceAlloc__ = NULL,    // pure virtual
    .vtable.__vaspaceFree__ = NULL,    // pure virtual
    .vtable.__vaspaceApplyDefaultAlignment__ = NULL,    // pure virtual
    .vtable.__vaspaceIncAllocRefCnt__ = &vaspaceIncAllocRefCnt_14ee5e,    // inline virtual body
    .vtable.__vaspaceGetVaStart__ = &vaspaceGetVaStart_IMPL,    // virtual
    .vtable.__vaspaceGetVaLimit__ = &vaspaceGetVaLimit_IMPL,    // virtual
    .vtable.__vaspaceGetVasInfo__ = NULL,    // pure virtual
    .vtable.__vaspaceGetFlags__ = &vaspaceGetFlags_edd98b,    // inline virtual body
    .vtable.__vaspaceMap__ = &vaspaceMap_14ee5e,    // inline virtual body
    .vtable.__vaspaceUnmap__ = &vaspaceUnmap_af5be7,    // inline virtual body
    .vtable.__vaspaceReserveMempool__ = &vaspaceReserveMempool_ac1694,    // inline virtual body
    .vtable.__vaspaceGetHeap__ = &vaspaceGetHeap_9451a7,    // inline virtual body
    .vtable.__vaspaceGetMapPageSize__ = &vaspaceGetMapPageSize_c26fae,    // inline virtual body
    .vtable.__vaspaceGetBigPageSize__ = &vaspaceGetBigPageSize_c26fae,    // inline virtual body
    .vtable.__vaspaceIsMirrored__ = &vaspaceIsMirrored_2fa1ff,    // inline virtual body
    .vtable.__vaspaceIsFaultCapable__ = &vaspaceIsFaultCapable_2fa1ff,    // inline virtual body
    .vtable.__vaspaceIsExternallyOwned__ = &vaspaceIsExternallyOwned_2fa1ff,    // inline virtual body
    .vtable.__vaspaceIsAtsEnabled__ = &vaspaceIsAtsEnabled_2fa1ff,    // inline virtual body
    .vtable.__vaspaceGetPasid__ = &vaspaceGetPasid_14ee5e,    // inline virtual body
    .vtable.__vaspaceGetPageDirBase__ = &vaspaceGetPageDirBase_9451a7,    // inline virtual body
    .vtable.__vaspaceGetKernelPageDirBase__ = &vaspaceGetKernelPageDirBase_9451a7,    // inline virtual body
    .vtable.__vaspacePinRootPageDir__ = &vaspacePinRootPageDir_14ee5e,    // inline virtual body
    .vtable.__vaspaceUnpinRootPageDir__ = &vaspaceUnpinRootPageDir_af5be7,    // inline virtual body
    .vtable.__vaspaceInvalidateTlb__ = &vaspaceInvalidateTlb_IMPL,    // virtual
    .vtable.__vaspaceGetPageTableInfo__ = &vaspaceGetPageTableInfo_14ee5e,    // inline virtual body
    .vtable.__vaspaceGetPteInfo__ = &vaspaceGetPteInfo_14ee5e,    // inline virtual body
    .vtable.__vaspaceSetPteInfo__ = &vaspaceSetPteInfo_14ee5e,    // inline virtual body
    .vtable.__vaspaceFreeV2__ = &vaspaceFreeV2_14ee5e,    // inline virtual body
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJVASPACE = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJVASPACE.rtti,    // [0]: (vaspace) this
        &__nvoc_metadata__OBJVASPACE.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJVASPACE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJVASPACE(OBJVASPACE *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJVASPACE(OBJVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJVASPACE_fail_Object;
    __nvoc_init_dataField_OBJVASPACE(pThis);
    goto __nvoc_ctor_OBJVASPACE_exit; // Success

__nvoc_ctor_OBJVASPACE_fail_Object:
__nvoc_ctor_OBJVASPACE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJVASPACE_1(OBJVASPACE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJVASPACE_1


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_OBJVASPACE(OBJVASPACE *pThis) {
    __nvoc_init_funcTable_OBJVASPACE_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJVASPACE(OBJVASPACE *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJVASPACE = pThis;    // (vaspace) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJVASPACE.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJVASPACE;    // (vaspace) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJVASPACE(pThis);
}


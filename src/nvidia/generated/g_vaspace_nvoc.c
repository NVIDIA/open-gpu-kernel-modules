#define NVOC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vaspace_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6c347f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_funcTable_OBJVASPACE(OBJVASPACE*);
NV_STATUS __nvoc_ctor_OBJVASPACE(OBJVASPACE*);
void __nvoc_init_dataField_OBJVASPACE(OBJVASPACE*);
void __nvoc_dtor_OBJVASPACE(OBJVASPACE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJVASPACE;

static const struct NVOC_RTTI __nvoc_rtti_OBJVASPACE_OBJVASPACE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJVASPACE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJVASPACE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJVASPACE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJVASPACE, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJVASPACE = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJVASPACE_OBJVASPACE,
        &__nvoc_rtti_OBJVASPACE_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_OBJVASPACE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJVASPACE
};

// Down-thunk(s) to bridge OBJVASPACE methods from ancestors (if any)

// Up-thunk(s) to bridge OBJVASPACE methods to ancestors (if any)

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJVASPACE = 
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

    // Per-class vtable definition
    static const struct NVOC_VTABLE__OBJVASPACE vtable = {
        .__vaspaceConstruct___ = NULL,    // pure virtual
        .__vaspaceAlloc__ = NULL,    // pure virtual
        .__vaspaceFree__ = NULL,    // pure virtual
        .__vaspaceApplyDefaultAlignment__ = NULL,    // pure virtual
        .__vaspaceIncAllocRefCnt__ = &vaspaceIncAllocRefCnt_14ee5e,    // inline virtual body
        .__vaspaceGetVaStart__ = &vaspaceGetVaStart_IMPL,    // virtual
        .__vaspaceGetVaLimit__ = &vaspaceGetVaLimit_IMPL,    // virtual
        .__vaspaceGetVasInfo__ = NULL,    // pure virtual
        .__vaspaceGetFlags__ = &vaspaceGetFlags_edd98b,    // inline virtual body
        .__vaspaceMap__ = &vaspaceMap_14ee5e,    // inline virtual body
        .__vaspaceUnmap__ = &vaspaceUnmap_af5be7,    // inline virtual body
        .__vaspaceReserveMempool__ = &vaspaceReserveMempool_ac1694,    // inline virtual body
        .__vaspaceGetHeap__ = &vaspaceGetHeap_9451a7,    // inline virtual body
        .__vaspaceGetMapPageSize__ = &vaspaceGetMapPageSize_c26fae,    // inline virtual body
        .__vaspaceGetBigPageSize__ = &vaspaceGetBigPageSize_c26fae,    // inline virtual body
        .__vaspaceIsMirrored__ = &vaspaceIsMirrored_2fa1ff,    // inline virtual body
        .__vaspaceIsFaultCapable__ = &vaspaceIsFaultCapable_2fa1ff,    // inline virtual body
        .__vaspaceIsExternallyOwned__ = &vaspaceIsExternallyOwned_2fa1ff,    // inline virtual body
        .__vaspaceIsAtsEnabled__ = &vaspaceIsAtsEnabled_2fa1ff,    // inline virtual body
        .__vaspaceGetPasid__ = &vaspaceGetPasid_14ee5e,    // inline virtual body
        .__vaspaceGetPageDirBase__ = &vaspaceGetPageDirBase_9451a7,    // inline virtual body
        .__vaspaceGetKernelPageDirBase__ = &vaspaceGetKernelPageDirBase_9451a7,    // inline virtual body
        .__vaspacePinRootPageDir__ = &vaspacePinRootPageDir_14ee5e,    // inline virtual body
        .__vaspaceUnpinRootPageDir__ = &vaspaceUnpinRootPageDir_af5be7,    // inline virtual body
        .__vaspaceInvalidateTlb__ = &vaspaceInvalidateTlb_IMPL,    // virtual
        .__vaspaceGetPageTableInfo__ = &vaspaceGetPageTableInfo_14ee5e,    // inline virtual body
        .__vaspaceGetPteInfo__ = &vaspaceGetPteInfo_14ee5e,    // inline virtual body
        .__vaspaceSetPteInfo__ = &vaspaceSetPteInfo_14ee5e,    // inline virtual body
        .__vaspaceFreeV2__ = &vaspaceFreeV2_14ee5e,    // inline virtual body
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_vtable = &vtable;    // (vaspace) this
    __nvoc_init_funcTable_OBJVASPACE_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJVASPACE(OBJVASPACE *pThis) {
    pThis->__nvoc_pbase_OBJVASPACE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJVASPACE(pThis);
}

